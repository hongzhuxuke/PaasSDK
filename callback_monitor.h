#ifndef __CALL_BACK_H__
#define __CALL_BACK_H__

#include "LiveEventInterface.h"
#include "paas_common.h"


/**============================================================================================================================================
**
**   ����SDK�¼������ص��ӿڣ����нӿڻص�����SDKҵ���̡߳�����ص�����appҵ���¼��������ڻص��н��С�
**
**============================================================================================================================================*/

class VHRoomDelegate{
public:
   //********************************************************   �ӿڵ��ûص��ɹ���ʧ��֪ͨ   ********************************************************//
   /*
   *   ����������ҵ����óɹ��¼�
   *   code: RoomEvent �¼�����
   *   userData: ������json����
   */
   virtual void OnSuccessedEvent(RoomEvent code, std::string userData = std::string()) = 0;
   /*
   *   ��������������HTTPҵ��API����ʧ���¼�
   *   respCode: ������
   *   msg��������Ϣ
   *   userData: ������json����
   */
   virtual void OnFailedEvent(RoomEvent code, int respCode, const std::string& msg, std::string userData = std::string()) = 0;
   /*
   *   ��ȡ���������Ա�б�ص�
   **/
   virtual void OnGetVHRoomMembers(const std::string& third_party_user_id, std::list<VHRoomMember>&) = 0;
   /*
   *   ��ȡ���߳������������б�ص�
   **/
   virtual void OnGetVHRoomKickOutMembers(std::list<VHRoomMember>&) = 0;

   //********************************************************   ������������״̬   ********************************************************//
   /*
   *   ��������״̬����
   */
   virtual void OnRoomConnectState(RoomConnectState) = 0;

   //********************************************************   �㲥��Ϣ�����¼�   ********************************************************//
   /*
    *   �յ�����������Ϣ ,��ϢΪ�㲥��Ϣ���յ���Ϣ��ͨ�����뻥������ʱ��ȡ��Ȩ���б��ж��û��Ƿ������Ȩ��
    */
   virtual void OnRecvBaseApplyInavPublishMsg(std::wstring& third_party_user_id) = 0;
   /*
   *   �յ����������Ϣ ,��ϢΪ�㲥��Ϣ���յ�ͬ�������ִ���������
   */
   virtual void OnRecvAuditInavPublishMsg(const std::wstring& third_party_user_id, AuditPublish) = 0;
   /*
   *   ����������Ϣ  ��ϢΪ�㲥��Ϣ���յ���Ϣ����ʾ������Ϣ
   */
   virtual void OnRecvBaseAskforInavPublishMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   �߳�����Ϣ  ��ϢΪ�㲥��Ϣ���յ���Ϣ��ִ���߳���
   */
   virtual void OnRecvKickInavStreamMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   �߳��������� , ��ϢΪ�㲥��Ϣ���յ���Ϣ���ж��ǵ�ǰ�û���ִ���߳��������
   */
   virtual void OnRecvKickInavMsg(const std::wstring& third_party_user_id) = 0;
   /*
   *   ��/��/�ܾ�������Ϣ ��ϢΪ�㲥��Ϣ
   */
   virtual void OnUserPublishCallback(const std::wstring& third_party_user_id, const std::string& stream_id, PushStreamEvent event) = 0;
   /*
   *   ��������ر���Ϣ  ���ܵ�����Ϣ�������ڻ����������Ա��ȫ�������˳�����
   */
   virtual void OnRecvInavCloseMsg() = 0;
   /*
   *   �û�������֪ͨ
   *   online�� �û�����/ �û����� / ���� / ȡ������
   *   user_id�� �û�id
   */
   virtual void OnServiceMsg(std::string msgType, const std::wstring user_id, const std::wstring nickname, const std::string rolename, bool is_banned, int devType) = 0;
};


class RecvMsgDelegate {
public:
   /*
   *   ��Ϣ����������Ϣ������
   */
   virtual void OnRecvMsgEvent(const std::string) = 0;
   /*
   *   �������������Ϣ������
   */
   virtual void OnRecvChatMsg(const std::string) = 0;
};

#endif