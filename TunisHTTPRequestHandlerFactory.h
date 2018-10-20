#ifndef TUNISHTTPREQUESTHANDLERFACTORY_H
#define TUNISHTTPREQUESTHANDLERFACTORY_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>

namespace tunis
{
    namespace http
    {
        class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
        {
        public:
            virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override;
        };

    }
}

#endif // TUNISHTTPREQUESTHANDLERFACTORY_H
