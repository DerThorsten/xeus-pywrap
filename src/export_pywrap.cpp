#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11_json/pybind11_json.hpp>
#include "xeus-pywrap/xinterpreter.hpp"
#include "xeus/xinput.hpp"
#include "xeus/xhelper.hpp"
#include <pybind11_json/pybind11_json.hpp>

namespace py = pybind11;

namespace xeus_pywrap
{

    void export_free_functions(py::module& m)
    {
       // return xeus::blocking_input_request(prompt, false);
       m.def("blocking_input_request", [](const std::string& prompt, bool password) {
            return xeus::blocking_input_request(prompt, password);
        }, py::arg("prompt"), py::arg("password") = false);

        m.def("create_error_reply", &xeus::create_error_reply, 
            py::arg("ename") = std::string(), 
            py::arg("evalue") = std::string(), 
            py::arg("trace_back") = nl::json::array()
        );

        m.def("create_successful_reply", &xeus::create_successful_reply, 
            py::arg("payload") = nl::json::array(), 
            py::arg("user_expressions") = nl::json::object()
        );

        m.def("create_complete_reply", &xeus::create_complete_reply, 
            py::arg("matches"), 
            py::arg("cursor_start"), 
            py::arg("cursor_end"), 
            py::arg("metadata") = nl::json::object()
        );

        m.def("create_inspect_reply", &xeus::create_inspect_reply, 
            py::arg("found") = false, 
            py::arg("data") = nl::json::object(), 
            py::arg("metadata") = nl::json::object()
        );

        m.def("create_is_complete_reply", &xeus::create_is_complete_reply, 
            py::arg("status") = std::string(), 
            py::arg("indent") = std::string("")
        );

        m.def("create_kernel_info_reply", [](
                const std::string& implementation = std::string(),
                const std::string& implementation_version = std::string(),
                const std::string& language_name = std::string(),
                const std::string& language_version = std::string(),
                const std::string& language_mimetype = std::string(),
                const std::string& language_file_extension = std::string(),
                const std::string& pygments_lexer = std::string(),
                const nl::json& language_codemirror_mode = nl::json::object(),
                const std::string& language_nbconvert_exporter = std::string(),
                const std::string& banner = std::string(),
                const nl::json& help_links = nl::json::array(),
                const std::vector<std::string>& supported_features = std::vector<std::string>()
        ){
            return xeus::create_info_reply(
                implementation,
                implementation_version,
                language_name,
                language_version,
                language_mimetype,
                language_file_extension,
                pygments_lexer,
                language_codemirror_mode,
                language_nbconvert_exporter,
                banner,
                help_links,
                supported_features
            );
        },
            py::arg("implementation") = std::string(), 
            py::arg("implementation_version") = std::string(), 
            py::arg("language_name") = std::string(), 
            py::arg("language_version") = std::string(), 
            py::arg("language_mimetype") = std::string(), 
            py::arg("language_file_extension") = std::string(), 
            py::arg("pygments_lexer") = std::string(), 
            py::arg("language_codemirror_mode") =  nl::json::object(),
            py::arg("language_nbconvert_exporter") = std::string(), 
            py::arg("banner") = std::string(), 
            py::arg("help_links") = nl::json::array(), 
            py::arg("supported_features") = std::vector<std::string>()
        );
       

        m.def("create_shutdown_reply", &xeus::create_shutdown_reply, py::arg("restart"));

        m.def("create_interrupt_reply", &xeus::create_interrupt_reply);

    }
    
    void export_interpreter(py::module& m)
    {

        // send reply callback wrapper
        py::class_<send_reply_callback_wrapper>(m, "send_reply_callback_wrapper")
            .def("__call__", [](send_reply_callback_wrapper& self, const nl::json& reply) { self.m_callback(reply); })
        ;

        // request config
        py::class_<xeus::execute_request_config>(m, "execute_request_config")
            // .def(py::init<>())
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
