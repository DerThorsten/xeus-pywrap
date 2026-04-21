#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11_json/pybind11_json.hpp>
#include "xeus-pywrap/xinterpreter.hpp"
#include "xeus/xinput.hpp"

namespace py = pybind11;

namespace xeus_pywrap
{

    void export_free_functions(py::module& m)
    {
       // return xeus::blocking_input_request(prompt, false);
       m.def("blocking_input_request", [](const std::string& prompt, bool password) {
            return xeus::blocking_input_request(prompt, password);
        }, py::arg("prompt"), py::arg("password") = false);
    }
    
    void export_interpreter(py::module& m)
    {

        // send reply callback wrapper
        py::class_<send_reply_callback_wrapper>(m, "send_reply_callback_wrapper")
            .def("__call__", [](send_reply_callback_wrapper& self, const nl::json& reply) { self.m_callback(reply); })
        ;

        // request config
        py::class_<xeus::execute_request_config>(m, "execute_request_config")
            .def(py::init<>())
            .def_readwrite("silent", &xeus::execute_request_config::silent)
            .def_readwrite("store_history", &xeus::execute_request_config::store_history)
            .def_readwrite("allow_stdin", &xeus::execute_request_config::allow_stdin)
        ;

        // raw cpp interpreter
        py::class_<interpreter>(m, "_raw_interpreter")
            .def("publish_stream", &interpreter::publish_stream)
            .def("display_data", &interpreter::display_data)
            .def("update_display_data", &interpreter::update_display_data)
            .def("publish_execution_input", &interpreter::publish_execution_input)
            .def("publish_execution_result", &interpreter::publish_execution_result)
            .def("publish_execution_error", &interpreter::publish_execution_error)


        ;
    }


    void export_pywrap(py::module& m)
    {
        export_interpreter(m);
        export_free_functions(m);
    }
}
