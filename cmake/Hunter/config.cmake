##
# MIT License
#
# Copyright (c) 2015-2018 Mathieu-Andre Chiasson
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

hunter_config(msdfgen
    URL https://github.com/mchiasson/msdfgen/archive/v1.5-p0.tar.gz
    SHA1 6ada9bd6e4859753d6683c8afe8eb576c9e265ad
)

hunter_config(PocoCpp 
    VERSION
        1.7.9-p1
    CMAKE_ARGS
        ENABLE_NETSSL=ON
        ENABLE_CRYPTO=ON
        ENABLE_PAGECOMPILER=ON
        ENABLE_PAGECOMPILER_FILE2PAGE=ON
)
