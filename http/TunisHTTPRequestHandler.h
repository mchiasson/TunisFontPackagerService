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
#ifndef KTDHTTPREQUESTHANDLER_H
#define KTDHTTPREQUESTHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

class TunisHTTPRequestHandler : public Poco::Net::HTTPRequestHandler
{
protected:
    enum GlyphMode
    {
        GLYPH_MODE_INVALID,
        GLYPH_MODE_SDF,
        GLYPH_MODE_SDF_TEST,
        GLYPH_MODE_MSDF,
        GLYPH_MODE_MSDF_TEST,
        GLYPH_MODE_MSDFA,
        GLYPH_MODE_MSDFA_TEST,
    };

    void sendError(Poco::Net::HTTPServerResponse& response, int status, const std::string &reason);
    virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) = 0;
    virtual void writeUsage(std::ostream &os) = 0;

    int parseUnicodeParam(const std::string &unicodeParam);
    int parseIntegerParam(const std::string &numberParam);
    GlyphMode parseModeParam(const std::string &mode);

    static std::string GenerateDisclaimerString();

};

#endif // KTDHTTPREQUESTHANDLER_H
