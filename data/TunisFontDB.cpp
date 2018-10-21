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
#include "data/TunisFontDB.h"
#include "data/TunisGoogleWebFontDB.h"
#include "http/TunisHTTPDownloadHelper.h"
#include "util/TunisPNG.h"

#include <Poco/LogStream.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include <Poco/String.h>

#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>


#include <fstream>
#include <sstream>
#include <iomanip>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

struct FtContext {
    msdfgen::Point2 position;
    msdfgen::Shape shape;
    msdfgen::Contour *contour;
};

static msdfgen::Point2 ftPoint2(const FT_Vector &vector) {
    return msdfgen::Point2(vector.x/64., vector.y/64.);
}

static int ftMoveTo(const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour = &context->shape.addContour();
    context->position = ftPoint2(*to);
    return 0;
}

static int ftLineTo(const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour->addEdge(new msdfgen::LinearSegment(context->position, ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

static int ftConicTo(const FT_Vector *control, const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour->addEdge(new msdfgen::QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

static int ftCubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour->addEdge(new msdfgen::CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

bool TunisFontDB::loadFont(TunisFont &font, const std::string &family, const TunisFontStyle &style, const std::string &version)
{
    Poco::LogStream log(Poco::Logger::get("TunisFontDB::loadFont"));

    std::string url = TunisGoogleWebFontDB::findFontURL(family, style.getAlternateName(), version);
    if (url.length() > 0)
    {
        log.notice() << "Downloading " << family << "-" << style.getName() << " at " << url << std::endl;
        std::string path =
                Poco::remove(Poco::toLower(family), ' ')
                + Poco::Path::separator()
                + version
                + Poco::Path::separator()
                + style.getAlternateName()
                + Poco::Path::separator();

        Poco::File tmpDir(path);
        tmpDir.createDirectories();

        std::string fileName = path + "font.ttf";
        std::ofstream ofs (fileName, std::ofstream::out|std::ofstream::binary);
        bool bDownloadSuccess = TunisHTTPDownloadHelper::download(url, ofs);
        ofs.close();

        if (bDownloadSuccess)
        {
            font.m_version = version;
            font.m_family = family;
            font.m_style = style;
            font.setFileName(fileName);
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

bool TunisFontDB::loadGlyph(TunisFont &font, int unicodes[], size_t unicodeCount, int fontSize)
{
    Poco::LogStream log(Poco::Logger::get("TunisFontDB::loadGlyph"));

    for (size_t uid = 0; uid < unicodeCount; ++uid)
    {
        // if it is a non-printable character, default to use to null glyph (to save memory)
        FT_UInt charIndex = FT_Get_Char_Index(font.m_face, unicodes[uid]);
        if (charIndex == 0)
        {
            unicodes[uid] = 0;
        }

        for(size_t i = 0; i < font.m_glyphs.size(); ++i)
        {
            TunisGlyph &candidate = font.m_glyphs[i];
            if (candidate.unicode == unicodes[uid] && candidate.fontSize == fontSize)
            {
                throw -999;
            }
        }

        FT_Error error = FT_Set_Pixel_Sizes(font.m_face, 0, fontSize);
        if (error != 0)
        {
            throw error;
        }

        error = FT_Load_Char(font.m_face, unicodes[uid],  FT_LOAD_RENDER);
        if (error != 0)
        {
            throw error;
        }

        std::string path =
                Poco::remove(Poco::toLower(font.m_family), ' ')
                + Poco::Path::separator()
                + font.m_version
                + Poco::Path::separator()
                + font.m_style.getAlternateName()
                + Poco::Path::separator()
                + std::to_string(fontSize)
                + Poco::Path::separator();

        std::string sdfPath(path + "sdf" + Poco::Path::separator());
        std::string msdfPath(path + "msdf" + Poco::Path::separator());
        std::string msdfaPath(path + "msdfa" + Poco::Path::separator());

        Poco::File(sdfPath).createDirectories();
        Poco::File(msdfPath).createDirectories();
        Poco::File(msdfaPath).createDirectories();

        std::stringstream unicodeHex;
        unicodeHex << std::setfill('0') << std::setw(4) << std::hex << unicodes[uid];
        std::string unicodeHexStr = unicodeHex.str();

        std::string sdfFileName = sdfPath + unicodeHex.str() + ".png";
        std::string msdfFileName = msdfPath + unicodeHex.str() + ".png";
        std::string msdfaFileName = msdfaPath + unicodeHex.str() + ".png";

        TunisGlyph glyph;
        glyph.unicode        = unicodes[uid];
        glyph.fontSize       = fontSize;
        glyph.width          = font.m_face->glyph->bitmap.width;
        glyph.height         = font.m_face->glyph->bitmap.rows;
        glyph.padding        = (int)((double)glyph.fontSize / 8.0 + 0.5);
        glyph.texture_width  = glyph.width > 0 ? glyph.width + glyph.padding*2 : 0;
        glyph.texture_height = glyph.height > 0 ? glyph.height + glyph.padding*2 : 0;
        glyph.xadvance       = (double)font.m_face->glyph->advance.x/64.0;
        glyph.yadvance       = (double)font.m_face->glyph->advance.y/64.0;
        glyph.xoffset        = (double)font.m_face->glyph->metrics.horiBearingX/64.0;
        glyph.yoffset        = (double)glyph.height - (double)font.m_face->glyph->metrics.horiBearingY/64.0;

        if (glyph.texture_width > 0 && glyph.texture_height > 0)
        {
            glyph.sdfFileName    = sdfFileName;
            glyph.msdfFileName   = msdfFileName;
            glyph.msdfaFileName  = msdfaFileName;

            FtContext context = { };
            FT_Outline_Funcs ftFunctions;
            ftFunctions.move_to = &ftMoveTo;
            ftFunctions.line_to = &ftLineTo;
            ftFunctions.conic_to = &ftConicTo;
            ftFunctions.cubic_to = &ftCubicTo;
            ftFunctions.shift = 0;
            ftFunctions.delta = 0;
            error = FT_Outline_Decompose(&font.m_face->glyph->outline, &ftFunctions, &context);
            if (error)
            {
                throw -998;
            }

            if (!context.shape.validate())
            {
                throw -997;
            }

            context.shape.normalize();
            edgeColoringSimple(context.shape, 3.13);

            msdfgen::Bitmap<float> sdf(glyph.texture_width, glyph.texture_height);
            msdfgen::Bitmap<msdfgen::FloatRGB> msdf(glyph.texture_width, glyph.texture_height);

            msdfgen::Vector2 frame(glyph.texture_width, glyph.texture_height);
            int glyph_left             = font.m_face->glyph->bitmap_left;
            int glyph_right            = glyph_left + glyph.width;
            int glyph_top              = font.m_face->glyph->bitmap_top;
            int glyph_bottom           = glyph_top - glyph.height;

            double l = glyph_left - (glyph.padding);
            double b = glyph_bottom - (glyph.padding);
            double r = glyph_right + (glyph.padding);
            double t = glyph_top + (glyph.padding);

            if (l >= r || b >= t)
            {
                l = 0;
                b = 0;
                r = 1;
                t = 1;
            }

            msdfgen::Vector2 dims(r-l, t-b);
            msdfgen::Vector2 scale = 1.0;
            msdfgen::Vector2 translate = 0.0;
            if (dims.x*frame.y < dims.y*frame.x)
            {
                translate.set(0.5*(frame.x/frame.y*dims.y-dims.x)-l, -b);
                scale = frame.y/dims.y;
            }
            else
            {
                translate.set(-l, 0.5*(frame.y/frame.x*dims.x-dims.y)-b);
                scale = frame.x/dims.x;
            }

            generateSDF(sdf, context.shape, glyph.padding, scale, translate);
            generateMSDF(msdf, context.shape, glyph.padding, scale, translate);

            TunisPNG msdfaPng(glyph.texture_width, glyph.texture_height, TunisPNG::RGBA);
            TunisPNG::ColorRGBA *msdfa = msdfaPng.getBufferAsRGBA();
            for (int y = 0; y < glyph.texture_height; ++y)
            {
                for(int x = 0; x < glyph.texture_width; ++x)
                {
                    msdfa[x + (glyph.texture_height - 1 - y)*glyph.texture_width] =
                    {
                            static_cast<uint8_t>(std::max(0, std::min(255, int(msdf(x, y).r * 256.0f + 0.5f)))),
                            static_cast<uint8_t>(std::max(0, std::min(255, int(msdf(x, y).g * 256.0f + 0.5f)))),
                            static_cast<uint8_t>(std::max(0, std::min(255, int(msdf(x, y).b * 256.0f + 0.5f)))),
                            static_cast<uint8_t>(std::max(0, std::min(255, int(sdf(x, y) * 256.0f + 0.5f)))),
                    };
                }
            }

            msdfgen::savePng(sdf, glyph.sdfFileName.c_str());
            msdfgen::savePng(msdf, glyph.msdfFileName.c_str());
            msdfaPng.store(glyph.msdfaFileName);
        }

        font.m_glyphs.push_back(glyph);
    }

    return font.getGlyphs().size() > 0;
}

