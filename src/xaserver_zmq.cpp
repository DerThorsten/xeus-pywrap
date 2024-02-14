#include  "xeus-pywrap/xaserver_zmq.hpp"

#include <iostream>

namespace xeus_pywrap
{
    std::unique_ptr<xeus::xserver> make_xaserver_zmq(xeus::xcontext& context,
                                              const xeus::xconfiguration& config,
                                              nl::json::error_handler_t eh)
    {
        return std::make_unique<xaserver_zmq>(context.get_wrapped_context<zmq::context_t>(), config, eh);
    }



    void xaserver_zmq::start_impl(xeus::xpub_message message)
    {
        std::cout<<"custom start impl"<<std::endl;
        this->start_publisher_thread();
        this->start_heartbeat_thread();
        m_request_stop = false;
        publish(std::move(message), xeus::channel::SHELL);

        // the polling is done externally, so
        // we don't need to do anything here and do
        // not block whatsoever
    }

    bool xaserver_zmq::stopped(){
        return this->m_request_stop;
    }

    void xaserver_zmq::poll(long timeout){
        try{
            xeus::xserver_zmq::poll(timeout);
        }
        catch(std::exception & e){
            std::cout<<"ex "<<e.what()<<std::endl;
        }
    }
}