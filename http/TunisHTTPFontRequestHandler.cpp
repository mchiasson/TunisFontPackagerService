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
#include "TunisHTTPFontRequestHandler.h"

#include "data/TunisGoogleWebFontDB.h"

#include <Poco/LogStream.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/StreamCopier.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>
#include <rapidjson/document.h>

#include <fstream>
#include <iomanip>

#include "data/TunisFontDB.h"

TunisHTTPFontRequestHandler::TunisHTTPFontRequestHandler(const std::string &id) :
    id(id)
{
}

void TunisHTTPFontRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Poco::LogStream log(Poco::Logger::get("http"));

    Poco::Net::HTMLForm form(request);

    if (!form.has("family"))
    {
        log.notice() << request.clientAddress().toString() << " did not provide 'family'";
        sendError(response, 400, "Missing '<kbd>family</kbd>' parameter");
        return;
    }
    std::string family = form.get("family");

    if (!form.has("style"))
    {
        log.notice() << request.clientAddress().toString() << " did not provide 'style'";
        sendError(response, 400, "Missing '<kbd>style</kbd>' parameter");
        return;
    }
    std::string style =  form.get("style");

    if (!form.has("version"))
    {
        log.notice() << request.clientAddress().toString() << " did not provide 'version'";
        sendError(response, 400, "Missing '<kbd>version</kbd>' parameter");
        return;
    }
    std::string version =  form.get("version");

    TunisFont font;
    if (TunisFontDB::loadFont(font, family, TunisFontStyle::GetStyleByName(style)))
    {
#if !defined(NDEBUG)
        response.set("Cache-Control", "no-cache, no-store, must-revalidate");
#endif
        try
        {
            response.sendFile(font.getFileName(), "font/ttf");
        }
        catch (Poco::FileNotFoundException e)
        {
            sendError(response, 404, e.message());
        }
    }
    else
    {
        std::string reason = std::string("Could not find font '") + family + "' with style '" + style + "'";
        sendError(response, 404, reason);
    }
}

void  TunisHTTPFontRequestHandler::writeUsage(std::ostream &os)
{
    os << "<p>The font api is an api that returns either a true-type font (.ttf) or open-type font (.otf) back to the user should the font be found in the internal database. If the font is not found, it will return 404 to the user.</p>";
    os << "<p>Example: <a href=\"/font?family=Roboto&style=regular\">/font?family=Roboto&style=Regular" << "</a></p>";
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
}
