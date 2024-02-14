/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <string>
#include <vector>
#include <iostream>

#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xhelper.hpp"
#include "xeus-pywrap/xinterpreter.hpp"

// pybind11
#include <pybind11/pybind11.h>
#include <pybind11_json/pybind11_json.hpp>

namespace nl = nlohmann;
namespace py = pybind11;

namespace xeus_pywrap
{
 
    interpreter::interpreter(nl::json && parameters)
    {
        // get the name of the python module to load
        std::string module_name = parameters["py_module"].get<std::string>();
        std::string factory_name = parameters["py_factory"].get<std::string>();

        py::module_ py_module = py::module_::import(module_name.c_str());
        py::object  py_factory = py_module.attr(factory_name.c_str());
        m_py_interpreter = py_factory(parameters);
        
        xeus::register_interpreter(this);

        m_py_interpreter.attr("_configure")();
        m_kernel_info_reply = m_py_interpreter.attr("_kernel_info_request")();

        // give the py interpreter a reference to the c++ interpreter
        py::object self = py::cast(this);
        m_py_interpreter.attr("_set_interpreter_impl")(self);
    }

    nl::json interpreter::execute_request_impl(int execution_count, // Typically the cell number
                                                      const  std::string & code, // Code to execute
                                                      bool silent,
                                                      bool store_history,
                                                      nl::json user_expressions,
                                                      bool allow_stdin)
    {
        try{
            return m_py_interpreter.attr("_execute_request")(
                execution_count, 
                code,
                silent,
                store_history,
                user_expressions,
                allow_stdin
            );
        }
        catch(std::exception& e){
            const auto error_message = e.what();
            publish_execution_error("Excetpion",  error_message, {});
            return xeus::create_error_reply(error_message, "Exception", {});
        }

    }

    void interpreter::configure_impl()
    {
        // nothing to do here
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {

        try{
            return m_py_interpreter.attr("_is_complete_request")(code);
        }
        catch(std::exception& e){
            const auto error_message = e.what();
            publish_execution_error("Excetpion",  error_message, {});
            return xeus::create_error_reply(error_message, "Exception", {});
        }
        
    }

    nl::json interpreter::complete_request_impl(const std::string&  code,
                                                     int cursor_pos)
    {
        try{
            return m_py_interpreter.attr("_complete_request")(code, cursor_pos);
        }
        catch(std::exception& e){
            const auto error_message = e.what();
            publish_execution_error("Excetpion",  error_message, {});
            return xeus::create_error_reply(error_message, "Exception", {});
        }
    }

    nl::json interpreter::inspect_request_impl(const std::string& /*code*/,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
    {

        try{
            return m_py_interpreter.attr("_inspect_request")();
        }
        catch(std::exception& e){
            const auto error_message = e.what();
            publish_execution_error("Excetpion",  error_message, {});
            return xeus::create_error_reply(error_message, "Exception", {});
        }
         
    }

    void interpreter::shutdown_request_impl() {
        std::cout<<"shotdown pls"<<std::endl;
        // try{
        //     m_py_interpreter.attr("_shutdown_request")();
        // }
        // catch(std::exception& e){
        //     const auto error_message = e.what();
        //     publish_execution_error("Excetpion",  error_message, {});
        // }
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        return m_kernel_info_reply;
    }

}
