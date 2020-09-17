#pragma once
#include <string>
#include "paas_common.h"
namespace vlive{
   class RespBaseInfo {
   public:
      int mCode = 0;                      //����ֵ
      std::string mNoticeMsg;             //��ʾ��Ϣ
   };

   class PushConfig {
   public:
      std::string stand;                  //ϵͳ�ڶ����� SD�����壩HD�����壩UHD�����壩CUSTOM
      std::string dpi;                    //�ֱ���
      std::string frame_rate;             //֡��
      std::string bitrate;                //����
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
      std::string inav_token;
      std::string push_address;//��·������ַ
   };
}
