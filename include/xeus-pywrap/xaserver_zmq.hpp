/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#pragma once

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

#include <xeus-zmq/xserver_zmq.hpp>


namespace nl = nlohmann;
namespace py = pybind11;

namespace xeus_pywrap
{

    class XEUS_PYWRAP_API xaserver_zmq;


    std::unique_ptr<xeus::xserver> make_xaserver_zmq(xeus::xcontext& context,
                                              const xeus::xconfiguration& config,
                                              nl::json::error_handler_t eh);
    
    class XEUS_PYWRAP_API xaserver_zmq : public xeus::xserver_zmq
    {
        public:

        virtual ~xaserver_zmq() = default;

        using xserver_zmq::xserver_zmq;
        using xserver_zmq::send_shell_impl;
        using xserver_zmq::send_control_impl;
        using xserver_zmq::send_stdin_impl;
        using xserver_zmq::publish_impl;
        using xserver_zmq::abort_queue_impl;
        using xserver_zmq::stop_impl;
        using xserver_zmq::update_config_impl;



        void poll(long timeout);

        // custom start_impl method
        void start_impl(xeus::xpub_message msg) override;

        bool stopped();
        
    };
}