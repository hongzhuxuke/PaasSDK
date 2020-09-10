#include "LogReport.h"
#include "base64.h"

LogReport::LogReport() 
:pHttpManagerInterface(nullptr)
{

}

LogReport::~LogReport() {

}

void LogReport::RegisterHttpSender(std::shared_ptr<HttpManagerInterface> sender) {
    pHttpManagerInterface = sender;
}

void LogReport::SendLog(std::string key, std::string id, std::string session, std::string bu, std::string token) {
    ////https//x'x'x'x'x'x'x'x'x?k=1&id=123&s=123&bu=0&token=base64
    Base64 base;
    const unsigned char *p = (const unsigned char*)token.c_str();
    std::string base64Toke = base.Encode(p, token.length());
    std::string sendData = LOG_URL + std::string("?k=") + key + std::string("&id=") + id + std::string("&s=") + session + std::string("&bu=") + bu + std::string("&token=") + base64Toke;
    HTTP_GET_REQUEST httpRequest(sendData);
    pHttpManagerInterface->HttpGetRequest(httpRequest,nullptr);
}
