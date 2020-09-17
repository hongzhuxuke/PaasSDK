#pragma once
#include "LiveEventInterface.h"
#include "paas_common.h"

namespace vlive {
   /*
 *  VLiveRoomMsgEventDelegate 为房间内消息事件通知。主要用于监听相关
 *  连麦邀请、踢出等消息事件。
 */
   class VLiveRoomMsgEventDelegate {
   public:
      /*
      *   监听业务API调用成功事件。包括[loginRoom]等接口调用，具体监听事件查看RoomEvent
      *   code: RoomEvent 事件类型
      *   userData: 附带的json数据
      */
      virtual void OnSuccessedEvent(vlive::RoomEvent code, std::string userData = std::string()) = 0;
      /*
      *   监听业务API调用失败事件。包括[loginRoom]等接口调用，具体监听事件查看RoomEvent
      *   respCode: 错误码
      *   msg：错误信息
      *   userData: 附带的json数据
      */
      virtual void OnFailedEvent(vlive::RoomEvent code, int respCode, std::string msg) = 0;
      /*
      *   收到申请上麦消息 ,消息为广播消息，收到消息后，通过进入互动房间时获取的权限列表，判断用户是否有审核权限
      */
      virtual void OnRecvApplyInavPublishMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   收到审核上麦消息 ,消息为广播消息，收到同意上麦后执行上麦操作
      */
      virtual void OnRecvAuditInavPublishMsg(const std::wstring&  third_party_user_id, vlive::AuditPublish) = 0;
      /*
      *   邀请上麦消息  消息为广播消息，收到消息后，提示邀请信息
      */
      virtual void OnRecvAskforInavPublishMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   踢出流消息  消息为广播消息，收到消息后，执行踢出流
      */
      virtual void OnRecvKickInavStreamMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   踢出互动房间 , 消息为广播消息，收到消息后，判断是当前用户后，执行踢出房间操作
      */
      virtual void OnRecvKickInavMsg(const std::wstring&  third_party_user_id) = 0;
      /*
      *   上/下/拒绝上麦消息 消息为广播消息
      */
      virtual void OnUserPublishCallback(const std::wstring& third_party_user_id, const std::string&  stream_id, vlive::PushStreamEvent event) = 0;
      /*
      *   互动房间关闭消息  接受到该消息后，所有在互动房间的人员，全部下麦，退出房间
      */
      virtual void OnRecvInavCloseMsg() = 0;
      /*
      *   用户上下线通知
      *   ChatMsgType： 用户上线/ 用户下线 / 禁言 / 取消禁言
      */
      virtual void OnRecvChatCtrlMsg(const vlive::ChatMsgType msgType, const char* msg) = 0;
      /*
      *   获取互动房间成员列表回调
      **/
      virtual void OnGetVHRoomMembers(const std::string& third_party_user_id, std::list<vlive::VHRoomMember>&) = 0;
      /*
      *   获取被踢出互动房间人列表回调
      **/
      virtual void OnGetVHRoomKickOutMembers(std::list<vlive::VHRoomMember>&) = 0;
      /*
      *   接收Socket消息
      */
      virtual void OnRecvSocketIOMsg(vlive::SocketIOMsgType msgType, std::string) = 0;
   };
}


/*
**===================================
**
**   互动SDK事件监听回调接口，所有接口回调处于SDK业务线程。
**   如果回调处理app业务事件不建议在回调中进行。
**
**===================================
*/
//class VHRoomMonitor :public VHRoomRecvInavMsgCallback, public RtcSDKEventDelegate {
//public:
//
//};

