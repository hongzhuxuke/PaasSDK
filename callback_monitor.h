#ifndef __CALL_BACK_H__
#define __CALL_BACK_H__

#include "LiveEventInterface.h"
#include "paas_common.h"


/**============================================================================================================================================
**
**   互动SDK事件监听回调接口，所有接口回调处于SDK业务线程。如果回调处理app业务事件不建议在回调中进行。
**
**============================================================================================================================================*/

class VHRoomDelegate{
public:
   //********************************************************   接口调用回调成功与失败通知   ********************************************************//
   /*
   *   监听房间内业务调用成功事件
   *   code: RoomEvent 事件类型
   *   userData: 附带的json数据
   */
   virtual void OnSuccessedEvent(RoomEvent code, std::string userData = std::string()) = 0;
   /*
   *   监听互动房间内HTTP业务API调用失败事件
   *   respCode: 错误码
   *   msg：错误信息
   *   userData: 附带的json数据
   */
   virtual void OnFailedEvent(RoomEvent code, int respCode, const std::string& msg, std::string userData = std::string()) = 0;
   /*
   *   获取互动房间成员列表回调
   **/
   virtual void OnGetVHRoomMembers(const std::string& third_party_user_id, std::list<VHRoomMember>&) = 0;
   /*
   *   获取被踢出互动房间人列表回调
   **/
   virtual void OnGetVHRoomKickOutMembers(std::list<VHRoomMember>&) = 0;

   //********************************************************   房间在线链接状态   ********************************************************//
   /*
   *   房间网络状态连接
   */
   virtual void OnRoomConnectState(RoomConnectState) = 0;

   //********************************************************   广播消息监听事件   ********************************************************//
   /*
    *   收到申请上麦消息 ,消息为广播消息，收到消息后，通过进入互动房间时获取的权限列表，判断用户是否有审核权限
    */
   virtual void OnRecvBaseApplyInavPublishMsg(std::wstring& third_party_user_id) = 0;
   /*
   *   收到审核上麦消息 ,消息为广播消息，收到同意上麦后执行上麦操作
   */
   virtual void OnRecvAuditInavPublishMsg(const std::wstring& third_party_user_id, AuditPublish) = 0;
   /*
   *   邀请上麦消息  消息为广播消息，收到消息后，提示邀请信息
   */
   virtual void OnRecvBaseAskforInavPublishMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   踢出流消息  消息为广播消息，收到消息后，执行踢出流
   */
   virtual void OnRecvKickInavStreamMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   踢出互动房间 , 消息为广播消息，收到消息后，判断是当前用户后，执行踢出房间操作
   */
   virtual void OnRecvKickInavMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   上/下/拒绝上麦消息 消息为广播消息
   */
   virtual void OnUserPublishCallback(const std::wstring& third_party_user_id, const std::string& stream_id, PushStreamEvent event) = 0;
   /*
   *   互动房间关闭消息  接受到该消息后，所有在互动房间的人员，全部下麦，退出房间
   */
   virtual void OnRecvInavCloseMsg() = 0;
   /*
   *   用户上下线通知
   *   online： 用户上线/ 用户下线 / 禁言 / 取消禁言
   *   user_id： 用户id
   */
   virtual void OnServiceMsg(std::string msgType, const std::wstring user_id, const std::wstring nickname, const std::string rolename, bool is_banned, int devType) = 0;
};


class RecvMsgDelegate {
public:
   /*
   *   消息服务，所有消息监听。
   */
   virtual void OnRecvMsgEvent(const std::string) = 0;
   /*
   *   聊天服务，所有消息监听。
   */
   virtual void OnRecvChatMsg(const std::string) = 0;
};

#endif