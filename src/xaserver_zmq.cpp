#include  "xeus-pywrap/xaserver_zmq.hpp"

#include <iostream>

namespace xeus_pywrap
{
    std::unique_ptr<xeus::xserver> make_xaserver_zmq(xeus::xcontext& context,
                                              const xeus::xconfiguration& config,
                                              nl::json::error_handler_t eh)
    {
        return std::make_unique<xaserver_zmq>(context, config, eh);
    }

    xaserver_zmq::xaserver_zmq(xeus::xcontext& context,
                               const xeus::xconfiguration& config,
                               nl::json::error_handler_t eh)
        : xeus::xserver_zmq(context, config, eh)
    {
    }

    void xaserver_zmq::poll(long timeout)
    {
        try
        {
            auto msg = xeus::xserver_zmq::poll_channels(timeout);
            if (msg)
            {
                if (msg.value().second == xeus::channel::SHELL)
                {
                    notify_shell_listener(std::move(msg.value().first));
                }
                else
                {
                    notify_control_listener(std::move(msg.value().first));
                }
            }
        }
        catch(std::exception & e)
        {
            std::cout << "ex " << e.what() << std::endl;
        }
    }

    bool xaserver_zmq::stopped() const
    {
        return is_stopped();
    }

    void xaserver_zmq::start_impl(xeus::xpub_message message)
    {
        std::cout<<"custom start impl"<<std::endl;
        start_publisher_thread();
        start_heartbeat_thread();
        set_request_stop(false);

        publish(std::move(message), xeus::channel::SHELL);

        // the polling is done externally, so
        // we don't need to do anything here and do
        // not block whatsoever
    }

    void xaserver_zmq::stop_impl()
    {
        set_request_stop(true);
        stop_channels();
    }
}
