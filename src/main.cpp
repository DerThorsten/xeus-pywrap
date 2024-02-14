/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/



#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#ifdef __GNUC__
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#include <fstream>

#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus-zmq/xserver_zmq.hpp"


#include "xeus-pywrap/xinterpreter.hpp"
#include "xeus-pywrap/xeus_pywrap_config.hpp"
#include "xeus-pywrap/xaserver_zmq.hpp"

#include "pybind11/embed.h"



namespace py = pybind11;

namespace xeus_pywrap
{
    void export_pywrap(py::module& m);
}

PYBIND11_EMBEDDED_MODULE(_xpywrap, m) {
    xeus_pywrap::export_pywrap(m);
}




#ifdef __GNUC__
void handler(int sig)
{
    void* array[10];

    // get void*'s for all entries on the stack
    size_t size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

bool should_print_version(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--version")
        {
            return true;
        }
    }
    return false;
}

std::string extract_filename(int argc, char* argv[])
{
    std::string res = "";
    for (int i = 0; i < argc; ++i)
    {
        if ((std::string(argv[i]) == "-f") && (i + 1 < argc))
        {
            res = argv[i + 1];
            for (int j = i; j < argc - 2; ++j)
            {
                argv[j] = argv[j + 2];
            }
            argc -= 2;
            break;
        }
    }
    return res;
}


std::string extract_parameter(std::string param, int argc, char* argv[], bool mandatory)
{
    std::string res = "";
    for (int i = 0; i < argc; ++i)
    {
        if ((std::string(argv[i]) == param) && (i + 1 < argc))
        {
            res = argv[i + 1];
            return res;
        }
    }
    if(mandatory)
    {
        throw std::runtime_error("No parameter provided for " + param);
    }
    return res;
}




int main(int argc, char* argv[])
{
    if (should_print_version(argc, argv))
    {
        std::clog << "xpywrap " << XEUS_PYWRAP_VERSION  << std::endl;
        return 0;
    }

    // If we are called from the Jupyter launcher, silence all logging. This
    // is important for a JupyterHub configured with cleanup_servers = False:
    // Upon restart, spawned single-user servers keep running but without the
    // std* streams. When a user then tries to start a new kernel, xpywrap
    // will get a SIGPIPE and exit.
    if (std::getenv("JPY_PARENT_PID") != NULL)
    {
        std::clog.setstate(std::ios_base::failbit);
    }

    // Registering SIGSEGV handler
#ifdef __GNUC__
    std::clog << "registering handler for SIGSEGV" << std::endl;
    signal(SIGSEGV, handler);
#endif

    auto context = xeus::make_context<zmq::context_t>();

    // Instantiating the xeus xinterpreter
    using interpreter_ptr = std::unique_ptr<xeus_pywrap::interpreter>;


    std::string module_name = extract_parameter("--module", argc, argv, true /*mandatory*/);
    std::string factory_name = extract_parameter("--factory", argc, argv, true /*mandatory*/);
    std::string connection_filename = extract_parameter("--connection", argc, argv, false /*mandatory*/);

    // make vector of string from argv
    std::vector<std::string> args;
    for(int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    nl::json interpreter_config = {
        {"py_module", module_name},
        {"py_factory", factory_name},
        {"py_args", args}
    };

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    interpreter_ptr interpreter = interpreter_ptr(new xeus_pywrap::interpreter(std::move(interpreter_config)));


    std::cout<<"the conection filename is: "<<connection_filename<<std::endl;

    std::unique_ptr<xeus::xkernel> kernel;

    if (!connection_filename.empty())
    {

        xeus::xconfiguration config = xeus::load_configuration(connection_filename);
 
        kernel.reset(new xeus::xkernel(config,
                                       xeus::get_user_name(),
                                       std::move(context),
                                       std::move(interpreter),
                                       xeus_pywrap::make_xaserver_zmq));

        std::cout <<
            "Starting xpywrap kernel...\n\n"
            "If you want to connect to this kernel from an other client, you can use"
            " the " + connection_filename + " file."
            << std::endl;


    }
    else
    {

        
        kernel.reset(new xeus::xkernel(xeus::get_user_name(),
                             std::move(context),
                             std::move(interpreter),
                             xeus_pywrap::make_xaserver_zmq));

        const auto& config = kernel->get_config();

        // dump connection file / save nlohmann::json config to file

        std::ofstream file("kernel.json");

        // write config to to json
        nl::json j_config;
        j_config["transport"] = config.m_transport;
        j_config["ip"] = config.m_ip;
        


        // same put ports as ints   
        j_config["control_port"] = std::stoi(config.m_control_port);
        j_config["shell_port"] = std::stoi(config.m_shell_port);
        j_config["stdin_port"] = std::stoi(config.m_stdin_port);
        j_config["iopub_port"] = std::stoi(config.m_iopub_port);

        j_config["signature_scheme"] = config.m_signature_scheme;
        j_config["key"] = config.m_key;

        file << j_config.dump(4) << std::endl;   
        file.close();
    
    }
    std::cout<<"lift-off"<<std::endl;
    kernel->start();

    auto & server = kernel->get_server();

    // get the python object
    auto py_server = py::cast(server, py::return_value_policy::reference);


    // import xpywrap
    auto xpywrap_module = py::module_::import("xpywrap");


    std::cout<<"kernel started"<<std::endl;
    std::cout << "custom event loop" << std::endl;

    auto runner = xpywrap_module.attr("runner");

    std::cout<<"run the runner"<<std::endl;
    try{
        runner(py_server);
    }
    catch(std::exception & e){
        std::cout<<"caught"<<e.what()<<std::endl;
    }



    return 0;
}
