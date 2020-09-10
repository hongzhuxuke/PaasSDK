#pragma once
#include <string>
#include <list>
#include <Windows.h>
#include "callback_monitor.h"
#include "WebRtcSDKInterface.h"

#ifdef  VHPAASSDK_EXPORT
#define VHPAASSDK_EXPORT     __declspec(dllimport)
#else
#define VHPAASSDK_EXPORT     __declspec(dllexport)
#endif

namespace vlive {

   class VHPaasInteractionRoom {
   public:
      /*
      *   �ӿ�˵�������ڱ��ص���ʹ�á�
      */
      virtual void SetDebugLogAddr(const std::string& addr){};
      /*
      *   �ӿ�˵������ʼ������ʼ���ӿڱ������������е��á�
      *   room_delegate�� ������Ϣ�¼��ص�
      *   rtc_deleage��   �����¼���Ϣ�ص�
      */
      virtual int InitSDK(WebRtcSDKEventInterface* obj, VHRoomDelegate * VHRoomDelegate,
         WebRtcSDKEventInterface* rtc_deleage, std::wstring logPath = std::wstring(),
         const std::string domain = "http://api.vhallyun.com/sdk/v1") = 0;
      virtual bool isConnetWebRtcRoom() = 0;
      virtual void ClearInitInfo() = 0;
      /*��ʼ������ý�巿���������*/
      virtual void InitRtcRoomParameter(const InavInfo& inavInfo, const std::string& log_server,
         const std::string& InavRoomId, const std::string& Appid) = 0;
      /*
      *   �ӿ�˵������½�ӿڡ���¼΢�𻥶�ҵ���������
      *   �ӿڲ�����
      *      accesstoken��ͨ��ƽ̨��ȡ������token
      *      appid��Ӧ��id
      *      thrid_user_id���û�id
      *      inav_roomid����������id
      *      listenChannel: ��������Ϣchannel_id �����緿��id,�����ŵ�id�ȡ�
      *      context:  ��¼ʱ��ӵ��Զ����ֶΣ�json��ʽ
      *      �ص�������VHRoomDelegate
      */
      virtual bool Login(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id, const std::string& inav_roomid, const std::string& live_roomid, const std::list<std::string> listenChannel, const std::string context = std::string()) = 0;
      /*
      *  ����Ͽ������ӿڡ�
      *  ���ص�OnRoomConnectState���յ� RoomConnectState_NetWorkDisConnect ��ʾ���������쳣����Ҫ�ֶ�������
      */
      virtual int ReconnectNetWork() = 0;
      /*
      *  �ӿ�˵�����ǳ�����������
      *  �ص�������VHRoomDelegate
      */
      virtual void LogOut() = 0;
      /**
      *  �ӿ�˵������ȡ��ǰ�û�Ȩ�ޡ�����Ȩ���ж���Щ���ܿ��á�
      */
      virtual Permission GetPermission() = 0;
      /*
      *  �ӿ�˵�������»�ȡ��ǰ�˺�Ȩ��
      *  �ص�������VHRoomDelegate
      */
      virtual void ReFreshPermission() = 0;
      /**
      *  �ӿ�˵�������뻥��ý�巿��
      *  �ص�������RtcSDKEventDelegate
      *  userData : �û��Զ����ֶΣ�Զ�˶���ʱ�ɽ�������ֶΡ�
      **/
      virtual bool JoinRTCRoom(std::string userData = std::string(), bool bEnableSimulCaset = true,
         std::string vid = std::string(), std::string vfid = std::string(), std::string strWebinarId = std::string(),
      int role = 1, int classType = 1) = 0;
      /*
      *  �ӿ�˵�����˳�����ý�巿��
      *  ����ֵ��true �˳��ɹ���false �˳�ʧ��
      */
      virtual bool LeaveRTCRoom() = 0;
      /*
      *  ��ֹ��������ȡ������Զ����
      */
      virtual void DisableSubScribeStream() = 0;
      /*
      *   ʹ�ܶ���������ʼ���ķ����е�Զ����
      */
      virtual void EnableSubScribeStream() = 0;
      /*
      *   �����Ƿ�������
      */
      virtual bool IsWebRtcRoomConnected() = 0;
      /*
      *  �ӿ�˵����SDK����������Ա�б�
      *  �ص�������VHRoomDelegate
      */
      virtual void AsynGetVHRoomMembers() = 0;
      /**
      *  �ӿ�˵������������
      *  �ص�������VHRoomDelegate
      *  ����ֵ��VhallLiveErrCode
      */
      virtual int ApplyInavPublish() = 0;
      /**
      *  �ӿ�˵���������������
      *  ����˵����
      *    audit_user_id: ������˵������û�ID
      *    type: �Ƿ�ͬ�⣬AuditPublish
      *  �ص�������VHRoomDelegate
      *  ����ֵ��VhallLiveErrCode
      **/
      virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) = 0;
      /**
      *  �ӿ�˵������������
      *  ����˵����
      *    audit_user_id: ������˵������û�ID
      *  �ص�������VHRoomDelegate
      *  ����ֵ��VhallLiveErrCode
      **/
      virtual int AskforInavPublish(const std::string& audit_user_id) = 0;
      /**
      *  �ӿ�˵������/��/�ܾ�����״̬��ִ,�û��ɹ���������÷����API
      *  ����˵����
      *       stream_id: ��������ID
      *       type:      PushStreamEvent
      *  �ص�������VHRoomDelegate
      */
      virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string()) = 0;
      /**
      *  �ӿ�˵�����߳���/ȡ���߳���
      *  ����˵����
      *     type: 1 �߳�����Ĭ�ϣ� 2 ȡ���߳���
      *     kick_user_id	string	��	���߳��ĵ������û�ID
      *  �ص�������VHRoomDelegate
      *  ����ֵ��VhallLiveErrCode
      **/
      virtual int KickInavStream(const std::string& kick_user_id, KickStream type) = 0;
      /**
      *  �ӿ�˵�����߳���������/ȡ���߳���������
      *  ����˵����
      *    type:1 �߳��������䣨Ĭ�ϣ� 2 ȡ���߳���������
      *    kick_user_id:���߳��ĵ������û�ID
      *  �ص�������VHRoomDelegate
      *  ����ֵ��VhallLiveErrCode
      **/
      virtual int KickInav(const std::string& kick_user_id, KickStream type) = 0;
      /*
      *  �ӿ�˵������ȡ���߳������������б�
      *  �ص�������VHRoomDelegate
      */
      virtual void GetKickInavList() = 0;
      /*
      *  �ӿ�˵����������·ֱ��,�˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�,���������湲���岥ʱ�������ʾ����������Ҫ����SetMainView�ӿڣ�
                    ����Ӧ����ID��Ϊ�����������ã����ֻ����ʾһ·��ͬʱ��Ҫ���ò���ģʽ����Ҫ�����ص�
      *  ����˵����
      *     layoutMode:�����˲���ģʽ
      *     profileIndex: ��·�ֱ���
      *  �ص�������RtcSDKEventDelegate
      *  ����ֵ��VhallLiveErrCode
      */
      virtual int StartConfigBroadCast(LayoutMode layoutMode,
         BroadCastVideoProfileIndex profileIndex, bool showBorder = true,
         std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")
         , SetBroadCastEventCallback fun = nullptr) = 0;

      /*  �ӿ�˵����������·ֱ���󣬿�ͨ���˽ӿ��޸Ļ�������
      *  �ص�������RtcSDKEventDelegate
      */
      //virtual int ChangeLayout(LayoutMode layoutMode, std::string custom = std::string());
      virtual int SetConfigBroadCastLayOut(LayoutMode mode, SetBroadCastEventCallback fun = nullptr) = 0;

      /**
      *  �ӿ�˵������������·ֱ��֮�󣬿��Խ�ĳ���û����������ڻ�������������浱�У��˽ӿ��ǻ�����������·����Ȩ�޵��û����е���
      *  �ص�������RtcSDKEventDelegate
      *  ����ֵ��  VhallLiveErrCode
      **/
      virtual int SetMainView(std::string stream_id/*,int stream_type*/, SetBroadCastEventCallback fun/* = nullptr*/) = 0;
      /*
      *   �ӿ�˵����ֹͣ��·ֱ�����˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�
      *   ����˵����
      *       live_room_id: ֱ������id
      *  �ص�������RtcSDKEventDelegate
      *   ����ֵ��VhallLiveErrCode
      */
      virtual int StopPublishInavAnother() = 0;
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
      *  ��ȡ����ͷ�б�����
      */
      virtual int GetCameraDevDetails(std::list<CameraDetailsInfo> &cameraDetails) = 0;
      /*
      *   �ӿ�˵������ȡ��˷��б�
      *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
      **/
      virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) = 0;
      /*
      *  �Ƿ������Ƶ����Ƶ�豸��
      *  ����ֵ��ֻҪ����һ���豸 ����true, �������Ƶ�豸��û���򷵻�false
      **/
      virtual bool HasVideoOrAudioDev() = 0;
      virtual bool HasVideoDev() = 0;
      virtual bool HasAudioDev() = 0;
      /**
      *   �ӿ�˵������ȡ�������б�
      *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
      **/
      virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList) = 0;
      /*
      *  �ӿ�˵��������ʹ�õ���˷磬�˽ӿ�Ϊȫ�ֽӿڡ��������֮�󣬻�ʵʱ��Ч����������˷�ɼ����豸�Ѿ����л���
      *  ����˵����
      *   index: GetMicDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
      */
      virtual void SetUseMicIndex(int index, std::string devId, std::wstring desktopCaptureId) = 0;
      /*
      *  �ӿ�˵��������ʹ�ò���������������
      *  ����˵����
      *   index: GetPlayerDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
      */
      virtual void SetUsePlayer(int index, std::string devId) = 0;
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
      *  ��ȡ��ǰʹ�õ������豸��Ϣ
      ***/
      virtual void GetCurrentCameraInfo(int &index, std::string& devId) = 0;
      /**
      * ����ͷ����Ԥ������Ԥ������֮����ҪֹͣԤ������������ͷ����һֱռ��
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int PreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
      /*
      *  ����Ԥ��Mic����
      */
      virtual int GetMicVolumValue() = 0;
      /*
*  ����Ԥ��mic
*/
      virtual void ChangePreViewMic(int micIndex) = 0;
      /*
      *  ֹͣԤ������
      */
      virtual int StopPreviewCamera() = 0;
      /*
      *   �ӿ�˵������ʼ����ͷ\��˷�ɼ�  ����ͷ�ɼ���ͼƬ�ɼ������л�ʹ�ò��Ҳ�Ӱ��������
      *   ����˵����
      *       devId�� ����ͷ�豸id
      *       VideoProfileIndex�������ֱ���
      *       doublePush: ����ʱ���Ƿ���˫����
      *   �ص�������RtcSDKEventDelegate
      *   ����ֵ��VhallLiveErrCode
      */
      virtual int StartLocalCapture(const std::string camera_devId, VideoProfileIndex index, bool doublePush = false) = 0;
      virtual int StartLocalAuxiliaryCapture(const std::string devId, VideoProfileIndex index) = 0;  //˫�ƻ�������
      /*
      *   �ӿ�˵����ͼƬ�ɼ�. �л�ͼƬ���滻����ͷ������
      *   ����˵����
      *       filePath��ͼƬ·����
      *       VideoProfileIndex���ֱ���
      *       doublePush: ����ʱ���Ƿ���˫����
      *   ������OnOpenCaptureCallback
      *   ����ֵ��VhallLiveErrCode
      */
      virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false) = 0;
      /*
      *   �ӿ�˵������ʼ����ͷ/ͼƬ�������� �������ȴ�����ͷ֮�����������
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int StartPushLocalStream() = 0;
      virtual int StartPushLocalAuxiliaryStream(std::string context = std::string()) = 0;
      /*
      *   �ӿ�˵����ֹͣ����ͷ��ͼƬ��������
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int StopPushLocalStream() = 0;
      virtual int StopPushLocalAuxiliaryStream() = 0;
      /*
      *   �ӿ�˵����ֹͣ����ͷ\ͼƬ�ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
      */
      virtual void StopLocalCapture() = 0;
      virtual void StopLocalAuxiliaryCapture() = 0;
      /**
      *   �ӿ�˵������ʼ����ɼ������ô˽ӿڱ���ȷ������˷��豸�������ù���˷磬�����ɼ��쳣��
      *   ������
      *       player_dev; ����ɼ�ʹ�õ��������豸id
      *       float:�ɼ����� 0-100
      **/
      virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring player_dev, const int volume) = 0;
      /*
      *   ֹͣ������Ƶ�ɼ�
      */
      virtual int StopLocalCapturePlayer() = 0;
      ///**
      //*   �ӿ�˵������ǰ���������Ƿ�������湲����Ƶ��
      //*/
      //virtual bool IsUserPushingDesktopStream() = 0;
      ///**
      //*   �ӿ�˵������ǰ���������Ƿ���ڲ岥��Ƶ��
      //*   ����ֵ��ture�ɹ���falseʧ��
      //*/
      //virtual bool IsUserPushingMediaFileStream() = 0;
      /**
      *   �ӿ�˵������ǰ�ɼ������Ƿ���������
      *   ����ֵ��ture�ɹ���falseʧ��
      */
      virtual bool IsPushingStream(VHStreamType streamType) = 0;
      /*
      *   �ӿ�˵������ǰ�ɼ������Ƿ����ڽ��б�������Դ�ɼ�
      */
      virtual bool IsCapturingStream(VHStreamType streamType) = 0;
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
      *       wnd�� ��Ⱦ���ھ��
      *       streamType: ������
      *   ����ֵ��ture�ɹ���falseʧ��
      */
      virtual bool StartRenderStream(void* wnd, vlive::VHStreamType streamType) = 0;   
      /*
      *   �ӿ�˵������ʼ��Ⱦý��������
      *   ����˵����
      *       user_id���û�id
      *       wnd�� ��Ⱦ���ھ��
      *       streamType: ������
      *   ����ֵ��ture�ɹ���falseʧ��
      */
      virtual bool StartRenderSubscribeStream(const std::wstring& user, vlive::VHStreamType streamType, void* wnd) = 0;
      /*
      *    ��ȡ���ڹ�������湲���б�
      */
      virtual std::vector<DesktopCaptureSource> GetDesktops(int streamType) = 0;
      /*
      *   �ӿ�˵������ʼ���湲��ɼ�,.
      *    index:GetDesktops����ֵ��DesktopCaptureSource.id
      *   �ص�������RtcSDKEventDelegate
      *   ����ֵ��VhallLiveErrCode
      */
      virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) = 0;
      /*
      *   �ӿ�˵����ֹͣ���湲��ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
      */
      virtual void StopDesktopCapture() = 0;
      /*
      *   �ӿ�˵������ʼ���湲��ɼ�����
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int StartPushDesktopStream(std::string context = std::string()) = 0;
      /*
      *   �ӿ�˵����ֹͣ���湲��ɼ�����
      *   �ص�������RtcSDKEventDelegate
      *   ����ֵ��VhallLiveErrCode
      */
      virtual int StopPushDesktopStream() = 0;
      /*
      * ����ѡ������Դ
      */
      virtual int SelectSource(int64_t id) = 0;
      /*
      * ֹͣ���Դ����ɼ�
      */
      virtual void StopSoftwareCapture() = 0;
      /*
      * ֹͣ�������ɼ�����
      */
      virtual int StopPushSoftWareStream() = 0;
      /*
      * ��ʼ�������ɼ�����
      */
      virtual int StartPushSoftWareStream() = 0;
      /*
      *   �ж�ͼƬ��Ч��
      */
      virtual int CheckPicEffectiveness(const std::string filePath) = 0;
      /**
      *  ��ȡ�岥�ļ�ԭʼ��С
      */
      virtual int GetPlayMeidaFileWidthAndHeight(std::string filePath, int& srcWidth, int &srcHeight) = 0;
      /*
      *   �ӿ�˵������ʼ�岥�ļ�
      *   ����ֵ��VhallLiveErrCode
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int StartPlayMediaFile(std::string filePath, VideoProfileIndex profileIndex = RTC_VIDEO_PROFILE_720P_16x9_H, long long seekPos = 0) = 0;
      /*
      *  �岥�ļ��Ƿ������Ƶ���档
      */
      virtual bool IsPlayFileHasVideo() = 0;
      /*
      *   ���ò岥�ļ�����
      */
      virtual int MediaFileVolumeChange(int vol) = 0;
      /**
      *   �л��岥�����ֱ���
      */
      virtual void ChangeMediaFileProfile(VideoProfileIndex profileIndex) = 0;
      /*
      *   �ӿ�˵����ֹͣ�岥�ļ�, ����Ѿ���ʼ������ͬʱֹͣ����
      */
      virtual void StopMediaFileCapture() = 0;
      /*
      *   �ӿ�˵������ʼ�岥�ļ�����
      *   ����ֵ��VhallLiveErrCode
      *   �ص�������RtcSDKEventDelegate
      */
      virtual int StartPushMediaFileStream() = 0;
      /*
      *   �ӿ�˵����ֹͣ�岥�ļ�����
      *   �ص�������RtcSDKEventDelegate
      */
      virtual void StopPushMediaFileStream() = 0;
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
      /**
      *  �ӿ�˵������������
      *   �ص�������RtcSDKEventDelegate
      */
      virtual void SubScribeRemoteStream(const std::string &stream_id, int delayTimeOut = 0) = 0;
      /*
       *  �ӿ�˵���������յ�Զ�����ݺ󣬿����޸Ķ��ĵĻ���Ϊ������С��.
      *   �ص�������RtcSDKEventDelegate
      */
      virtual bool ChangeSubScribeUserSimulCast(const std::wstring& user_id, vlive::VHStreamType streamType, VHSimulCastType type) = 0;
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
      /**
      *  �򿪻�ر����ж��ĵ���Ƶ
      */
      virtual int MuteAllSubScribeAudio(bool mute) = 0;
      /**
      *   �ӿ�˵����ע����Ϣ�����ӿڣ��û������ڼ����ײ����е���Ϣ�¼����ݡ�����û�����ע����ͨ�� InitSDK �ӿ�ע���VHRoomDelegate���м�����
      */
      virtual void RegisterMsgListenr(RecvMsgDelegate*) = 0;
      /*
      *   ֹͣ��������Զ��������
      */
      virtual void StopRecvAllRemoteStream() = 0;
      /*
      *   ִ��paasSdk����
      */
      virtual void implementTask(int iTask) = 0;
      /*
      *   ��������׼�����
      */
      virtual bool IsEnableConfigMixStream() = 0;
      virtual void SetEnableConfigMixStream(bool enable) = 0;
      /*
      *   ���ر���˫���� id
      */
      virtual std::string GetLocalAuxiliaryId() = 0;
      /*
      *   ����Զ��˫���� id
      */
      virtual std::string GetAuxiliaryId() = 0;
      /*
*   ����������
*/
      virtual VHStreamType  CalcStreamType(const bool& bAudio, const bool& bVedio) = 0;
      /*
*   ���òɼ�������������ֵ
*/
      virtual int SetLocalCapturePlayerVolume(const int volume) = 0;
      /*
*   ֹͣ������Ƶ�ļ��ӿ�
*/
      virtual int StopPlayAudioFile() = 0;

      /*
*   ��ȡ�����ļ�����
*/
      virtual int GetPlayAudioFileVolum() = 0;
      /*
*   ��Ӳ�����Ƶ�ļ��ӿ�
*/
      virtual int PlayAudioFile(std::string fileName, int devIndex) = 0;

      /*
   *   �л���һ������ͷ
   */
      virtual int ChangeNextCamera(VideoProfileIndex videoProfile) = 0;
      /**
      *  ��ȡ��ǰʹ�õ������豸��Ϣ
      ***/
      virtual void GetCurrentCamera(std::string& devId) = 0;
      /**
*  ��ȡ��ǰʹ�õ���˷��豸��Ϣ
***/
      virtual void GetCurrentMic(int &index, std::string& devId) = 0;
      /*
      *��ȡ���ж��ĵ���Ƶ״̬(��/�ر�)
      */
      virtual bool GetMuteAllAudio() = 0;
      /*
*  ����ʹ�õ���˷�
*  index: GetMicDevices��ȡ���б��� vhall::AudioDevProperty�е� devIndex
*/
      virtual int SetUsedMic(int micIndex, std::string micDevId, std::wstring desktopCaptureId) = 0;
      /*
      *  �жϸ���ý�������Ƿ�֧��
      */
      virtual bool IsSupportMediaFormat(CaptureStreamAVType type) = 0;
      //��ȾԶ����
      virtual bool StopRenderRemoteStream(const std::wstring& user, const std::string streamId, vlive::VHStreamType streamType) = 0;
      virtual bool StartRenderRemoteStreamByInterface(const std::wstring& user, vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive) = 0;
      virtual bool IsRemoteStreamIsExist(const std::wstring& user, vlive::VHStreamType streamType) = 0;
      /**
*   ��ʼ��Ⱦ"����"�����豸�����湲���ļ��岥ý��������.
*/
      virtual bool StartRenderLocalStream(vlive::VHStreamType streamType, void* wnd) = 0;
      virtual bool StartRenderLocalStreamByInterface(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive) = 0;
      virtual bool IsStreamExit(std::string id) = 0;
      /**
*   ��ȡ�������������ͣ��Ƿ��������Ƶ���ݡ�
*   ����ֵ��0 ���ҳɹ�������ʧ��
*/
      virtual int GetSubScribeStreamFormat(const std::wstring& user, vlive::VHStreamType streamType, bool &hasVideo, bool &hasAudio) = 0;
      /**
*   ��ȡ��ID
*   ����ֵ���������id������id ���򷵻ؿ��ַ���
*/
      virtual std::string GetSubScribeStreamId(const std::wstring& user, vlive::VHStreamType streamType) = 0;
      /*��ǰ�����Ƿ���ڸ������͵�Զ����*/
      virtual bool IsExitSubScribeStream(const vlive::VHStreamType& streamType) = 0;
      virtual bool IsExitSubScribeStream(const std::string& id, const vlive::VHStreamType& streamType) = 0;

      virtual void GetStreams(std::list<std::string>& streams) = 0;
      virtual void ClearSubScribeStream() = 0;
      virtual std::string LocalStreamId() = 0;
      /**
*  ��ȡ������Ƶ������.3���һ��
*/
      virtual double GetPushDesktopVideoLostRate() = 0;
   };

   VHPAASSDK_EXPORT VHPaasInteractionRoom* GetPaasSDKInstance();
   VHPAASSDK_EXPORT void DestoryPaasSDKInstance();

}