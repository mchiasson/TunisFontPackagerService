/*
 * MIT License
 *
 * Copyright (c) 2017-2018 Mathieu-Andre Chiasson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef KTDFONTWRITER_H
#define KTDFONTWRITER_H

#include <string>

class TunisFontStyle;

class TunisAtlas
{
public:
    TunisAtlas(const std::string &family, const std::string &style, int fontSize);

    inline const std::string &getAtlasPath() const { return atlasPath; }
    inline const std::string &getsdfAtlasFileName() const { return sdfAtlasFileName; }
    inline const std::string &getmsdfAtlasFileName() const { return msdfAtlasFileName; }
    inline const std::string &getmsdfaAtlasFileName() const { return msdfaAtlasFileName; }

    bool generate();

private:

    std::string family;
    const TunisFontStyle *pStyle;
    int fontSize;

    std::string atlasPath;
    std::string sdfAtlasFileName;
    std::string msdfAtlasFileName;
    std::string msdfaAtlasFileName;
};

#endif // KTDFONTWRITER_H
