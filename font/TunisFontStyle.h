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
#ifndef KTDFONTSTYLE_H
#define KTDFONTSTYLE_H

#include <string>

class TunisFontStyle
{
public:
    static const TunisFontStyle Invalid;

    static const TunisFontStyle Thin;
    static const TunisFontStyle ThinItalic;
    static const TunisFontStyle ExtraLight;
    static const TunisFontStyle ExtraLightItalic;
    static const TunisFontStyle Light;
    static const TunisFontStyle LightItalic;
    static const TunisFontStyle Regular;
    static const TunisFontStyle RegularItalic;
    static const TunisFontStyle Medium;
    static const TunisFontStyle MediumItalic;
    static const TunisFontStyle SemiBold;
    static const TunisFontStyle SemiBoldItalic;
    static const TunisFontStyle Bold;
    static const TunisFontStyle BoldItalic;
    static const TunisFontStyle ExtraBold;
    static const TunisFontStyle ExtraBoldItalic;
    static const TunisFontStyle Black;
    static const TunisFontStyle BlackItalic;

    static const TunisFontStyle &GetStyleByName(const std::string &name);

    inline bool operator< (const TunisFontStyle &other) {return m_id <  other.m_id;}
    inline bool operator<=(const TunisFontStyle &other) {return m_id <= other.m_id;}
    inline bool operator> (const TunisFontStyle &other) {return m_id >  other.m_id;}
    inline bool operator>=(const TunisFontStyle &other) {return m_id >= other.m_id;}
    inline bool operator==(const TunisFontStyle &other) {return m_id == other.m_id;}
    inline bool operator!=(const TunisFontStyle &other) {return m_id != other.m_id;}

    inline int getId() const {return m_id;}
    inline const std::string &getName() const {return m_name;}
    inline const std::string &getAlternateName() const {return m_alternateName;}
    inline int getThickness() const {return m_thickness;}
    inline bool isItalic() const {return m_italic;}
private:
    TunisFontStyle(int id, const std::string &name, const std::string &alternateName, int thickness, bool italic);
    int m_id;
    std::string m_name;
    std::string m_alternateName;
    int m_thickness;
    bool m_italic;
};

#endif // KTDFONTSTYLE_H
