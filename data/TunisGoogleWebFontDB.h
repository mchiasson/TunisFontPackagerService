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
#ifndef KTDGOOGLEWEBFONTDB_H
#define KTDGOOGLEWEBFONTDB_H

#include <string>
#include <vector>
#include <map>

#include <font/TunisFontStyle.h>

struct WebFont
{
    std::string family;
    std::string category;
    std::string version;
    std::string lastModified;
    std::vector<std::string> subsets;
    std::vector<std::string> variants;
    std::map<std::string, std::string> files;
};

class TunisGoogleWebFontDB
{
public:

    static std::string getJSONDatabase();
    static std::string findFontURL(const std::string &family, const std::string &style, const std::string &version);
    static std::vector<WebFont> getFontList();

};

#endif // KTDGOOGLEWEBFONTDB_H
