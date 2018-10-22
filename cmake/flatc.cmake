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

function(flatc out_var)

    find_program(FLATC_EXECUTABLE flatc HINTS ${FLATBUFFERS_ROOT}/bin)

    foreach(file ${ARGN})
      get_filename_component(basename ${file} NAME_WE)
      set(schema "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
      set(h      "${CMAKE_CURRENT_BINARY_DIR}/${basename}_generated.h")
      add_custom_command(OUTPUT ${h}
          COMMAND ${FLATC_EXECUTABLE} --cpp --js ${schema}
          DEPENDS ${schema}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Preprocessing ${file}"
          VERBATIM
      )
      set_source_files_properties(${h} PROPERTIES GENERATED 1)
      list(APPEND result ${h})
    endforeach()

    set(${out_var} "${result}" PARENT_SCOPE)

endfunction()
