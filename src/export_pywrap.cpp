#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11_json/pybind11_json.hpp>
#include <xeus-pywrap/xinterpreter.hpp>

#ifndef __EMSCRIPTEN__
#include "xeus-pywrap/xaserver_zmq.hpp"
#endif 

namespace py = pybind11;

namespace xeus_pywrap

    


{
    void export_interpreter(py::module& m)
    {
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

    #ifndef __EMSCRIPTEN__
    void export_server(py::module& m)
    {
        py::class_<xaserver_zmq>(m, "_xaserver_zmq")
            .def("poll", &xaserver_zmq::poll)
            .def("stopped", &xaserver_zmq::stopped);
        ;
    }
    #endif


    void export_pywrap(py::module& m)
    {
        #ifndef __EMSCRIPTEN__
        export_server(m);
        #endif

        export_interpreter(m);
    }
}
