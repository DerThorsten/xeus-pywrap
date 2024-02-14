/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/


#ifndef XEUS_PYWRAP_INTERPRETER_HPP
#define XEUS_PYWRAP_INTERPRETER_HPP

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "xeus_pywrap_config.hpp"
#include "xeus/xinterpreter.hpp"

#include <pybind11/pybind11.h>


namespace nl = nlohmann;
namespace py = pybind11;

namespace xeus_pywrap
{
    class XEUS_PYWRAP_API interpreter : public xeus::xinterpreter
    {
    public:

        interpreter(nl::json && parameters);
        virtual ~interpreter() = default;

    protected:

        void configure_impl() override;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

        py::object m_py_interpreter;

        // we cache the kernel info, since the kernel info request
        // may be called from different threads which
        // may cause problems with the python interpreter
        nl::json m_kernel_info_reply;
    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
