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

////测试环境
//#define HOST_URL "http://t-open.e.vhall.com/sdk/v1"
//
////线上环境
////#define HOST_URL "http://api.vhallyun.com/sdk/v1"
class LogReport;
class CThreadLock;

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
   eTask_BroadCast, //上报开启旁路状态
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
   /*初始化连接媒体房间所需参数*/
   virtual void InitRtcRoomParameter(const InavInfo& inavInfo,const std::string& log_server, 
      const std::string& InavRoomId, const std::string& Appid);
   /*
   *   接口说明：用于本地调试使用。
   */
   virtual void SetDebugLogAddr(const std::string& addr);
   /*
   *   接口说明：初始化。初始化接口必须在主函数中调用。
   *   room_delegate： 房间消息事件回调
   *   rtc_deleage：   互动事件消息回调
   */
   virtual int InitSDK(WebRtcSDKEventInterface* obj, VHRoomDelegate * VHRoomDelegate, 
      WebRtcSDKEventInterface* rtc_deleage, std::wstring logPath = std::wstring(),
      const std::string domain = "http://api.vhallyun.com/sdk/v1"
      );
   /*
   *   接口说明：登陆接口。登录微吼互动业务服务器。
   *   接口参数：
   *      accesstoken：通过平台获取产生的token
   *      appid：应用id
   *      thrid_user_id：用户id
   *      inav_roomid：互动房间id
   *      listenChannel: 监听的消息channel_id ，例如房间id,聊天信道id等。
   *      context:  登录时添加的自定义字段，json格式
   *      回调监听：VHRoomDelegate
   */
   virtual bool Login(const std::string& accesstoken, 
      const std::string& appid, 
      const std::string& thrid_user_id,
      const std::string& inav_roomid, 
      const std::string& live_roomid, 
      const std::list<std::string> listenChannel, 
      const std::string context = std::string()) ;
   /*
   *  网络断开重连接口。
   *  到回调OnRoomConnectState接收到 RoomConnectState_NetWorkDisConnect 表示网络连接异常，需要手动重连。
   */
   virtual int ReconnectNetWork();
   /*
   *  接口说明：登出互动服务处理。
   *  回调监听：VHRoomDelegate
   */
   virtual void LogOut();
   /**
   *  接口说明：获取当前用户权限。根据权限判断那些功能可用。
   */
   virtual Permission GetPermission() ;
   /*
   *  接口说明：重新获取当前账号权限
   *  回调监听：VHRoomDelegate
   */
   virtual void ReFreshPermission();
   virtual void ClearInitInfo();
   /**
   *  接口说明：进入互动媒体房间
   *  回调监听：RtcSDKEventDelegate
   *  userData : 用户自定义字段，远端订阅时可解析相关字段。
   **/
   virtual bool JoinRTCRoom(std::string userData = std::string(), bool bEnableSimulCaset = true, 
      std::string vid = std::string(),std::string vfid = std::string(), std::string strWebinarId = std::string()
   ,int role = 1, int classType = 1);
   virtual bool isConnetWebRtcRoom();
   /*
   *  接口说明：退出互动媒体房间
   *  返回值：true 退出成功；false 退出失败
   */
   virtual bool LeaveRTCRoom();
   /*
*  禁止订阅流并取消订阅远端流
*/
   virtual void DisableSubScribeStream();
   /*
*   使能订阅流并开始订阅房间中的远端流
*/
   virtual void EnableSubScribeStream();
   /*
   *   房间是否已连接
   */
   virtual bool IsWebRtcRoomConnected();
   /*
   *  接口说明：SDK互动房间人员列表
   *  回调监听：VHRoomDelegate
   */
   virtual void AsynGetVHRoomMembers();
   /**
   *  接口说明：申请上麦。
   *  回调监听：VHRoomDelegate
   *  返回值：VhallLiveErrCode
   */
   virtual int ApplyInavPublish();
   /**
   *  接口说明：审核申请上麦
   *  参数说明：
   *    audit_user_id: 受审核人第三放用户ID
   *    type: 是否同意，AuditPublish
   *  回调监听：VHRoomDelegate
   *  返回值：VhallLiveErrCode
   **/
   virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) ;
   /**
   *  接口说明：邀请上麦
   *  参数说明：
   *    audit_user_id: 受审核人第三放用户ID
   *  回调监听：VHRoomDelegate
   *  返回值：VhallLiveErrCode
   **/
   virtual int AskforInavPublish(const std::string& audit_user_id);
   /**
   *  接口说明：上/下/拒绝上麦状态回执,用户成功推流后调用服务端API
   *  参数说明：
   *       stream_id: 操作的流ID
   *       type:      PushStreamEvent
   *  回调监听：VHRoomDelegate
   */
   virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string());
   /**
   *  接口说明：踢出流/取消踢出流
   *  参数说明：
   *     type: 1 踢出流（默认） 2 取消踢出流
   *     kick_user_id	string	是	被踢出的第三方用户ID
   *  回调监听：VHRoomDelegate
   *  返回值：VhallLiveErrCode
   **/
   virtual int KickInavStream(const std::string& kick_user_id, KickStream type);
   /**
   *  接口说明：踢出互动房间/取消踢出互动房间
   *  参数说明：
   *    type:1 踢出互动房间（默认） 2 取消踢出互动房间
   *    kick_user_id:被踢出的第三方用户ID
   *  回调监听：VHRoomDelegate
   *  返回值：VhallLiveErrCode
   **/
   virtual int KickInav(const std::string& kick_user_id, KickStream type);
   /*
   *  接口说明：获取被踢出互动房间人列表
   *  回调监听：VHRoomDelegate
   */
   virtual void GetKickInavList();
   /*
   *  接口说明：开启旁路直播,此接口是互动房间有旁路推流权限的用户进行调用的,当处理桌面共享或插播时如果想显示在主画面需要调用SetMainView接口，
                 将对应的流ID作为参数进行设置，如果只想显示一路则同时需要设置布局模式。需要监听回调
   *  参数说明：
   *     layoutMode:混流端布局模式
   *     profileIndex: 旁路分辨率
   *  回调监听：RtcSDKEventDelegate
   *  返回值：VhallLiveErrCode
   */
   virtual int StartConfigBroadCast(LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true,
      std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")
      , SetBroadCastEventCallback fun = nullptr);


   /*
   *  接口说明：开启旁路直播后，可通过此接口修改混流布局
   *  回调监听：RtcSDKEventDelegate
   */
   //virtual int ChangeLayout(LayoutMode layoutMode, std::string custom = std::string());
   virtual int SetConfigBroadCastLayOut(LayoutMode mode, SetBroadCastEventCallback fun = nullptr);

   /**
   *  接口说明：当开启旁路直播之后，可以将某个用户的流设置在混流画面的主画面当中，此接口是互动房间有旁路推流权限的用户进行调用
   *  回调监听：RtcSDKEventDelegate
   *  返回值：  VhallLiveErrCode
   **/
   virtual int SetMainView(std::string stream_id/*, int stream_type*/, SetBroadCastEventCallback fun = nullptr);
   /*
   *   接口说明：停止旁路直播。此接口是互动房间有旁路推流权限的用户进行调用的
   *   参数说明：
   *       live_room_id: 直播房间id
   *  回调监听：RtcSDKEventDelegate
   *   返回值：VhallLiveErrCode
   */
   virtual int StopPublishInavAnother();
   /*
   *   接口说明：互动房间是否已经连接成功
   *   返回值：true互动房间已连接，false互动房间链接断开
   */
   virtual bool IsVHMediaConnected();
   /*
   *   接口说明：获取摄像头列表
   *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
   */
   virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras);
   /*
   *  是否存在音频或视频设备。
   *  返回值：只要存在一类设备 返回true, 如果音视频设备都没有则返回false
   **/
   virtual bool HasVideoOrAudioDev() ;
   virtual bool HasVideoDev();
   virtual bool HasAudioDev();
   /*
   *   接口说明：获取麦克风列表
   *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
   **/
   virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList);
   /*
    *  获取摄像头列表详情
    */
   virtual int GetCameraDevDetails(std::list<CameraDetailsInfo> &cameraDetails);
   /**
   *   接口说明：获取扬声器列表
   *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
   **/
   virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList);
   /*
   *  接口说明：设置使用的麦克风，此接口为全局接口。如果设置之后，会实时生效。既推流麦克风采集的设备已经被切换。
   *  参数说明：
   *   index: GetMicDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
   */
   virtual void SetUseMicIndex(int index, std::string devId, std::wstring desktopCaptureId);
   /*
   *  接口说明：设置使用播放声音的扬声器
   *  参数说明：
   *   index: GetPlayerDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
   */
   virtual void SetUsePlayer(int index, std::string devId) ;
   /*
   *   接口说明：设置当前使用的麦克风音量
   *   参数说明：
   *       vol:参数范围 0~100
   **/
   virtual bool SetCurrentMicVol(int vol);
   /*
   *   接口说明：获取当前使用的麦克风音量
   *   返回值：当前音量
   **/
   virtual int GetCurrentMicVol();
   /*
   *   接口说明：设置当前使用的扬声器音量
   *   参数说明：
   *       vol:参数范围 0~100
   **/
   virtual bool SetCurrentPlayVol(int vol);
   /*
   *   接口说明：获取当前使用的扬声器音量
   *   返回值：返回当前的音量值
   **/
   virtual int GetCurrentPlayVol();
   /*
   *   接口说明：关闭摄像头
   *   返回值：true 关闭成功/ false 关闭失败
   */
   virtual bool CloseCamera();
   /*
   *   接口说明：打开已经开始采集的摄像头
   *   返回值：true 打开成功/ false 打开失败
   */
   virtual bool OpenCamera();
   /*
   *   接口说明：摄像头是否关闭
   *   返回值：true 已经关闭/ false 未关闭
   */
   virtual bool IsCameraOpen();
   /*
   *   接口说明：关闭麦克风
   *   返回值：true 关闭成功/ false 关闭失败
   */
   virtual bool CloseMic();
   /*
   *   接口说明：打开麦克风
   *   返回值：true 打开成功/ false 打开失败
   */
   virtual bool OpenMic();
   /*
   *   接口说明：麦克风是否打开
   *   返回值：true 已经关闭/ false 未关闭
   */
   virtual bool IsMicOpen();
   /**
   *  获取当前使用的摄像设备信息
   ***/
   virtual void VHPaasRoomImpl::GetCurrentCameraInfo(int &index, std::string& devId);
   /**
   * 摄像头画面预览，当预览结束之后需要停止预览，否则摄像头将被一直占用
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int PreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex /*= -1*/);
   /*
*  返回预览Mic音量
*/
   virtual int GetMicVolumValue();
   /*
*  更换预览mic
*/
   virtual void ChangePreViewMic(int micIndex);
   /*
   *  停止预览画面
   */
   virtual int StopPreviewCamera();
   /*
   *   接口说明：开始摄像头\麦克风采集  摄像头采集和图片采集可以切换使用并且不影响推流。
   *   参数说明：
   *       devId： 摄像头设备id
   *       VideoProfileIndex：推流分辨率
   *       doublePush: 推流时，是否开启双流。
   *   回调监听：RtcSDKEventDelegate
   *   返回值：VhallLiveErrCode
   */
   virtual int StartLocalCapture(const std::string camera_devId, VideoProfileIndex index, bool doublePush = false);
   virtual int StartLocalAuxiliaryCapture(const std::string devId, VideoProfileIndex index);  //双推画面推流
   /*
   *   接口说明：图片采集. 切换图片会替换摄像头的推流
   *   参数说明：
   *       filePath：图片路径。
   *       VideoProfileIndex：分辨率
   *       doublePush: 推流时，是否开启双流。
   *   监听：OnOpenCaptureCallback
   *   返回值：VhallLiveErrCode
   */
   virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false);
   /*
   *   接口说明：开始摄像头/图片数据推流 ，必须先打开摄像头之后才能推流。
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int StartPushLocalStream();
   virtual int StartPushLocalAuxiliaryStream(std::string context = std::string());
   /*
   *   接口说明：停止摄像头、图片数据推流
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int StopPushLocalStream();
   virtual int StopPushLocalAuxiliaryStream();
   /*
   *   接口说明：停止摄像头\图片采集  如果已经开始推流会同时停止推流
   */
   virtual void StopLocalCapture();
   virtual void StopLocalAuxiliaryCapture();
   /**
   *   接口说明：开始桌面采集。调用此接口必须确保有麦克风设备，且设置过麦克风，否则会采集异常。
   *   参数：
   *       player_dev; 桌面采集使用的扬声器设备id
   *       float:采集音量 0-100
   **/
   virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring player_dev, const int volume);
   /*
   *   接口说明：当前采集类型是否正在进行本地数据源采集
   */
   virtual bool IsCapturingStream(VHStreamType streamType);
   /*
   *   停止桌面音频采集
   */
   virtual int StopLocalCapturePlayer();
   ///**
   //*   接口说明：当前互动房间是否存在桌面共享视频流
   //*/
   //virtual bool IsUserPushingDesktopStream();
   ///**
   //*   接口说明：当前互动房间是否存在插播视频流
   //*   返回值：ture成功。false失败
   //*/
   //virtual bool IsUserPushingMediaFileStream();
   /**
   *   接口说明：当前采集类型是否正在推流
   *   返回值：ture成功。false失败
   */
   virtual bool IsPushingStream(VHStreamType streamType);
   /**
   *   接口说明：获取流用户推流ID
   *   参数说明：
   *       user_id：用户id
   *       streamType: 流类型
   *   返回值：流id
   */
   virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType);
   /*
   *   接口说明：开始渲染媒体数据流
   *   参数说明：
   *       wnd： 渲染窗口句柄
   *       streamType: 流类型
   *   返回值：ture成功。false失败
   */
   virtual bool StartRenderStream(void* wnd, vlive::VHStreamType streamType);
   /*
   *   接口说明：开始渲染媒体数据流
   *   参数说明：
   *       user_id：用户id
   *       wnd： 渲染窗口句柄
   *       streamType: 流类型
   *   返回值：ture成功。false失败
   */
   virtual bool StartRenderSubscribeStream(const std::wstring& user, vlive::VHStreamType streamType, void* wnd);
   /*
   *    获取窗口共享和桌面共享列表
   */
   virtual std::vector<DesktopCaptureSource> GetDesktops(int streamType);
   /*
   *   接口说明：开始桌面共享采集,.
   *    index:GetDesktops返回值中DesktopCaptureSource.id
   *   回调监听：RtcSDKEventDelegate
   *   返回值：VhallLiveErrCode
   */
   virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex);
   /*
   *   接口说明：停止桌面共享采集  如果已经开始推流会同时停止推流
   */
   virtual void StopDesktopCapture();
   /*
   *   接口说明：开始桌面共享采集推流
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int StartPushDesktopStream(std::string context = std::string()) ;
   /*
   *   接口说明：停止桌面共享采集推流
   *   回调监听：RtcSDKEventDelegate
   *   返回值：VhallLiveErrCode
   */
   virtual int StopPushDesktopStream() ;
   /*
   * 设置选择的软件源
   */
   virtual int SelectSource(int64_t id);
   /*
   * 停止软件源共享采集
   */
   virtual void StopSoftwareCapture();
   /*
   * 停止软件共享采集推流
   */
   virtual int StopPushSoftWareStream();
   /*
   * 开始软件共享采集推流
   */
   virtual int StartPushSoftWareStream();
   /*
   *   判断图片有效性
   */
   virtual int CheckPicEffectiveness(const std::string filePath);
   /**
   *  获取插播文件原始大小
   */
   virtual int GetPlayMeidaFileWidthAndHeight(std::string filePath, int& srcWidth, int &srcHeight);
   /*
   *   接口说明：开始插播文件
   *   返回值：VhallLiveErrCode
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int StartPlayMediaFile(std::string filePath, VideoProfileIndex profileIndex = RTC_VIDEO_PROFILE_720P_16x9_H, long long seekPos = 0);
   /*
   *  插播文件是否带有视频画面。
   */
   virtual bool IsPlayFileHasVideo();        
   /*
   *   设置插播文件音量
   */
   virtual int MediaFileVolumeChange(int vol);
   /**
   *   切换插播推流分辨率
   */
   virtual void ChangeMediaFileProfile(VideoProfileIndex profileIndex);

   /*
   *   接口说明：停止插播文件, 如果已经开始推流会同时停止推流
   */
   virtual void StopMediaFileCapture();
   /*
   *   接口说明：开始插播文件推流
   *   返回值：VhallLiveErrCode
   *   回调监听：RtcSDKEventDelegate
   */
   virtual int StartPushMediaFileStream();
   /*
   *   接口说明：停止插播文件推流
   *   回调监听：RtcSDKEventDelegate
   */
   virtual void StopPushMediaFileStream();
   /*
   *   接口说明：插播文件暂停处理
   *   返回值：VhallLiveErrCode
   */
   virtual void MediaFilePause();
   /*
   *   插播文件恢复处理
   *   返回值：VhallLiveErrCode
   */
   virtual void MediaFileResume();
   /*
   *   接口说明：插播文件快进处理
   *   返回值：VhallLiveErrCode
   */
   virtual void MediaFileSeek(const unsigned long long& seekTimeInMs) ;
   /*
   *   接口说明：插播文件总时长
   */
   virtual const long long MediaFileGetMaxDuration();
   /*
   *   接口说明：插播文件当前时长
   */
   virtual const long long MediaFileGetCurrentDuration();
   /*
   *   接口说明：插播文件的当前状态
   *   返回值：播放状态  MEDIA_FILE_PLAY_STATE
   */
   virtual int MediaGetPlayerState() ;
   /**
   *  接口说明：订阅流。
   *   回调监听：RtcSDKEventDelegate
   */
   virtual void SubScribeRemoteStream(const std::string &stream_id, int delayTimeOut = 0);
   /*
    *  接口说明：当接收到远端数据后，可以修改订阅的画面为大流或小流.
   *   回调监听：RtcSDKEventDelegate
   */
   virtual bool ChangeSubScribeUserSimulCast(const std::wstring& user_id, vlive::VHStreamType streamType, VHSimulCastType type);
   /*
   *   接口说明：打开、关闭远端用户本地视频
   *   参数说明：
   *       user_id：用户id
   *       close：true 关闭/ false 打开
   *   返回值：VhallLiveErrCode
   */
   virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false);
   /*
   *   接口说明：打开、关闭远端用户本地声音
   *   参数说明：
   *       user_id：用户id
   *       close：true 关闭/ false 打开
   *   返回值：VhallLiveErrCode
   */
   virtual int OperateRemoteUserAudio(std::wstring user_id, bool close = false) ;
   /**
   *  打开或关闭所有订阅的音频
   */
   virtual int MuteAllSubScribeAudio(bool mute);
   /**
   *   接口说明：注册消息监听接口，用户可用于监听底层所有的消息事件内容。如果用户不关注，可通过 InitSDK 接口注册的VHRoomDelegate进行监听。
   */
   virtual void RegisterMsgListenr(RecvMsgDelegate*) ;
   ///*
   //*   停止接收所有远端数据流
   //*/
   //virtual void StopRecvAllRemoteStream();

   virtual void implementTask(int iTask);
   /*
*   混流配置准备完成
*/
   virtual bool IsEnableConfigMixStream();
   virtual void SetEnableConfigMixStream(bool enable);
   static DWORD WINAPI ThreadProcess(LPVOID p);
   void ProcessTask();
   /*
   *   返回本地双推流 id
   */
   virtual std::string GetLocalAuxiliaryId();
   /*
*   返回远端双推流 id
*/
   virtual std::string GetAuxiliaryId();
   /*
*   返回流类型
*/
   VHStreamType  CalcStreamType(const bool& bAudio, const bool& bVedio);
   /*
*   设置采集扬声器的音量值
*/
   virtual int SetLocalCapturePlayerVolume(const int volume);
   /*
*   停止播放音频文件接口
*/
   virtual int StopPlayAudioFile();
   /*
*   获取播放文件音量
*/
   virtual int GetPlayAudioFileVolum();
   /*
*   添加播放音频文件接口
*/
   virtual int PlayAudioFile(std::string fileName, int devIndex);
   /*
*   切换下一个摄像头
*/
   virtual int ChangeNextCamera(VideoProfileIndex videoProfile);
   /**
*  获取当前使用的摄像设备信息
***/
   virtual void GetCurrentCamera(std::string& devId);
   /**
*  获取当前使用的麦克风设备信息
***/
   virtual void GetCurrentMic(int &index, std::string& devId);
   /*
*获取所有订阅的音频状态(打开/关闭)
*/
   virtual bool GetMuteAllAudio();
   /*
*  设置使用的麦克风
*  index: GetMicDevices获取的列表中 vhall::AudioDevProperty中的 devIndex
*/
   virtual int SetUsedMic(int micIndex, std::string micDevId, std::wstring desktopCaptureId);
   //渲染远端流
   virtual bool StopRenderRemoteStream(const std::wstring& user, const std::string streamId, vlive::VHStreamType streamType);
   virtual bool StartRenderRemoteStreamByInterface(const std::wstring& user, vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive);
   virtual bool IsRemoteStreamIsExist(const std::wstring& user, vlive::VHStreamType streamType);
   /**
*   开始渲染"本地"摄像设备、桌面共享、文件插播媒体数据流.
*/
   virtual bool StartRenderLocalStream(vlive::VHStreamType streamType, void* wnd);
   virtual bool StartRenderLocalStreamByInterface(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive);
   virtual bool IsStreamExit(std::string id);
   /**
*   获取订阅流中流类型，是否包括音视频数据。
*   返回值：0 查找成功，其它失败
*/
   virtual int GetSubScribeStreamFormat(const std::wstring& user, vlive::VHStreamType streamType, bool &hasVideo, bool &hasAudio);
   /**
*   获取流ID
*   返回值：如果有流id返回流id 否则返回空字符串
*/
   virtual std::string GetSubScribeStreamId(const std::wstring& user, vlive::VHStreamType streamType);
   /*当前房间是否存在给定类型的远端流*/
   virtual bool IsExitSubScribeStream(const vlive::VHStreamType& streamType);
   virtual bool IsExitSubScribeStream(const std::string& id, const vlive::VHStreamType& streamType);
   virtual void GetStreams(std::list<std::string>& streams);
   virtual void ClearSubScribeStream();
   virtual void StopRecvAllRemoteStream();//停止接受远端流
   virtual std::string LocalStreamId();
   /**
*  获取推流视频丢包率.3秒读一次
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
   std::string GetPushAnotherConfig(int state);//默认1开启，  2 关闭
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
   *  接口说明：同步paas服务开启旁路
   *  回调监听：RtcSDKEventDelegate
   */
   void VHPaasRoomImpl::GetPublishInavAnotherWithParam();
   /*
   *  判断给定媒体类型是否支持
   */
   virtual bool IsSupportMediaFormat(CaptureStreamAVType type);
private:
   void GetVHRoomInfo();
   void PostTaskEvent(CPaasTaskEvent task);
private:
   VHRoomDelegate * mVHRoomDelegate = nullptr;
   //RtcSDKEventDelegate* mRtcDelegate = nullptr;
   //VHRoomMonitor * mpVHRoomMonitor = nullptr;                  //回调对象
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
   std::atomic_bool mbLoginDone = false; //登陆成功 只上报一次。是否已经连接socket.io

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
