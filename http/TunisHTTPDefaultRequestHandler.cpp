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
#include "TunisHTTPDefaultRequestHandler.h"

#include <http/TunisHTTPFontRequestHandler.h>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/LogStream.h>

#include <TunisRC.h>

void TunisHTTPDefaultRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
#if !defined(NDEBUG)
        response.set("Cache-Control", "no-cache, no-store, must-revalidate");
#endif

    if (request.getURI() == "/domready.js")
    {
        response.setContentType("text/javascript");
        response.send() << public_domready_js;
    }
    else if (request.getURI() == "/glmatrix.js")
    {
        response.setContentType("text/javascript");
        response.send() << public_glmatrix_js;
    }
    else if (request.getURI() == "/glutil.js")
    {
        response.setContentType("text/javascript");
        response.send() << public_glutil_js;
    }
    else
    {
        sendError(response, 404, std::string("Could not find '") + request.getURI() + "'");
    }
}

void  TunisHTTPDefaultRequestHandler::writeUsage(std::ostream &os)
{
    os << "<h1>Tunis Font Packaging Service</h1>";
    os << "<ul>";
    os << "<li><h3><a href=\"/atlas\">/atlas</a></h3></li>";
    os << "<li><h3><a href=\"/font\">/font</a></h3></li>";
    os << "<li><h3><a href=\"/glyph\">/glyph</a></h3></li>";
    os << "<li><h3><a href=\"/GlyphAnalyzer\">/GlyphAnalyzer</a></h3></li>";
    os << "</ul>";
}
