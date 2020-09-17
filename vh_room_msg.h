#pragma once
#include <string>
#include "paas_common.h"
namespace vlive{
   class RespBaseInfo {
   public:
      int mCode = 0;                      //返回值
      std::string mNoticeMsg;             //提示信息
   };

   class PushConfig {
   public:
      std::string stand;                  //系统内定类型 SD（标清）HD（高清）UHD（超清）CUSTOM
      std::string dpi;                    //分辨率
      std::string frame_rate;             //帧率
      std::string bitrate;                //码率
   };

   class LogInfo {
   public:
      //log_info
      std::string session_id;             //唯一会话标识
      std::string uid;                    // 观看用户id
      std::string ip;                     //请求者IP地址
      std::string third_party_user_id;    //第三方用户ID
      std::string account_id;             //开发者ID
      std::string app_name;               //app名称
   };

   //桌面启动登录响应信息 http://doc.vhall.com/docs/show/1229
   class LoginRespInfo :public RespBaseInfo, public PushConfig, public LogInfo {
   public:
      std::string socket_server;             //消息服务域名
      std::string nginx_server;              //聊天服务器域名
      std::string connection_token;           //消息连接TOKEN
      std::string document_server;                //文档服务器域名
      std::string log_server;                  //日志上报服务器域名
   };

   class InavInfo :public RespBaseInfo, public PushConfig, public LogInfo, public Permission {
   public:
      std::string inav_token;
      std::string push_address;//旁路推流地址
   };
}
