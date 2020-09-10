#pragma once
#include <string>
#include "paas_common.h"
class RespBaseInfo {
public:
    int mCode = 0;                      //����ֵ
    std::string mNoticeMsg;             //��ʾ��Ϣ
    void reset() {
       mCode = 0;                      //����ֵ
       mNoticeMsg = "";             //��ʾ��Ϣ
    }

};

class PushConfig {
public:
    std::string stand;                  //ϵͳ�ڶ����� SD�����壩HD�����壩UHD�����壩CUSTOM
    std::string dpi;                    //�ֱ���
    std::string frame_rate;             //֡��
    std::string bitrate;                //����

    void reset() {
       stand = "";                  //ϵͳ�ڶ����� SD�����壩HD�����壩UHD�����壩CUSTOM
       dpi = "";                    //�ֱ���
       frame_rate = "";             //֡��
       bitrate = "";                //����
    }
};

class CPaasTaskEvent
{
public:
   CPaasTaskEvent() {

   }
   ~CPaasTaskEvent(){

   }

   int mEventType = -1;
};

class LogInfo {
public:
    //log_info
    std::string session_id;             //Ψһ�Ự��ʶ
    std::string uid;                    // �ۿ��û�id
    std::string ip;                     //������IP��ַ
    std::string third_party_user_id;    //�������û�ID
    std::string account_id;             //������ID
    std::string app_name;               //app����

    void reset() {

       session_id = "";;             //Ψһ�Ự��ʶ
       uid = "";;                    // �ۿ��û�id
       ip = "";;                     //������IP��ַ
       third_party_user_id = "";;    //�������û�ID
       account_id = "";;             //������ID
       app_name = "";;               //app����
    }
};

//����������¼��Ӧ��Ϣ http://doc.vhall.com/docs/show/1229
class LoginRespInfo :public RespBaseInfo, public PushConfig, public LogInfo {
public:
    std::string socket_server;             //��Ϣ��������
    std::string nginx_server;              //�������������
    std::string connection_token;           //��Ϣ����TOKEN
    std::string document_server;                //�ĵ�����������
    std::string log_server;                  //��־�ϱ�����������
};

class InavInfo :public RespBaseInfo, public PushConfig, public LogInfo, public Permission {
public:
   //log_info
   //std::string session_id;             //Ψһ�Ự��ʶ
   //std::string uid;                    // �ۿ��û�id
   //std::string ip;                     //������IP��ַ
   //std::string third_party_user_id;    //�������û�ID
   //std::string account_id;             //������ID
   //std::string app_name;               //app����

    std::string inav_token;
    std::string push_address;//��·������ַ

    void reset() {
       inav_token = "";
       push_address = "";//��·������ַ
       LogInfo::reset();

       RespBaseInfo::reset(); 
       PushConfig::reset(); 
       Permission::reset();

    }
};