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

# Function to handle C++ server pages code generation
# It basically calls the cpspc tool from the Poco libraries and generates
# the corresponding .cpp and .h file in the build directory
# Adapted from http://www.cmake.org/pipermail/cmake/2010-June/037733.html
function(preprocess_cpsp out_var)
  set(result)
  find_program(POCO_CPSPC_EXECUTABLE cpspc HINTS ${Poco_DIR}/../../../bin)
  foreach(file ${ARGN})
    get_filename_component(basename ${file} NAME_WE)
    set(cpsp "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    set(cpp  "${CMAKE_CURRENT_BINARY_DIR}/${basename}.cpp")
    set(h    "${CMAKE_CURRENT_BINARY_DIR}/${basename}.h")
    add_custom_command(OUTPUT ${cpp} ${h}
        COMMAND ${POCO_CPSPC_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR} ${cpsp}
        DEPENDS ${cpsp}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Preprocessing ${file}"
        VERBATIM
    )
    set_source_files_properties(${cpp} PROPERTIES GENERATED 1)
    set_source_files_properties(${h}   PROPERTIES GENERATED 1)
    list(APPEND result ${cpp} ${h})
  endforeach()
  set(${out_var} "${result}" PARENT_SCOPE)
endfunction()
