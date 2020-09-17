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

////测试环境
//#define HOST_URL "http://t-open.e.vhall.com/sdk/v1"
//
////线上环境
////#define HOST_URL "http://api.vhallyun.com/sdk/v1"

#define PAASSDK_VER    "1.0.0.0"
#define DIP_1080P   "1080P"
#define DIP_720P    "720P"
#define DIP_540P    "540P"
#define DIP_480P    "480P"
#define DIP_240P    "240P"

//（标清）
#define DPI_SD      "SD"
//（高清）
#define DPI_HD      "HD"
// （超清）
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
    //初始化话接口必须在main函数中调用。
    virtual void InitSDK(VLiveRoomMsgEventDelegate* liveRoomDelegate, VRtcEngineEventDelegate* rtcRoomDelegate, const std::string domain = "http://api.vhallyun.com/sdk/v1", std::wstring logPath = std::wstring());

    virtual void SetHttpProxy(bool enable, std::string addr = std::string(), int port = 0, std::string userName = std::string(), std::string pwd = std::string());
    /****
    * 登录互动业务服务器.登录成功之后即可加入“互动媒体房间”
    **/
    virtual bool LoginRoom(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id, const std::string& roomid, const std::string& live_roomid,std::string channel_id = std::string(), const std::string context = std::string());
    /**
    *  获取当前用户权限
    */
    virtual Permission GetPermission();
    /*
    *  登出互动服务处理
    */
    virtual void LogoutRoom() ;
    /**
    * 进入互动媒体房间
    **/
    virtual void JoinRtcRoom(std::string userData = std::string());
    /*
    * 退出互动媒体房间
    */
    virtual bool LeaveRtcRoom();
    /*
    * SDK互动房间人员列表
    **/
    virtual void AsynGetVHRoomMembers();
    /**
    * 申请上麦
    */
    virtual int ApplyInavPublish();
    /**
    * 审核申请上麦
    * audit_user_id: 受审核人第三放用户ID
    * type: 是否同意 AuditPublish
    * 返回值：VhallLiveErrCode
    **/
    virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type);
    /**
    * 邀请上麦
    * audit_user_id: 受审核人第三方用户ID
    **/
    virtual int AskforInavPublish(const std::string& audit_user_id);
    /**
    * 上/下/拒绝上麦状态回执,用户成功推流后调用服务端API
    * stream_id: 操作的流ID
    * type	int	否	1 上麦(默认) 2 下麦 3 拒绝上麦
    */
    virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string());
    /**
    * 踢出流/取消踢出流  实际作用于AccessToken
    * type	int	否	1 踢出流（默认） 2 取消踢出流
    * kick_user_id	string	是	被踢出的第三方用户ID
    **/
    virtual int KickInavStream(const std::string& kick_user_id, KickStream type);
    /**
    *  踢出互动房间/取消踢出互动房间  作用于进入房间
    *   type	int	否	1 踢出互动房间（默认） 2 取消踢出互动房间
    *   kick_user_id	string	是	被踢出的第三方用户ID
    * 返回值：VhallLiveErrCode
    **/
    virtual int KickInav(const std::string& kick_user_id, KickStream type);
    /*
    * 获取被踢出互动房间人列表
    */
    virtual void GetKickInavList();

    /*
    *   开启旁路直播
    *   live_room_id: 直播房间id
    *   width： 混流端布局的宽
    *   height： 混流端布局的高
    *   fps：帧率
    *   bitrate： 码率
    */
    virtual int StartPublishInavAnother(std::string live_room_id, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true, std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000"));
    /**
    *  当开启旁路直播之后，可以将某个用户的流设置在混流画面的主画面当中
    **/
    virtual int SetMainView(std::string stream_id);
    /*
     *  接口说明：开启旁路直播后，可通过此接口修复混流布局
     */
    virtual int ChangeLayout(std::string layoutMode, std::string custom = std::string());
    /*
    *   停止旁路直播
    *   live_room_id: 直播房间id
    */
    virtual int StopPublishInavAnother(std::string live_room_id);

    virtual bool IsVHMediaConnected();
    /*  开始摄像头\麦克风采集
    *   回调检测 OnStartLocalCapture OnCameraCaptureErr  OnLocalMicOpenErr
    */
    virtual int StartLocalCapture(const std::string devId,VideoProfileIndex index, bool doublePush = false);

    virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false);
    /**
     *   开始桌面采集与采集音量控制
     *   参数：
     *       dev_index:设备索引
     *       dev_id;设备id
     *       float:采集音量
     **/
    virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring dev_id, const int volume);
    /*
    *   停止桌面音频采集
    */
    virtual int StopLocalCapturePlayer();
    /*停止摄像头\麦克风采集*/
    virtual void StopLocalCapture();

    /*开始摄像头数据推流  回调检测：OnStartPushLocalStream*/
    virtual int StartPushLocalStream();
    /*停止摄像头数据推流 回调检测：OnStopPushLocalStream*/
    virtual int StopPushLocalStream();
    /**
    *   当前采集类型是否正在推流
    */
    virtual bool IsPushingStream(VHStreamType streamType);
    /**
    *   获取流ID
    */
    virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType);
    /*
    *   当前采集类型是否正在进行本地数据源采集
    */
    virtual bool IsCapturingStream(VHStreamType streamType);
    /**
    *   当前互动房间是否存在桌面共享视频流
    */
    virtual bool IsUserPushingDesktopStream();
    /**
    *   当前互动房间是否存在插播视频流
    */
    virtual bool IsUserPushingMediaFileStream();
    /*开始桌面共享采集  回调检测：OnStartDesktopCaptureSuc OnStartDesktopCaptureErr*/
    virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) ;
    /*停止桌面共享采集*/
    virtual void StopDesktopCapture() ;
    /**
    *  设置桌面共享锐化.主要针对桌面采集文字时，遇到文字颜色不明显时，可以通过此接口提高文字亮度，提高共享文字清晰度的效果。
    *  此接口调用，需要在桌面共享采集成功之后进行调用。即监听[OnOpenCaptureCallback]回调事件。
    *  当关闭时可在桌面共享过程中随时关闭。
    */
    virtual int SetDesktopEdgeEnhance(bool enable);
    /**
    *  接口说明：摄像头预览。
    **/
    virtual int StartPreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) ;
    /**
   *   接口说明：摄像头预览。
   **/
    virtual int StartPreviewCamera(std::shared_ptr<vhall::VideoRenderReceiveInterface> recv, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) ;
    /**
    *   接口说明：结束摄像头预览。
    **/
    virtual int StopPreviewCamera() ;
    /**
    *  接口说明：检测是否支持美颜功能
    **/
    virtual bool IsSupprotBeauty();
    /*
   *   接口说明：设置摄像头美颜级别。Level级别为0-5，其中level=0是表示关闭，开启需要将level设置为1-5任意值。
   *   此接口调用在摄像头开启或关闭前调用均可以。当改变此值时推流画面会实时生效。
   *   需要特别注意的是，如果设备第一次打开，可使用[PreviewCamera]接口直接打开并显示回显。但是当
   *   此设备已经被占用，建议使用[bool StartRenderLocalStream(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> recv); ]
   *   接口进行本地流自定义渲染，当预览没有画面时可通过设备ID进行匹配，是否接收到此设备数据，如果接收到则通过【vhall::VideoRenderReceiveInterface】返回的数据
   *   进行自定义渲染。
   *   回调监听： OnStopPushStreamCallback
   */
    virtual int SetCameraBeautyLevel(int level);
    virtual int SetPreviewCameraBeautyLevel(int level);
    /*开始桌面共享采集推流 回调检测：OnStartPushDesktopStream*/
    virtual int StartPushDesktopStream() ;
    /*停止桌面共享采集推流 回调检测：OnStopPushDesktopStream*/
    virtual int StopPushDesktopStream() ;
    /*开始插播文件*/
    virtual int InitMediaFile() ;
	/**开始渲染媒体数据流*/
	virtual bool StartRenderStream(const std::wstring& user, void*  wnd, vlive::VHStreamType  streamType);
   /**
   **  播放。推流成功后可以调用此接口进行文件播放
   */
   virtual bool PlayFile(std::string file, VideoProfileIndex profileIndex);
    /*停止插播文件*/
    virtual void StopMediaFileCapture();
    /*开始插播文件推流  回调检测：OnStartPushMediaFileStream*/
    virtual int StartPushMediaFileStream() ;
    /*停止插播文件推流 回调检测：OnStopPushMediaFileStream*/
    virtual void StopPushMediaFileStream() ;
    //插播文件暂停处理
    virtual void MediaFilePause() ;
    //插播文件恢复处理
    virtual void MediaFileResume() ;
    //插播文件快进处理
    virtual void MediaFileSeek(const unsigned long long& seekTimeInMs);
    //插播文件总时长
    virtual const long long MediaFileGetMaxDuration() ;
    //插播文件当前时长
    virtual const long long MediaFileGetCurrentDuration();
    //插播文件的当前状态 
    //返回值：播放状态  MEDIA_FILE_PLAY_STATE
    virtual int MediaGetPlayerState();
    /**获取摄像头列表**/
    virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras);
    /**获取麦克风列表**/
    virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) ;
    /**获取扬声器列表**/
    virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList);
    /*
    *  设置使用的麦克风
    *  index: GetMicDevices获取的列表中 vhall::VideoDevProperty中的 devIndex
    */
    virtual void SetUseMicIndex(int index, std::string devId);
    /*
    *  设置使用的扬声器
    *  index: GetPlayerDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
    */
    virtual void SetUserPlayIndex(int index, std::string devId) ;
    /**设置当前使用的麦克风音量**/
    virtual bool SetCurrentMicVol(int vol);
    /**获取当前使用的麦克风音量**/
    virtual int GetCurrentMicVol();
    /**设置当前使用的扬声器音量**/
    virtual bool SetCurrentPlayVol(int vol);
    /**获取当前使用的扬声器音量**/
    virtual int GetCurrentPlayVol();
    //关闭摄像头
    virtual bool CloseCamera();
    //打开摄像头
    virtual bool OpenCamera();
	//关闭摄像头
	virtual bool IsCameraOpen();
    virtual bool CloseMic();
    /*
    *   打开麦克风
    */
    virtual bool OpenMic();
	//打开摄像头
	virtual bool IsMicOpen();
    /*
    *   打开、关闭远端用户本地视频
    */
    virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false) ;
    /*
    *   打开、关闭远端用户本地声音
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
    std::string GetPushAnotherConfig(int state);//默认1开启，  2 关闭
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
    std::atomic_bool mbLoginDone = false; //登陆成功 只上报一次。是否已经连接socket.io

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
     