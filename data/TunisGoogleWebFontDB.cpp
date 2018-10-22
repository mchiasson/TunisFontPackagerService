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
#include "TunisGoogleWebFontDB.h"

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
#include <Poco/Base64Decoder.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <sstream>

static const char * data= "aHR0cHM6Ly93d3cu"
                          "Z29vZ2xlYXBpcy5j"
                          "b20vd2ViZm9udHMv"
                          "djEvd2ViZm9udHM/"
                          "a2V5PUFJemFTeUQ2"
                          "NTlaNnlhOGlaN2dM"
                          "Qk04NlJHZHVmTVJJ"
                          "OFQxblpDQQ==";

std::string TunisGoogleWebFontDB::getJSONDatabase()
{
    std::istringstream istr(data);
    Poco::Base64Decoder decoder(istr);
    std::string result;
    decoder >> result;
    result += "&sort=popularity";
    Poco::URI uri(std::move(result));

    std::string json;

    const Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;
    session.sendRequest(request);
    std::istream& is = session.receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK)
    {
        std::string jsonData;
        Poco::StreamCopier::copyToString(is, jsonData);

        rapidjson::Document d;
        d.Parse(jsonData.data());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);

        json = buffer.GetString();

    }
    return json;
}

std::string TunisGoogleWebFontDB::findFontURL(const std::string &family, const std::string &style)
{
    std::string url;

    std::istringstream istr(data);
    Poco::Base64Decoder decoder(istr);
    std::string result;
    decoder >> result;
    result += "&sort=popularity";
    Poco::URI uri(std::move(result));

    const Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;
    session.sendRequest(request);
    std::istream& is = session.receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK)
    {
        std::string jsonData;
        Poco::StreamCopier::copyToString(is, jsonData);

        rapidjson::Document document;
        document.Parse(jsonData.c_str());
        const rapidjson::Value &items = document["items"];
        for (rapidjson::SizeType itemId = 0; itemId < items.Size(); itemId++)
        {
            const rapidjson::Value &item = items[itemId];
            std::string candidateFamily = item["family"].GetString();

            if (family == candidateFamily)
            {
                const rapidjson::Value &files = item["files"];
                if (files.HasMember(style.c_str()))
                {
                    url = files[style.c_str()].GetString();
                    break;
                }
            }
        }
    }

    return url;
}

std::vector<WebFont> TunisGoogleWebFontDB::getFontList()
{
    std::vector<WebFont> webfonts;

    std::istringstream istr(data);
    Poco::Base64Decoder decoder(istr);
    std::string result;
    decoder >> result;
    result += "&sort=popularity";
    Poco::URI uri(std::move(result));

    const Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;
    session.sendRequest(request);
    std::istream& is = session.receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK)
    {
        std::string jsonData;
        Poco::StreamCopier::copyToString(is, jsonData);

        rapidjson::Document document;
        document.Parse(jsonData.c_str());
        const rapidjson::Value &items = document["items"];
        webfonts.resize(items.Size());
        for (rapidjson::SizeType itemId = 0; itemId < items.Size(); itemId++)
        {
            const rapidjson::Value &item = items[itemId];
            WebFont &webfont = webfonts[itemId];

            webfont.family       = item["family"].GetString();
            webfont.category     = item["category"].GetString();
            webfont.version      = item["version"].GetString();
            webfont.lastModified = item["lastModified"].GetString();

            const rapidjson::Value &variants = item["variants"];
            for (rapidjson::SizeType variantId = 0; variantId < variants.Size(); variantId++)
            {
                webfont.variants.push_back(variants[variantId].GetString());
            }

            const rapidjson::Value &subsets = item["subsets"];
            for (rapidjson::SizeType subsetId = 0; subsetId < subsets.Size(); subsetId++)
            {
                webfont.subsets.push_back(subsets[subsetId].GetString());
            }

            const rapidjson::Value &files = item["files"];
            rapidjson::Value::ConstMemberIterator fileItr = files.MemberBegin(), fileEnd = files.MemberEnd();
            while (fileItr != fileEnd)
            {
                webfont.files[fileItr->name.GetString()] = fileItr->value.GetString();
                ++fileItr;
            }
        }
    }

    return webfonts;
}
