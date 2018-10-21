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
#include "TunisHTTPRootRequestHandler.h"

#include "data/TunisGoogleWebFontDB.h"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/URI.h>
#include <Poco/LogStream.h>

#include <iomanip>

void TunisHTTPRootRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{

    std::vector<WebFont> webfonts = TunisGoogleWebFontDB::getFontList();

    response.setContentType("text/html");

    std::ostream &os = response.send();

    os << std::left
       << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">"
       << "<html><head><title>Index of /</title></head>"
       << "<body>"
       << "<h1>Index of /</h1>"
       << "<pre>"
       << std::setw(41) << "<a href=\"\">Name</a>"
       << std::setw(41) << "<a href=\"\">Family</a>"
       << std::setw(28) << "<a href=\"\">Category</a>"
       << std::setw(23) << "<a href=\"\">Version</a>"
       << "<a href=\"\">Last modified</a>"
       << "<hr>";

    for (size_t i = 0; i < webfonts.size(); ++i)
    {
        WebFont &webfont = webfonts[i];

        os << std::setw(26) << webfont.family
           << std::setw(15) << webfont.category
           << std::setw(6) << webfont.version
           << webfont.lastModified << "<br>";
    }

    os << "<hr>"
       << GenerateDisclaimerString()
       << "</pre>"
       << "</body>"
       << "</html>";
}

void TunisHTTPRootRequestHandler::writeUsage(std::ostream &) {}

