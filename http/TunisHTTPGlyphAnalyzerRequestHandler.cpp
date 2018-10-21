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
#include "TunisHTTPGlyphAnalyzerRequestHandler.h"

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>

#include "data/TunisFontDB.h"
#include "font/TunisFont.h"
#include "font/TunisGlyph.h"

void TunisHTTPGlyphAnalyzerRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
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
    std::string unicode = form.get("unicode");
    int unicodeValue = parseUnicodeParam(unicode);

    TunisFont font;
    TunisFontDB::loadFont(font, family, TunisFontStyle::GetStyleByName(style), version);

    response.setContentType("text/html");
    std::ostream &os = response.send();
    os << "<html>";
    os << "<head>";
    os << "<title>Tunis Glyph Analyzer</title>";
    os << "<style>";
    os << "table, th, td {";
    os << "    border: 1px solid black;";
    os << "    border-collapse: collapse;";
    os << "}";
    os << "th, td {";
    os << "    padding: 15px;";
    os << "    text-align: center;";
    os << "}";
    os << "</style>";
    os << "</head>";
    os << "<body>";
    os << "<table>";
    os << "<tr>";
    os << "<th>Size</th>";
    os << "<th>SDF</th>";
    os << "<th>SDF Test</th>";
    os << "<th>MSDF</th>";
    os << "<th>MSDF Test</th>";
    os << "</tr>";
    for (int s = 8; s <= 128; s+=8)
    {
        TunisFontDB::loadGlyph(font, &unicodeValue, 1, s);
        os << "<tr>";
        os << "<td>" << s << "</td>";
        os << "<td><img src=\"/glyph?family=" << family << "&style=" << style << "&unicode=" << unicode << "&size=" << s << "&mode=sdf\"></td>";
        os << "<td><img src=\"/glyph?family=" << family << "&style=" << style << "&unicode=" << unicode << "&size=" << s << "&mode=sdftest\"></td>";
        os << "<td><img src=\"/glyph?family=" << family << "&style=" << style << "&unicode=" << unicode << "&size=" << s << "&mode=msdf\"></td>";
        os << "<td><img src=\"/glyph?family=" << family << "&style=" << style << "&unicode=" << unicode << "&size=" << s << "&mode=msdftest\"></td>";
        os << "</tr>";
    }
    os << "</table>";
    os << "</body>";
    os << "</html>";

}

void  TunisHTTPGlyphAnalyzerRequestHandler::writeUsage(std::ostream &os)
{
    os << "<p>The GlyphAnalizer api is a web page that shows all of the glyphs of various rendering techniques at large scale of every supported sizes</p>";
    os << "<p>Example: <a href=\"/GlyphAnalyzer?family=Roboto&style=regular&unicode=0x0041\">/GlyphAnalyzer?family=Roboto&style=regular&unicode=0x0041" << "</a></p>";
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
}
