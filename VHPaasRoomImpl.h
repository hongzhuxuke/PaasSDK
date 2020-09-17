#pragma once
#include "vh_room_msg.h"
#include "VHPaasInteractionRoom.h"
#include "VhallNetWorkInterface.h"
#include "WebRtcSDKInterface.h"
#include "VHWebSocketInterface.h"
#include <atomic>
#include "LogReport.h"
#include <thread>
#include <mutex>
#include <atomic>

////���Ի���
//#define HOST_URL "http://t-open.e.vhall.com/sdk/v1"
//
////���ϻ���
////#define HOST_URL "http://api.vhallyun.com/sdk/v1"

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

namespace vlive{
 
enum TaskIndex {
   TaskIndex_ReConnect_Socket = 1,
};

class TaskData {
public:
   int task_id;
};

class BroadCastInfo {
public:
    LayoutMode layoutMode;
    BroadCastVideoProfileIndex profileIndex;
    bool showBorder;
    int width;
    int height;
    int fps;
    int bitrate;
    BROAD_CAST_DPI dpi;
    std::string stream_id;
    std::string live_room_id;
    std::string dpiDesc;
    std::string boradColor;
    std::string backGroundColor;
};
}


using namespace vlive;
class VHPaasRoomImpl :public VLiveRoom, public WebSocketCallBackInterface {
public:
    VHPaasRoomImpl();
    virtual ~VHPaasRoomImpl();

public:
    virtual void OnOpen();
    virtual void OnFail() ;
    virtual void OnReconnecting();
    virtual void OnReconnect(unsigned, unsigned);
    virtual void OnClose();
    virtual void OnSocketOpen(std::string const& nsp);
    virtual void OnSocketClose(std::string const& nsp) ;
    virtual void OnRecvChatCtrlMsg(const char* msgType, const char* msg);
    virtual void OnRecvAllMsg(const char *, int){};
    //��ʼ�����ӿڱ�����main�����е��á�
    virtual void InitSDK(VLiveRoomMsgEventDelegate* liveRoomDelegate, VRtcEngineEventDelegate* rtcRoomDelegate, const std::string domain = "http://api.vhallyun.com/sdk/v1", std::wstring logPath = std::wstring());

    virtual void SetHttpProxy(bool enable, std::string addr = std::string(), int port = 0, std::string userName = std::string(), std::string pwd = std::string());
    /****
    * ��¼����ҵ�������.��¼�ɹ�֮�󼴿ɼ��롰����ý�巿�䡱
    **/
    virtual bool LoginRoom(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id, const std::string& roomid, const std::string& live_roomid,std::string channel_id = std::string(), const std::string context = std::string());
    /**
    *  ��ȡ��ǰ�û�Ȩ��
    */
    virtual Permission GetPermission();
    /*
    *  �ǳ�����������
    */
    virtual void LogoutRoom() ;
    /**
    * ���뻥��ý�巿��
    **/
    virtual void JoinRtcRoom(std::string userData = std::string());
    /*
    * �˳�����ý�巿��
    */
    virtual bool LeaveRtcRoom();
    /*
    * SDK����������Ա�б�
    **/
    virtual void AsynGetVHRoomMembers();
    /**
    * ��������
    */
    virtual int ApplyInavPublish();
    /**
    * �����������
    * audit_user_id: ������˵������û�ID
    * type: �Ƿ�ͬ�� AuditPublish
    * ����ֵ��VhallLiveErrCode
    **/
    virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type);
    /**
    * ��������
    * audit_user_id: ������˵������û�ID
    **/
    virtual int AskforInavPublish(const std::string& audit_user_id);
    /**
    * ��/��/�ܾ�����״̬��ִ,�û��ɹ���������÷����API
    * stream_id: ��������ID
    * type	int	��	1 ����(Ĭ��) 2 ���� 3 �ܾ�����
    */
    virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string());
    /**
    * �߳���/ȡ���߳���  ʵ��������AccessToken
    * type	int	��	1 �߳�����Ĭ�ϣ� 2 ȡ���߳���
    * kick_user_id	string	��	���߳��ĵ������û�ID
    **/
    virtual int KickInavStream(const std::string& kick_user_id, KickStream type);
    /**
    *  �߳���������/ȡ���߳���������  �����ڽ��뷿��
    *   type	int	��	1 �߳��������䣨Ĭ�ϣ� 2 ȡ���߳���������
    *   kick_user_id	string	��	���߳��ĵ������û�ID
    * ����ֵ��VhallLiveErrCode
    **/
    virtual int KickInav(const std::string& kick_user_id, KickStream type);
    /*
    * ��ȡ���߳������������б�
    */
    virtual void GetKickInavList();

    /*
    *   ������·ֱ��
    *   live_room_id: ֱ������id
    *   width�� �����˲��ֵĿ�
    *   height�� �����˲��ֵĸ�
    *   fps��֡��
    *   bitrate�� ����
    */
    virtual int StartPublishInavAnother(std::string live_room_id, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true, std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000"));
    /**
    *  ��������·ֱ��֮�󣬿��Խ�ĳ���û����������ڻ�������������浱��
    **/
    virtual int SetMainView(std::string stream_id);
    /*
     *  �ӿ�˵����������·ֱ���󣬿�ͨ���˽ӿ��޸���������
     */
    virtual int ChangeLayout(std::string layoutMode, std::string custom = std::string());
    /*
    *   ֹͣ��·ֱ��
    *   live_room_id: ֱ������id
    */
    virtual int StopPublishInavAnother(std::string live_room_id);

    virtual bool IsVHMediaConnected();
    /*  ��ʼ����ͷ\��˷�ɼ�
    *   �ص���� OnStartLocalCapture OnCameraCaptureErr  OnLocalMicOpenErr
    */
    virtual int StartLocalCapture(const std::string devId,VideoProfileIndex index, bool doublePush = false);

    virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false);
    /**
     *   ��ʼ����ɼ���ɼ���������
     *   ������
     *       dev_index:�豸����
     *       dev_id;�豸id
     *       float:�ɼ�����
     **/
    virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring dev_id, const int volume);
    /*
    *   ֹͣ������Ƶ�ɼ�
    */
    virtual int StopLocalCapturePlayer();
    /*ֹͣ����ͷ\��˷�ɼ�*/
    virtual void StopLocalCapture();

    /*��ʼ����ͷ��������  �ص���⣺OnStartPushLocalStream*/
    virtual int StartPushLocalStream();
    /*ֹͣ����ͷ�������� �ص���⣺OnStopPushLocalStream*/
    virtual int StopPushLocalStream();
    /**
    *   ��ǰ�ɼ������Ƿ���������
    */
    virtual bool IsPushingStream(VHStreamType streamType);
    /**
    *   ��ȡ��ID
    */
    virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType);
    /*
    *   ��ǰ�ɼ������Ƿ����ڽ��б�������Դ�ɼ�
    */
    virtual bool IsCapturingStream(VHStreamType streamType);
    /**
    *   ��ǰ���������Ƿ�������湲����Ƶ��
    */
    virtual bool IsUserPushingDesktopStream();
    /**
    *   ��ǰ���������Ƿ���ڲ岥��Ƶ��
    */
    virtual bool IsUserPushingMediaFileStream();
    /*��ʼ���湲��ɼ�  �ص���⣺OnStartDesktopCaptureSuc OnStartDesktopCaptureErr*/
    virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) ;
    /*ֹͣ���湲��ɼ�*/
    virtual void StopDesktopCapture() ;
    /**
    *  �������湲����.��Ҫ�������ɼ�����ʱ������������ɫ������ʱ������ͨ���˽ӿ�����������ȣ���߹������������ȵ�Ч����
    *  �˽ӿڵ��ã���Ҫ�����湲��ɼ��ɹ�֮����е��á�������[OnOpenCaptureCallback]�ص��¼���
    *  ���ر�ʱ�������湲���������ʱ�رա�
    */
    virtual int SetDesktopEdgeEnhance(bool enable);
    /**
    *  �ӿ�˵��������ͷԤ����
    **/
    virtual int StartPreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) ;
    /**
   *   �ӿ�˵��������ͷԤ����
   **/
    virtual int StartPreviewCamera(std::shared_ptr<vhall::VideoRenderReceiveInterface> recv, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) ;
    /**
    *   �ӿ�˵������������ͷԤ����
    **/
    virtual int StopPreviewCamera() ;
    /**
    *  �ӿ�˵��������Ƿ�֧�����չ���
    **/
    virtual bool IsSupprotBeauty();
    /*
   *   �ӿ�˵������������ͷ���ռ���Level����Ϊ0-5������level=0�Ǳ�ʾ�رգ�������Ҫ��level����Ϊ1-5����ֵ��
   *   �˽ӿڵ���������ͷ������ر�ǰ���þ����ԡ����ı��ֵʱ���������ʵʱ��Ч��
   *   ��Ҫ�ر�ע����ǣ�����豸��һ�δ򿪣���ʹ��[PreviewCamera]�ӿ�ֱ�Ӵ򿪲���ʾ���ԡ����ǵ�
   *   ���豸�Ѿ���ռ�ã�����ʹ��[bool StartRenderLocalStream(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> recv); ]
   *   �ӿڽ��б������Զ�����Ⱦ����Ԥ��û�л���ʱ��ͨ���豸ID����ƥ�䣬�Ƿ���յ����豸���ݣ�������յ���ͨ����vhall::VideoRenderReceiveInterface�����ص�����
   *   �����Զ�����Ⱦ��
   *   �ص������� OnStopPushStreamCallback
   */
    virtual int SetCameraBeautyLevel(int level);
    virtual int SetPreviewCameraBeautyLevel(int level);
    /*��ʼ���湲��ɼ����� �ص���⣺OnStartPushDesktopStream*/
    virtual int StartPushDesktopStream() ;
    /*ֹͣ���湲��ɼ����� �ص���⣺OnStopPushDesktopStream*/
    virtual int StopPushDesktopStream() ;
    /*��ʼ�岥�ļ�*/
    virtual int InitMediaFile() ;
	/**��ʼ��Ⱦý��������*/
	virtual bool StartRenderStream(const std::wstring& user, void*  wnd, vlive::VHStreamType  streamType);
   /**
   **  ���š������ɹ�����Ե��ô˽ӿڽ����ļ�����
   */
   virtual bool PlayFile(std::string file, VideoProfileIndex profileIndex);
    /*ֹͣ�岥�ļ�*/
    virtual void StopMediaFileCapture();
    /*��ʼ�岥�ļ�����  �ص���⣺OnStartPushMediaFileStream*/
    virtual int StartPushMediaFileStream() ;
    /*ֹͣ�岥�ļ����� �ص���⣺OnStopPushMediaFileStream*/
    virtual void StopPushMediaFileStream() ;
    //�岥�ļ���ͣ����
    virtual void MediaFilePause() ;
    //�岥�ļ��ָ�����
    virtual void MediaFileResume() ;
    //�岥�ļ��������
    virtual void MediaFileSeek(const unsigned long long& seekTimeInMs);
    //�岥�ļ���ʱ��
    virtual const long long MediaFileGetMaxDuration() ;
    //�岥�ļ���ǰʱ��
    virtual const long long MediaFileGetCurrentDuration();
    //�岥�ļ��ĵ�ǰ״̬ 
    //����ֵ������״̬  MEDIA_FILE_PLAY_STATE
    virtual int MediaGetPlayerState();
    /**��ȡ����ͷ�б�**/
    virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras);
    /**��ȡ��˷��б�**/
    virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) ;
    /**��ȡ�������б�**/
    virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList);
    /*
    *  ����ʹ�õ���˷�
    *  index: GetMicDevices��ȡ���б��� vhall::VideoDevProperty�е� devIndex
    */
    virtual void SetUseMicIndex(int index, std::string devId);
    /*
    *  ����ʹ�õ�������
    *  index: GetPlayerDevices��ȡ���б��� VhallLiveDeviceInfo�е� devIndex
    */
    virtual void SetUserPlayIndex(int index, std::string devId) ;
    /**���õ�ǰʹ�õ���˷�����**/
    virtual bool SetCurrentMicVol(int vol);
    /**��ȡ��ǰʹ�õ���˷�����**/
    virtual int GetCurrentMicVol();
    /**���õ�ǰʹ�õ�����������**/
    virtual bool SetCurrentPlayVol(int vol);
    /**��ȡ��ǰʹ�õ�����������**/
    virtual int GetCurrentPlayVol();
    //�ر�����ͷ
    virtual bool CloseCamera();
    //������ͷ
    virtual bool OpenCamera();
	//�ر�����ͷ
	virtual bool IsCameraOpen();
    virtual bool CloseMic();
    /*
    *   ����˷�
    */
    virtual bool OpenMic();
	//������ͷ
	virtual bool IsMicOpen();
    /*
    *   �򿪡��ر�Զ���û�������Ƶ
    */
    virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false) ;
    /*
    *   �򿪡��ر�Զ���û���������
    */
    virtual int OperateRemoteUserAudio(std::wstring user_id, bool close = false) ;
	virtual void ReFreshPermission();
    virtual int MuteAllSubScribeAudio(bool mute);
private:
    std::string GetBaseUrlInfo();
    std::string GetInvaStartUrl();
    std::string GetInavInfoUrl();
    std::string GetInavUserListUrl();
    std::string GetLeaveInavtUrl();
    std::string GetApplyInavPublishUrl();
    std::string GetAuditInavPublishUrl(const std::string& userid,int type);
    std::string GetAskforInavPublishUrl(const std::string& userid);
    std::string GetUserPublishCallbackUrl(const std::string& userid, int type);
    std::string GetKickInavStreamUrl(const std::string& user_id, int type);
    std::string GetKickInavUrl(const std::string& userid, int type);
    std::string GetKickInavListUrl();
    std::string GetAccessTokenPermission();
    std::string GetPublishInavAnother(const std::string& live_room_id,int start);
    std::string GetSetLayOut(std::string layoutMode, std::string custom = std::string());
    //std::string GetPublishInavAnotherWithParam(const std::string& live_room_id, int start);
    std::string GetPushAnotherConfig(int state);//Ĭ��1������  2 �ر�
    std::string GetLayoutDesc(LayoutMode layout);

    void SendServerJoinMsg(const std::string context);  
    void SendServerLeaveMsg(const std::string context);
    void SendJoinMsg(std::string id,const std::string context);
    void SendLeaveMsg(std::string id, const std::string context);
    void SetBroadCastCallBack(const std::string result,const std::string& msg);
    void SetBroadCastCallBackWithParam(const std::string result, const std::string& msg);

    std::string WString2String(const std::wstring& ws);
    std::wstring String2WString(const std::string& s);

    static DWORD WINAPI ThreadProcess(LPVOID p);
    void TaskProcess();
    void InsertProcessTask(TaskData);
    TaskData GetProcessTask();

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
    void CheckPermission(VHStreamType streamType,std::string data, int code);
    void ParamRecvInavMsg(const std::string msg);
    void ParamRecvSocketMsg(const std::string msg);

private:
    void GetVHRoomInfo();

private:
   VLiveRoomMsgEventDelegate * mpVHRoomMonitor = nullptr;
   VRtcEngineEventDelegate* mpRtcRoomDelegate = nullptr;
    std::shared_ptr<HttpManagerInterface> mpHttpManagerInterface = nullptr;
    WebRtcSDKInterface* mpWebRtcSDKInterface = nullptr;
    VHWebSocketInterface* mpVHWebSocketInterface = nullptr;
    VHWebSocketInterface* mpVHMsgChatInterface = nullptr;
    LoginRespInfo mLoginInfo;
    InavInfo mInavInfo;

    std::string mAccesstoken;
    std::string mAppid;
    std::string mThridUserId;
    std::string mInavRoomId;
    std::string mLiveRoomId;
    std::string mChannelId;
    std::string mContext;
    std::string mDomain;
    std::atomic_bool bIsSettinglayout;
    std::atomic_bool mbLoginDone = false; //��½�ɹ� ֻ�ϱ�һ�Ρ��Ƿ��Ѿ�����socket.io

    bool bInit = false;
    bool mIsHost;

    BroadCastInfo mCurrentBroadCastInfo;
    LogReport mLogReport;
    //http proxy
    std::atomic_bool bIsEnableProxy = false;
    std::string mProxyAddr;
    std::string mProxyUserName;
    std::string mProxyUserPwd;
    int mProxyPort;

   static std::atomic_bool mbThreadRunning;
   std::thread *mProcessThread = nullptr;

   std::mutex mTaskMutex;
   std::list<TaskData> mTaskList;
};
     