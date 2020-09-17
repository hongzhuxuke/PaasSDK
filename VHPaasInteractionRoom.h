#pragma once
#include <string>
#include <list>
#include <Windows.h>
#include "callback_monitor.h"
#include "VideoRenderReceiver.h"

#ifdef  VHPAASSDK_EXPORT
#define VHPAASSDK_EXPORT     __declspec(dllimport)
#else
#define VHPAASSDK_EXPORT     __declspec(dllexport)
#endif

namespace vlive {

class VLiveRoom {
public:
    /*
    *   �ӿ�˵������ʼ�����ӿڱ������������е��á�
    *   ������VLiveRoomMsgEventDelegate ��������������¼�֪ͨ��
    *         VRtcEngineEventDelegate   ����RTC�����¼�֪ͨ��
    */
    virtual void InitSDK(VLiveRoomMsgEventDelegate* liveRoom_delegate, VRtcEngineEventDelegate* rtcRoom_delegate, const std::string domain = "http://api.vhallyun.com/sdk/v1", std::wstring logPath = std::wstring()) = 0;
    /*
    *   �ӿ�˵���������Ҫʹ��HTTP������Ҫ����[InitSDK]֮����ô˽ӿڣ���������д�����Ժ��Դ˽ӿڡ�
    *   ����˵����
    *    enable�� true��ʾ��ʼ��false��ʾ�ر�
    *    addr�� �����ַ
    *    port�� ����˿�
    *    userName�� �����û���
    *    pwd�� �����û�����
    **/
    virtual void SetHttpProxy(bool enable, std::string addr = std::string(), int port = 0 ,std::string userName = std::string(), std::string pwd = std::string()) = 0;
    /*
    *   �ӿ�˵������½�ӿڡ���¼΢��ҵ�����������¼�ɹ�֮�󼴿ɼ��롰����ý�巿�䡱
    *   �ӿڲ�����
    *      accesstoken��ͨ��ƽ̨��ȡ������token
    *      appid��Ӧ��id
    *      thrid_user_id���û�id
    *      inav_roomid����������id
    *      channel_id: ��������Ϣchannel_id
    *      context:  ��¼ʱ��ӵ��Զ����ֶΣ�json��ʽ
    *   �ص�����������RoomEvent::RoomEvent_Login�¼�
    */
    virtual bool LoginRoom(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id,const std::string& inav_roomid,const std::string& live_roomid,const std::string channel_id = std::string(), const std::string context = std::string()) = 0;
    /*
    *  �ӿ�˵�����ǳ�΢��ҵ�������������Ѿ����뻥�����䣬��Ҫ�ȵ���LeaveVHMediaRoom ��
    *  �ص�����������RoomEvent::RoomEvent_Logout�¼�
    */ 
    virtual void LogoutRoom() = 0;
    /**
    *  �ӿ�˵������ȡ��ǰ�û�Ȩ�ޡ�����Ȩ���ж���Щ���ܿ��á�
    */
    virtual Permission GetPermission() = 0;
    /**
    *  �ӿ�˵�������뻥��ý�巿��, ������[LoginRoom]��
    *  �ص�����������VHRoomConnectEnumö���¼�
    *  userData : �û��Զ����ֶ�
    **/
    virtual void JoinRtcRoom(std::string userData = std::string()) = 0;
    /*
    *  �ӿ�˵�����˳�����ý�巿��
    *  ����ֵ��true �˳��ɹ���false �˳�ʧ��
    */
    virtual bool LeaveRtcRoom() = 0;
    /*
    *  �ӿ�˵����SDK����������Ա�б�
    *  �ص�������
    *       �ɹ��������ص�OnGetVHRoomMembers
    *       ʧ�ܣ�OnFailedEvent
    */
    virtual void AsynGetVHRoomMembers() = 0;
    /**
    *  �ӿ�˵������������
    *  �ص������¼���RoomEvent::RoomEvent_Apply_Push
    *  ����ֵ��VhallLiveErrCode 
    */
    virtual int ApplyInavPublish() = 0;
    /**
    *  �ӿ�˵���������������
    *  ����˵����
    *    audit_user_id: ������˵������û�ID 
    *    type: �Ƿ�ͬ�⣬AuditPublish
    *  �ص������¼���RoomEvent::RoomEvent_AuditInavPublish
    *  ����ֵ��VhallLiveErrCode
    **/
    virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) = 0; 
    /**
    *  �ӿ�˵������������
    *  ����˵����
    *    audit_user_id: ������˵������û�ID
    *  �ص������¼���RoomEvent::RoomEvent_AskforInavPublish
    *  ����ֵ��VhallLiveErrCode
    **/
    virtual int AskforInavPublish(const std::string& audit_user_id) = 0;
    /**
    *  �ӿ�˵������/��/�ܾ�����״̬��ִ,�û��ɹ���������÷����API
    *  ����˵����
    *       stream_id: ��������ID
    *       type:      PushStreamEvent
    *  �ص������¼���RoomEvent::RoomEvent_UserPublishCallback
    */
    virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string()) = 0;
    /**
    *  �ӿ�˵�����߳���/ȡ���߳���
    *  ����˵����
    *     type: 1 �߳�����Ĭ�ϣ� 2 ȡ���߳���
    *     kick_user_id	string	��	���߳��ĵ������û�ID
    *  �ص������¼���RoomEvent::RoomEvent_KickInavStream
    *  ����ֵ��VhallLiveErrCode
    **/
    virtual int KickInavStream(const std::string& kick_user_id, KickStream type) = 0;
    /**
    *  �ӿ�˵�����߳���������/ȡ���߳���������
    *  ����˵����
    *    type:1 �߳��������䣨Ĭ�ϣ� 2 ȡ���߳���������
    *    kick_user_id:���߳��ĵ������û�ID
    *  �ص������¼���RoomEvent::RoomEvent_KickInav
    *  ����ֵ��VhallLiveErrCode
    **/
    virtual int KickInav(const std::string& kick_user_id, KickStream type) = 0;
    /*
    *  �ӿ�˵������ȡ���߳������������б�
    *  �ص�������OnGetVHRoomKickOutMembers��OnFailedEvent
    */
    virtual void GetKickInavList() = 0;
    /*
    *  �ӿ�˵����������·ֱ��,�˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�,���������湲���岥ʱ�������ʾ����������Ҫ����SetMainView�ӿڣ�
                  ����Ӧ����ID��Ϊ�����������ã����ֻ����ʾһ·��ͬʱ��Ҫ���ò���ģʽ����Ҫ�����ص�
    *  ����˵����
    *     live_room_id: ֱ������id
    *     layoutMode:�����˲���ģʽ
    *     width�� �����˲��ֵĿ�
    *     height�� �����˲��ֵĸ�
    *     fps��֡��
    *     bitrate�� ����
    *  �ص�������RoomEvent::RoomEvent_Start_PublishInavAnother
    *  ����ֵ��VhallLiveErrCode
    */
    virtual int StartPublishInavAnother(std::string live_room_id, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true, std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")) = 0;
    /*
    *  �ӿ�˵����������·ֱ���󣬿�ͨ���˽ӿ��޸���������
    */
    virtual int ChangeLayout(std::string layoutMode, std::string custom = std::string()) = 0;
    /**
    *  �ӿ�˵������������·ֱ��֮�󣬿��Խ�ĳ���û����������ڻ�������������浱�У��˽ӿ��ǻ�����������·����Ȩ�޵��û����е���
    *  �ص�������RoomEvent::RoomEvent_SetMainView
    *  ����ֵ��VhallLiveErrCode
    **/
    virtual int SetMainView(std::string stream_id) = 0;
    /*
    *   �ӿ�˵����ֹͣ��·ֱ�����˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�
    *   ����˵����
    *       live_room_id: ֱ������id
    *   �ص�������RoomEvent::RoomEvent_Stop_PublishInavAnother
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int StopPublishInavAnother(std::string live_room_id) = 0;
    /*
    *   �ӿ�˵�������������Ƿ��Ѿ����ӳɹ�
    *   ����ֵ��true�������������ӣ�false�����������ӶϿ�
    */
    virtual bool IsVHMediaConnected() = 0;
    /*
    *   �ӿ�˵������ȡ����ͷ�б�
    *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
    */
    virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras) = 0;
    /*
    *   �ӿ�˵������ȡ��˷��б�
    *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
    **/
    virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) = 0;
    /**
    *   �ӿ�˵������ȡ�������б�
    *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
    **/
    virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList) = 0;
    /*
    *  �ӿ�˵��������ʹ�õ���˷�
    *  ����˵����
    *   index: GetMicDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
    */
    virtual void SetUseMicIndex(int index,std::string devId) = 0;
    /*
    *  �ӿ�˵��������ʹ�õ�������
    *  ����˵����
    *   index: GetPlayerDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
    */
    virtual void SetUserPlayIndex(int index, std::string devId) = 0;
    /*
    *   �ӿ�˵�������õ�ǰʹ�õ���˷�����
    *   ����˵����
    *       vol:������Χ 0~100
    **/
    virtual bool SetCurrentMicVol(int vol) = 0;
    /*
    *   �ӿ�˵������ȡ��ǰʹ�õ���˷�����
    *   ����ֵ����ǰ����
    **/
    virtual int GetCurrentMicVol() = 0;
    /*
    *   �ӿ�˵�������õ�ǰʹ�õ�����������
    *   ����˵����
    *       vol:������Χ 0~100
    **/
    virtual bool SetCurrentPlayVol(int vol) = 0;
    /*
    *   �ӿ�˵������ȡ��ǰʹ�õ�����������
    *   ����ֵ�����ص�ǰ������ֵ
    **/
    virtual int GetCurrentPlayVol() = 0;
    /*
    *   �ӿ�˵�����ر�����ͷ
    *   ����ֵ��true �رճɹ�/ false �ر�ʧ��
    */
    virtual bool CloseCamera() = 0;
    /*
    *   �ӿ�˵�������Ѿ���ʼ�ɼ�������ͷ
    *   ����ֵ��true �򿪳ɹ�/ false ��ʧ��
    */
    virtual bool OpenCamera() = 0;
    /*
    *   �ӿ�˵��������ͷ�Ƿ�ر�
    *   ����ֵ��true �Ѿ��ر�/ false δ�ر�
    */
    virtual bool IsCameraOpen() = 0;
    /*
    *   �ӿ�˵�����ر���˷�
    *   ����ֵ��true �رճɹ�/ false �ر�ʧ��
    */
    virtual bool CloseMic() = 0;
    /*
    *   �ӿ�˵��������˷�
    *   ����ֵ��true �򿪳ɹ�/ false ��ʧ��
    */
    virtual bool OpenMic() = 0;
    /*
    *   �ӿ�˵������˷��Ƿ��
    *   ����ֵ��true �Ѿ��ر�/ false δ�ر�
    */
    virtual bool IsMicOpen() = 0;
    /**
    *  �������湲����.��Ҫ�������ɼ�����ʱ������������ɫ������ʱ������ͨ���˽ӿ�����������ȣ���߹������������ȵ�Ч����
    *  �˽ӿڵ��ã���Ҫ�����湲��ɼ��ɹ�֮����е��á�������[OnOpenCaptureCallback]�ص��¼���
    *  ���ر�ʱ�������湲���������ʱ�رա�
    */
    virtual int SetDesktopEdgeEnhance(bool enable) = 0;

    /*
    *   �ӿ�˵�����򿪡��ر�Զ���û�������Ƶ
    *   ����˵����
    *       user_id���û�id
    *       close��true �ر�/ false ��
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false) = 0;
    /*
    *   �ӿ�˵�����򿪡��ر�Զ���û���������
    *   ����˵����
    *       user_id���û�id
    *       close��true �ر�/ false ��
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int OperateRemoteUserAudio(std::wstring user_id, bool close = false) = 0;
    /*  
    *   �ӿ�˵������ʼ����ͷ\��˷�ɼ�  ����ͷ�ɼ���ͼƬ�ɼ������л�ʹ�ò��Ҳ�Ӱ�������� 
    *   ����˵����
    *       VideoProfileIndex��ͨ����ȡ�豸��Ϣʱ�õ�VideoDevProperty::mDevFormatList���豸֧�ֵķֱ��ʣ����ݷֱ�����Ϣѡ����ӦVideoProfileIndex
    *   ������OnOpenCaptureCallback
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int StartLocalCapture(const std::string devId,VideoProfileIndex index, bool doublePush = false) = 0;
    /*
    *   �ӿ�˵����ͼƬ�ɼ�. �л�ͼƬ���滻����ͷ������
    *   ����˵����
    *       VideoProfileIndex���ֱ���
    *   ������OnOpenCaptureCallback
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false) = 0;
    /**
    *   ��ʼ����ɼ���ɼ���������
    *   ������
    *       dev_index:�豸����
    *       dev_id;�豸id
    *       float:�ɼ����� 0-100
    **/
    virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring dev_id, const int volume) = 0;
    /*
    *   ֹͣ������Ƶ�ɼ�
    */
    virtual int StopLocalCapturePlayer() = 0;
    /*
    *   �ӿ�˵����ֹͣ����ͷ\ͼƬ�ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
    */
    virtual void StopLocalCapture() = 0;
    /*
    *   �ӿ�˵������ʼ����ͷ�������� �������ȴ�����ͷ֮���������
    *   �ص�������OnPushStreamSuc OnPushStreamError
    */
    virtual int StartPushLocalStream() = 0;
    /*
    *   �ӿ�˵����ֹͣ����ͷ��ͼƬ�������� 
    *   �ص������� OnStopPushStreamCallback
    */
    virtual int StopPushLocalStream() = 0;
    /**
    *  �ӿ�˵��������ͷԤ����
    **/
    virtual int StartPreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
    /**
   *   �ӿ�˵��������ͷԤ����
   **/
   virtual int StartPreviewCamera(std::shared_ptr<vhall::VideoRenderReceiveInterface> recv, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
   /**
   *   �ӿ�˵������������ͷԤ����
   **/
   virtual int StopPreviewCamera() = 0;
   /**
   *  �ӿ�˵��������Ƿ�֧�����չ���
   **/
   virtual bool IsSupprotBeauty() = 0;
    /*
   *   �ӿ�˵������������ͷ���ռ���Level����Ϊ0-5������level=0�Ǳ�ʾ�رգ�������Ҫ��level����Ϊ1-5����ֵ��
   *   �˽ӿڵ���������ͷ������ر�ǰ���þ����ԡ����ı��ֵʱ���������ʵʱ��Ч��
   *   ��Ҫ�ر�ע����ǣ�����豸��һ�δ򿪣���ʹ��[PreviewCamera]�ӿ�ֱ�Ӵ򿪲���ʾ���ԡ����ǵ�
   *   ���豸�Ѿ���ռ�ã�����ʹ��[bool StartRenderLocalStream(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> recv); ]
   *   �ӿڽ��б������Զ�����Ⱦ����Ԥ��û�л���ʱ��ͨ���豸ID����ƥ�䣬�Ƿ���յ����豸���ݣ�������յ���ͨ����vhall::VideoRenderReceiveInterface�����ص�����
   *   �����Զ�����Ⱦ��
   *   �ص������� OnStopPushStreamCallback
   */
    virtual int SetCameraBeautyLevel(int level) = 0;
    /*
   *   �ӿ�˵��������Ԥ������ͷ���ռ���Level����Ϊ0-5������level=0�Ǳ�ʾ�رգ�������Ҫ��level����Ϊ1-5����ֵ��
   */
    virtual int SetPreviewCameraBeautyLevel(int level) = 0;
    /*
    *   �ӿ�˵������ǰ�ɼ������Ƿ����ڽ��б�������Դ�ɼ�
    */
    virtual bool IsCapturingStream(VHStreamType streamType) = 0;
    /**
    *   �ӿ�˵������ǰ���������Ƿ�������湲����Ƶ��
    */
    virtual bool IsUserPushingDesktopStream() = 0;
    /**
    *   �ӿ�˵������ǰ���������Ƿ���ڲ岥��Ƶ��
    *   ����ֵ��ture�ɹ���falseʧ��
    */
    virtual bool IsUserPushingMediaFileStream() = 0;
    /**
    *   �ӿ�˵������ǰ�ɼ������Ƿ���������
    *   ����ֵ��ture�ɹ���falseʧ��
    */
    virtual bool IsPushingStream(VHStreamType streamType) = 0;
    /**
    *   �ӿ�˵������ȡ���û�����ID
    *   ����˵����
    *       user_id���û�id
    *       streamType: ������
    *   ����ֵ����id
    */
    virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType) = 0;
    /*
    *   �ӿ�˵������ʼ��Ⱦý��������
    *   ����˵����
    *       user_id���û�id
    *       wnd�� ��Ⱦ���ھ��
    *       streamType: ������
    *   ����ֵ��ture�ɹ���falseʧ��
    */
    virtual bool StartRenderStream(const std::wstring& user,void*  wnd, vlive::VHStreamType streamType) = 0;
    /*
    *   �ӿ�˵������ʼ���湲��ɼ�  
    *   ������OnOpenCaptureCallback
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) = 0;
    /*
    *   �ӿ�˵����ֹͣ���湲��ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
    */
    virtual void StopDesktopCapture() = 0;
    /*
    *   �ӿ�˵������ʼ���湲��ɼ����� 
    *   �ص���⣺OnPushStreamSuc OnPushStreamError
    */
    virtual int StartPushDesktopStream() = 0;
    /*
    *   �ӿ�˵����ֹͣ���湲��ɼ����� 
    *   ��Ҫ�ص����:OnStopPushStreamCallback
    *   ����ֵ��VhallLiveErrCode
    */
    virtual int StopPushDesktopStream() = 0;
    /*
    *   �ӿ�˵������ʼ���ļ��岥����
    *   ����ֵ��VhallLiveErrCode
    *   �ص�������OnOpenCaptureCallback
    */
    virtual int InitMediaFile() = 0;
    /*
    *   �ӿ�˵����ֹͣ�岥�ļ�, ����Ѿ���ʼ������ͬʱֹͣ����
    */
    virtual void StopMediaFileCapture() = 0;
    /*
    *   �ӿ�˵������ʼ�岥�ļ����� 
    *   ����ֵ��VhallLiveErrCode
    *   �ص���⣺OnPushStreamError  OnPushStreamSuc
    */
    virtual int StartPushMediaFileStream() = 0;
    /*
    *   �ӿ�˵����ֹͣ�岥�ļ�����
    *   �ص���⣺OnStopPushMediaFileStream
    */
    virtual void StopPushMediaFileStream() = 0;
    /**
    **  ���š������ɹ�����Ե��ô˽ӿڽ����ļ�����
    */
    virtual bool PlayFile(std::string file, VideoProfileIndex profileIndex) = 0;
    /*
    *   �ӿ�˵�����岥�ļ���ͣ����
    *   ����ֵ��VhallLiveErrCode
    */
    virtual void MediaFilePause() = 0;
    /*
    *   �岥�ļ��ָ�����
    *   ����ֵ��VhallLiveErrCode
    */
    virtual void MediaFileResume() = 0;
    /*
    *   �ӿ�˵�����岥�ļ��������
    *   ����ֵ��VhallLiveErrCode
    */
    virtual void MediaFileSeek(const unsigned long long& seekTimeInMs) = 0;
    /*
    *   �ӿ�˵�����岥�ļ���ʱ��
    */
    virtual const long long MediaFileGetMaxDuration() = 0;
    /*  
    *   �ӿ�˵�����岥�ļ���ǰʱ��
    */
    virtual const long long MediaFileGetCurrentDuration() = 0;
    /*
    *   �ӿ�˵�����岥�ļ��ĵ�ǰ״̬ 
    *   ����ֵ������״̬  MEDIA_FILE_PLAY_STATE
    */
    virtual int MediaGetPlayerState() = 0;
	/*
    *   �ӿ�˵�������»�ȡ��ǰ�˺�Ȩ��
    */
	virtual void ReFreshPermission() = 0;
    /**
    *  �򿪻�ر����ж��ĵ���Ƶ
    */
    virtual int MuteAllSubScribeAudio(bool mute) = 0;
};

VHPAASSDK_EXPORT VLiveRoom* GetPaasSDKInstance();
VHPAASSDK_EXPORT void DestoryPaasSDKInstance();

}