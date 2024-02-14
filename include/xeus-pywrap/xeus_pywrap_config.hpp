/***************************************************************************
* Copyright (c) 2024, Thorsten Beier                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#ifndef XEUS_PYWRAP_CONFIG_HPP
#define XEUS_PYWRAP_CONFIG_HPP

// Project version
#define XEUS_PYWRAP_VERSION_MAJOR 0
#define XEUS_PYWRAP_VERSION_MINOR 1
#define XEUS_PYWRAP_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define XEUS_PYWRAP_CONCATENATE(A, B) XEUS_PYWRAP_CONCATENATE_IMPL(A, B)
#define XEUS_PYWRAP_CONCATENATE_IMPL(A, B) A##B
#define XEUS_PYWRAP_STRINGIFY(a) XEUS_PYWRAP_STRINGIFY_IMPL(a)
#define XEUS_PYWRAP_STRINGIFY_IMPL(a) #a

#define XEUS_PYWRAP_VERSION XEUS_PYWRAP_STRINGIFY(XEUS_PYWRAP_CONCATENATE(XEUS_PYWRAP_VERSION_MAJOR,   \
                 XEUS_PYWRAP_CONCATENATE(.,XEUS_PYWRAP_CONCATENATE(XEUS_PYWRAP_VERSION_MINOR,   \
                                  XEUS_PYWRAP_CONCATENATE(.,XEUS_PYWRAP_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef XEUS_PYWRAP_EXPORTS
        #define XEUS_PYWRAP_API __declspec(dllexport)
    #else
        #define XEUS_PYWRAP_API __declspec(dllimport)
    #endif
#else
    #define XEUS_PYWRAP_API
#endif

#endif