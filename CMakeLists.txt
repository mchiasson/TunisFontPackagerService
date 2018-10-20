##
# MIT License
#
# Copyright (c) 2018 Mathieu-Andre Chiasson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##
cmake_minimum_required(VERSION 3.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.34.tar.gz"
    SHA1 "70287b1ffa810ee4e952052a9adff9b4856d0d54"
    LOCAL
)

project(TunisFontPackagerService VERSION 0.1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

hunter_add_package(msdfgen)
hunter_add_package(PocoCpp)
hunter_add_package(OpenSSL)
hunter_add_package(RapidJSON)

find_package(msdfgen CONFIG REQUIRED)
find_package(Poco REQUIRED NetSSL CONFIG)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenMP)

list(APPEND deps
    msdfgen::lib_msdfgen
    Poco::Net
    Poco::NetSSL
    OpenSSL::SSL
    OpenSSL::Crypto
    RapidJSON::rapidjson
)

if (TARGET OpenMP::OpenMP_CXX)
    list(APPEND deps OpenMP::OpenMP_CXX)
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    TunisHTTPRequestHandlerFactory.cpp
    TunisHTTPRequestHandlerFactory.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${deps})
