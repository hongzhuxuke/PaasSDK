#pragma once
#include "LiveEventInterface.h"
#include "paas_common.h"

namespace vlive {
   /*
 *  VLiveRoomMsgEventDelegate Ϊ��������Ϣ�¼�֪ͨ����Ҫ���ڼ������
 *  �������롢�߳�����Ϣ�¼���
 */
   class VLiveRoomMsgEventDelegate {
   public:
      /*
      *   ����ҵ��API���óɹ��¼�������[loginRoom]�Ƚӿڵ��ã���������¼��鿴RoomEvent
      *   code: RoomEvent �¼�����
      *   userData: ������json����
      */
      virtual void OnSuccessedEvent(vlive::RoomEvent code, std::string userData = std::string()) = 0;
      /*
      *   ����ҵ��API����ʧ���¼�������[loginRoom]�Ƚӿڵ��ã���������¼��鿴RoomEvent
      *   respCode: ������
      *   msg��������Ϣ
      *   userData: ������json����
      */
      virtual void OnFailedEvent(vlive::RoomEvent code, int respCode, std::string msg) = 0;
      /*
      *   �յ�����������Ϣ ,��ϢΪ�㲥��Ϣ���յ���Ϣ��ͨ�����뻥������ʱ��ȡ��Ȩ���б��ж��û��Ƿ������Ȩ��
      */
      virtual void OnRecvApplyInavPublishMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   �յ����������Ϣ ,��ϢΪ�㲥��Ϣ���յ�ͬ�������ִ���������
      */
      virtual void OnRecvAuditInavPublishMsg(const std::wstring&  third_party_user_id, vlive::AuditPublish) = 0;
      /*
      *   ����������Ϣ  ��ϢΪ�㲥��Ϣ���յ���Ϣ����ʾ������Ϣ
      */
      virtual void OnRecvAskforInavPublishMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   �߳�����Ϣ  ��ϢΪ�㲥��Ϣ���յ���Ϣ��ִ���߳���
      */
      virtual void OnRecvKickInavStreamMsg(const std::wstring& third_party_user_id) = 0;
      /*
      *   �߳��������� , ��ϢΪ�㲥��Ϣ���յ���Ϣ���ж��ǵ�ǰ�û���ִ���߳��������
      */
      virtual void OnRecvKickInavMsg(const std::wstring&  third_party_user_id) = 0;
      /*
      *   ��/��/�ܾ�������Ϣ ��ϢΪ�㲥��Ϣ
      */
      virtual void OnUserPublishCallback(const std::wstring& third_party_user_id, const std::string&  stream_id, vlive::PushStreamEvent event) = 0;
      /*
      *   ��������ر���Ϣ  ���ܵ�����Ϣ�������ڻ����������Ա��ȫ�������˳�����
      */
      virtual void OnRecvInavCloseMsg() = 0;
      /*
      *   �û�������֪ͨ
      *   ChatMsgType�� �û�����/ �û����� / ���� / ȡ������
      */
      virtual void OnRecvChatCtrlMsg(const vlive::ChatMsgType msgType, const char* msg) = 0;
      /*
      *   ��ȡ���������Ա�б�ص�
      **/
      virtual void OnGetVHRoomMembers(const std::string& third_party_user_id, std::list<vlive::VHRoomMember>&) = 0;
      /*
      *   ��ȡ���߳������������б�ص�
      **/
      virtual void OnGetVHRoomKickOutMembers(std::list<vlive::VHRoomMember>&) = 0;
      /*
      *   ����Socket��Ϣ
      */
      virtual void OnRecvSocketIOMsg(vlive::SocketIOMsgType msgType, std::string) = 0;
   };
}


/*
**===================================
**
**   ����SDK�¼������ص��ӿڣ����нӿڻص�����SDKҵ���̡߳�
**   ����ص�����appҵ���¼��������ڻص��н��С�
**
**===================================
*/
//class VHRoomMonitor :public VHRoomRecvInavMsgCallback, public RtcSDKEventDelegate {
//public:
//
//};

