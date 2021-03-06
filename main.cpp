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
#include <iostream>

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/LogStream.h>

#include "http/TunisHTTPRequestHandlerFactory.h"

using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

class TunisFontPackagingService : public ServerApplication
{
protected:

    virtual int main(const std::vector<std::string> &args) override
    {
        LogStream log(Poco::Logger::get("logger"));

        UInt16 port = 3000;
        if (args.size() > 0)
        {
            port = static_cast<UInt16>(std::stoi(args[0]));
        }

        ServerSocket socket(port);

        HTTPServerParams *pParams = new HTTPServerParams();
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);

        HTTPServer server(new TunisHTTPRequestHandlerFactory(), socket, pParams);

        log.notice() << "Starting HTTP server on port " << port << std::endl;
        server.start();

        waitForTerminationRequest();

        log.notice() << "Shutting down server" << std::endl;

        server.stop();

        return EXIT_OK;
    }

};

POCO_SERVER_MAIN(TunisFontPackagingService)
