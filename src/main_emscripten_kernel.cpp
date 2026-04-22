/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <iostream>
#include <memory>


#include <emscripten/bind.h>

#include "xeus-pywrap/xinterpreter.hpp"
#include "xeus/xembind.hpp"
#include "pybind11/embed.h"

#include <pyjs/export_pyjs_module.hpp>
#include <pyjs/export_js_module.hpp>


namespace py = pybind11;

namespace xeus_pywrap
{
    void export_pywrap(py::module& m);
}


std::string extract_parameter(std::string param, std::vector<std::string> argv, bool mandatory)
{
    std::string res = "";
    for (int i = 0; i < argv.size(); ++i)
    {
        if ((std::string(argv[i]) == param) && (i + 1 < argv.size()))
        {
            res = argv[i + 1];
            return res;
        }
    }
    if(mandatory)
    {
        throw std::runtime_error("No parameter provided for " + param);
    }
    return res;
}

template <class interpreter_type>
static interpreter_type* builder_with_args(emscripten::val js_args)
{

    std::vector<std::string> args = emscripten::vecFromJSArray<std::string>(js_args);

    std::string module_name = extract_parameter("--module", args, true /*mandatory*/);
    std::string factory_name = extract_parameter("--factory", args, true /*mandatory*/);
    std::string connection_filename = extract_parameter("--connection", args, false /*mandatory*/);



    emscripten::val jprefix("/");
    emscripten::val jversion("3.13");
    emscripten::val jverbose(true);


    emscripten::val init_phase_1 =  emscripten::val ::module_property("pywrap_init_phase_1");
    init_phase_1(jprefix, jversion, jverbose);

    emscripten::val init_phase_2 =  emscripten::val ::module_property("pywrap_init_phase_2");
    init_phase_2(jprefix, jversion, jverbose);

    auto globals = py::globals();
    
    nl::json interpreter_config = {
        {"py_module", module_name},
        {"py_factory", factory_name},
        {"py_args", args}
    };

    auto* res = new interpreter_type(globals, std::move(interpreter_config));
    return res;
}


PYBIND11_EMBEDDED_MODULE(_xpywrap, m) {
    xeus_pywrap::export_pywrap(m);
}

PYBIND11_EMBEDDED_MODULE(pyjs_core, m) {
    pyjs::export_pyjs_module(m);
}

EMSCRIPTEN_BINDINGS(my_module) {
    pyjs::export_js_module();
    xeus::export_core();
    using interpreter_type = xeus_pywrap::interpreter;
    xeus::export_kernel<interpreter_type, &builder_with_args<interpreter_type>>("xkernel");
}

