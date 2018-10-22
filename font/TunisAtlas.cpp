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
#include "TunisAtlas.h"
#include "data/TunisFontDB.h"
#include "font/TunisFont.h"
#include "util/TunisPNG.h"


#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>

#include <Poco/File.h>
#include <Poco/LogStream.h>
#include <Poco/Path.h>
#include <Poco/String.h>

#include <cmath>

#include "TunisFonts_generated.h"


const char HEADER[] = {'K', 'F', 'N', 'T'};


static bool glyphSortTest(TunisGlyph lhs, TunisGlyph rhs)
{
    // order from widest to narrowest.
    return lhs.texture_width > rhs.texture_height;
}

static size_t getNearestPowerOfTwo(size_t value)
{
    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
#if UINTPTR_MAX == 0xffffffffffffffff
    value |= value >> 32;
#endif
    return ++value;
}

TunisAtlas::TunisAtlas(const std::string &family, const std::string &style, const std::string &version, int fontSize) :
    family(family),
    pStyle(&TunisFontStyle::GetStyleByName(style)),
    version(version),
    fontSize(fontSize)

{
    atlasPath =
            Poco::remove(Poco::toLower(family), ' ')
            + Poco::Path::separator()
            + version
            + Poco::Path::separator()
            + pStyle->getAlternateName()
            + Poco::Path::separator()
            + std::to_string(fontSize)
            + Poco::Path::separator();

    sdfAtlasFileName = atlasPath + "sdf.png";
    msdfAtlasFileName = atlasPath + "msdf.png";
    msdfaAtlasFileName = atlasPath + "msdfa.png";
}

bool TunisAtlas::generate()
{
    Poco::LogStream log(Poco::Logger::get("font"));

    std::vector<TunisGlyph> glyphs;
    int atlasWidth = 0;
    int atlasHeight = 0;
    uint16_t padding = 0;
    int margin = 0;

    TunisFont font;
    if (!TunisFontDB::loadFont(font, family, *pStyle, version))
    {
        return false;
    }

    int unicodes[] = {
        // 0020-007F Basic Latin
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        // 00A0-00FF Latin-1 Supplement
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
    };

    if (!TunisFontDB::loadGlyph(font, unicodes, sizeof(unicodes)/sizeof(int), fontSize))
    {
        return false;
    }

    glyphs = font.getGlyphs();
    std::sort(glyphs.begin(), glyphs.end(), glyphSortTest);

    size_t estimatedTotalArea = 0;
    for (size_t i = 0; i < glyphs.size(); ++i)
    {
        const TunisGlyph &glyph = glyphs[i];
        estimatedTotalArea += (glyph.texture_width + (glyph.padding)) * (glyph.texture_height + (glyph.padding)); // adding extra padding (see my diagram in the comment block below...)
    }

    atlasWidth = atlasHeight = getNearestPowerOfTwo((size_t)ceil(sqrt(estimatedTotalArea * 1.05))); // adding 5% packing error margin
    log.notice() << "Optimal power-of-two texture size : " << atlasWidth << " x " << atlasHeight << std::endl;

    std::vector<uint8_t> atlasBuffer;
    atlasBuffer.resize(atlasWidth*atlasHeight*sizeof(TunisPNG::ColorRGBA));
    TunisPNG::ColorRGBA *atlas = (TunisPNG::ColorRGBA *)&atlasBuffer.front();

    std::vector<bool> occupancyMap;
    occupancyMap.resize(atlasWidth*atlasHeight*sizeof(bool));

    log.notice() << "Generating " << msdfaAtlasFileName << "...";
    for(size_t i = 0; i < glyphs.size(); ++i)
    {
        TunisGlyph &glyph = glyphs[i];

        if (glyph.width == 0 || glyph.height == 0)
        {
            continue;
        }

        TunisPNG msdfaPNG;
        msdfaPNG.load(glyph.msdfaFileName);
        const TunisPNG::ColorRGBA *msdfa = (const TunisPNG::ColorRGBA *)&msdfaPNG.getBuffer().front();

        // We already have padding for the SDF/MSDF range, but we also need
        // and outter padding like this for the atlas:

        // +--------------------------------------+
        // |                                      |
        // |               margin                 |
        // |                                      |
        // |      +------------------------+      |
        // |      |                        |      |
        // |      |        padding         |      |
        // |      |                        |      |
        // |      |      +----------+      |      |
        // |      |      |          |      |      |
        // |      |      |  Glyph   |      |      |
        // |      |      |          |      |      |
        // |      |      |          |      |      |
        // |      |      +----------+      |      |
        // |      |                        |      |
        // |      |                        |      |
        // |      |                        |      |
        // |      +------------------------+      |
        // |                                      |
        // |                                      |
        // |                                      |
        // +--------------------------------------+

        // Otherwise, some colors from a given MSDF glyph will interract
        // with the colors from a neighbouring MSDF glyph, creating unwanted
        // thin vertical or horizontal lines on the edges of the glyph.  We
        // could render the glyph in a tight box if we wanted to, but we
        // need a wider area if we want to do drop shadows in any direction.

        padding = glyph.padding;
        margin = padding/2;

        int glyphTotalWidth = glyph.width + 2 * (padding + margin);
        int glyphTotalHeight = glyph.height + 2 * (padding + margin);

        bool bSpotFound = false;
        for(int y = 0; !bSpotFound && y < atlasHeight; ++y)
        {
            for(int x = 0; !bSpotFound && x < atlasWidth; ++x)
            {
                if (x+glyphTotalWidth < atlasWidth && y+glyphTotalHeight < atlasHeight)
                {
                    bool bOccupied = false;
                    for(int y2 = 0; !bOccupied && y2 < glyphTotalHeight; ++y2)
                    {
                        for(int x2 = 0; !bOccupied && x2 < glyphTotalWidth; ++x2)
                        {
                            bOccupied = occupancyMap[(x+x2)+((y+y2)*atlasWidth)];
                        }
                    }

                    if (!bOccupied)
                    {
                        // We found a spot!
                        bSpotFound = true;

                        glyph.atlasX = x + padding + margin;
                        glyph.atlasY = y + padding + margin;


                        for(int y2 = 0; y2 < glyphTotalHeight; ++y2)
                        {
                            for(int x2 = 0; x2 < glyphTotalWidth; ++x2)
                            {
                                occupancyMap[(x+x2)+((y+y2)*atlasWidth)] = true;
                            }
                        }

                        const int atlasMaxX = margin+glyph.texture_width;
                        const int atlasMaxY = margin+glyph.texture_height;
                        for(int y2 = margin; y2 < atlasMaxY; ++y2)
                        {
                            for(int x2 = margin; x2 < atlasMaxX; ++x2)
                            {
                                TunisPNG::ColorRGBA *atlasPixel = &atlas[((x+x2)+((y+y2)*atlasWidth))];
                                const TunisPNG::ColorRGBA *msdfaPixel = &msdfa[(x2-margin)+((y2-margin)*glyph.texture_width)];
                                atlasPixel->r = msdfaPixel->r;
                                atlasPixel->g = msdfaPixel->g;
                                atlasPixel->b = msdfaPixel->b;
                                atlasPixel->a = msdfaPixel->a;

                            }
                        }
                    }
                }
            }
        }

        if (!bSpotFound)
        {
            log.error() << "Could not find a spot to insert '" << glyph.unicode << "' into texture atlas!" << std::endl;
        }
    }
    log.notice() << "Generation of " << msdfaAtlasFileName << " completed!";

    Poco::File tmp(atlasPath);
    tmp.createDirectories();

    TunisPNG atlasPNG(atlasBuffer, atlasWidth, atlasHeight, TunisPNG::RGBA);
    atlasPNG.store(msdfaAtlasFileName);

    std::string fontName = font.getFamily() + ' ' + font.getStyle().getName();
    const FT_Face face = font.getFace();

    flatbuffers::FlatBufferBuilder builder;

    std::vector< flatbuffers::Offset<tunis::Glyph> > glyphBlock;

    for (size_t i = 0; i < glyphs.size(); ++i)
    {
        const TunisGlyph &glyph = glyphs[i];

        std::ifstream infile(glyph.msdfaFileName, std::ios::binary | std::ios::in);

        std::vector<uint8_t> imageBuffer;

        //get length of file
        infile.seekg(0, infile.end);
        std::streamsize length = infile.tellg();
        infile.seekg(0, infile.beg);

        if (length > 0)
        {
            imageBuffer.resize(length);
            infile.read(reinterpret_cast<char*>(&imageBuffer[0]), length);
        }
        auto imageData = builder.CreateVector(imageBuffer.data(), imageBuffer.size());

        std::vector<tunis::Kerning> kernings;

        if (FT_HAS_KERNING(face))
        {

            for (size_t j = 0; j < glyphs.size(); ++j)
            {
                FT_Vector kerning;

                FT_Error error = FT_Get_Kerning(face,
                                                FT_Get_Char_Index(face, glyphs[i].unicode),
                                                FT_Get_Char_Index(face, glyphs[j].unicode),
                                                FT_KERNING_UNSCALED,
                                                &kerning);

                if (error) continue;

                if (kerning.x > 0)
                {
                    kernings.emplace_back(tunis::Kerning(glyphs[j].unicode, kerning.x/64.0f));
                }
            }
        }

        auto kerningVector = builder.CreateVectorOfStructs(kernings);

        tunis::GlyphBuilder glyphBuilder(builder);
        glyphBuilder.add_unicode(glyph.unicode);
        glyphBuilder.add_width(glyph.texture_width);
        glyphBuilder.add_height(glyph.texture_height);
        glyphBuilder.add_xadvance(glyph.xadvance);
        glyphBuilder.add_xoffset(glyph.xoffset);
        glyphBuilder.add_yoffset(glyph.yoffset);
        glyphBuilder.add_kernings(kerningVector);
        glyphBuilder.add_image(imageData);
        glyphBlock.push_back(glyphBuilder.Finish());

    }

    auto familyString = builder.CreateString(family);
    auto glyphVector = builder.CreateVector(glyphBlock.data(), glyphBlock.size());

    tunis::FontBuilder fontBuilder(builder);
    fontBuilder.add_family(familyString);
    fontBuilder.add_style(static_cast<tunis::Style>(pStyle->getThickness() / 100));
    fontBuilder.add_italic(pStyle->isItalic());
    fontBuilder.add_fontSize(face->size->metrics.y_ppem);
    fontBuilder.add_padding(padding);
    fontBuilder.add_lineHeight(face->size->metrics.height/64.0f);
    fontBuilder.add_glyphs(glyphVector);
    auto fontData = fontBuilder.Finish();

    builder.Finish(fontData);

    std::ofstream outfile;
    outfile.open("fonts.tfp", std::ios::binary | std::ios::out);
    outfile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                  static_cast<std::streamsize>(builder.GetSize()));
    outfile.close();


    return true;
}
