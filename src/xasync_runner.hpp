/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#ifndef XEUS_PYWRAP_XASYNC_RUNNER_HPP
#define XEUS_PYWRAP_XASYNC_RUNNER_HPP


#include <memory>

#include "xeus-pywrap/xeus_pywrap_config.hpp"
#include "xeus-zmq/xshell_runner.hpp"
#include "pybind11/pybind11.h"


namespace py = pybind11;

namespace xeus_pywrap
{

    class XEUS_PYWRAP_HIDDEN xasync_runner final : public xeus::xshell_runner
    {
    public:

        xasync_runner(py::dict globals);
        xasync_runner(const xasync_runner&) = delete;
        xasync_runner& operator=(const xasync_runner&) = delete;
        xasync_runner(xasync_runner&&) = delete;
        xasync_runner& operator=(xasync_runner&&) = delete;
        ~xasync_runner() override = default;
        
    private:
        void on_message_doorbell_shell();
        void on_message_doorbell_controller();

        void run_impl() override;
        
        py::dict m_global_dict;

    };

} // namespace xeus_pywrap

#endif // XEUS_PYWRAP_XASYNC_RUNNER_HPP
