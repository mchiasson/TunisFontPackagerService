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
if (NOT DEFINED resource_name)
    message(FATAL_ERROR "Missing -Dresource_name=...")
endif()
if (NOT DEFINED resource_dir)
    message(FATAL_ERROR "Missing -Dresource_dir=...")
endif()

if (NOT DEFINED destination_dir)
    message(FATAL_ERROR "Missing -Ddestination_dir=...")
endif()

file(WRITE ${destination_dir}/${resource_name}.c "")
file(WRITE ${destination_dir}/${resource_name}.h "")

string(TOUPPER ${resource_name} resource_name_upper)
file(APPEND ${destination_dir}/${resource_name}.c "#include \"${resource_name}.h\"\n")
file(APPEND ${destination_dir}/${resource_name}.h "#ifndef ${resource_name_upper}_H_\n#define ${resource_name_upper}_H_\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n")

file(GLOB_RECURSE resource_files FOLLOW_SYMLINKS ${resource_dir}/*)
foreach(resource_file ${resource_files})
    file(RELATIVE_PATH rel_resource_file ${resource_dir}/.. ${resource_file})
    string(MAKE_C_IDENTIFIER ${rel_resource_file} resource_id)
    file(READ ${resource_file} file_data HEX)
    string(LENGTH ${file_data} file_data_size)
    math(EXPR file_data_size "${file_data_size} / 2") # 1 byte in hex is represented with 2 characters, where in reality, it's 1 charcacter (1 byte).
    if (file_data_size GREATER 0)
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," file_data ${file_data})
        file(APPEND ${destination_dir}/${resource_name}.c "\nconst unsigned char ${resource_id}[${file_data_size}] = {${file_data}};")
        file(APPEND ${destination_dir}/${resource_name}.h "\nextern const unsigned char ${resource_id}[${file_data_size}];")
    endif()
endforeach()

file(APPEND ${destination_dir}/${resource_name}.c "\n")
file(APPEND ${destination_dir}/${resource_name}.h "\n\n#ifdef __cplusplus\n}\n#endif\n\n#endif /* ${resource_name_upper}_H_ */\n")
