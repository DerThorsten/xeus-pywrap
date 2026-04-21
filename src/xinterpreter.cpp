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
#include <pybind11/eval.h>
#include <pybind11_json/pybind11_json.hpp>

namespace nl = nlohmann;
namespace py = pybind11;

namespace xeus_pywrap
{
    void exec(const py::object& code, const py::object& scope)
    {
        py::exec("exec(_code_, _scope_, _scope_)", py::globals(), py::dict(py::arg("_code_") = code, py::arg("_scope_") = scope));
    }

    py::module create_module(const std::string& module_name)
    {
        return py::module_::create_extension_module(module_name.c_str(), nullptr, new py::module_::module_def);
    }
    
    py::module make_request_context_module()
    {
        py::module context_module = create_module("request_context_module");

        py::class_<xeus::xrequest_context>(context_module, "RequestContext")
            .def(py::init<>())
            .def_property_readonly("header", &xeus::xrequest_context::header);

        exec(py::str(R"(
import contextvars as cv
request_context = cv.ContextVar('request_context')

def set_request_context(ctx):
    request_context.set(ctx)

def get_request_context():
    return request_context.get()
        )"), context_module.attr("__dict__"));

        return context_module;
    }

    py::module get_request_context_module()
    {
        static py::module rc_module = make_request_context_module();
        return rc_module;
    }

    interpreter::interpreter(
        py::dict globals,
        nl::json && parameters
    )
    :   m_globals(globals),
        m_py_interpreter(py::none()),
        m_kernel_info_reply()
    {
        get_request_context_module();

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

    void interpreter::execute_request_impl(send_reply_callback cb,
                                           int execution_count,
                                           const std::string& code,
                                           xeus::execute_request_config config,
                                           nl::json user_expressions)
    /*nl::json interpreter::execute_request_impl(int execution_count, // Typically the cell number
                                                      const  std::string & code, // Code to execute
                                                      bool silent,
                                                      bool store_history,
                                                      nl::json user_expressions,
                                                      bool allow_stdin)*/
    {
        try
        {

            send_reply_callback_wrapper wrapper;
            wrapper.m_callback = cb;

            m_py_interpreter.attr("_execute_request")(
                wrapper,
                execution_count, 
                code,
                config.silent,
                config.store_history,
                user_expressions,
                config.allow_stdin
            );
        }
        catch(std::exception& e){
            const auto error_message = e.what();
            publish_execution_error("Excetpion",  error_message, {});
            xeus::create_error_reply(error_message, "Exception", {});
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

    nl::json  interpreter::shutdown_request_impl(bool restart) {
        std::cout<<"shotdown pls"<<std::endl;
        // try{
        //     m_py_interpreter.attr("_shutdown_request")();
        // }
        // catch(std::exception& e){
        //     const auto error_message = e.what();
        //     publish_execution_error("Excetpion",  error_message, {});
        // }
        
        return xeus::create_shutdown_reply(false);
        
    }

    nl::json interpreter::interrupt_request_impl()
    {
        return xeus::create_interrupt_reply();      
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        return m_kernel_info_reply;
    }

    void interpreter::set_request_context(xeus::xrequest_context context)
    {
        //py::gil_scoped_acquire acquire;
        py::module context_module = get_request_context_module();
        context_module.attr("set_request_context")(context);
    }

    const xeus::xrequest_context& interpreter::get_request_context() const noexcept
    {
        //py::gil_scoped_acquire acquire;
        py::module context_module = get_request_context_module();
        py::object res = context_module.attr("get_request_context")();
        const auto& ret = *(res.cast<xeus::xrequest_context*>());
        return ret;
        //return *(res.cast<xeus::xrequest_context*>());
    }
}
