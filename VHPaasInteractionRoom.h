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
    *   接口说明：初始化。接口必须在主函数中调用。
    *   参数：VLiveRoomMsgEventDelegate 监听房间内相关事件通知。
    *         VRtcEngineEventDelegate   监听RTC引擎事件通知。
    */
    virtual void InitSDK(VLiveRoomMsgEventDelegate* liveRoom_delegate, VRtcEngineEventDelegate* rtcRoom_delegate, const std::string domain = "http://api.vhallyun.com/sdk/v1", std::wstring logPath = std::wstring()) = 0;
    /*
    *   接口说明：如果需要使用HTTP代理，需要调用[InitSDK]之后调用此接口，如果不进行代理可以忽略此接口。
    *   参数说明：
    *    enable： true表示开始，false表示关闭
    *    addr： 代理地址
    *    port： 代理端口
    *    userName： 代理用户名
    *    pwd： 代理用户密码
    **/
    virtual void SetHttpProxy(bool enable, std::string addr = std::string(), int port = 0 ,std::string userName = std::string(), std::string pwd = std::string()) = 0;
    /*
    *   接口说明：登陆接口。登录微吼业务服务器，登录成功之后即可加入“互动媒体房间”
    *   接口参数：
    *      accesstoken：通过平台获取产生的token
    *      appid：应用id
    *      thrid_user_id：用户id
    *      inav_roomid：互动房间id
    *      channel_id: 监听的消息channel_id
    *      context:  登录时添加的自定义字段，json格式
    *   回调监听：监听RoomEvent::RoomEvent_Login事件
    */
    virtual bool LoginRoom(const std::string& accesstoken, const std::string& appid, const std::string& thrid_user_id,const std::string& inav_roomid,const std::string& live_roomid,const std::string channel_id = std::string(), const std::string context = std::string()) = 0;
    /*
    *  接口说明：登出微吼业务服务器。如果已经加入互动房间，需要先调用LeaveVHMediaRoom 。
    *  回调监听：监听RoomEvent::RoomEvent_Logout事件
    */ 
    virtual void LogoutRoom() = 0;
    /**
    *  接口说明：获取当前用户权限。根据权限判断那些功能可用。
    */
    virtual Permission GetPermission() = 0;
    /**
    *  接口说明：进入互动媒体房间, 区分于[LoginRoom]。
    *  回调监听：监听VHRoomConnectEnum枚举事件
    *  userData : 用户自定义字段
    **/
    virtual void JoinRtcRoom(std::string userData = std::string()) = 0;
    /*
    *  接口说明：退出互动媒体房间
    *  返回值：true 退出成功；false 退出失败
    */
    virtual bool LeaveRtcRoom() = 0;
    /*
    *  接口说明：SDK互动房间人员列表
    *  回调监听：
    *       成功：监听回调OnGetVHRoomMembers
    *       失败：OnFailedEvent
    */
    virtual void AsynGetVHRoomMembers() = 0;
    /**
    *  接口说明：申请上麦。
    *  回调监听事件：RoomEvent::RoomEvent_Apply_Push
    *  返回值：VhallLiveErrCode 
    */
    virtual int ApplyInavPublish() = 0;
    /**
    *  接口说明：审核申请上麦
    *  参数说明：
    *    audit_user_id: 受审核人第三放用户ID 
    *    type: 是否同意，AuditPublish
    *  回调监听事件：RoomEvent::RoomEvent_AuditInavPublish
    *  返回值：VhallLiveErrCode
    **/
    virtual int AuditInavPublish(const std::string& audit_user_id, AuditPublish type) = 0; 
    /**
    *  接口说明：邀请上麦
    *  参数说明：
    *    audit_user_id: 受审核人第三放用户ID
    *  回调监听事件：RoomEvent::RoomEvent_AskforInavPublish
    *  返回值：VhallLiveErrCode
    **/
    virtual int AskforInavPublish(const std::string& audit_user_id) = 0;
    /**
    *  接口说明：上/下/拒绝上麦状态回执,用户成功推流后调用服务端API
    *  参数说明：
    *       stream_id: 操作的流ID
    *       type:      PushStreamEvent
    *  回调监听事件：RoomEvent::RoomEvent_UserPublishCallback
    */
    virtual void UserPublishCallback(PushStreamEvent type, const std::string& stream_id = std::string()) = 0;
    /**
    *  接口说明：踢出流/取消踢出流
    *  参数说明：
    *     type: 1 踢出流（默认） 2 取消踢出流
    *     kick_user_id	string	是	被踢出的第三方用户ID
    *  回调监听事件：RoomEvent::RoomEvent_KickInavStream
    *  返回值：VhallLiveErrCode
    **/
    virtual int KickInavStream(const std::string& kick_user_id, KickStream type) = 0;
    /**
    *  接口说明：踢出互动房间/取消踢出互动房间
    *  参数说明：
    *    type:1 踢出互动房间（默认） 2 取消踢出互动房间
    *    kick_user_id:被踢出的第三方用户ID
    *  回调监听事件：RoomEvent::RoomEvent_KickInav
    *  返回值：VhallLiveErrCode
    **/
    virtual int KickInav(const std::string& kick_user_id, KickStream type) = 0;
    /*
    *  接口说明：获取被踢出互动房间人列表
    *  回调函数：OnGetVHRoomKickOutMembers、OnFailedEvent
    */
    virtual void GetKickInavList() = 0;
    /*
    *  接口说明：开启旁路直播,此接口是互动房间有旁路推流权限的用户进行调用的,当处理桌面共享或插播时如果想显示在主画面需要调用SetMainView接口，
                  将对应的流ID作为参数进行设置，如果只想显示一路则同时需要设置布局模式。需要监听回调
    *  参数说明：
    *     live_room_id: 直播房间id
    *     layoutMode:混流端布局模式
    *     width： 混流端布局的宽
    *     height： 混流端布局的高
    *     fps：帧率
    *     bitrate： 码率
    *  回调监听：RoomEvent::RoomEvent_Start_PublishInavAnother
    *  返回值：VhallLiveErrCode
    */
    virtual int StartPublishInavAnother(std::string live_room_id, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder = true, std::string boradColor = std::string("0x666666"), std::string backGroundColor = std::string("0x000000")) = 0;
    /*
    *  接口说明：开启旁路直播后，可通过此接口修复混流布局
    */
    virtual int ChangeLayout(std::string layoutMode, std::string custom = std::string()) = 0;
    /**
    *  接口说明：当开启旁路直播之后，可以将某个用户的流设置在混流画面的主画面当中，此接口是互动房间有旁路推流权限的用户进行调用
    *  回调监听：RoomEvent::RoomEvent_SetMainView
    *  返回值：VhallLiveErrCode
    **/
    virtual int SetMainView(std::string stream_id) = 0;
    /*
    *   接口说明：停止旁路直播。此接口是互动房间有旁路推流权限的用户进行调用的
    *   参数说明：
    *       live_room_id: 直播房间id
    *   回调监听：RoomEvent::RoomEvent_Stop_PublishInavAnother
    *   返回值：VhallLiveErrCode
    */
    virtual int StopPublishInavAnother(std::string live_room_id) = 0;
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
    *   接口说明：获取麦克风列表
    *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
    **/
    virtual void GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) = 0;
    /**
    *   接口说明：获取扬声器列表
    *   参数说明：返回获取到的设备列表，如果设备列表为空说明没有获取到相应的设备
    **/
    virtual void GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList) = 0;
    /*
    *  接口说明：设置使用的麦克风
    *  参数说明：
    *   index: GetMicDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
    */
    virtual void SetUseMicIndex(int index,std::string devId) = 0;
    /*
    *  接口说明：设置使用的扬声器
    *  参数说明：
    *   index: GetPlayerDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
    */
    virtual void SetUserPlayIndex(int index, std::string devId) = 0;
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
    *  设置桌面共享锐化.主要针对桌面采集文字时，遇到文字颜色不明显时，可以通过此接口提高文字亮度，提高共享文字清晰度的效果。
    *  此接口调用，需要在桌面共享采集成功之后进行调用。即监听[OnOpenCaptureCallback]回调事件。
    *  当关闭时可在桌面共享过程中随时关闭。
    */
    virtual int SetDesktopEdgeEnhance(bool enable) = 0;

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
    /*  
    *   接口说明：开始摄像头\麦克风采集  摄像头采集和图片采集可以切换使用并且不影响推流。 
    *   参数说明：
    *       VideoProfileIndex：通过获取设备信息时得到VideoDevProperty::mDevFormatList中设备支持的分辨率，根据分辨率信息选区对应VideoProfileIndex
    *   监听：OnOpenCaptureCallback
    *   返回值：VhallLiveErrCode
    */
    virtual int StartLocalCapture(const std::string devId,VideoProfileIndex index, bool doublePush = false) = 0;
    /*
    *   接口说明：图片采集. 切换图片会替换摄像头的推流
    *   参数说明：
    *       VideoProfileIndex：分辨率
    *   监听：OnOpenCaptureCallback
    *   返回值：VhallLiveErrCode
    */
    virtual int StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush = false) = 0;
    /**
    *   开始桌面采集与采集音量控制
    *   参数：
    *       dev_index:设备索引
    *       dev_id;设备id
    *       float:采集音量 0-100
    **/
    virtual int StartLocalCapturePlayer(const int dev_index, const std::wstring dev_id, const int volume) = 0;
    /*
    *   停止桌面音频采集
    */
    virtual int StopLocalCapturePlayer() = 0;
    /*
    *   接口说明：停止摄像头\图片采集  如果已经开始推流会同时停止推流
    */
    virtual void StopLocalCapture() = 0;
    /*
    *   接口说明：开始摄像头数据推流 ，必须先打开摄像头之后才能推流
    *   回调监听：OnPushStreamSuc OnPushStreamError
    */
    virtual int StartPushLocalStream() = 0;
    /*
    *   接口说明：停止摄像头、图片数据推流 
    *   回调监听： OnStopPushStreamCallback
    */
    virtual int StopPushLocalStream() = 0;
    /**
    *  接口说明：摄像头预览。
    **/
    virtual int StartPreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
    /**
   *   接口说明：摄像头预览。
   **/
   virtual int StartPreviewCamera(std::shared_ptr<vhall::VideoRenderReceiveInterface> recv, const std::string& devGuid, VideoProfileIndex index, int micIndex = -1) = 0;
   /**
   *   接口说明：结束摄像头预览。
   **/
   virtual int StopPreviewCamera() = 0;
   /**
   *  接口说明：检测是否支持美颜功能
   **/
   virtual bool IsSupprotBeauty() = 0;
    /*
   *   接口说明：设置摄像头美颜级别。Level级别为0-5，其中level=0是表示关闭，开启需要将level设置为1-5任意值。
   *   此接口调用在摄像头开启或关闭前调用均可以。当改变此值时推流画面会实时生效。
   *   需要特别注意的是，如果设备第一次打开，可使用[PreviewCamera]接口直接打开并显示回显。但是当
   *   此设备已经被占用，建议使用[bool StartRenderLocalStream(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> recv); ]
   *   接口进行本地流自定义渲染，当预览没有画面时可通过设备ID进行匹配，是否接收到此设备数据，如果接收到则通过【vhall::VideoRenderReceiveInterface】返回的数据
   *   进行自定义渲染。
   *   回调监听： OnStopPushStreamCallback
   */
    virtual int SetCameraBeautyLevel(int level) = 0;
    /*
   *   接口说明：设置预览摄像头美颜级别。Level级别为0-5，其中level=0是表示关闭，开启需要将level设置为1-5任意值。
   */
    virtual int SetPreviewCameraBeautyLevel(int level) = 0;
    /*
    *   接口说明：当前采集类型是否正在进行本地数据源采集
    */
    virtual bool IsCapturingStream(VHStreamType streamType) = 0;
    /**
    *   接口说明：当前互动房间是否存在桌面共享视频流
    */
    virtual bool IsUserPushingDesktopStream() = 0;
    /**
    *   接口说明：当前互动房间是否存在插播视频流
    *   返回值：ture成功。false失败
    */
    virtual bool IsUserPushingMediaFileStream() = 0;
    /**
    *   接口说明：当前采集类型是否正在推流
    *   返回值：ture成功。false失败
    */
    virtual bool IsPushingStream(VHStreamType streamType) = 0;
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
    *       user_id：用户id
    *       wnd： 渲染窗口句柄
    *       streamType: 流类型
    *   返回值：ture成功。false失败
    */
    virtual bool StartRenderStream(const std::wstring& user,void*  wnd, vlive::VHStreamType streamType) = 0;
    /*
    *   接口说明：开始桌面共享采集  
    *   监听：OnOpenCaptureCallback
    *   返回值：VhallLiveErrCode
    */
    virtual int StartDesktopCapture(int index, VideoProfileIndex profileIndex) = 0;
    /*
    *   接口说明：停止桌面共享采集  如果已经开始推流会同时停止推流
    */
    virtual void StopDesktopCapture() = 0;
    /*
    *   接口说明：开始桌面共享采集推流 
    *   回调检测：OnPushStreamSuc OnPushStreamError
    */
    virtual int StartPushDesktopStream() = 0;
    /*
    *   接口说明：停止桌面共享采集推流 
    *   需要回调检测:OnStopPushStreamCallback
    *   返回值：VhallLiveErrCode
    */
    virtual int StopPushDesktopStream() = 0;
    /*
    *   接口说明：初始化文件插播对象
    *   返回值：VhallLiveErrCode
    *   回调监听：OnOpenCaptureCallback
    */
    virtual int InitMediaFile() = 0;
    /*
    *   接口说明：停止插播文件, 如果已经开始推流会同时停止推流
    */
    virtual void StopMediaFileCapture() = 0;
    /*
    *   接口说明：开始插播文件推流 
    *   返回值：VhallLiveErrCode
    *   回调检测：OnPushStreamError  OnPushStreamSuc
    */
    virtual int StartPushMediaFileStream() = 0;
    /*
    *   接口说明：停止插播文件推流
    *   回调检测：OnStopPushMediaFileStream
    */
    virtual void StopPushMediaFileStream() = 0;
    /**
    **  播放。推流成功后可以调用此接口进行文件播放
    */
    virtual bool PlayFile(std::string file, VideoProfileIndex profileIndex) = 0;
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
	/*
    *   接口说明：重新获取当前账号权限
    */
	virtual void ReFreshPermission() = 0;
    /**
    *  打开或关闭所有订阅的音频
    */
    virtual int MuteAllSubScribeAudio(bool mute) = 0;
};

VHPAASSDK_EXPORT VLiveRoom* GetPaasSDKInstance();
VHPAASSDK_EXPORT void DestoryPaasSDKInstance();

}