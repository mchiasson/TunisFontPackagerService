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
#include "TunisFontStyle.h"

#include <Poco/String.h>

const TunisFontStyle TunisFontStyle::Invalid          = TunisFontStyle(0,  "Invalid",            "",            0, false);
const TunisFontStyle TunisFontStyle::Thin             = TunisFontStyle(1,  "Thin",               "100",       100, false);
const TunisFontStyle TunisFontStyle::ThinItalic       = TunisFontStyle(2,  "Thin Italic",        "100italic", 100, true);
const TunisFontStyle TunisFontStyle::ExtraLight       = TunisFontStyle(3,  "Extra-Light",        "200",       200, false);
const TunisFontStyle TunisFontStyle::ExtraLightItalic = TunisFontStyle(4,  "Extra-Light Italic", "200italic", 200, true);
const TunisFontStyle TunisFontStyle::Light            = TunisFontStyle(5,  "Light",              "300",       300, false);
const TunisFontStyle TunisFontStyle::LightItalic      = TunisFontStyle(6,  "Light Italic",       "300italic", 300, true);
const TunisFontStyle TunisFontStyle::Regular          = TunisFontStyle(7,  "Regular",            "regular",   400, false);
const TunisFontStyle TunisFontStyle::RegularItalic    = TunisFontStyle(8,  "Regular Italic",     "italic",    400, true);
const TunisFontStyle TunisFontStyle::Medium           = TunisFontStyle(9,  "Medium",             "500",       500, false);
const TunisFontStyle TunisFontStyle::MediumItalic     = TunisFontStyle(10, "Medium Italic",      "500italic", 500, true);
const TunisFontStyle TunisFontStyle::SemiBold         = TunisFontStyle(11, "Semi-Bold",          "600",       600, false);
const TunisFontStyle TunisFontStyle::SemiBoldItalic   = TunisFontStyle(12, "Semi-Bold Italic",   "600italic", 600, true);
const TunisFontStyle TunisFontStyle::Bold             = TunisFontStyle(13, "Bold",               "700",       700, false);
const TunisFontStyle TunisFontStyle::BoldItalic       = TunisFontStyle(14, "Bold Italic",        "700italic", 700, true);
const TunisFontStyle TunisFontStyle::ExtraBold        = TunisFontStyle(15, "Extra-Bold",         "800",       800, false);
const TunisFontStyle TunisFontStyle::ExtraBoldItalic  = TunisFontStyle(16, "Extra-Bold Italic",  "800italic", 800, true);
const TunisFontStyle TunisFontStyle::Black            = TunisFontStyle(17, "Black",              "900",       900, false);
const TunisFontStyle TunisFontStyle::BlackItalic      = TunisFontStyle(18, "Black Italic",       "900italic", 900, true);

const TunisFontStyle &TunisFontStyle::GetStyleByName(const std::string &name)
{
    if (Poco::icompare(name, Thin            .m_name) == 0 || Poco::icompare(name, Thin.getAlternateName()            ) == 0) return Thin            ;
    if (Poco::icompare(name, ThinItalic      .m_name) == 0 || Poco::icompare(name, ThinItalic.getAlternateName()      ) == 0) return ThinItalic      ;
    if (Poco::icompare(name, ExtraLight      .m_name) == 0 || Poco::icompare(name, ExtraLight.getAlternateName()      ) == 0) return ExtraLight      ;
    if (Poco::icompare(name, ExtraLightItalic.m_name) == 0 || Poco::icompare(name, ExtraLightItalic.getAlternateName()) == 0) return ExtraLightItalic;
    if (Poco::icompare(name, Light           .m_name) == 0 || Poco::icompare(name, Light.getAlternateName()           ) == 0) return Light           ;
    if (Poco::icompare(name, LightItalic     .m_name) == 0 || Poco::icompare(name, LightItalic.getAlternateName()     ) == 0) return LightItalic     ;
    if (Poco::icompare(name, Regular         .m_name) == 0 || Poco::icompare(name, Regular.getAlternateName()         ) == 0) return Regular         ;
    if (Poco::icompare(name, RegularItalic   .m_name) == 0 || Poco::icompare(name, RegularItalic.getAlternateName()   ) == 0) return RegularItalic   ;
    if (Poco::icompare(name, Medium          .m_name) == 0 || Poco::icompare(name, Medium.getAlternateName()          ) == 0) return Medium          ;
    if (Poco::icompare(name, MediumItalic    .m_name) == 0 || Poco::icompare(name, MediumItalic.getAlternateName()    ) == 0) return MediumItalic    ;
    if (Poco::icompare(name, SemiBold        .m_name) == 0 || Poco::icompare(name, SemiBold.getAlternateName()        ) == 0) return SemiBold        ;
    if (Poco::icompare(name, SemiBoldItalic  .m_name) == 0 || Poco::icompare(name, SemiBoldItalic.getAlternateName()  ) == 0) return SemiBoldItalic  ;
    if (Poco::icompare(name, Bold            .m_name) == 0 || Poco::icompare(name, Bold.getAlternateName()            ) == 0) return Bold            ;
    if (Poco::icompare(name, BoldItalic      .m_name) == 0 || Poco::icompare(name, BoldItalic.getAlternateName()      ) == 0) return BoldItalic      ;
    if (Poco::icompare(name, ExtraBold       .m_name) == 0 || Poco::icompare(name, ExtraBold.getAlternateName()       ) == 0) return ExtraBold       ;
    if (Poco::icompare(name, ExtraBoldItalic .m_name) == 0 || Poco::icompare(name, ExtraBoldItalic.getAlternateName() ) == 0) return ExtraBoldItalic ;
    if (Poco::icompare(name, Black           .m_name) == 0 || Poco::icompare(name, Black.getAlternateName()           ) == 0) return Black           ;
    if (Poco::icompare(name, BlackItalic     .m_name) == 0 || Poco::icompare(name, BlackItalic.getAlternateName()     ) == 0) return BlackItalic     ;

    return Invalid;
}

TunisFontStyle::TunisFontStyle(int id, const std::string &name, const std::string &alternateName, int thickness, bool italic) :
    m_id(id),
    m_name(name),
    m_alternateName(alternateName),
    m_thickness(thickness),
    m_italic(italic)
{
}
