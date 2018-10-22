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
#include "TunisHTTPAtlasRequestHandler.h"

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
#include <Poco/File.h>
#include <Poco/Path.h>
#include <rapidjson/document.h>

#include <fstream>
#include <sstream>

#include "font/TunisAtlas.h"


void TunisHTTPAtlasRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
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
    std::string style =  form.get("style");

    if (!form.has("version"))
    {
        sendError(response, 400, "Missing '<kbd>version</kbd>' parameter");
        return;
    }
    std::string version =  form.get("version");

    if (!form.has("size"))
    {
        sendError(response, 400, "Missing '<kbd>size</kbd>' parameter");
        return;
    }
    int size = parseIntegerParam(form.get("size"));

    std::string format = form.get("format", "png");

    TunisAtlas atlas(family, style, size);

#if !defined(NDEBUG)
    response.set("Cache-Control", "no-cache, no-store, must-revalidate");
#endif
    try
    {
        if (format == "png")
        {
            response.sendFile(atlas.getmsdfaAtlasFileName(), "image/png");
        }
        else
        {
            sendError(response, 400, std::string("Unknown format " + format));
        }
    }
    catch (Poco::FileNotFoundException)
    {

        if (atlas.generate())
        {
            try
            {
                if (format == "png")
                {
                    response.sendFile(atlas.getmsdfaAtlasFileName(), "image/png");
                }
                else
                {
                    sendError(response, 400, std::string("Unknown format " + format));
                }
            }
            catch(Poco::FileNotFoundException)
            {
                std::string reason = std::string("Could not generate font '") + family + "' with style '" + style + "'. Disk writing operation may have failed.";
                sendError(response, 500, reason);
            }
        }
        else
        {
            std::string reason = std::string("Could not find font '") + family + "' with style '" + style + "'";
            sendError(response, 404, reason);
        }
    }
}

void  TunisHTTPAtlasRequestHandler::writeUsage(std::ostream &os)
{

}
