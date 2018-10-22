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
#ifndef KTDFONT_H
#define KTDFONT_H

#include "font/TunisFontStyle.h"
#include "font/TunisGlyph.h"

#include <Poco/String.h>

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

class TunisFont
{
    friend class TunisFontDB;

public:
    TunisFont();
    TunisFont(const TunisFont& other);
    ~TunisFont();
    TunisFont &operator=(const TunisFont& other);

    inline const std::string &getFamily() const {return m_family;}
    inline const TunisFontStyle &getStyle() const {return m_style;}
    inline const std::string &getFileName() const {return m_fileName;}
    inline const std::vector<TunisGlyph> &getGlyphs() const {return m_glyphs;}
    inline const FT_Face getFace() const { return m_face; }

private:

    void setFileName(const std::string &fileName);

    FT_Library m_library;
    FT_Face m_face;

    std::string  m_family;
    TunisFontStyle m_style;
    std::string  m_fileName;
    std::vector<TunisGlyph> m_glyphs;
};

#endif // KTDFONT_H
