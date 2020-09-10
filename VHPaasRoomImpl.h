#pragma once
#include "vh_room_msg.h"
#include "VHPaasInteractionRoom.h"
#include "VhallNetWorkInterface.h"
#include "paas_common.h"
#include "VHWebSocketInterface.h"
#include <atomic>
#include <thread>
//#include "LogReport.h"
#include "thread_lock.h"
#include <mutex>

////���Ի���
//#define HOST_URL "http://t-open.e.vhall.com/sdk/v1"
//
////���ϻ���
////#define HOST_URL "http://api.vhallyun.com/sdk/v1"
class LogReport;
class CThreadLock;

#define PAASSDK_VER    "1.0.0.0"
#define DIP_1080P   "1080P"
#define DIP_720P    "720P"
#define DIP_540P    "540P"
#define DIP_480P    "480P"
#define DIP_240P    "240P"

//�����壩
#define DPI_SD      "SD"
//�����壩
#define DPI_HD      "HD"
// �����壩
#define DPI_UHD     "UHD"
//CUSTOM
#define DPI_CUSTOM     "CUSTOM"

class BroadCastInfo {
public:
   std::string live_room_id;
   LayoutMode layoutMode;
   int width;
   int height;
   int fps;
   int bitrate;
   std::string stream_id;
   BROAD_CAST_DPI dpi;
   std::string dpiDesc;
};

enum eTask
{
   eTask_BroadCast, //�ϱ�������·״̬
};

using namespace vlive;
class VHPaasRoomImpl :public VHPaasInteractionRoom, public WebSocketCallBackInterface {
public:
   VHPaasRoomImpl();
   virtual ~VHPaasRoomImpl();

public:
   virtual void OnOpen();
   virtual void OnFail();
   virtual void OnReconnecting();
   virtual void OnReconnect(unsigned, unsigned);
   virtual void OnClose();
   virtual void OnSocketOpen(std::string const& nsp);
   virtual void OnSocketClose(std::string const& nsp);
   virtual void OnServiceMsg(std::string type, const std::wstring user_id, const std::wstring nickname, const std::string rolename, bool is_banned, int devType = -1, int uv = 0);
   virtual void OnRecvAllMsg(const char*, int length);
   /*��ʼ������ý�巿���������*/
   virtual void InitRtcRoomParameter(const InavInfo& inavInfo,const std::string& log_server, 
      const std::string& InavRoomId, const std::string& Appid);
   /*
   *   �ӿ�˵�������ڱ��ص���ʹ�á�
   */
   virtual void SetDebugLogAddr(const std::string& addr);
   /*
   *   �ӿ�˵������ʼ������ʼ���ӿڱ������������е��á�
   *   room_delegate�� ������Ϣ�¼��ص�
   *   rtc_deleage��   �����¼���Ϣ�ص�
   */
   virtual int InitSDK(WebRtcSDKEventInterface* obj, VHRoomDelegate * VHRoomDelegate, 
      WebRtcSDKEventInterface* rtc_deleage, std::wstring logPath = std::wstring(),
      const std::string domain = "http://api.vhallyun.com/sdk/v1"
      );
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
   virtual bool Login(const std::string& accesstoken, 
      const std::string& appid, 
      const std::string& thrid_user_id,
      const std::string& inav_roomid, 
      const std::string& live_roomid, 
      const std::list<std::string> listenChannel, 
      const std::string context = std::string()) ;
   /*
   *  ����Ͽ������ӿڡ�
   *  ���ص�OnRoomConnectState���յ� RoomConnectState_NetWorkDisConnect ��ʾ���������쳣����Ҫ�ֶ�������
   */
   virtual int ReconnectNetWork();
   /*
   *  �ӿ�˵�����ǳ�����������
   *  �ص�������VHRoomDelegate
   */
   virtual void LogOut();
   /**
   *  �ӿ�˵������ȡ��ǰ�û�Ȩ�ޡ�����Ȩ���ж���Щ���ܿ��á�
   */
   virtual Permission GetPermission() ;
   /*
   *  �ӿ�˵�������»�ȡ��ǰ�˺�Ȩ��
   *  �ص�������VHRoomDelegate
   */
   virtual void ReFreshPermission();
   virtual void ClearInitInfo();
   /**
   *  �ӿ�˵�������뻥��ý�巿��
   *  �ص�������RtcSDKEventDelegate
   *  userData : �û��Զ����ֶΣ�Զ�˶���ʱ�ɽ�������ֶΡ�
   **/
   virtual bool JoinRTCRoom(std::string userData = std::string(), bool bEnableSimulCaset = true, 
      std::string vid = std::string(),std::string vfid = std::string(), std::string strWebinarId = std::string()
   ,int role = 1, int classType = 1);
   virtual bool isConnetWebRtcRoom();
   /*
   *  �ӿ�˵�����˳�����ý�巿��
   *  ����ֵ��true �˳��ɹ���false �˳�ʧ��
   */
   virtual bool LeaveRTCRoom();
   /*
*  ��ֹ��������ȡ������Զ����
*/
   virtual void DisableSubScribeStream();
   /*
*   ʹ�ܶ���������ʼ���ķ����е�Զ����
*/
   virtual void EnableSubScribeStream();
   /*
   *   �����Ƿ�������
   */
   virtual bool IsWebRtcRoomConnected();
   /*
   *  �ӿ�˵����SDK����������Ա�б�
   *  �ص�������VHRoomDelegate
   */
   virtual void AsynGetVHRoomMembers();
   /**
   *  �ӿ�˵������������
   *  �ص�������VHRoomDelegate
   *  ����ֵ��VhallLiveErrCode
   */
   virtual int ApplyInavPublish();
   /**
   *  �ӿ�˵���������������
   *  ����˵����
   *    audit_user_id: ������˵������û�ID
   *    type: �Ƿ�ͬ�⣬AuditPublish
   *  �ص�������VHRoomDelegate
   *  ����ֵ��VhallLiveErrCode
   **/
   virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) ;
   /**
   *  �ӿ�˵������������
   *  ����˵����
   *    audit_user_id: ������˵������û�ID
   *  �ص�������VHRoomDelegate
   *  ����ֵ��VhallLiveErrCode
   **/
   virtual int AskforInavPublish(const std::string& audit_user_id);
   /**
   *  �ӿ�˵������/��/�ܾ�����״̬��ִ,�û��ɹ���������÷����API
   *  ����˵����
   *       stream_id: ��������ID
   *       type:      PushStreamEvent
   *  �ص�������VHRoomDelegate
   */
   virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string());
   /**
   *  �ӿ�˵�����߳���/ȡ���߳���
   *  ����˵����
   *     type: 1 �߳�����Ĭ�ϣ� 2 ȡ���߳���
   *     kick_user_id	string	��	���߳��ĵ������û�ID
   *  �ص�������VHRoomDelegate
   *  ����ֵ��VhallLiveErrCode
   **/
   virtual int KickInavStream(const std::string& kick_user_id, KickStream type);
   /**
   *  �ӿ�˵�����߳���������/ȡ���߳���������
   *  ����˵����
   *    type:1 �߳��������䣨Ĭ�ϣ� 2 ȡ���߳���������
   *    kick_user_id:���߳��ĵ������û�ID
   *  �ص�������VHRoomDelegate
   *  ����ֵ��VhallLiveErrCode
   **/
   virtual int KickInav(const std::string& kick_user_id, KickStream type);
   /*
   *  �ӿ�˵������ȡ���߳������������б�
   *  �ص�������VHRoomDelegate
   */
   virtual void GetKickInavList();
   /*
   *  �ӿ�˵����������·ֱ��,�˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�,���������湲���岥ʱ�������ʾ����������Ҫ����SetMainView�ӿڣ�
                 ����Ӧ����ID��Ϊ�����������ã����ֻ����ʾһ·��ͬʱ��Ҫ���ò���ģʽ����Ҫ�����ص�
   *  ����˵����
   *     layoutMode:�����˲���ģʽ
   *     profileIndex: ��·�ֱ���
   *  �ص�������RtcSDKEventDelegate
   *  ����ֵ��VhallLiveErrCode
   */
   virtual int StartConfigBroadCast(LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true,
      std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")
      , SetBroadCastEventCallback fun = nullptr);


   /*
   *  �ӿ�˵����������·ֱ���󣬿�ͨ���˽ӿ��޸Ļ�������
   *  �ص�������RtcSDKEventDelegate
   */
   //virtual int ChangeLayout(LayoutMode layoutMode, std::string custom = std::string());
   virtual int SetConfigBroadCastLayOut(LayoutMode mode, SetBroadCastEventCallback fun = nullptr);

   /**
   *  �ӿ�˵������������·ֱ��֮�󣬿��Խ�ĳ���û����������ڻ�������������浱�У��˽ӿ��ǻ�����������·����Ȩ�޵��û����е���
   *  �ص�������RtcSDKEventDelegate
   *  ����ֵ��  VhallLiveErrCode
   **/
   virtual int SetMainView(std::string stream_id/*, int stream_type*/, SetBroadCastEventCallback fun = nullptr);
   /*
   *   �ӿ�˵����ֹͣ��·ֱ�����˽ӿ��ǻ�����������·����Ȩ�޵��û����е��õ�
   *   ����˵����
   *       live_room_id: ֱ������id
   *  �ص�������RtcSDKEventDelegate
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int StopPublishInavAnother();
   /*
   *   �ӿ�˵�������������Ƿ��Ѿ����ӳɹ�
   *   ����ֵ��true�������������ӣ�false�����������ӶϿ�
   */
   virtual bool IsVHMediaConnected();
   /*
   *   �ӿ�˵������ȡ����ͷ�б�
   *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
   */
   virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras);
   /*
   *  �Ƿ������Ƶ����Ƶ�豸��
   *  ����ֵ��ֻҪ����һ���豸 ����true, �������Ƶ�豸��û���򷵻�false
   **/
   virtual bool HasVideoOrAudioDev() ;
   virtual bool HasVideoDev();
   virtual bool HasAudioDev();
   /*
   *   �ӿ�˵������ȡ��˷��б�
   *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
   **/
   virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList);
   /*
    *  ��ȡ����ͷ�б�����
    */
   virtual int GetCameraDevDetails(std::list<CameraDetailsInfo> &cameraDetails);
   /**
   *   �ӿ�˵������ȡ�������б�
   *   ����˵�������ػ�ȡ�����豸�б�����豸�б�Ϊ��˵��û�л�ȡ����Ӧ���豸
   **/
   virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList);
   /*
   *  �ӿ�˵��������ʹ�õ���˷磬�˽ӿ�Ϊȫ�ֽӿڡ��������֮�󣬻�ʵʱ��Ч����������˷�ɼ����豸�Ѿ����л���
   *  ����˵����
   *   index: GetMicDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
   */
   virtual void SetUseMicIndex(int index, std::string devId, std::wstring desktopCaptureId);
   /*
   *  �ӿ�˵��������ʹ�ò���������������
   *  ����˵����
   *   index: GetPlayerDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
   */
   virtual void SetUsePlayer(int index, std::string devId) ;
   /*
   *   �ӿ�˵�������õ�ǰʹ�õ���˷�����
   *   ����˵����
   *       vol:������Χ 0~100
   **/
   virtual bool SetCurrentMicVol(int vol);
   /*
   *   �ӿ�˵������ȡ��ǰʹ�õ���˷�����
   *   ����ֵ����ǰ����
   **/
   virtual int GetCurrentMicVol();
   /*
   *   �ӿ�˵�������õ�ǰʹ�õ�����������
   *   ����˵����
   *       vol:������Χ 0~100
   **/
   virtual bool SetCurrentPlayVol(int vol);
   /*
   *   �ӿ�˵������ȡ��ǰʹ�õ�����������
   *   ����ֵ�����ص�ǰ������ֵ
   **/
   virtual int GetCurrentPlayVol();
   /*
   *   �ӿ�˵�����ر�����ͷ
   *   ����ֵ��true �رճɹ�/ false �ر�ʧ��
   */
   virtual bool CloseCamera();
   /*
   *   �ӿ�˵�������Ѿ���ʼ�ɼ�������ͷ
   *   ����ֵ��true �򿪳ɹ�/ false ��ʧ��
   */
   virtual bool OpenCamera();
   /*
   *   �ӿ�˵��������ͷ�Ƿ�ر�
   *   ����ֵ��true �Ѿ��ر�/ false δ�ر�
   */
   virtual bool IsCameraOpen();
   /*
   *   �ӿ�˵�����ر���˷�
   *   ����ֵ��true �رճɹ�/ false �ر�ʧ��
   */
   virtual bool CloseMic();
   /*
   *   �ӿ�˵��������˷�
   *   ����ֵ��true �򿪳ɹ�/ false ��ʧ��
   */
   virtual bool OpenMic();
   /*
   *   �ӿ�˵������˷��Ƿ��
   *   ����ֵ��true �Ѿ��ر�/ false δ�ر�
   */
   virtual bool IsMicOpen();
   /**
   *  ��ȡ��ǰʹ�õ������豸��Ϣ
   ***/
   virtual void VHPaasRoomImpl::GetCurrentCameraInfo(int &index, std::string& devId);
   /**
   * ����ͷ����Ԥ������Ԥ������֮����ҪֹͣԤ������������ͷ����һֱռ��
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int PreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex /*= -1*/);
   /*
*  ����Ԥ��Mic����
*/
   virtual int GetMicVolumValue();
   /*
*  ����Ԥ��mic
*/
   virtual void ChangePreViewMic(int micIndex);
   /*
   *  ֹͣԤ������
   */
   virtual int StopPreviewCamera();
   /*
   *   �ӿ�˵������ʼ����ͷ\��˷�ɼ�  ����ͷ�ɼ���ͼƬ�ɼ������л�ʹ�ò��Ҳ�Ӱ��������
   *   ����˵����
   *       devId�� ����ͷ�豸id
   *       VideoProfileIndex�������ֱ���
   *       doublePush: ����ʱ���Ƿ���˫����
   *   �ص�������RtcSDKEventDelegate
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int StartLocalCapture(const std::string camera_devId, VideoProfileIndex index, bool doublePush = false);
   virtual int StartLocalAuxiliaryCapture(const std::string devId, VideoProfileIndex index);  //˫�ƻ�������
   /*
   *   �ӿ�˵����ͼƬ�ɼ�. �л�ͼƬ���滻����ͷ������
   *   ����˵����
   *       filePath��ͼƬ·����
   *       VideoProfileIndex���ֱ���
   *       doublePush: ����ʱ���Ƿ���˫����
   *   ������OnOpenCaptureCallback
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false);
   /*
   *   �ӿ�˵������ʼ����ͷ/ͼƬ�������� �������ȴ�����ͷ֮�����������
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int StartPushLocalStream();
   virtual int StartPushLocalAuxiliaryStream(std::string context = std::string());
   /*
   *   �ӿ�˵����ֹͣ����ͷ��ͼƬ��������
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int StopPushLocalStream();
   virtual int StopPushLocalAuxiliaryStream();
   /*
   *   �ӿ�˵����ֹͣ����ͷ\ͼƬ�ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
   */
   virtual void StopLocalCapture();
   virtual void StopLocalAuxiliaryCapture();
   /**
   *   �ӿ�˵������ʼ����ɼ������ô˽ӿڱ���ȷ������˷��豸�������ù���˷磬�����ɼ��쳣��
   *   ������
   *       player_dev; ����ɼ�ʹ�õ��������豸id
   *       float:�ɼ����� 0-100
   **/
   virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring player_dev, const int volume);
   /*
   *   �ӿ�˵������ǰ�ɼ������Ƿ����ڽ��б�������Դ�ɼ�
   */
   virtual bool IsCapturingStream(VHStreamType streamType);
   /*
   *   ֹͣ������Ƶ�ɼ�
   */
   virtual int StopLocalCapturePlayer();
   ///**
   //*   �ӿ�˵������ǰ���������Ƿ�������湲����Ƶ��
   //*/
   //virtual bool IsUserPushingDesktopStream();
   ///**
   //*   �ӿ�˵������ǰ���������Ƿ���ڲ岥��Ƶ��
   //*   ����ֵ��ture�ɹ���falseʧ��
   //*/
   //virtual bool IsUserPushingMediaFileStream();
   /**
   *   �ӿ�˵������ǰ�ɼ������Ƿ���������
   *   ����ֵ��ture�ɹ���falseʧ��
   */
   virtual bool IsPushingStream(VHStreamType streamType);
   /**
   *   �ӿ�˵������ȡ���û�����ID
   *   ����˵����
   *       user_id���û�id
   *       streamType: ������
   *   ����ֵ����id
   */
   virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType);
   /*
   *   �ӿ�˵������ʼ��Ⱦý��������
   *   ����˵����
   *       wnd�� ��Ⱦ���ھ��
   *       streamType: ������
   *   ����ֵ��ture�ɹ���falseʧ��
   */
   virtual bool StartRenderStream(void* wnd, vlive::VHStreamType streamType);
   /*
   *   �ӿ�˵������ʼ��Ⱦý��������
   *   ����˵����
   *       user_id���û�id
   *       wnd�� ��Ⱦ���ھ��
   *       streamType: ������
   *   ����ֵ��ture�ɹ���falseʧ��
   */
   virtual bool StartRenderSubscribeStream(const std::wstring& user, vlive::VHStreamType streamType, void* wnd);
   /*
   *    ��ȡ���ڹ�������湲���б�
   */
   virtual std::vector<DesktopCaptureSource> GetDesktops(int streamType);
   /*
   *   �ӿ�˵������ʼ���湲��ɼ�,.
   *    index:GetDesktops����ֵ��DesktopCaptureSource.id
   *   �ص�������RtcSDKEventDelegate
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex);
   /*
   *   �ӿ�˵����ֹͣ���湲��ɼ�  ����Ѿ���ʼ������ͬʱֹͣ����
   */
   virtual void StopDesktopCapture();
   /*
   *   �ӿ�˵������ʼ���湲��ɼ�����
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int StartPushDesktopStream(std::string context = std::string()) ;
   /*
   *   �ӿ�˵����ֹͣ���湲��ɼ�����
   *   �ص�������RtcSDKEventDelegate
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int StopPushDesktopStream() ;
   /*
   * ����ѡ������Դ
   */
   virtual int SelectSource(int64_t id);
   /*
   * ֹͣ���Դ����ɼ�
   */
   virtual void StopSoftwareCapture();
   /*
   * ֹͣ�������ɼ�����
   */
   virtual int StopPushSoftWareStream();
   /*
   * ��ʼ�������ɼ�����
   */
   virtual int StartPushSoftWareStream();
   /*
   *   �ж�ͼƬ��Ч��
   */
   virtual int CheckPicEffectiveness(const std::string filePath);
   /**
   *  ��ȡ�岥�ļ�ԭʼ��С
   */
   virtual int GetPlayMeidaFileWidthAndHeight(std::string filePath, int& srcWidth, int &srcHeight);
   /*
   *   �ӿ�˵������ʼ�岥�ļ�
   *   ����ֵ��VhallLiveErrCode
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int StartPlayMediaFile(std::string filePath, VideoProfileIndex profileIndex = RTC_VIDEO_PROFILE_720P_16x9_H, long long seekPos = 0);
   /*
   *  �岥�ļ��Ƿ������Ƶ���档
   */
   virtual bool IsPlayFileHasVideo();        
   /*
   *   ���ò岥�ļ�����
   */
   virtual int MediaFileVolumeChange(int vol);
   /**
   *   �л��岥�����ֱ���
   */
   virtual void ChangeMediaFileProfile(VideoProfileIndex profileIndex);

   /*
   *   �ӿ�˵����ֹͣ�岥�ļ�, ����Ѿ���ʼ������ͬʱֹͣ����
   */
   virtual void StopMediaFileCapture();
   /*
   *   �ӿ�˵������ʼ�岥�ļ�����
   *   ����ֵ��VhallLiveErrCode
   *   �ص�������RtcSDKEventDelegate
   */
   virtual int StartPushMediaFileStream();
   /*
   *   �ӿ�˵����ֹͣ�岥�ļ�����
   *   �ص�������RtcSDKEventDelegate
   */
   virtual void StopPushMediaFileStream();
   /*
   *   �ӿ�˵�����岥�ļ���ͣ����
   *   ����ֵ��VhallLiveErrCode
   */
   virtual void MediaFilePause();
   /*
   *   �岥�ļ��ָ�����
   *   ����ֵ��VhallLiveErrCode
   */
   virtual void MediaFileResume();
   /*
   *   �ӿ�˵�����岥�ļ��������
   *   ����ֵ��VhallLiveErrCode
   */
   virtual void MediaFileSeek(const unsigned long long& seekTimeInMs) ;
   /*
   *   �ӿ�˵�����岥�ļ���ʱ��
   */
   virtual const long long MediaFileGetMaxDuration();
   /*
   *   �ӿ�˵�����岥�ļ���ǰʱ��
   */
   virtual const long long MediaFileGetCurrentDuration();
   /*
   *   �ӿ�˵�����岥�ļ��ĵ�ǰ״̬
   *   ����ֵ������״̬  MEDIA_FILE_PLAY_STATE
   */
   virtual int MediaGetPlayerState() ;
   /**
   *  �ӿ�˵������������
   *   �ص�������RtcSDKEventDelegate
   */
   virtual void SubScribeRemoteStream(const std::string &stream_id, int delayTimeOut = 0);
   /*
    *  �ӿ�˵���������յ�Զ�����ݺ󣬿����޸Ķ��ĵĻ���Ϊ������С��.
   *   �ص�������RtcSDKEventDelegate
   */
   virtual bool ChangeSubScribeUserSimulCast(const std::wstring& user_id, vlive::VHStreamType streamType, VHSimulCastType type);
   /*
   *   �ӿ�˵�����򿪡��ر�Զ���û�������Ƶ
   *   ����˵����
   *       user_id���û�id
   *       close��true �ر�/ false ��
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false);
   /*
   *   �ӿ�˵�����򿪡��ر�Զ���û���������
   *   ����˵����
   *       user_id���û�id
   *       close��true �ر�/ false ��
   *   ����ֵ��VhallLiveErrCode
   */
   virtual int OperateRemoteUserAudio(std::wstring user_id, bool close = false) ;
   /**
   *  �򿪻�ر����ж��ĵ���Ƶ
   */
   virtual int MuteAllSubScribeAudio(bool mute);
   /**
   *   �ӿ�˵����ע����Ϣ�����ӿڣ��û������ڼ����ײ����е���Ϣ�¼����ݡ�����û�����ע����ͨ�� InitSDK �ӿ�ע���VHRoomDelegate���м�����
   */
   virtual void RegisterMsgListenr(RecvMsgDelegate*) ;
   ///*
   //*   ֹͣ��������Զ��������
   //*/
   //virtual void StopRecvAllRemoteStream();

   virtual void implementTask(int iTask);
   /*
*   ��������׼�����
*/
   virtual bool IsEnableConfigMixStream();
   virtual void SetEnableConfigMixStream(bool enable);
   static DWORD WINAPI ThreadProcess(LPVOID p);
   void ProcessTask();
   /*
   *   ���ر���˫���� id
   */
   virtual std::string GetLocalAuxiliaryId();
   /*
*   ����Զ��˫���� id
*/
   virtual std::string GetAuxiliaryId();
   /*
*   ����������
*/
   VHStreamType  CalcStreamType(const bool& bAudio, const bool& bVedio);
   /*
*   ���òɼ�������������ֵ
*/
   virtual int SetLocalCapturePlayerVolume(const int volume);
   /*
*   ֹͣ������Ƶ�ļ��ӿ�
*/
   virtual int StopPlayAudioFile();
   /*
*   ��ȡ�����ļ�����
*/
   virtual int GetPlayAudioFileVolum();
   /*
*   ��Ӳ�����Ƶ�ļ��ӿ�
*/
   virtual int PlayAudioFile(std::string fileName, int devIndex);
   /*
*   �л���һ������ͷ
*/
   virtual int ChangeNextCamera(VideoProfileIndex videoProfile);
   /**
*  ��ȡ��ǰʹ�õ������豸��Ϣ
***/
   virtual void GetCurrentCamera(std::string& devId);
   /**
*  ��ȡ��ǰʹ�õ���˷��豸��Ϣ
***/
   virtual void GetCurrentMic(int &index, std::string& devId);
   /*
*��ȡ���ж��ĵ���Ƶ״̬(��/�ر�)
*/
   virtual bool GetMuteAllAudio();
   /*
*  ����ʹ�õ���˷�
*  index: GetMicDevices��ȡ���б��� vhall::AudioDevProperty�е� devIndex
*/
   virtual int SetUsedMic(int micIndex, std::string micDevId, std::wstring desktopCaptureId);
   //��ȾԶ����
   virtual bool StopRenderRemoteStream(const std::wstring& user, const std::string streamId, vlive::VHStreamType streamType);
   virtual bool StartRenderRemoteStreamByInterface(const std::wstring& user, vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive);
   virtual bool IsRemoteStreamIsExist(const std::wstring& user, vlive::VHStreamType streamType);
   /**
*   ��ʼ��Ⱦ"����"�����豸�����湲���ļ��岥ý��������.
*/
   virtual bool StartRenderLocalStream(vlive::VHStreamType streamType, void* wnd);
   virtual bool StartRenderLocalStreamByInterface(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive);
   virtual bool IsStreamExit(std::string id);
   /**
*   ��ȡ�������������ͣ��Ƿ��������Ƶ���ݡ�
*   ����ֵ��0 ���ҳɹ�������ʧ��
*/
   virtual int GetSubScribeStreamFormat(const std::wstring& user, vlive::VHStreamType streamType, bool &hasVideo, bool &hasAudio);
   /**
*   ��ȡ��ID
*   ����ֵ���������id������id ���򷵻ؿ��ַ���
*/
   virtual std::string GetSubScribeStreamId(const std::wstring& user, vlive::VHStreamType streamType);
   /*��ǰ�����Ƿ���ڸ������͵�Զ����*/
   virtual bool IsExitSubScribeStream(const vlive::VHStreamType& streamType);
   virtual bool IsExitSubScribeStream(const std::string& id, const vlive::VHStreamType& streamType);
   virtual void GetStreams(std::list<std::string>& streams);
   virtual void ClearSubScribeStream();
   virtual void StopRecvAllRemoteStream();//ֹͣ����Զ����
   virtual std::string LocalStreamId();
   /**
*  ��ȡ������Ƶ������.3���һ��
*/
   virtual double GetPushDesktopVideoLostRate();
public:
   //void OnStartPublishInavAnother(int error_code, std::string res_msg, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true, std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000"));
   void OnStopPublishInavAnother(int error_code, std::string res_msg);
private:
   std::string GetBaseUrlInfo();
   std::string GetInvaStartUrl();
   std::string GetInavInfoUrl();
   std::string GetInavUserListUrl();
   std::string GetLeaveInavtUrl();
   std::string GetApplyInavPublishUrl();
   std::string GetAuditInavPublishUrl(const std::string& userid, int type);
   std::string GetAskforInavPublishUrl(const std::string& userid);
   std::string GetUserPublishCallbackUrl(const std::string& userid, int type);
   std::string GetKickInavStreamUrl(const std::string& user_id, int type);
   std::string GetKickInavUrl(const std::string& userid, int type);
   std::string GetKickInavListUrl();
   std::string GetAccessTokenPermission();
   std::string GetPublishInavAnother(const std::string& live_room_id, int start);
   std::string GetSetLayOut(std::string layoutMode, std::string custom = std::string());
   std::string GetPublishInavAnotherWithParam(const std::string& live_room_id, int start);
   std::string GetPushAnotherConfig(int state);//Ĭ��1������  2 �ر�
   std::string GetLayoutDesc(LayoutMode layout);

   void SendServerJoinMsg(const std::string context);
   void SendJoinMsg(std::string id, const std::string context);
   //void SetBroadCastCallBack(const std::string result, const std::string& msg);
   //void SetBroadCastCallBackWithParam(const std::string result, const std::string& msg);

   std::string WString2String(const std::wstring& ws);
   std::wstring String2WString(const std::string& s);
private:

   void UploadReportSDKInit();
   void UploadReportSDKLogin(std::string sdkAppid, std::string thrid_user_id);

private:
   void AnalysisStartInfo(std::string data, int code);
   void ParamToStartInfoResp(const std::string& msg);

   void ToGetInavInfo();
   void AnalysisGetInavInfo(std::string data, int code);
   void ReFreshPermissionAnalysisInfo(std::string data, int code);
   void ParamToInavInfoResp(std::string data);
   void RefFreshPermissionToInfo(std::string data);

   void AnalysisGetInavUserList(std::string data, int code);
   void AnalysisGetKickOutInavUserList(std::string data, int code);
   bool ParamRoomEvent(RoomEvent event, std::string data, int code, std::string userData);
   void CheckPermission(VHStreamType streamType, std::string data, int code, std::string context = std::string());
   void ParamRecvInavMsg(const std::string msg);
   void ParamRecvSocketMsg(const std::string msg);

   /*
   *  �ӿ�˵����ͬ��paas��������·
   *  �ص�������RtcSDKEventDelegate
   */
   void VHPaasRoomImpl::GetPublishInavAnotherWithParam();
   /*
   *  �жϸ���ý�������Ƿ�֧��
   */
   virtual bool IsSupportMediaFormat(CaptureStreamAVType type);
private:
   void GetVHRoomInfo();
   void PostTaskEvent(CPaasTaskEvent task);
private:
   VHRoomDelegate * mVHRoomDelegate = nullptr;
   //RtcSDKEventDelegate* mRtcDelegate = nullptr;
   //VHRoomMonitor * mpVHRoomMonitor = nullptr;                  //�ص�����
   WebRtcSDKEventInterface* mRtcDelegate = nullptr;

   RecvMsgDelegate *mMsgDelegate = nullptr;
   std::shared_ptr<HttpManagerInterface> mpHttpManagerInterface = nullptr;
   WebRtcSDKInterface* mpWebRtcSDKInterface = nullptr;
   VHWebSocketInterface* mpVHWebSocketInterface = nullptr;
   VHWebSocketInterface* mpVHMsgChatInterface = nullptr;
   LoginRespInfo mLoginInfo;
   InavInfo mInavInfo;
   std::mutex mInavInfoLock;

   std::string mAccesstoken;
   std::string mAppid;
   std::string mThridUserId;
   std::string mInavRoomId;
   std::string mLiveRoomId;
   std::list<std::string> mListenChannels;
   std::string mContext;
   std::string mDomain;
   std::atomic_bool bIsSettinglayout;
   std::atomic_bool mbLoginDone = false; //��½�ɹ� ֻ�ϱ�һ�Ρ��Ƿ��Ѿ�����socket.io

   bool bInit = false;
   bool mIsHost;

   BroadCastInfo mCurrentBroadCastInfo;
   LogReport* mpLogReport = nullptr;
   //http proxy
   std::atomic_bool bIsEnableProxy = false;
   std::string mProxyAddr;
   std::string mProxyUserName;
   std::string mProxyUserPwd;
   int mProxyPort;

   std::atomic_bool mbDisConnectWebSocket = false;

   std::thread* mProcessThread = nullptr;

   static HANDLE mStatic_TaskEvent;   //
   static HANDLE mStatic_ThreadExitEvent;
   static std::atomic_bool mStatic_bThreadRun; //
   static CThreadLock mStatic_taskListMutex;  //
   static std::list<CPaasTaskEvent> mStatic_taskEventList; //
};
