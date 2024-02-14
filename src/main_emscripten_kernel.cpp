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

EMSCRIPTEN_BINDINGS(my_module) {
    pyjs::export_js_module();
    xeus::export_core();
    using interpreter_type = xeus_pywrap::interpreter;
    xeus::export_parametrizable_kernel<interpreter_type>("xkernel");
}

PYBIND11_EMBEDDED_MODULE(_xpywrap, m) {
    xeus_pywrap::export_pywrap(m);
}

PYBIND11_EMBEDDED_MODULE(pyjs, m) {
    pyjs::export_pyjs_module(m);
}