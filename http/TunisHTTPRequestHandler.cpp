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
#include "TunisHTTPRequestHandler.h"

#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/DateTime.h>

std::string TunisHTTPRequestHandler::GenerateDisclaimerString()
{
    std::string yearString = std::to_string(2017);
    int year = Poco::DateTime().year();
    if (year > 2017)
    {
        yearString += '-' + std::to_string(year);
    }
    return std::string("Powered by Tunis Font Packaging Service. Copyright (c) ") + yearString + " Mathieu-Andre Chiasson ALL RIGHTS RESERVED.";
}

void TunisHTTPRequestHandler::sendError(Poco::Net::HTTPServerResponse& response, int status, const std::string &reason)
{
    response.setStatusAndReason((Poco::Net::HTTPServerResponse::HTTPStatus)status, reason);
    response.setContentType("text/html");

    std::string title = std::to_string(status) + " " + Poco::Net::HTTPServerResponse::getReasonForStatus(response.getStatus());

    std::ostream& os = response.send();
    os << "<html>";
    os << "<head><title>" << title << "</title></head>";
    os << "<body>";
    os << "<h1>" << title << "</h1>";
    os << "<p>" << reason << "</p>";
    writeUsage(os);
    os << "</body>";
    os << "</html>";

}

int TunisHTTPRequestHandler::parseUnicodeParam(const std::string &unicodeParam)
{
    int unicode = -1;

    try
    {
        if (unicodeParam.length() >= 3)
        {
            std::string header = unicodeParam.substr(0, 2);
            if (header == "U " || header == "U+" ||header == "0x")
            {
                std::size_t count = 0;
                int num = std::stoi(unicodeParam.substr(2).c_str(), &count, 16);
                if (count > 0)
                {
                    unicode = num;
                }
            }
            else if (unicodeParam[0] == '\'' && unicodeParam[2] == '\'')
            {
                unicode = (int) unicodeParam[1];
            }
        }

        if (unicode == -1)
        {
            size_t count = 0;
            int num = std::stoi(unicodeParam.c_str(), &count);
            if (count > 0)
            {
                unicode = num;
            }
        }
    }
    catch (std::invalid_argument)
    {
        // do nothing. it will return -1.
    }

    return unicode;
}

int TunisHTTPRequestHandler::parseIntegerParam(const std::string &numberParam)
{
    int number = -1;

    try
    {
        size_t count = 0;
        int num = std::stoi(numberParam, &count);
        if (count > 0)
        {
            number = num;
        }
    }
    catch (std::invalid_argument)
    {
        // do nothing. it will return -1.
    }

    return number;
}

TunisHTTPRequestHandler::GlyphMode TunisHTTPRequestHandler::parseModeParam(const std::string &mode)
{
     if (Poco::icompare(mode, "sdf") == 0)  return GLYPH_MODE_SDF;
     if (Poco::icompare(mode, "sdftest") == 0)  return GLYPH_MODE_SDF_TEST;
     if (Poco::icompare(mode, "msdf") == 0) return GLYPH_MODE_MSDF;
     if (Poco::icompare(mode, "msdftest") == 0) return GLYPH_MODE_MSDF_TEST;
     if (Poco::icompare(mode, "msdfa") == 0) return GLYPH_MODE_MSDFA;
     if (Poco::icompare(mode, "msdftest") == 0) return GLYPH_MODE_MSDFA_TEST;
     return GLYPH_MODE_INVALID;
}

