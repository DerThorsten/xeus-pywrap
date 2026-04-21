/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#ifndef XEUS_PYWRAP_XASERVER_HPP
#define XEUS_PYWRAP_XASERVER_HPP



#include "xeus_pywrap_config.hpp"
#include "xeus/xkernel.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;
namespace nl = nlohmann;

namespace xeus_pywrap
{

    XEUS_PYWRAP_API XEUS_PYWRAP_FORCE_PYBIND11_EXPORT xeus::xkernel::server_builder make_async_server_factory(py::dict globals);

} // namespace xeus_pywrap

#endif // XEUS_PYWRAP_XASERVER_HPP