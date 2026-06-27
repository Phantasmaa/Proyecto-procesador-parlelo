# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.16)
project(procesador_paralelo
        VERSION 2.0.0
        DESCRIPTION "Producer/consumer pipeline over System V IPC"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(PP_CORE_SOURCES
    lib/coder.cpp
    lib/func.cpp
    lib/ipc.cpp
)

set(PP_CORE_HEADERS
    lib/inc.h
    lib/coder.h
    lib/func.h
    lib/ipc.h
    data/data.h
)

add_library(pp_core STATIC ${PP_CORE_SOURCES} ${PP_CORE_HEADERS})
target_include_directories(pp_core PUBLIC ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/data)
target_link_libraries(pp_core PUBLIC Threads::Threads OpenSSL::Crypto)

add_executable(sender   src/sender.cpp)
target_link_libraries(sender   PRIVATE pp_core)

add_executable(receiver src/receiver.cpp)
target_link_libraries(receiver PRIVATE pp_core)

# Install the binaries and the seed data; logs stay inside the container.
install(TARGETS sender receiver RUNTIME DESTINATION bin)
install(DIRECTORY data DESTINATION share/procesador-paralelo)
