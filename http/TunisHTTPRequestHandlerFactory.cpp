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
#include "TunisHTTPRequestHandlerFactory.h"

#include "TunisHTTPAtlasRequestHandler.h"
#include "TunisHTTPDefaultRequestHandler.h"
#include "TunisHTTPGlyphAnalyzerRequestHandler.h"
#include "TunisHTTPGlyphRequestHandler.h"
#include "TunisHTTPFontRequestHandler.h"
#include "TunisHTTPRootRequestHandler.h"

#include "TunisFontRequestHandler.h"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/LogStream.h>
#include <Poco/RegularExpression.h>

Poco::Net::HTTPRequestHandler * TunisHTTPRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
    Poco::LogStream log(Poco::Logger::get("http"));

    bool bSecure = ((Poco::Net::HTTPServerRequestImpl*)&request)->socket().secure();
    log.notice() << "received request " << (bSecure?"https://":"http://") << request.getHost() << request.getURI() << " from " << request.clientAddress().toString() << std::endl;

    Poco::RegularExpression reRoot         ("^\\/(\\?.*)?$");            // starts with either / or /?
    Poco::RegularExpression reFont         ("^\\/(\\w+)(\\?.*)?$");        // starts with either /<fontfamily> or /<fontfamily>?
    Poco::RegularExpression reAtlas        ("^\\/atlas\\??.*$");         // starts with either /atlas or /atlas?
    Poco::RegularExpression reGlyph        ("^\\/glyph\\??.*$");         // starts with either /glyph or /glyph?
    Poco::RegularExpression reGlyphAnalyzer("^\\/GlyphAnalyzer\\??.*$"); // starts with either /GlyphAnalyzer or /GlyphAnalyzer?

    std::vector<std::string> groups;

    if(reAtlas.match(request.getURI()))
    {
        return new TunisHTTPAtlasRequestHandler();
    }
    else if(reGlyphAnalyzer.match(request.getURI()))
    {
        return new TunisHTTPGlyphAnalyzerRequestHandler();
    }
    else if(reGlyph.match(request.getURI()))
    {
        return new TunisHTTPGlyphRequestHandler();
    }
    else if(reRoot.match(request.getURI()))
    {
//        return new TunisHTTPRootRequestHandler();
        return new TunisFontRequestHandler();
    }
    else if(reFont.split(request.getURI(), groups))
    {
        return new TunisHTTPFontRequestHandler(groups[1]);
    }


    // unsupported request.
    return new TunisHTTPDefaultRequestHandler();
}
