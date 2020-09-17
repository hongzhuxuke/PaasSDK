#ifndef __PAAS_COMMON_H__
#define __PAAS_COMMON_H__

#include <string>
#include <functional>
#include <iostream>

namespace vlive {
   enum SocketIOMsgType {
      SocketIOMsgType_None = 0,
      SocketIOMsgType_CMD = 1,
      SocketIOMsgType_FLASH = 2,
   };

   //旁路直播混流时分辨率
   enum BROAD_CAST_DPI {
      BROAD_CAST_DPI_HUHD = 0,        // "1920*1080"
      BROAD_CAST_DPI_UHD,             // "1280*720"
      BROAD_CAST_DPI_MHD,             // "960*540
      BROAD_CAST_DPI_HD,              // "640*480"
      BROAD_CAST_DPI_SD,              // "320*240"
   };

   //旁路直播混流帧率
   enum FRAME_FPS {
      //帧率 15, 20 (默认), 25 帧
      FRAME_FPS_15 = 15,
      FRAME_FPS_20 = 20,
      FRAME_FPS_25 = 25,
   };

   //旁路直播混流码率
   enum BITRATE {
      //码率 800, 1000 (默认), 1200
      BITRATE_800 = 800,
      BITRATE_1000 = 1000,
      BITRATE_1200 = 1200,
   };

   enum RoomEvent
   {
      RoomEvent_OnNetWork_Reconnecting = 0,   //网络正在进行重连
      RoomEvent_OnNetWork_Reconnect,       //网络重连成功
      RoomEvent_OnNetWork_Close,           //网络连接断开
      RoomEvent_Login,                    //登录事件
      RoomEvent_Logout,                   //登出时间
      RoomEvent_InavUserList,             //互动房间人员列表
      RoomEvent_Apply_Push,               //申请上麦
      RoomEvent_AuditInavPublish,         //审核申请上麦
      RoomEvent_AskforInavPublish,        //邀请上麦
      RoomEvent_UserPublishCallback,      //上/下/拒绝上麦状态回执
      RoomEvent_KickInavStream,           //踢出流/取消踢出流
      RoomEvent_KickInav,                 //踢出互动房间/取消踢出互动房间
      RoomEvent_GetKickInavList,          //获取被踢出互动房间人列表
      RoomEvent_NoPushStreamPermission,   //查看AccessToken是否具有某权限
      RoomEvent_Start_PublishInavAnother, //开始旁路布局
      RoomEvent_Stop_PublishInavAnother,  //停止旁路布局
      RoomEvent_SetMainView,              //设置大画面
      RoomEvent_SetLayOut,                //设置布局
   };



   //用户状态 
   enum MemberStatus {
      MemberStatus_Pushing = 1,  // 1 推流中
      MemberStatus_Watching,     // 2 观看中
      MemberStatus_BeInvited,    // 3 受邀中
      MemberStatus_Application,  // 4 申请中
      MemberStatus_KickOut       // 5 被踢出
   };

   enum AuditPublish {
      AuditPublish_Accept = 1,  //同意上麦
      AuditPublish_Refused      //拒绝上麦
   };

   enum PushStreamEvent {
      //*type	int	否	1 上麦(默认) 2 下麦 3 拒绝上麦
      PushStreamEvent_Upper = 1,
      PushStreamEvent_Lower,
      PushStreamEvent_Refused,
   };

   enum KickStream {
      KickStream_KickOutUser = 1,     //1 踢出流（默认）
      KickStream_CancelKickOutUser   // 2 取消踢出流
   };

   class VHRoomMember {
   public:
      MemberStatus status;
      std::wstring thirdPartyUserId;
   };

   class Permission {
   public:
      bool kick_inav = false;                 //踢出互动房间 / 取消踢出互动房间
      bool kick_inav_stream = false;          //踢出路流 / 取消踢出流
      bool publish_inav_another = false;      //推旁路直播 / 结束推旁路直播
      bool apply_inav_publish = false;        //申请上麦
      bool publish_inav_stream = false;       //推流
      bool askfor_inav_publish = false;       //邀请推流
      bool audit_inav_publish = false;        //审核申请上麦
   };

   //聊天消息
   enum ChatMsgType {
      ChatMsgType_OnLine = 1,   //用户上线消息
      ChatMsgType_OffLine,      //用户下线消息
      ChatMsgType_Disable,      //禁言频道内某个用户的聊天（频道内单个用户的禁言）
      ChatMsgType_Disable_All,  // 禁言整个频道的聊天（全员禁言）
      ChatMsgType_Permit,        // 取消禁言频道内某个用户的聊天（取消频道内单个用户的禁言）
      ChatMsgType_Permit_All,    // 取消禁言整个频道的聊天（取消全员禁言）
   };

   class ChatMsgInfo {
   public:
   };
}


#endif