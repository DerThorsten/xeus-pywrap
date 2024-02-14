#############################################################################
#Copyright (c) 2024,
#
#Distributed under the terms of the BSD 3-Clause License.
#
#The full license is in the file LICENSE, distributed with this software.
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-pywrap)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(XEUS_PYWRAP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_PYWRAP_INCLUDE_DIR}/xeus-pywrap/xeus_pywrap_config.hpp" version_defines
     REGEX "#define XEUS_PYWRAP_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define XEUS_PYWRAP_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_PYWRAP_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XEUS_PYWRAP_VERSION_MAJOR}.${XEUS_PYWRAP_VERSION_MINOR}.${XEUS_PYWRAP_VERSION_PATCH})
message(STATUS "Building xeus-pywrap v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XEUS_PYWRAP_KERNELSPEC_PATH)
    set(XEUS_PYWRAP_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xpywrap/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xpywrap/kernel.json"
)

# Build options
# =============

option(XEUS_PYWRAP_BUILD_STATIC "Build xeus-pywrap static library" ON)
OPTION(XEUS_PYWRAP_BUILD_SHARED "Split xpywrap build into executable and library" ON)
OPTION(XEUS_PYWRAP_BUILD_EXECUTABLE "Build the xpywrap executable" ON)

OPTION(XEUS_PYWRAP_USE_SHARED_XEUS "Link xpywrap  with the xeus shared library (instead of the static library)" ON)
OPTION(XEUS_PYWRAP_USE_SHARED_XEUS_PYWRAP "Link xpywrap  with the xeus shared library (instead of the static library)" ON)


if(EMSCRIPTEN)
    add_compile_definitions(XEUS_PYWRAP_EMSCRIPTEN_WASM_BUILD)
    message("Build with emscripten")
    SET(XEUS_PYWRAP_BUILD_STATIC ON)
    SET(XEUS_PYWRAP_BUILD_SHARED OFF)
    SET(XEUS_PYWRAP_BUILD_EXECUTABLE OFF)
    SET(XEUS_PYWRAP_USE_SHARED_XEUS OFF)
    SET(XEUS_PYWRAP_USE_SHARED_XEUS_PYWRAP OFF)
    SET(XEUS_PYWRAP_BUILD_TESTS OFF)
endif()


# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xeus_REQUIRED_VERSION 3.0.5)


find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
find_package(pybind11 ${pybind11_REQUIRED_VERSION} REQUIRED)
find_package(pybind11_json ${pybind11_json_REQUIRED_VERSION} REQUIRED)
find_package(PythonInterp ${PythonLibsNew_FIND_VERSION} REQUIRED)

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "\n${CONTENTS}")
endfunction()

if(EMSCRIPTEN)
    find_package(xeus-lite REQUIRED)
    find_package(pyjs ${pyjs_REQUIRED_VERSION} REQUIRED)
    configure_file(${pyjs_PRE_JS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/pre.js COPYONLY)
    cat(${pyjs_POST_JS_PATH}  post.js.in)
    configure_file( post.js.in  ${CMAKE_CURRENT_BINARY_DIR}/post.js COPYONLY)
endif()

# Flags
# =====
include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    

    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
    if (HAS_CPP_17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++17 support!")
    endif ()
endif ()

if(EMSCRIPTEN)
    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")
endif()


# Source files
# ============

set(XEUS_PYWRAP_HEADERS
    include/xeus-pywrap/xeus_pywrap_config.hpp
    include/xeus-pywrap/xinterpreter.hpp
)

set(XEUS_PYWRAP_SRC
    src/xinterpreter.cpp
    src/export_pywrap.cpp
)

if(NOT EMSCRIPTEN)
    list(APPEND XEUS_PYWRAP_SRC
        src/xaserver_zmq.cpp
    )
endif()

    
 set(XEUS_PYWRAP_MAIN_SRC
    src/main.cpp
)


# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xeus_pywrap_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # C++17 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
        if (HAS_CPP_17_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_17)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-pywrap requires C++17 support!")
        endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xpywrap)
macro(xeus_pywrap_set_kernel_options target_name)
    if (XEUS_PYWRAP_USE_SHARED_XEUS_PYWRAP)
        target_link_libraries(${target_name} PRIVATE xeus-pywrap)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-pywrap-static)
    endif()

    if(NOT EMSCRIPTEN)
        find_package(Threads)
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    
endmacro()

# Common macro for shared and static library
macro(xeus_pywrap_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_PYWRAP_SRC} ${XEUS_PYWRAP_HEADERS})
    xeus_pywrap_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_PYWRAP_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XEUS_PYWRAP_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_PYWRAP_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:${XEUS_PYWRAP_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (XEUS_PYWRAP_USE_SHARED_XEUS)
        set(XEUS_PYWRAP_XEUS_TARGET xeus)
    else ()
        set(XEUS_PYWRAP_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC ${XEUS_PYWRAP_XEUS_TARGET} 
        xtl pybind11::pybind11 pybind11_json
    )
    if (WIN32 OR CYGWIN)
        target_link_libraries(${target_name} PRIVATE ${PYTHON_LIBRARIES})
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()
    if(NOT EMSCRIPTEN)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    
endmacro()

# xeus-pywrap
# ===========

set(XEUS_PYWRAP_TARGETS "")

if (XEUS_PYWRAP_BUILD_SHARED)
    # Build libraries
    xeus_pywrap_create_target(xeus-pywrap SHARED xeus-pywrap)
    list(APPEND XEUS_PYWRAP_TARGETS xeus-pywrap)
    target_link_libraries(xeus-pywrap PRIVATE xeus-zmq)
endif ()

if (XEUS_PYWRAP_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xeus_pywrap_create_target(xeus-pywrap-static STATIC xeus-pywrap-static)
    else ()
        xeus_pywrap_create_target(xeus-pywrap-static STATIC xeus-pywrap)
    endif ()
    if(EMSCRIPTEN)
        target_compile_options(xeus-pywrap-static PRIVATE -fPIC)
    endif()
    list(APPEND XEUS_PYWRAP_TARGETS xeus-pywrap-static)
    target_link_libraries(xeus-pywrap-static PRIVATE xeus-zmq)
endif ()

# xpywrap
# =======
if (XEUS_PYWRAP_BUILD_EXECUTABLE)
    find_package(xeus-zmq 1.0.2 REQUIRED)
    add_executable(xpywrap ${XEUS_PYWRAP_MAIN_SRC})
    target_compile_features(xpywrap PRIVATE cxx_std_17)
    xeus_pywrap_set_common_options(xpywrap)
    xeus_pywrap_set_kernel_options(xpywrap)
    target_link_libraries(xpywrap PRIVATE xeus-zmq)
endif()

if(EMSCRIPTEN)
    SET(PYTHON_UTIL_LIBS
        $ENV{PREFIX}/lib/libbz2.a
        $ENV{PREFIX}/lib/libz.a
        $ENV{PREFIX}/lib/libsqlite3.a
        $ENV{PREFIX}/lib/libffi.a
    )
    find_package(zlib REQUIRED)

    include(WasmBuildOptions)
    find_package(xeus-lite REQUIRED)
    add_executable(xpywrap src/main_emscripten_kernel.cpp )
    target_link_libraries(xpywrap PRIVATE xeus-lite pyjs)
    target_compile_options(xpywrap PRIVATE -fPIC)
    target_link_libraries(xpywrap PRIVATE ${PYTHON_UTIL_LIBS}  )
    XEUS_PYWRAP_set_kernel_options(xpywrap)
    xeus_wasm_compile_options(xpywrap)
    xeus_wasm_link_options(xpywrap "web,worker")
    target_link_options(xpywrap
        PUBLIC "SHELL: -s LZ4=1"
        PUBLIC "SHELL: --post-js  ${CMAKE_CURRENT_BINARY_DIR}/post.js"
        PUBLIC "-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=\"['\$Browser', '\$ERRNO_CODES']\" "
    )
endif()

target_link_libraries(xpywrap PRIVATE pybind11::embed)


# Installation
# ============
include(CMakePackageConfigHelpers)

set(XEUS_PYWRAP_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-pywrapConfig.cmake")

# Install xeus-pywrap and xeus-pywrap-static
if (XEUS_PYWRAP_BUILD_SHARED)
    install(TARGETS ${XEUS_PYWRAP_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-pywrap)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xpywrap
if (XEUS_PYWRAP_BUILD_EXECUTABLE)
    install(TARGETS xpywrap
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (XEUS_PYWRAP_BUILD_EXECUTABLE OR EMSCRIPTEN)
    # Configuration and data directories for jupyter and xeus-pywrap
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xpywrap Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-pywrapConfig.cmake' for a build tree
set(XEUS_PYWRAP_CONFIG_CODE "####### Expanded from \@XEUS_PYWRAP_CONFIG_CODE\@ #######\n")
set(XEUS_PYWRAP_CONFIG_CODE "${XEUS_PYWRAP_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_PYWRAP_CONFIG_CODE "${XEUS_PYWRAP_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-pywrapConfig.cmake.in for an install tree
set(XEUS_PYWRAP_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_PYWRAP_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_PYWRAP_CMAKECONFIG_INSTALL_DIR})

if (XEUS_PYWRAP_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_PYWRAP_CMAKECONFIG_INSTALL_DIR})
endif ()

if(EMSCRIPTEN)
    install(TARGETS xpywrap
            ARCHIVE DESTINATION ${XEUS_PYWRAP_CMAKECONFIG_INSTALL_DIR})

    install(FILES
            "$<TARGET_FILE_DIR:xpywrap>/xpywrap.js"
            "$<TARGET_FILE_DIR:xpywrap>/xpywrap.wasm"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

# install pure python part
