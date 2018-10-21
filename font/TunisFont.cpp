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
#include "TunisFont.h"

TunisFont::TunisFont() :
    m_face(nullptr),
    m_style(TunisFontStyle::Invalid)
{
    FT_Init_FreeType(&m_library);
}

TunisFont::TunisFont(const TunisFont& other) :
    m_face(nullptr),
    m_style(TunisFontStyle::Invalid)
{
    *this = other;

    FT_Init_FreeType(&m_library);

    if (m_fileName.length() > 0)
    {
        FT_New_Face(m_library, m_fileName.c_str(), 0, &m_face);
    }
}

TunisFont::~TunisFont()
{
    if (m_face)
    {
        FT_Done_Face(m_face);
        m_face = nullptr;
    }

    FT_Done_FreeType(m_library);
    m_library = nullptr;
}

TunisFont &TunisFont::operator=(const TunisFont& other)
{
    if (m_face)
    {
        FT_Done_Face(m_face);
    }

    *this = other;
    if (m_fileName.length() > 0)
    {
        FT_New_Face(m_library, m_fileName.c_str(), 0, &m_face);
    }

    return *this;
}

void TunisFont::setFileName(const std::string &fileName)
{
    m_fileName = fileName;
    if (m_fileName.length() > 0)
    {
        FT_New_Face(m_library, m_fileName.c_str(), 0, &m_face);
    }
}
