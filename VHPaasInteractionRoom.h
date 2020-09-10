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
      *   接口说明：用于本地调试使用。
      */
      virtual void SetDebugLogAddr(const std::string& addr){};
      /*
      *   接口说明：初始化。初始化接口必须在主函数中调用。
      *   room_delegate： 房间消息事件回调
      *   rtc_deleage：   互动事件消息回调
      */
      virtual int InitSDK(WebRtcSDKEventInterface* obj, VHRoomDelegate * VHRoomDelegate,
         WebRtcSDKEventInterface* rtc_deleage, std::wstring logPath = std::wstring(),
         const std::string domain = "http://api.vhallyun.com/sdk/v1") = 0;
      virtual bool isConnetWebRtcRoom() = 0;
      virtual void ClearInitInfo() = 0;
      /*初始化连接媒体房间所需参数*/
      virtual void InitRtcRoomParameter(const InavInfo& inavInfo, const std::string& log_server,
         const std::string& InavRoomId, const std::string& Appid) = 0;
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
      virtual bool Login(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id, const std::string& inav_roomid, const std::string& live_roomid, const std::list<std::string> listenChannel, const std::string context = std::string()) = 0;
      /*
      *  网络断开重连接口。
      *  到回调OnRoomConnectState接收到 RoomConnectState_NetWorkDisConnect 表示网络连接异常，需要手动重连。
      */
      virtual int ReconnectNetWork() = 0;
      /*
      *  接口说明：登出互动服务处理。
      *  回调监听：VHRoomDelegate
      */
      virtual void LogOut() = 0;
      /**
      *  接口说明：获取当前用户权限。根据权限判断那些功能可用。
      */
      virtual Permission GetPermission() = 0;
      /*
      *  接口说明：重新获取当前账号权限
      *  回调监听：VHRoomDelegate
      */
      virtual void ReFreshPermission() = 0;
      /**
      *  接口说明：进入互动媒体房间
      *  回调监听：RtcSDKEventDelegate
      *  userData : 用户自定义字段，远端订阅时可解析相关字段。
      **/
      virtual bool JoinRTCRoom(std::string userData = std::string(), bool bEnableSimulCaset = true,
         std::string vid = std::string(), std::string vfid = std::string(), std::string strWebinarId = std::string(),
      int role = 1, int classType = 1) = 0;
      /*
      *  接口说明：退出互动媒体房间
      *  返回值：true 退出成功；false 退出失败
      */
      virtual bool LeaveRTCRoom() = 0;
      /*
      *  禁止订阅流并取消订阅远端流
      */
      virtual void DisableSubScribeStream() = 0;
      /*
      *   使能订阅流并开始订阅房间中的远端流
      */
      virtual void EnableSubScribeStream() = 0;
      /*
      *   房间是否已连接
      */
      virtual bool IsWebRtcRoomConnected() = 0;
      /*
      *  接口说明：SDK互动房间人员列表
      *  回调监听：VHRoomDelegate
      */
      virtual void AsynGetVHRoomMembers() = 0;
      /**
      *  接口说明：申请上麦。
      *  回调监听：VHRoomDelegate
      *  返回值：VhallLiveErrCode
      */
      virtual int ApplyInavPublish() = 0;
      /**
      *  接口说明：审核申请上麦
      *  参数说明：
      *    audit_user_id: 受审核人第三放用户ID
      *    type: 是否同意，AuditPublish
      *  回调监听：VHRoomDelegate
      *  返回值：VhallLiveErrCode
      **/
      virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) = 0;
      /**
      *  接口说明：邀请上麦
      *  参数说明：
      *    audit_user_id: 受审核人第三放用户ID
      *  回调监听：VHRoomDelegate
      *  返回值：VhallLiveErrCode
      **/
      virtual int AskforInavPublish(const std::string& audit_user_id) = 0;
      /**
      *  接口说明：上/下/拒绝上麦状态回执,用户成功推流后调用服务端API
      *  参数说明：
      *       stream_id: 操作的流ID
      *       type:      PushStreamEvent
      *  回调监听：VHRoomDelegate
      */
      virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string()) = 0;
      /**
      *  接口说明：踢出流/取消踢出流
      *  参数说明：
      *     type: 1 踢出流（默认） 2 取消踢出流
      *     kick_user_id	string	是	被踢出的第三方用户ID
      *  回调监听：VHRoomDelegate
      *  返回值：VhallLiveErrCode
      **/
      virtual int KickInavStream(const std::string& kick_user_id, KickStream type) = 0;
      /**
      *  接口说明：踢出互动房间/取消踢出互动房间
      *  参数说明：
      *    type:1 踢出互动房间（默认） 2 取消踢出互动房间
      *    kick_user_id:被踢出的第三方用户ID
      *  回调监听：VHRoomDelegate
      *  返回值：VhallLiveErrCode
      **/
      virtual int KickInav(const std::string& kick_user_id, KickStream type) = 0;
      /*
      *  接口说明：获取被踢出互动房间人列表
      *  回调监听：VHRoomDelegate
      */
      virtual void GetKickInavList() = 0;
      /*
      *  接口说明：开启旁路直播,此接口是互动房间有旁路推流权限的用户进行调用的,当处理桌面共享或插播时如果想显示在主画面需要调用SetMainView接口，
                    将对应的流ID作为参数进行设置，如果只想显示一路则同时需要设置布局模式。需要监听回调
      *  参数说明：
      *     layoutMode:混流端布局模式
      *     profileIndex: 旁路分辨率
      *  回调监听：RtcSDKEventDelegate
      *  返回值：VhallLiveErrCode
      */
      virtual int StartConfigBroadCast(LayoutMode layoutMode,
         BroadCastVideoProfileIndex profileIndex, bool showBorder = true,
         std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")
         , SetBroadCastEventCallback fun = nullptr) = 0;

      /*  接口说明：开启旁路直播后，可通过此接口修改混流布局
      *  回调监听：RtcSDKEventDelegate
      */
      //virtual int ChangeLayout(LayoutMode layoutMode, std::string custom = std::string());
      virtual int SetConfigBroadCastLayOut(LayoutMode mode, SetBroadCastEventCallback fun = nullptr) = 0;

      /**
      *  接口说明：当开启旁路直播之后，可以将某个用户的流设置在混流画面的主画面当中，此接口是互动房间有旁路推流权限的用户进行调用
      *  回调监听：RtcSDKEventDelegate
      *  返回值：  VhallLiveErrCode
      **/
      virtual int SetMainView(std::string stream_id/*,int stream_type*/, SetBroadCastEventCallback fun/* = nullptr*/) = 0;
      /*
      *   接口说明：停止旁路直播。此接口是互动房间有旁路推流权限的用户进行调用的
      *   参数说明：
      *       live_room_id: 直播房间id
      *  回调监听：RtcSDKEventDelegate
      *   返回值：VhallLiveErrCode
      */
      virtual int StopPublishInavAnother() = 0;
      /*
      *   接口说明：互动房间是否已经连接成功
      *   返回值：true互动房间已连接，false互动房间链接断开
      */
      virtual bool IsVHMediaConnected() = 0;
      /*
      *   接口说明：获取摄像头列表
      *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
      */
      virtual void GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras) = 0;

      /*
      *  获取摄像头列表详情
      */
      virtual int GetCameraDevDetails(std::list<CameraDetailsInfo> &cameraDetails) = 0;
      /*
      *   接口说明：获取麦克风列表
      *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
      **/
      virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) = 0;
      /*
      *  是否存在音频或视频设备。
      *  返回值：只要存在一类设备 返回true, 如果音视频设备都没有则返回false
      **/
      virtual bool HasVideoOrAudioDev() = 0;
      virtual bool HasVideoDev() = 0;
      virtual bool HasAudioDev() = 0;
      /**
      *   接口说明：获取扬声器列表
      *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
      **/
      virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList) = 0;
      /*
      *  接口说明：设置使用的麦克风，此接口为全局接口。如果设置之后，会实时生效。既推流麦克风采集的设备已经被切换。
      *  参数说明：
      *   index: GetMicDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
      */
      virtual void SetUseMicIndex(int index, std::string devId, std::wstring desktopCaptureId) = 0;
      /*
      *  接口说明：设置使用播放声音的扬声器
      *  参数说明：
      *   index: GetPlayerDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
      */
      virtual void SetUsePlayer(int index, std::string devId) = 0;
      /*
      *   接口说明：设置当前使用的麦克风音量
      *   参数说明：
      *       vol:参数范围 0~100
      **/
      virtual bool SetCurrentMicVol(int vol) = 0;
      /*
      *   接口说明：获取当前使用的麦克风音量
      *   返回值：当前音量
      **/
      virtual int GetCurrentMicVol() = 0;
      /*
      *   接口说明：设置当前使用的扬声器音量
      *   参数说明：
      *       vol:参数范围 0~100
      **/
      virtual bool SetCurrentPlayVol(int vol) = 0;
      /*
      *   接口说明：获取当前使用的扬声器音量
      *   返回值：返回当前的音量值
      **/
      virtual int GetCurrentPlayVol() = 0;
      /*
      *   接口说明：关闭摄像头
      *   返回值：true 关闭成功/ false 关闭失败
      */
      virtual bool CloseCamera() = 0;
      /*
      *   接口说明：打开已经开始采集的摄像头
      *   返回值：true 打开成功/ false 打开失败
      */
      virtual bool OpenCamera() = 0;
      /*
      *   接口说明：摄像头是否关闭
      *   返回值：true 已经关闭/ false 未关闭
      */
      virtual bool IsCameraOpen() = 0;
      /*
      *   接口说明：关闭麦克风
      *   返回值：true 关闭成功/ false 关闭失败
      */
      virtual bool CloseMic() = 0;
      /*
      *   接口说明：打开麦克风
      *   返回值：true 打开成功/ false 打开失败
      */
      virtual bool OpenMic() = 0;
      /*
      *   接口说明：麦克风是否打开
      *   返回值：true 已经关闭/ false 未关闭
      */
      virtual bool IsMicOpen() = 0;
      /**
      *  获取当前使用的摄像设备信息
      ***/
      virtual void GetCurrentCameraInfo(int &index, std::string& devId) = 0;
      /**
      * 摄像头画面预览，当预览结束之后需要停止预览，否则摄像头将被一直占用
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int PreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
      /*
      *  返回预览Mic音量
      */
      virtual int GetMicVolumValue() = 0;
      /*
*  更换预览mic
*/
      virtual void ChangePreViewMic(int micIndex) = 0;
      /*
      *  停止预览画面
      */
      virtual int StopPreviewCamera() = 0;
      /*
      *   接口说明：开始摄像头\麦克风采集  摄像头采集和图片采集可以切换使用并且不影响推流。
      *   参数说明：
      *       devId： 摄像头设备id
      *       VideoProfileIndex：推流分辨率
      *       doublePush: 推流时，是否开启双流。
      *   回调监听：RtcSDKEventDelegate
      *   返回值：VhallLiveErrCode
      */
      virtual int StartLocalCapture(const std::string camera_devId, VideoProfileIndex index, bool doublePush = false) = 0;
      virtual int StartLocalAuxiliaryCapture(const std::string devId, VideoProfileIndex index) = 0;  //双推画面推流
      /*
      *   接口说明：图片采集. 切换图片会替换摄像头的推流
      *   参数说明：
      *       filePath：图片路径。
      *       VideoProfileIndex：分辨率
      *       doublePush: 推流时，是否开启双流。
      *   监听：OnOpenCaptureCallback
      *   返回值：VhallLiveErrCode
      */
      virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false) = 0;
      /*
      *   接口说明：开始摄像头/图片数据推流 ，必须先打开摄像头之后才能推流。
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int StartPushLocalStream() = 0;
      virtual int StartPushLocalAuxiliaryStream(std::string context = std::string()) = 0;
      /*
      *   接口说明：停止摄像头、图片数据推流
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int StopPushLocalStream() = 0;
      virtual int StopPushLocalAuxiliaryStream() = 0;
      /*
      *   接口说明：停止摄像头\图片采集  如果已经开始推流会同时停止推流
      */
      virtual void StopLocalCapture() = 0;
      virtual void StopLocalAuxiliaryCapture() = 0;
      /**
      *   接口说明：开始桌面采集。调用此接口必须确保有麦克风设备，且设置过麦克风，否则会采集异常。
      *   参数：
      *       player_dev; 桌面采集使用的扬声器设备id
      *       float:采集音量 0-100
      **/
      virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring player_dev, const int volume) = 0;
      /*
      *   停止桌面音频采集
      */
      virtual int StopLocalCapturePlayer() = 0;
      ///**
      //*   接口说明：当前互动房间是否存在桌面共享视频流
      //*/
      //virtual bool IsUserPushingDesktopStream() = 0;
      ///**
      //*   接口说明：当前互动房间是否存在插播视频流
      //*   返回值：ture成功。false失败
      //*/
      //virtual bool IsUserPushingMediaFileStream() = 0;
      /**
      *   接口说明：当前采集类型是否正在推流
      *   返回值：ture成功。false失败
      */
      virtual bool IsPushingStream(VHStreamType streamType) = 0;
      /*
      *   接口说明：当前采集类型是否正在进行本地数据源采集
      */
      virtual bool IsCapturingStream(VHStreamType streamType) = 0;
      /**
      *   接口说明：获取流用户推流ID
      *   参数说明：
      *       user_id：用户id
      *       streamType: 流类型
      *   返回值：流id
      */
      virtual std::string GetUserStreamID(const std::wstring user_id, VHStreamType streamType) = 0;
      /*
      *   接口说明：开始渲染媒体数据流
      *   参数说明：
      *       wnd： 渲染窗口句柄
      *       streamType: 流类型
      *   返回值：ture成功。false失败
      */
      virtual bool StartRenderStream(void* wnd, vlive::VHStreamType streamType) = 0;   
      /*
      *   接口说明：开始渲染媒体数据流
      *   参数说明：
      *       user_id：用户id
      *       wnd： 渲染窗口句柄
      *       streamType: 流类型
      *   返回值：ture成功。false失败
      */
      virtual bool StartRenderSubscribeStream(const std::wstring& user, vlive::VHStreamType streamType, void* wnd) = 0;
      /*
      *    获取窗口共享和桌面共享列表
      */
      virtual std::vector<DesktopCaptureSource> GetDesktops(int streamType) = 0;
      /*
      *   接口说明：开始桌面共享采集,.
      *    index:GetDesktops返回值中DesktopCaptureSource.id
      *   回调监听：RtcSDKEventDelegate
      *   返回值：VhallLiveErrCode
      */
      virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) = 0;
      /*
      *   接口说明：停止桌面共享采集  如果已经开始推流会同时停止推流
      */
      virtual void StopDesktopCapture() = 0;
      /*
      *   接口说明：开始桌面共享采集推流
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int StartPushDesktopStream(std::string context = std::string()) = 0;
      /*
      *   接口说明：停止桌面共享采集推流
      *   回调监听：RtcSDKEventDelegate
      *   返回值：VhallLiveErrCode
      */
      virtual int StopPushDesktopStream() = 0;
      /*
      * 设置选择的软件源
      */
      virtual int SelectSource(int64_t id) = 0;
      /*
      * 停止软件源共享采集
      */
      virtual void StopSoftwareCapture() = 0;
      /*
      * 停止软件共享采集推流
      */
      virtual int StopPushSoftWareStream() = 0;
      /*
      * 开始软件共享采集推流
      */
      virtual int StartPushSoftWareStream() = 0;
      /*
      *   判断图片有效性
      */
      virtual int CheckPicEffectiveness(const std::string filePath) = 0;
      /**
      *  获取插播文件原始大小
      */
      virtual int GetPlayMeidaFileWidthAndHeight(std::string filePath, int& srcWidth, int &srcHeight) = 0;
      /*
      *   接口说明：开始插播文件
      *   返回值：VhallLiveErrCode
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int StartPlayMediaFile(std::string filePath, VideoProfileIndex profileIndex = RTC_VIDEO_PROFILE_720P_16x9_H, long long seekPos = 0) = 0;
      /*
      *  插播文件是否带有视频画面。
      */
      virtual bool IsPlayFileHasVideo() = 0;
      /*
      *   设置插播文件音量
      */
      virtual int MediaFileVolumeChange(int vol) = 0;
      /**
      *   切换插播推流分辨率
      */
      virtual void ChangeMediaFileProfile(VideoProfileIndex profileIndex) = 0;
      /*
      *   接口说明：停止插播文件, 如果已经开始推流会同时停止推流
      */
      virtual void StopMediaFileCapture() = 0;
      /*
      *   接口说明：开始插播文件推流
      *   返回值：VhallLiveErrCode
      *   回调监听：RtcSDKEventDelegate
      */
      virtual int StartPushMediaFileStream() = 0;
      /*
      *   接口说明：停止插播文件推流
      *   回调监听：RtcSDKEventDelegate
      */
      virtual void StopPushMediaFileStream() = 0;
      /*
      *   接口说明：插播文件暂停处理
      *   返回值：VhallLiveErrCode
      */
      virtual void MediaFilePause() = 0;
      /*
      *   插播文件恢复处理
      *   返回值：VhallLiveErrCode
      */
      virtual void MediaFileResume() = 0;
      /*
      *   接口说明：插播文件快进处理
      *   返回值：VhallLiveErrCode
      */
      virtual void MediaFileSeek(const unsigned long long& seekTimeInMs) = 0;
      /*
      *   接口说明：插播文件总时长
      */
      virtual const long long MediaFileGetMaxDuration() = 0;
      /*
      *   接口说明：插播文件当前时长
      */
      virtual const long long MediaFileGetCurrentDuration() = 0;
      /*
      *   接口说明：插播文件的当前状态
      *   返回值：播放状态  MEDIA_FILE_PLAY_STATE
      */
      virtual int MediaGetPlayerState() = 0;
      /**
      *  接口说明：订阅流。
      *   回调监听：RtcSDKEventDelegate
      */
      virtual void SubScribeRemoteStream(const std::string &stream_id, int delayTimeOut = 0) = 0;
      /*
       *  接口说明：当接收到远端数据后，可以修改订阅的画面为大流或小流.
      *   回调监听：RtcSDKEventDelegate
      */
      virtual bool ChangeSubScribeUserSimulCast(const std::wstring& user_id, vlive::VHStreamType streamType, VHSimulCastType type) = 0;
      /*
      *   接口说明：打开、关闭远端用户本地视频
      *   参数说明：
      *       user_id：用户id
      *       close：true 关闭/ false 打开
      *   返回值：VhallLiveErrCode
      */
      virtual int OperateRemoteUserVideo(std::wstring user_id, bool close = false) = 0;
      /*
      *   接口说明：打开、关闭远端用户本地声音
      *   参数说明：
      *       user_id：用户id
      *       close：true 关闭/ false 打开
      *   返回值：VhallLiveErrCode
      */
      virtual int OperateRemoteUserAudio(std::wstring user_id, bool close = false) = 0;
      /**
      *  打开或关闭所有订阅的音频
      */
      virtual int MuteAllSubScribeAudio(bool mute) = 0;
      /**
      *   接口说明：注册消息监听接口，用户可用于监听底层所有的消息事件内容。如果用户不关注，可通过 InitSDK 接口注册的VHRoomDelegate进行监听。
      */
      virtual void RegisterMsgListenr(RecvMsgDelegate*) = 0;
      /*
      *   停止接收所有远端数据流
      */
      virtual void StopRecvAllRemoteStream() = 0;
      /*
      *   执行paasSdk任务
      */
      virtual void implementTask(int iTask) = 0;
      /*
      *   混流配置准备完成
      */
      virtual bool IsEnableConfigMixStream() = 0;
      virtual void SetEnableConfigMixStream(bool enable) = 0;
      /*
      *   返回本地双推流 id
      */
      virtual std::string GetLocalAuxiliaryId() = 0;
      /*
      *   返回远端双推流 id
      */
      virtual std::string GetAuxiliaryId() = 0;
      /*
*   返回流类型
*/
      virtual VHStreamType  CalcStreamType(const bool& bAudio, const bool& bVedio) = 0;
      /*
*   设置采集扬声器的音量值
*/
      virtual int SetLocalCapturePlayerVolume(const int volume) = 0;
      /*
*   停止播放音频文件接口
*/
      virtual int StopPlayAudioFile() = 0;

      /*
*   获取播放文件音量
*/
      virtual int GetPlayAudioFileVolum() = 0;
      /*
*   添加播放音频文件接口
*/
      virtual int PlayAudioFile(std::string fileName, int devIndex) = 0;

      /*
   *   切换下一个摄像头
   */
      virtual int ChangeNextCamera(VideoProfileIndex videoProfile) = 0;
      /**
      *  获取当前使用的摄像设备信息
      ***/
      virtual void GetCurrentCamera(std::string& devId) = 0;
      /**
*  获取当前使用的麦克风设备信息
***/
      virtual void GetCurrentMic(int &index, std::string& devId) = 0;
      /*
      *获取所有订阅的音频状态(打开/关闭)
      */
      virtual bool GetMuteAllAudio() = 0;
      /*
*  设置使用的麦克风
*  index: GetMicDevices获取的列表中 vhall::AudioDevProperty中的 devIndex
*/
      virtual int SetUsedMic(int micIndex, std::string micDevId, std::wstring desktopCaptureId) = 0;
      /*
      *  判断给定媒体类型是否支持
      */
      virtual bool IsSupportMediaFormat(CaptureStreamAVType type) = 0;
      //渲染远端流
      virtual bool StopRenderRemoteStream(const std::wstring& user, const std::string streamId, vlive::VHStreamType streamType) = 0;
      virtual bool StartRenderRemoteStreamByInterface(const std::wstring& user, vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive) = 0;
      virtual bool IsRemoteStreamIsExist(const std::wstring& user, vlive::VHStreamType streamType) = 0;
      /**
*   开始渲染"本地"摄像设备、桌面共享、文件插播媒体数据流.
*/
      virtual bool StartRenderLocalStream(vlive::VHStreamType streamType, void* wnd) = 0;
      virtual bool StartRenderLocalStreamByInterface(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive) = 0;
      virtual bool IsStreamExit(std::string id) = 0;
      /**
*   获取订阅流中流类型，是否包括音视频数据。
*   返回值：0 查找成功，其它失败
*/
      virtual int GetSubScribeStreamFormat(const std::wstring& user, vlive::VHStreamType streamType, bool &hasVideo, bool &hasAudio) = 0;
      /**
*   获取流ID
*   返回值：如果有流id返回流id 否则返回空字符串
*/
      virtual std::string GetSubScribeStreamId(const std::wstring& user, vlive::VHStreamType streamType) = 0;
      /*当前房间是否存在给定类型的远端流*/
      virtual bool IsExitSubScribeStream(const vlive::VHStreamType& streamType) = 0;
      virtual bool IsExitSubScribeStream(const std::string& id, const vlive::VHStreamType& streamType) = 0;

      virtual void GetStreams(std::list<std::string>& streams) = 0;
      virtual void ClearSubScribeStream() = 0;
      virtual std::string LocalStreamId() = 0;
      /**
*  获取推流视频丢包率.3秒读一次
*/
      virtual double GetPushDesktopVideoLostRate() = 0;
   };

   VHPAASSDK_EXPORT VHPaasInteractionRoom* GetPaasSDKInstance();
   VHPAASSDK_EXPORT void DestoryPaasSDKInstance();

}