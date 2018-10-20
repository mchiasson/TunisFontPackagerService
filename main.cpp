#include <iostream>

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/LogStream.h>

#include "TunisHTTPRequestHandlerFactory.h"

using namespace tunis::http;
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

        HTTPServer server(new RequestHandlerFactory(), socket, pParams);

        log.notice() << "Starting HTTP server on port " << port << std::endl;
        server.start();

        waitForTerminationRequest();

        log.notice() << "Shutting down server" << std::endl;

        server.stop();

        return EXIT_OK;
    }

};

POCO_SERVER_MAIN(TunisFontPackagingService)
