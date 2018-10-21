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
#include "TunisHTTPDownloadHelper.h"

#include <Poco/URI.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/LogStream.h>

#include <memory>

bool TunisHTTPDownloadHelper::download(const std::string &url, std::ostream &os)
{
    Poco::LogStream log(Poco::Logger::get("data"));

    Poco::URI uri(url);

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;

    std::unique_ptr<Poco::Net::HTTPClientSession> pSession;

    if (url.substr(0, 8) == "https://")
    {
        const Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        pSession.reset(new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort(), context));
    }
    else
    {
        pSession.reset(new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort()));
    }

    pSession->sendRequest(request);
    std::istream &is = pSession->receiveResponse(response);

    if (response.getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK)
    {
        Poco::StreamCopier::copyStream(is, os);
        return true;
    }
    else
    {
        log.error() << "Could not download '" << url << "' : " << response.getStatus() << Poco::Net::HTTPServerResponse::getReasonForStatus(response.getStatus()) << " : " << response.getReason();
    }

    return false;
}
