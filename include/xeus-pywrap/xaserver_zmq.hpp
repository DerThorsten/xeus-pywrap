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

        
        xaserver_zmq(xeus::xcontext& context,
                     const xeus::xconfiguration& config,
                     nl::json::error_handler_t eh);
        virtual ~xaserver_zmq() = default;

        void poll(long timeout);
        bool stopped() const;

    private:

        void start_impl(xeus::xpub_message msg) override;
        void stop_impl() override;
    };
}
