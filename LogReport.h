#pragma once

#include <iostream>
#include "VhallNetWorkInterface.h"
//��ʽ���� 
#define LOG_URL "http://pingback.e.vhall.com/msdk.gif"

//���Ի��� 
//#define LOG_URL "http://pingback.e.vhall.com/msdk.gif?test=1&"

//https://x'x'x'x'x'x'x'x'x?k=1&id=123&s=123&bu=0&token=base64

#define SDK_INIT        "292001" //	SDK��ʼ��
#define SDK_LOGIN       "292002" //	��¼��ʼ��

class LogReport {
public:
    LogReport();
    ~LogReport();

public:
    void RegisterHttpSender(std::shared_ptr<HttpManagerInterface> sender);
    void SendLog(std::string key,std::string id, std::string session, std::string bu, std::string token);

private:
   std::shared_ptr<HttpManagerInterface> pHttpManagerInterface;
};
