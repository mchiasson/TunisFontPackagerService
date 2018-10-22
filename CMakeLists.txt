##
# MIT License
#
# Copyright (c) 2017-2018 Mathieu-Andre Chiasson
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
hunter_add_package(PNG)
hunter_add_package(flatbuffers)

find_package(msdfgen CONFIG REQUIRED)
find_package(Poco REQUIRED NetSSL CONFIG)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(PNG CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)
find_package(OpenMP)

list(APPEND deps
    msdfgen::lib_msdfgen
    Poco::Net
    Poco::NetSSL
    OpenSSL::SSL
    OpenSSL::Crypto
    RapidJSON::rapidjson
    PNG::png
    flatbuffers::flatbuffers
)

if (TARGET OpenMP::OpenMP_CXX)
    list(APPEND deps OpenMP::OpenMP_CXX)
endif()

##
# Compile .cpsp files
##
file(GLOB PAGES_IN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "http/views/*.cpsp")
include(cpsp)
cpsp(PAGES_OUT ${PAGES_IN})

##
# Compile .fbs
##
file(GLOB FBS_IN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "schema/*.fbs")
include(flatc)
flatc(FBS_OUT ${FBS_IN})
add_custom_command(
    OUTPUT  ${PROJECT_BINARY_DIR}/TunisSchema.c
            ${PROJECT_BINARY_DIR}/TunisSchema.h
    COMMAND ${CMAKE_COMMAND}
            -Dresource_name=TunisSchema
            -Dresource_dir=${PROJECT_BINARY_DIR}/schema
            -Ddestination_dir=${PROJECT_BINARY_DIR}
            -P ${PROJECT_SOURCE_DIR}/cmake/RCUtilCommand.cmake
    DEPENDS ${FBS_OUT}
)
set_source_files_properties(${PROJECT_BINARY_DIR}/TunisSchema.c PROPERTIES GENERATED TRUE)
set_source_files_properties(${PROJECT_BINARY_DIR}/TunisSchema.h PROPERTIES GENERATED TRUE)


##
# Generate embedded resources
##
file(GLOB_RECURSE HTTP_PUBLIC_FILES http/public/*)
add_custom_command(
    OUTPUT  ${PROJECT_BINARY_DIR}/TunisRC.c
            ${PROJECT_BINARY_DIR}/TunisRC.h
    COMMAND ${CMAKE_COMMAND}
            -Dresource_name=TunisRC
            -Dresource_dir=${PROJECT_SOURCE_DIR}/http/public
            -Ddestination_dir=${PROJECT_BINARY_DIR}
            -P ${PROJECT_SOURCE_DIR}/cmake/RCUtilCommand.cmake
    DEPENDS ${HTTP_PUBLIC_FILES}
)
set_source_files_properties(${PROJECT_BINARY_DIR}/TunisRC.c PROPERTIES GENERATED TRUE)
set_source_files_properties(${PROJECT_BINARY_DIR}/TunisRC.h PROPERTIES GENERATED TRUE)

add_executable(${PROJECT_NAME}
    main.cpp
    data/TunisFontDB.cpp
    data/TunisFontDB.h
    data/TunisGoogleWebFontDB.cpp
    data/TunisGoogleWebFontDB.h
    font/TunisAtlas.cpp
    font/TunisAtlas.h
    font/TunisFont.cpp
    font/TunisFont.h
    font/TunisFontStyle.cpp
    font/TunisFontStyle.h
    font/TunisGlyph.h
    http/TunisHTTPAtlasRequestHandler.cpp
    http/TunisHTTPAtlasRequestHandler.h
    http/TunisHTTPDefaultRequestHandler.cpp
    http/TunisHTTPDefaultRequestHandler.h
    http/TunisHTTPDownloadHelper.cpp
    http/TunisHTTPDownloadHelper.h
    http/TunisHTTPFontRequestHandler.cpp
    http/TunisHTTPFontRequestHandler.h
    http/TunisHTTPGlyphAnalyzerRequestHandler.cpp
    http/TunisHTTPGlyphAnalyzerRequestHandler.h
    http/TunisHTTPGlyphRequestHandler.cpp
    http/TunisHTTPGlyphRequestHandler.h
    http/TunisHTTPRequestHandler.cpp
    http/TunisHTTPRequestHandlerFactory.cpp
    http/TunisHTTPRequestHandlerFactory.h
    http/TunisHTTPRequestHandler.h
    http/TunisHTTPRootRequestHandler.cpp
    http/TunisHTTPRootRequestHandler.h
    util/TunisPNG.cpp
    util/TunisPNG.h

    ##
    # Dynamic Web views
    ##
    ${PAGES_IN}
    ${PAGES_OUT}

    ##
    # Generated Flatbuffers schema
    ##
    ${FBS_IN}
    ${FBS_OUT}
    ${PROJECT_BINARY_DIR}/TunisSchema.c
    ${PROJECT_BINARY_DIR}/TunisSchema.h

    # Embedded HTTP public files
    ${HTTP_PUBLIC_FILES}
    ${PROJECT_BINARY_DIR}/TunisRC.c
    ${PROJECT_BINARY_DIR}/TunisRC.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${deps})
