#include "TunisHTTPRequestHandlerFactory.h"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/LogStream.h>
#include <Poco/RegularExpression.h>

using namespace tunis::http;
using namespace Poco;
using namespace Poco::Net;

HTTPRequestHandler * RequestHandlerFactory::createRequestHandler(const HTTPServerRequest &request)
{
    LogStream log(Logger::get("RequestHandlerFactory"));
    log.notice() << "received request " << request.getHost() << request.getURI() << " from " << request.clientAddress().toString() << std::endl;


    return nullptr;
}
