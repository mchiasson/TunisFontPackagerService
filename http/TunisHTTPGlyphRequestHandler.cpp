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
#include "TunisHTTPGlyphRequestHandler.h"

#include <Poco/LogStream.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>

#include <sstream>
#include <iomanip>

#include "data/TunisFontDB.h"

void TunisHTTPGlyphRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Poco::Net::HTMLForm form(request);

    if (!form.has("family"))
    {
        sendError(response, 400, "Missing '<kbd>family</kbd>' parameter");
        return;
    }
    std::string family = form.get("family");

    if (!form.has("style"))
    {
        sendError(response, 400, "Missing '<kbd>style</kbd>' parameter");
        return;
    }
    std::string style = form.get("style");

    if (!form.has("version"))
    {
        sendError(response, 400, "Missing '<kbd>version</kbd>' parameter");
        return;
    }
    std::string version = form.get("version");

    if (!form.has("unicode"))
    {
        sendError(response, 400, "Missing '<kbd>unicode</kbd>' parameter");
        return;
    }

    int unicode = parseUnicodeParam(form.get("unicode"));
    if (unicode == -1)
    {
        sendError(response, 400, "Invalid '<kbd>unicode</kbd>' parameter format. Format examples: <ul><li><kbd>U+004B</kbd></li><li><kbd>0x004B</kbd></li><li><kbd>75</kbd></li><li><kbd>'K'</kbd></li></ul>");
        return;
    }

    if (!form.has("size"))
    {
        sendError(response, 400, "Missing '<kbd>size</kbd>' parameter");
        return;
    }

    int size = parseIntegerParam(form.get("size"));
    if (size < 8 || size > 128)
    {
        sendError(response, 400, "Invalid '<kbd>size</kbd>' parameter format. size must be a positive number, greater than 8, and smaller than 128.");
        return;
    }
    // round to the nearest increment of '8', meaning that we're only supporting 16 glyph sizes :  8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128
    size = (int)(size / 8.0 + 0.5) * 8;

    if (!form.has("mode"))
    {
        sendError(response, 400, "Missing '<kbd>mode</kbd>' parameter.");
        return;
    }

    GlyphMode mode = parseModeParam(form.get("mode"));
    if (mode == GLYPH_MODE_INVALID)
    {
        sendError(response, 400, "Invalid '<kbd>mode</kbd>' parameter format. Available choices are <ul><li><kbd>sdf</kbd></li><li><kbd>sdftest</kbd></li><li><kbd>msdf</kbd></li><li><kbd>msdftest</kbd></li></ul>");
        return;
    }

    // first load the font from the database.
    TunisFont font;
    if (!TunisFontDB::loadFont(font, family, TunisFontStyle::GetStyleByName(style)))
    {
        std::stringstream reason;
        reason << "Could not find font '" << form.get("family") << "' with style '" << form.get("style") << "'";
        sendError(response, 404, reason.str());
        return;
    }

    if (!TunisFontDB::loadGlyph(font, &unicode, 1, size))
    {
        std::stringstream reason;
        reason << "Could not find/generate glyph '" << form.get("unicode") << "' from font '" << form.get("family") << "' with style '" << form.get("style") << "'";
        sendError(response, 500, reason.str());
        return;
    }

#if !defined(NDEBUG)
        response.set("Cache-Control", "no-cache, no-store, must-revalidate");
#endif
    try
    {
        switch(mode)
        {
        case GLYPH_MODE_SDF:
            response.sendFile(font.getGlyphs()[0].sdfFileName, "image/png");
            break;
        case GLYPH_MODE_MSDF:
            response.sendFile(font.getGlyphs()[0].msdfFileName, "image/png");
            break;
        case GLYPH_MODE_MSDFA:
            response.sendFile(font.getGlyphs()[0].msdfaFileName, "image/png");
            break;
        }
    }
    catch(Poco::FileNotFoundException)
    {
        std::stringstream unicodeHex;
        unicodeHex << std::setfill('0') << std::setw(4) << std::hex << unicode;
        std::string unicodeHexStr = unicodeHex.str();


        std::stringstream ss;
        ss << "no image available. Are you sure it is a printable glyph? You can always check here: <a href=\"https://unicode-table.com/en/#" << unicodeHexStr << "\">https://unicode-table.com/en/#" << unicodeHexStr << "</a>";

        sendError(response, 404, ss.str());
        return;
    }
}

void  TunisHTTPGlyphRequestHandler::writeUsage(std::ostream &os)
{
    os << "<p>The glyph api is an api that returns a specific glyph</p>";
    os << "<p>Example: <a href=\"/glyph?family=Roboto&style=regular&unicode=0x0041&size=64&mode=msdf\">/glyph?family=Roboto&style=regular&unicode=0x0041&size=64&mode=msdf" << "</a></p>";
    os << "<p>'<kbd>family</kbd>' : the family of the font.</p>";
    os << "<p>'<kbd>style</kbd>' : the style of the font. Here are the list of styles :</p>";
    os << "<ul>";
    os << "<li>'<kbd>Thin</kbd>' (equivalent to '<kbd>100</kbd>')</li>";
    os << "<li>'<kbd>Thin Italic</kbd>' (equivalent to '<kbd>100italic</kbd>')</li>";
    os << "<li>'<kbd>Extra-Light</kbd>' (equivalent to '<kbd>200</kbd>')</li>";
    os << "<li>'<kbd>Extra-Light Italic</kbd>' (equivalent to '<kbd>200italic</kbd>')</li>";
    os << "<li>'<kbd>Light</kbd>' (equivalent to '<kbd>300</kbd>')</li>";
    os << "<li>'<kbd>Light Italic</kbd>' (equivalent to '<kbd>300italic</kbd>')</li>";
    os << "<li>'<kbd>Regular</kbd>'</li>";
    os << "<li>'<kbd>Regular Italic</kbd>' (equivalent to '<kbd>Italic</kbd>'</li>";
    os << "<li>'<kbd>Medium</kbd>' (equivalent to '<kbd>500</kbd>')</li>";
    os << "<li>'<kbd>Medium Italic</kbd>' (equivalent to '<kbd>500italic</kbd>')</li>";
    os << "<li>'<kbd>Semi-Bold</kbd>' (equivalent to '<kbd>600</kbd>')</li>";
    os << "<li>'<kbd>Semi-Bold Italic</kbd>' (equivalent to '<kbd>600italic</kbd>')</li>";
    os << "<li>'<kbd>Bold</kbd>' (equivalent to '<kbd>700</kbd>')</li>";
    os << "<li>'<kbd>Bold Italic</kbd>' (equivalent to '<kbd>700italic</kbd>')</li>";
    os << "<li>'<kbd>Extra-Bold</kbd>' (equivalent to '<kbd>800</kbd>')</li>";
    os << "<li>'<kbd>Extra-Bold Italic</kbd>' (equivalent to '<kbd>800italic</kbd>')</li>";
    os << "<li>'<kbd>Black</kbd>(equivalent to '<kbd>900</kbd>')</li>";
    os << "<li>'<kbd>Black Italic</kbd>' (equivalent to '<kbd>900italic</kbd>')</li>";
    os << "</ul>";
    os << "<p>'<kbd>unicode</kbd>' : the unicode of the glyph to analyze. We support the following formats:</p>";
    os << "<ul>";
    os << "<li>Unicode string. Example: <kbd>U+0041<kbd></li>";
    os << "<li>Hexadecimal value. Example: <kbd>0x0041<kbd></li>";
    os << "<li>Decimal value. Example: <kbd>64<kbd></li>";
    os << "<li>Character value. Example: <kbd>'A'<kbd></li>";
    os << "</ul>";
    os << "<p>'<kbd>size</kbd>' : the size of the glyph. For space reason, we only support the following sizes:</p>";
    os << "<ul>";
    os << "<li><kbd>8<kbd></li>";
    os << "<li><kbd>16<kbd></li>";
    os << "<li><kbd>24<kbd></li>";
    os << "<li><kbd>32<kbd></li>";
    os << "<li><kbd>40<kbd></li>";
    os << "<li><kbd>48<kbd></li>";
    os << "<li><kbd>56<kbd></li>";
    os << "<li><kbd>64<kbd></li>";
    os << "<li><kbd>72<kbd></li>";
    os << "<li><kbd>80<kbd></li>";
    os << "<li><kbd>88<kbd></li>";
    os << "<li><kbd>96<kbd></li>";
    os << "<li><kbd>104<kbd></li>";
    os << "<li><kbd>112<kbd></li>";
    os << "<li><kbd>120<kbd></li>";
    os << "<li><kbd>128<kbd></li>";
    os << "</ul>";
    os << "<p>'<kbd>mode</kbd>' : the mode of the glyph. We support the following formats:</p>";
    os << "<ul>";
    os << "<li><kbd>sdf<kbd></li>";
    os << "<li><kbd>msdf<kbd></li>";
    os << "</ul>";
}

