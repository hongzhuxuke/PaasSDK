#include "VHPaasRoomImpl.h"
#include <mutex>
#include "./log/vhall_paasSdk_log.h"
#include "lib/rapidjson/include/rapidjson/rapidjson.h"
#include "lib/rapidjson/include/rapidjson/stringbuffer.h"
#include "lib/rapidjson/include/rapidjson/writer.h"
#include "lib/rapidjson/include/rapidjson/reader.h"
#include "lib/rapidjson/include/rapidjson/document.h"
#include <string>
#include <sstream>
#include <stdio.h>    
#include <iostream> 
#include "src/sio_client.h"
#include "LogReport.h"


using namespace rapidjson;
using namespace std;

static std::mutex gSDKMutex;
static VHPaasRoomImpl* gPaasSDKInstance = nullptr;
static std::atomic_bool mbIsRuning = false;
CThreadLock VHPaasRoomImpl::mStatic_taskListMutex;
HANDLE VHPaasRoomImpl::mStatic_TaskEvent;   //
HANDLE VHPaasRoomImpl::mStatic_ThreadExitEvent;
std::atomic_bool VHPaasRoomImpl::mStatic_bThreadRun = true; //
std::list<CPaasTaskEvent> VHPaasRoomImpl::mStatic_taskEventList; //



#define RTC_FAILED    "failed"
#define RTC_SUCCESS   "success"


std::string& replace_all(std::string& str, const std::string& old_value, const std::string&   new_value)
{
   while (true) {
      std::string::size_type   pos(0);
      if ((pos = str.find(old_value)) != std::string::npos)
         str.replace(pos, old_value.length(), new_value);
      else
         break;
   }
   return str;
}

std::string WString2String(const std::wstring& ws)
{
    std::string strLocale = setlocale(LC_ALL, "");
    const wchar_t* wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char *chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs(chDest, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[]chDest;
    setlocale(LC_ALL, strLocale.c_str());
    return strResult;
}

VHPaasRoomImpl::VHPaasRoomImpl() {

    mIsHost = true;
    bIsSettinglayout = false;
    mpLogReport = new LogReport;

    mStatic_bThreadRun = true;
    mProcessThread = new std::thread(VHPaasRoomImpl::ThreadProcess, this);
    if (mProcessThread) {
       LOGD("create task thread success");

       mStatic_TaskEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
       mStatic_ThreadExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }


    LOGD("PAASSDK_VER : %s", PAASSDK_VER);
}

VHPaasRoomImpl::~VHPaasRoomImpl() {
    mbIsRuning = false;
    LOGD("VHPaasRoomImpl::~VHPaasRoomImpl");
    if (mpHttpManagerInterface) {
        //mpHttpManagerInterface->Release();
        DestoryHttpManagerInstance();
        mpHttpManagerInterface = nullptr;
    }
    if (mpWebRtcSDKInterface) {
        //delete mpWebRtcSDKInterface;
        //mpWebRtcSDKInterface = nullptr;
        DestroyWebRtcSDKInstance();
    }
    if (mpVHWebSocketInterface) {
        mbDisConnectWebSocket = true;
        mpVHWebSocketInterface->DisConnectWebSocket();
        mpVHWebSocketInterface->SyncDisConnectServer();
        delete mpVHWebSocketInterface;
        mpVHWebSocketInterface = nullptr;
        mbLoginDone = false;
    }
}
void VHPaasRoomImpl::OnOpen() {
    LOGD("OnOpen");
}

void VHPaasRoomImpl::OnFail() {
    LOGD("OnFail");
    if (mbIsRuning && mVHRoomDelegate) {
       mVHRoomDelegate->OnRoomConnectState(RoomConnectState_DisConnect);
    }
}
 
void VHPaasRoomImpl::OnReconnecting() {
    LOGD("OnReconnecting");
    if (mbIsRuning && mVHRoomDelegate) {
       mVHRoomDelegate->OnRoomConnectState(RoomConnectState_Reconnecting);
    }
}

void VHPaasRoomImpl::OnReconnect(unsigned code, unsigned num) {
    LOGD("OnReconnect code %d num %d", code, num);
    if (mbIsRuning && mVHRoomDelegate) {
        mVHRoomDelegate->OnRoomConnectState(RoomConnectState_Connected);
    }
}
void VHPaasRoomImpl::OnClose() {
    LOGD("OnClose");
    //不是主动断开的，要重连
    if (mVHRoomDelegate && !mbDisConnectWebSocket) {
       mVHRoomDelegate->OnRoomConnectState(RoomConnectState_NetWorkDisConnect);
    }
}
 
void VHPaasRoomImpl::OnSocketOpen(std::string const& nsp) {
    LOGD("OnSocketOpen msg:%s mbLoginDone:%d mbIsRuning:%d", nsp.c_str(), (int)mbLoginDone, (int)mbIsRuning);
    if (mbIsRuning && mVHRoomDelegate && !mbLoginDone) {
        mVHRoomDelegate->OnSuccessedEvent(RoomEvent_Login);
        mbLoginDone = true;
    };

    if (mbLoginDone) {
        SendServerJoinMsg(mContext);
    }
}

void VHPaasRoomImpl::SendServerJoinMsg(const std::string context) {
   std::list<std::string>::iterator iter = mListenChannels.begin();
   while (iter != mListenChannels.end()) {
      SendJoinMsg(*iter, context);
      iter++;
   }

    //if (mChannelId.size() > 0) {
    //    SendJoinMsg(mChannelId, context);
    //}

    //if (mInavRoomId.size() > 0) {
    //    SendJoinMsg(mInavRoomId, context);
    //}
}

void VHPaasRoomImpl::SendJoinMsg(std::string id, const std::string context) {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value username(StringRef(id.c_str()));
    document.AddMember("channel", username, allocator);

    rapidjson::Value password(StringRef(mThridUserId.c_str()));
    document.AddMember("third_party_user_id", password, allocator);


    rapidjson::Value contextStr(StringRef(context.c_str()));
    document.AddMember("context", contextStr, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();

    if (mpVHWebSocketInterface) {
        mpVHWebSocketInterface->SendMsg("join", data);
    }
    LOGD("OnSocketOpen  SendMsg:%s", data.c_str());
}
 
void VHPaasRoomImpl::OnSocketClose(std::string const& nsp) {
    LOGD("OnSocketClose  nsp:%s", nsp.c_str());
}

void VHPaasRoomImpl::OnServiceMsg(std::string type, const std::wstring user_id, const std::wstring nickname, const std::string rolename,bool is_banned, int devType, int uv/* = 0*/) {
    if (mbIsRuning && mVHRoomDelegate) {
        mVHRoomDelegate->OnServiceMsg(type, user_id, nickname, rolename, is_banned, devType);
    }
}

void VHPaasRoomImpl::OnRecvAllMsg(const char* data, int length) {
   if (mMsgDelegate) {
      mMsgDelegate->OnRecvChatMsg(std::string(data,length));
   }
}

void VHPaasRoomImpl::InitRtcRoomParameter(const InavInfo & inavInfo, const std::string & log_server, const std::string & InavRoomId, const std::string & Appid)
{
   //mInavInfo.inav_token = inavInfo.inav_token;
   //mInavInfo.third_party_user_id = inavInfo.third_party_user_id;
   //mInavInfo.push_address = inavInfo.push_address;
   //mInavInfo.account_id = inavInfo.account_id;
   //mLoginInfo.log_server = log_server;
   //mInavRoomId = InavRoomId;
   //mAppid = Appid;
}

void VHPaasRoomImpl::SetDebugLogAddr(const std::string& addr) {
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->SetDebugLogAddr(addr);
   }
}

int VHPaasRoomImpl::InitSDK(WebRtcSDKEventInterface* obj, VHRoomDelegate * VHRoomDelegate,WebRtcSDKEventInterface* rtc_delegate, std::wstring logPath /*= std::string()*/,const std::string domain /*= "http://api.vhallyun.com/sdk/v1"*/) {
	//std::wstring logFile = L"paassSdk";
	InitPaasSdkLog(logPath);
    LOGD("InitSDK");
    if (mbIsRuning) {
       return -1;
    }
    //mpVHRoomMonitor = monitor;
    mVHRoomDelegate = VHRoomDelegate;
    mRtcDelegate = rtc_delegate;
    mDomain = domain;
    if (mpHttpManagerInterface == nullptr) {
        mpHttpManagerInterface = GetHttpManagerInstance();
        LOGD("new HttpManager");
        if (mpHttpManagerInterface) {
            LOGD("Init HttpManager");
            mpHttpManagerInterface->Init(logPath);
            mpLogReport->RegisterHttpSender(mpHttpManagerInterface);
            UploadReportSDKInit();
        }
    }
    if (mpWebRtcSDKInterface == nullptr) {
        mpWebRtcSDKInterface = GetWebRtcSDKInstance();
        LOGD("GetWebRtcSDKInstance");
        if (mpWebRtcSDKInterface) {
            LOGD("RegisterCallBackObj");
            mpWebRtcSDKInterface->InitSDK(mRtcDelegate, logPath);
        }
    }
    //if (mpVHWebSocketInterface == nullptr) {
    //    mpVHWebSocketInterface = CreateVHWebSocket();
    //    LOGD("CreateVHWebSocket");
    //    mpVHWebSocketInterface->RegisterCallback(this);
    //}
    mbIsRuning = true;
    return 0;
}

/*
*  网络断开重连接口。
*  到回调OnRoomConnectState接收到 RoomConnectState_NetWorkDisConnect 表示网络连接异常，需要手动重连。
*/
int VHPaasRoomImpl::ReconnectNetWork(){
   //消息服务
   std::string url = replace_all(mLoginInfo.socket_server, "http", "ws");
   mpVHWebSocketInterface->ConnectServer(url.c_str(), mLoginInfo.connection_token.c_str());
   LOGD("url :%s connection_token %s", url.c_str(), mLoginInfo.connection_token.c_str());
   return 0;
}

bool VHPaasRoomImpl::Login(const std::string& accesstoken, const std::string& appid, 
   const std::string& thrid_user_id, const std::string& roomid, const std::string& live_roomid, const std::list<std::string> listenChannel, const std::string context /*= std::string()*/) {
    LOGD("LoginHVRoom");
    if (accesstoken.length() == 0 || appid.length() == 0 || thrid_user_id.length() == 0 || roomid.length() == 0) {
        LOGD("param is null");
        return false;
    }

    UploadReportSDKLogin(appid, thrid_user_id);
    LOGD("LoginHVRoom accesstoken:%s appid:%s thrid_user_id:%s roomid:%s", accesstoken.c_str(), appid.c_str(), thrid_user_id.c_str(), roomid.c_str());
    mContext = context;
    mAccesstoken = accesstoken;
    mAppid = appid;
    mThridUserId = thrid_user_id;
    mInavRoomId = live_roomid;
    mLiveRoomId = roomid;
    mListenChannels = listenChannel;
    //登录时先请求start信息
    HTTP_GET_REQUEST httpGet(GetInvaStartUrl()/*,"", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            //解析响应的start信息
            LOGD("data:%s code:%d", data.c_str(), code);
            AnalysisStartInfo(data, code);
        });
        return true;
    }
    return false;
}

void VHPaasRoomImpl::RegisterMsgListenr(RecvMsgDelegate* recv_msg_delegate) {
   mMsgDelegate = recv_msg_delegate;
}

void VHPaasRoomImpl::StopRecvAllRemoteStream() {
   if (mpWebRtcSDKInterface) {
      LOGD("StopRecvAllRemoteStream");
      mpWebRtcSDKInterface->StopRecvAllRemoteStream();
   }
}

std::string VHPaasRoomImpl::LocalStreamId()
{
   std::string strRef = "";
   if (mpWebRtcSDKInterface) {
      LOGD("LocalStreamId");
      strRef = mpWebRtcSDKInterface->LocalStreamId();
   }
   return strRef;
}

void VHPaasRoomImpl::AnalysisStartInfo(std::string data, int code) {
    if (code != 0) {
        if (mbIsRuning && mVHRoomDelegate) {
            LOGD("OnFailedEvent data:%s code:%d", data.c_str(), code);
            mVHRoomDelegate->OnFailedEvent(RoomEvent_Login, code, data);
        }
    }
    else {
        //解析start登录数据
        ParamToStartInfoResp(data);
        if (mbIsRuning && mVHRoomDelegate) {
            if (mLoginInfo.mCode != 200) {
                LOGD("OnFailedEvent data:%s code:%d", data.c_str(), code);
                mVHRoomDelegate->OnFailedEvent(RoomEvent_Login, mLoginInfo.mCode, mLoginInfo.mNoticeMsg);
            }
            else {
                //如果成功获取互动房间信息
                ToGetInavInfo();
            }
        }
    }
}

void VHPaasRoomImpl::ToGetInavInfo() {
    //请求互动房间信息
    HTTP_GET_REQUEST httpGet(GetInavInfoUrl()/*,"", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            LOGD("AnalysisGetInavInfo data:%s code:%d", data.c_str(), code);
            AnalysisGetInavInfo(data, code);
        });
    }
}

void VHPaasRoomImpl::ReFreshPermission()
{
	//请求互动房间信息
	HTTP_GET_REQUEST httpGet(GetInavInfoUrl()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
	if (mpHttpManagerInterface) {
		mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
			LOGD("ReFreshPermission data:%s code:%d", data.c_str(), code);
			ReFreshPermissionAnalysisInfo(data, code);
			
		});
	}
}


void VHPaasRoomImpl::AnalysisGetInavInfo(std::string data, int code) {
    if (code != 0) {
        if (mbIsRuning && mVHRoomDelegate) {
            LOGD("OnFailedEvent data:%s code:%d", data.c_str(), code);
            mVHRoomDelegate->OnFailedEvent(RoomEvent_Login, code, data);
        }
    }
    else {
        //解析登录数据
        ParamToInavInfoResp(data);
        //完成解析登录数据

        if (mbIsRuning && mVHRoomDelegate) {
            if (mInavInfo.mCode != 200) {
                LOGD("OnFailedEvent data:%s code:%d", data.c_str(), code);
                mVHRoomDelegate->OnFailedEvent(RoomEvent_Login, mInavInfo.mCode, mInavInfo.mNoticeMsg);
            }
            else {
                if (mpVHWebSocketInterface) {
                    mpVHWebSocketInterface->On("connected", [&](const std::string& name, const std::string& msg)->void {
                        LOGD("connected msg:%s", msg.c_str());
                    });
                    mpVHWebSocketInterface->On("joined", [&](const std::string& name, const std::string& msg)->void {
                        LOGD("joined msg:%s", msg.c_str());
                    });
                    mpVHWebSocketInterface->On("Inav", [&](const std::string& name, const std::string& msg)->void {
                        LOGD("Inav msg:%s", msg.c_str());
                        ParamRecvInavMsg(msg);
                        ParamRecvSocketMsg(msg);
                    });
                    mpVHWebSocketInterface->On("msg", [&](const std::string& name, const std::string& msg)->void {
                        LOGD("Inav msg:%s", msg.c_str());
                        ParamRecvInavMsg(msg);
                        ParamRecvSocketMsg(msg);
                    });
                    mpVHWebSocketInterface->On("flash", [&](const std::string& name, const std::string& msg)->void {
                        ParamRecvSocketMsg(msg);
                    });
                    mpVHWebSocketInterface->On("cmd", [&](const std::string& name, const std::string& msg)->void {
                        LOGD("cmd msg:%s", msg.c_str());
                        ParamRecvSocketMsg(msg);
                    });
                    mbLoginDone = false;

                    ////消息服务
                    //std::string url = replace_all(mLoginInfo.socket_server, "http", "ws");
                    //mpVHWebSocketInterface->ConnectServer(url.c_str(), mLoginInfo.connection_token.c_str());
                    //LOGD("url :%s connection_token %s", url.c_str(), mLoginInfo.connection_token.c_str());
                    ////聊天服务
                    //if (mListenChannels.size() > 0) {
                    //   std::string chatUrl = replace_all(mLoginInfo.nginx_server, "http://", "");
                    //   std::string listenChannel;
                    //   std::list<std::string>::iterator iter = mListenChannels.begin();
                    //   while (iter != mListenChannels.end()) {
                    //      listenChannel = (*iter) + "/";
                    //      iter++;
                    //   }
                    //   mpVHWebSocketInterface->ConnectWebSocket(mLoginInfo.nginx_server, listenChannel);
                    //}
                }
            }
        }
    }
}

void VHPaasRoomImpl::ReFreshPermissionAnalysisInfo(std::string data, int code)
{
	if (code != 0) {
		if (mbIsRuning && mVHRoomDelegate) {
			LOGD("ReFreshPermissionAnalysisInfo data:%s code:%d", data.c_str(), code);
			mVHRoomDelegate->OnFailedEvent(RoomEvent_GetPermission, code, data);
		}
	}
	else {
		//解析登录数据
		RefFreshPermissionToInfo(data);
	}
}
Permission VHPaasRoomImpl::GetPermission() {
    Permission per;
    per.kick_inav = mInavInfo.kick_inav;
    per.kick_inav_stream = mInavInfo.kick_inav_stream;
    per.publish_inav_another = mInavInfo.publish_inav_another;
    per.apply_inav_publish = mInavInfo.apply_inav_publish;
    per.publish_inav_stream = mInavInfo.publish_inav_stream;
    per.askfor_inav_publish = mInavInfo.askfor_inav_publish;
    per.audit_inav_publish = mInavInfo.audit_inav_publish;
    return per;
}

std::string VHPaasRoomImpl::GetBaseUrlInfo() {
    std::string baseInfo = std::string("client=windows&app_id=") + mAppid + std::string("&third_party_user_id=") + mThridUserId + std::string("&access_token=") + mAccesstoken +std::string("&package_check=") + std::string("windows");
    return baseInfo;
}

std::string VHPaasRoomImpl::GetInvaStartUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/init/start?") + baseInfo;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetInavInfoUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/get-inav-info?") + baseInfo + std::string("&inav_id=") + mInavRoomId + std::string("&secure=false") + std::string("&room_id=") + mLiveRoomId;// .arg(baseInfo).arg(mInavRoomId);
    LOGD("%s urld:%s", __FUNCDNAME__, url.c_str());
    return url;
}


void VHPaasRoomImpl::ParamToStartInfoResp(const std::string& msg) {
    LOGD("msg:%s", msg.c_str());
    rapidjson::Document doc;
    doc.Parse<0>(msg.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            mLoginInfo.mCode = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            mLoginInfo.mNoticeMsg = doc["msg"].GetString();
        }
        if (200 == mLoginInfo.mCode) {
            if (doc.HasMember("data") && doc["data"].IsObject()) {
                rapidjson::Value& resp = doc["data"];
                if (resp.HasMember("socket_server") && resp["socket_server"].IsString()) {
                    mLoginInfo.socket_server = resp["socket_server"].GetString();
                }
                if (resp.HasMember("nginx_server") && resp["nginx_server"].IsString()) {
                    mLoginInfo.nginx_server = resp["nginx_server"].GetString();
                }
                if (resp.HasMember("connection_token") && resp["connection_token"].IsString()) {
                    mLoginInfo.connection_token = resp["connection_token"].GetString();
                }
                if (resp.HasMember("socket_server") && resp["socket_server"].IsString()) {
                    mLoginInfo.socket_server = resp["socket_server"].GetString();
                }
                if (resp.HasMember("log_server") && resp["log_server"].IsString()) {
                    mLoginInfo.log_server = resp["log_server"].GetString();
                }
                if (resp.HasMember("document_server") && resp["document_server"].IsString()) {
                    mLoginInfo.document_server = resp["document_server"].GetString();
                }

                if (resp.HasMember("log_info") && resp["log_info"].IsObject()) {
                    rapidjson::Value& respLogInfo = resp["log_info"];
                    if (respLogInfo.HasMember("session_id") && respLogInfo["session_id"].IsString()) {
                        mLoginInfo.session_id = respLogInfo["session_id"].GetString();
                    }
                    if (respLogInfo.HasMember("uid") && respLogInfo["uid"].IsString()) {
                        mLoginInfo.uid = respLogInfo["uid"].GetString();
                    }
                    if (respLogInfo.HasMember("ip") && respLogInfo["ip"].IsString()) {
                        mLoginInfo.ip = respLogInfo["ip"].GetString();
                    }
                    if (respLogInfo.HasMember("third_party_user_id") && respLogInfo["third_party_user_id"].IsString()) {
                        mLoginInfo.third_party_user_id = respLogInfo["third_party_user_id"].GetString();
                    }
                    if (respLogInfo.HasMember("account_id") && respLogInfo["account_id"].IsInt()) {
                        int account = respLogInfo["account_id"].GetInt();
                        mLoginInfo.account_id = to_string(account);
                    }
                    if (respLogInfo.HasMember("app_name") && respLogInfo["app_name"].IsString()) {
                        mLoginInfo.app_name = respLogInfo["app_name"].GetString();
                    }
                }

                if (resp.HasMember("push_config") && resp["push_config"].IsObject()) {
                    rapidjson::Value& pushconfigObj = resp["push_config"];
                    if (pushconfigObj.HasMember("stand") && pushconfigObj["stand"].IsString()) {
                        mLoginInfo.stand = pushconfigObj["stand"].GetString();
                    }
                    if (pushconfigObj.HasMember("dpi") && pushconfigObj["dpi"].IsString()) {
                        mLoginInfo.dpi = pushconfigObj["dpi"].GetString();
                    }
                    if (pushconfigObj.HasMember("frame_rate") && pushconfigObj["frame_rate"].IsString()) {
                        mLoginInfo.frame_rate = pushconfigObj["frame_rate"].GetString();
                    }
                    if (pushconfigObj.HasMember("bitrate") && pushconfigObj["bitrate"].IsString()) {
                        mLoginInfo.bitrate = pushconfigObj["bitrate"].GetString();
                    }
                }
            }
        }
        else {

        }
    }
}

void VHPaasRoomImpl::RefFreshPermissionToInfo(std::string data){
	LOGD("data:%s", data.c_str());
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if (doc.HasParseError()) {
		LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject()) {
      int code;
      std::string msg;
      if (doc.HasMember("code") && doc["code"].IsInt()) {
         code = doc["code"].GetInt();
      }
      if (doc.HasMember("msg") && doc["msg"].IsString()) {
         msg = doc["msg"].GetString();
      }
      if (code == 200) {
         if (doc.HasMember("data") && doc["data"].IsObject()) {
            rapidjson::Value& resp = doc["data"];
            if (resp.HasMember("permission") && resp["permission"].IsArray()) {
               rapidjson::Value& permission = resp["permission"];
               for (int i = 0; i < permission.Size(); i++) {
                  rapidjson::Value& cur_permission = permission[i];
                  std::string pemStr = cur_permission.GetString();
                  if (pemStr == "kick_inav") {
                     mInavInfo.kick_inav = true;
                  }
                  else if (pemStr == "kick_inav_stream") {
                     mInavInfo.kick_inav_stream = true;
                  }
                  else if (pemStr == "publish_inav_another") {
                     mInavInfo.publish_inav_another = true;
                  }
                  else if (pemStr == "apply_inav_publish") {
                     mInavInfo.apply_inav_publish = true;
                  }
                  else if (pemStr == "publish_inav_stream") {
                     mInavInfo.publish_inav_stream = true;
                  }
                  else if (pemStr == "askfor_inav_publish") {
                     mInavInfo.askfor_inav_publish = true;
                  }
                  else if (pemStr == "audit_inav_publish") {
                     mInavInfo.audit_inav_publish = true;
                  }
               }
               mVHRoomDelegate->OnSuccessedEvent(RoomEvent_GetPermission);
            }
         }
      }
      else {
         mVHRoomDelegate->OnFailedEvent(RoomEvent_GetPermission, code, msg);
      }
	}
}

void VHPaasRoomImpl::ParamToInavInfoResp(std::string data) {
    LOGD("data:%s", data.c_str());
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            mInavInfo.mCode = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            mInavInfo.mNoticeMsg = doc["msg"].GetString();
        }

        if (200 == mInavInfo.mCode) {
            if (doc.HasMember("data") && doc["data"].IsObject()) {
                rapidjson::Value& resp = doc["data"];
                LOGE("mInavInfoLock.lock()\n");
                mInavInfoLock.lock();
                LOGE("mInavInfoLock.locked\n");

                if (resp.HasMember("inav_token") && resp["inav_token"].IsString()) {
                    mInavInfo.inav_token = resp["inav_token"].GetString();
                }
                mInavInfoLock.unlock();

                LOGE("mInavInfoLock.unlock()\n");
                if (resp.HasMember("pushUrl") && resp["pushUrl"].IsString()) {
                    mInavInfo.push_address = resp["pushUrl"].GetString();
                }

                if (resp.HasMember("permission") && resp["permission"].IsArray()) {
                    rapidjson::Value& permission = resp["permission"];
                    for (int i = 0; i < permission.Size(); i++) {
                        rapidjson::Value& cur_permission = permission[i];
                        std::string pemStr = cur_permission.GetString();
                        if (pemStr == "kick_inav") {
                            mInavInfo.kick_inav = true;
                        }
                        else if (pemStr == "kick_inav_stream") {
                            mInavInfo.kick_inav_stream = true;
                        }
                        else if (pemStr == "publish_inav_another") {
                            mInavInfo.publish_inav_another = true;
                        }
                        else if (pemStr == "apply_inav_publish") {
                            mInavInfo.apply_inav_publish = true;
                        }
                        else if (pemStr == "publish_inav_stream") {
                            mInavInfo.publish_inav_stream = true;
                        }
                        else if (pemStr == "askfor_inav_publish") {
                            mInavInfo.askfor_inav_publish = true;
                        }
                        else if (pemStr == "audit_inav_publish") {
                            mInavInfo.audit_inav_publish = true;
                        }
                    }
                }

                if (resp.HasMember("log_info") && resp.IsObject()) {
                    rapidjson::Value& log_info = resp["log_info"];
                    if (log_info.HasMember("session_id") && log_info["session_id"].IsString()) {
                        mInavInfo.session_id = log_info["session_id"].GetString();
                    }
                    if (log_info.HasMember("uid") && log_info["uid"].IsString()) {
                        mInavInfo.uid = log_info["uid"].GetString();
                    }
                    if (log_info.HasMember("ip") && log_info["ip"].IsString()) {
                        mInavInfo.ip = log_info["ip"].GetString();
                    }

                    if (log_info.HasMember("third_party_user_id") && log_info["third_party_user_id"].IsString()) {
                       //string temp = log_info["third_party_user_id"].GetString();
                       //mInavInfo.third_party_user_id = temp;
                       // int iId = atoi(mInavInfo.third_party_user_id.c_str());
                       // int iLength = mInavInfo.third_party_user_id.length();

                        mInavInfo.third_party_user_id = log_info["third_party_user_id"].GetString();
                        LOGE(" RESET third_party_user_id %s \n", mInavInfo.third_party_user_id.c_str());
                    }
                    else if(log_info.HasMember("third_party_user_id") && log_info["third_party_user_id"].IsInt()) {
                       char s[64] = { 0 };
                       itoa(log_info["third_party_user_id"].GetInt(), s,10);
                       mInavInfo.third_party_user_id = s;
                       LOGE(" RESET int third_party_user_id %s\n", mInavInfo.third_party_user_id.c_str());
                    }

                    if (log_info.HasMember("account_id") && log_info["account_id"].IsString()) {
                        mInavInfo.account_id = log_info["account_id"].GetString();
                    }
                    if (log_info.HasMember("account_id") && log_info["account_id"].IsInt()) {
                        mInavInfo.account_id = to_string(log_info["account_id"].GetInt());
                    }
                    if (log_info.HasMember("app_name") && log_info["app_name"].IsString()) {
                        mInavInfo.app_name = log_info["app_name"].GetString();
                    }
                }

                if (resp.HasMember("push_config") && resp.IsObject()) {
                    rapidjson::Value& push_config = resp["push_config"];
                    if (push_config.HasMember("stand") && push_config["stand"].IsString()) {
                        mInavInfo.stand = push_config["stand"].GetString();
                    }
                    if (push_config.HasMember("dpi") && push_config["dpi"].IsString()) {
                        mInavInfo.dpi = push_config["dpi"].GetString();
                    }
                    if (push_config.HasMember("frame_rate") && push_config["frame_rate"].IsString()) {
                        mInavInfo.frame_rate = push_config["frame_rate"].GetString();
                    }
                    if (push_config.HasMember("bitrate") && push_config["bitrate"].IsString()) {
                        mInavInfo.bitrate = push_config["bitrate"].GetString();
                    }

                }

            }
        }
        else {

        }
    }
}

void VHPaasRoomImpl::LogOut() {
    //请求互动房间退出信息
    LOGD("LogoutHVRoom");
    HTTP_GET_REQUEST httpGet(GetLeaveInavtUrl()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code,const std::string userData)->void {
            ParamRoomEvent(RoomEvent_Logout, data, code, userData);
        });
    }

    if (mpVHWebSocketInterface) {
       mbDisConnectWebSocket = true;
       mpVHWebSocketInterface->DisConnectWebSocket();
       mpVHWebSocketInterface->SyncDisConnectServer();
    }
}

void VHPaasRoomImpl::ClearInitInfo()
{
   mContext = "";
   mAccesstoken = "";
   mAppid = "";
   mThridUserId = "";
   mInavRoomId = "";
   mLiveRoomId = "";
   mListenChannels.clear();

   //memset(&mInavInfo, 0, sizeof(mInavInfo));
   mInavInfo.reset();
   mLoginInfo.log_server = "";
   mStatic_bThreadRun = false;
   CThreadLockHandle lockHandle(&mStatic_taskListMutex);
   mStatic_taskEventList.clear();
}

bool VHPaasRoomImpl::JoinRTCRoom(std::string userData, bool bEnableSimulCaset
   ,std::string vid , std::string vfid, std::string strWebinarId, int role, int classType) {
    LOGD("JoinVHMediaRoom mInavInfo.inav_token ：%s", mInavInfo.inav_token.c_str());
    bool bRet = false;
    LOGE("mInavInfoLock.lock()\n");
    mInavInfoLock.lock();
    LOGE("mInavInfoLock.locked\n");
    if (mpWebRtcSDKInterface && mInavInfo.inav_token.length() > 0) {
        WebRtcRoomOption option;
        option.strRoomToken = String2WString(mInavInfo.inav_token);
        option.strUserId = String2WString(mInavInfo.third_party_user_id);
        option.role = role;
        LOGE("option.strUserId %ws\n", option.strUserId.c_str());
        option.strLogUpLogUrl = String2WString(mLoginInfo.log_server);
        option.strThirdPushStreamUrl = String2WString(mInavInfo.push_address);
        option.strAccountId = String2WString(mInavInfo.account_id);

        option.strPassRoomId = String2WString(mLiveRoomId);//pass_room_id
        option.strRoomId = String2WString(strWebinarId);
        option.strBusinesstype = L"1";
        option.strappid = String2WString(mAppid);
        option.strVfid = vfid;
        option.strVid = vid;
        option.userData = userData;
        option.classType = classType;
        mpWebRtcSDKInterface->ConnetWebRtcRoom(option);
        LOGD("ConnetWebRtcRoom mInavInfo.inav_token.length(): %d  mInavInfo.inav_token: %s", mInavInfo.inav_token.length() ,mInavInfo.inav_token.c_str());
        mpWebRtcSDKInterface->EnableSimulCast(bEnableSimulCaset);
        bRet = true;
    }
    else {

    }

    mInavInfoLock.unlock();
    LOGE("mInavInfoLock.unlock()\n");
    return bRet;
}

bool VHPaasRoomImpl::isConnetWebRtcRoom()
{
   bool bRef = false;
   if (mpWebRtcSDKInterface) {
      bRef = mpWebRtcSDKInterface->isConnetWebRtcRoom();
   }
   return bRef;
}

void VHPaasRoomImpl::AsynGetVHRoomMembers() {
    LOGD("AsynGetVHRoomMembers");
    //请求互动房间信息
    HTTP_GET_REQUEST httpGet(GetInavUserListUrl()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            LOGD("AnalysisGetInavUserList code:%d", code);
            AnalysisGetInavUserList(data, code);
        });
    }
}


std::string VHPaasRoomImpl::GetInavUserListUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/inav-user-list?") + baseInfo + std::string("&inav_id=") + mInavRoomId;
    return url;
}

void VHPaasRoomImpl::AnalysisGetInavUserList(std::string data, int code) {
    mInavInfo.mCode = code;
    std::list<VHRoomMember> memberList;
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            mInavInfo.mCode = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            mInavInfo.mNoticeMsg = doc["msg"].GetString();
        }
        if (200 == mInavInfo.mCode) {
            if (doc.HasMember("data") && doc["data"].IsObject()) {
                rapidjson::Value& resp = doc["data"];
                if (resp.HasMember("lists") && resp["lists"].IsArray()) {
                    rapidjson::Value& members = resp["lists"];
                    for (int i = 0; i < members.Size(); i++) {
                        rapidjson::Value& cur_members = members[i];
                        VHRoomMember vhMember;
                        if (cur_members.HasMember("third_party_user_id") && cur_members["third_party_user_id"].IsString()) {
                            std::string id = cur_members["third_party_user_id"].GetString();
                            vhMember.thirdPartyUserId = String2WString(id);
                            LOGD("AnalysisGetInavUserList vhMember.thirdPartyUserId:%s", id.c_str());
                        }
                        if (cur_members.HasMember("status") && cur_members["status"].IsInt()) {
                            vhMember.status = (MemberStatus)cur_members["status"].GetInt();
                            LOGD("AnalysisGetInavUserList vhMember.status:%d ", vhMember.status);
                        }
                        memberList.push_back(vhMember);
                    }
                }
            }
        }
        else {

        }
    }

    if (mInavInfo.mCode == 200) {
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnGetVHRoomMembers(mInavInfo.mNoticeMsg, memberList);
        }
    }
    else {
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnFailedEvent(RoomEvent_InavUserList, mInavInfo.mCode, mInavInfo.mNoticeMsg);
        }
    }
}

void VHPaasRoomImpl::AnalysisGetKickOutInavUserList(std::string data, int code) {
    LOGD("AnalysisGetKickOutInavUserList code:%d", code);
    std::list<VHRoomMember> memberList;
    mInavInfo.mCode = code;
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            mInavInfo.mCode = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            mInavInfo.mNoticeMsg = doc["msg"].GetString();
        }
        if (200 == mInavInfo.mCode) {
            if (doc.HasMember("data") && doc["data"].IsObject()) {
                rapidjson::Value& resp = doc["data"];
                if (resp.HasMember("lists") && resp["lists"].IsArray()) {
                    rapidjson::Value& members = resp["lists"];
                    for (int i = 0; i < members.Size(); i++) {
                        rapidjson::Value& cur_members = members[i];
                        VHRoomMember vhMember;
                        std::string id = cur_members.GetString();
                        vhMember.thirdPartyUserId = String2WString(id);
                        LOGD("AnalysisGetInavUserList vhMember.status:%d vhMember.thirdPartyUserId:%s", vhMember.status, id.c_str());
                        memberList.push_back(vhMember);
                    }
                }
            }
        }
        else {

        }
    }

    if (mInavInfo.mCode == 200) {
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnGetVHRoomKickOutMembers(memberList);
        }
    }
    else {
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnFailedEvent(RoomEvent_GetKickInavList, mInavInfo.mCode, mInavInfo.mNoticeMsg);
        }
    }
}

/*
* 退出房间
*/
bool VHPaasRoomImpl::LeaveRTCRoom() {
    LOGD("DisConnetVHMediaRoom");
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->DisConnetWebRtcRoom();
    }
    return bRet;
}
void VHPaasRoomImpl::DisableSubScribeStream()
{
   LOGD("start");
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->DisableSubScribeStream();
   }

   LOGD("end");
}
void VHPaasRoomImpl::EnableSubScribeStream()
{
   LOGD("start");
   if ( mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->EnableSubScribeStream();
   }
   LOGD("end");
}


bool VHPaasRoomImpl::IsWebRtcRoomConnected() {
   LOGD("IsWebRtcRoomConnected");
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->IsWebRtcRoomConnected();
   }
   return bRet;
}

int VHPaasRoomImpl::ApplyInavPublish() {
    LOGD("ApplyInavPublish");
    if (!mInavInfo.apply_inav_publish) {
        return VhallLive_NO_PERMISSION;
    }
    //请求互动房间信息
    HTTP_GET_REQUEST httpGet(GetApplyInavPublishUrl()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code,const std::string userData)->void {
            ParamRoomEvent(RoomEvent_Apply_Push, data, code, userData);
        });
    }
    return VhallLive_OK;
}

int VHPaasRoomImpl::AuditInavPublish(const std::string& audit_user_id, AuditPublish type) {
    if (!mInavInfo.audit_inav_publish) {
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    LOGD("AuditInavPublish audit_user_id:%s type:%d", audit_user_id.c_str(), type);

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value process_id(StringRef(audit_user_id.c_str()));
    document.AddMember("process_id", process_id, allocator);
    rapidjson::Value process_type(type);
    document.AddMember("type", process_type, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();


    HTTP_GET_REQUEST httpGet(GetAuditInavPublishUrl(audit_user_id, type)/*, data, bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            ParamRoomEvent(RoomEvent_AuditInavPublish, data, code, userData );
        });
    }
    return VhallLive_OK;
}     

int VHPaasRoomImpl::AskforInavPublish(const std::string& audit_user_id) {
    if (!mInavInfo.askfor_inav_publish) {  //没有邀请上麦权限
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    LOGD("AskforInavPublish audit_user_id:%s ", audit_user_id.c_str());

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value process_id(StringRef(audit_user_id.c_str()));
    document.AddMember("process_id", process_id, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();

    HTTP_GET_REQUEST httpGet(GetAskforInavPublishUrl(audit_user_id)/*, data,bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            ParamRoomEvent(RoomEvent_AskforInavPublish, data, code, userData);
        });
    }
    return VhallLive_OK;
}

void VHPaasRoomImpl::UserPublishCallback(PushStreamEvent type, const std::string& stream_id /*= std::string()*/) {
    LOGD("UserPublishCallback stream_id:%s type:%d", stream_id.c_str(), type);

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value process_id(StringRef(stream_id.c_str()));
    document.AddMember("process_id", process_id, allocator);
    rapidjson::Value process_type(type);
    document.AddMember("type", process_type, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();

    HTTP_GET_REQUEST httpGet(GetUserPublishCallbackUrl(stream_id, type)/*, data, bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            ParamRoomEvent(RoomEvent_UserPublishCallback, data, code, userData);
        });
    }
}

int VHPaasRoomImpl::KickInavStream(const std::string& kick_user_id, KickStream type) {
    if (!mInavInfo.kick_inav_stream) {  //没有邀请上麦权限
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    LOGD("KickInavStream kick_user_id:%s type:%d", kick_user_id.c_str(), type);

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value process_id(StringRef(kick_user_id.c_str()));
    document.AddMember("process_id", process_id, allocator);
    rapidjson::Value process_type(type);
    document.AddMember("type", process_type, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();

    HTTP_GET_REQUEST httpGet(GetKickInavStreamUrl(kick_user_id, type)/*, data, bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            ParamRoomEvent(RoomEvent_KickInavStream, data, code, userData);
        });
    }
    return VhallLive_OK;
}

int VHPaasRoomImpl::KickInav(const std::string& kick_user_id, KickStream type) {
    if (!mInavInfo.kick_inav) {
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    LOGD("KickInav kick_user_id:%s type:%d", kick_user_id.c_str(), type);

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value process_id(StringRef(kick_user_id.c_str()));
    document.AddMember("process_id", process_id, allocator);
    rapidjson::Value process_type(type);
    document.AddMember("type", process_type, allocator);

    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();

    HTTP_GET_REQUEST httpGet(GetKickInavUrl(kick_user_id, type)/*, data, bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            ParamRoomEvent(RoomEvent_KickInav, data, code, userData);
        });
    }
    return 0;
}

void VHPaasRoomImpl::GetKickInavList() {
    LOGD("GetKickInavList");
    HTTP_GET_REQUEST httpGet(GetKickInavListUrl()/*,"", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            AnalysisGetKickOutInavUserList(data, code);
        });
    }
}

/*
*   开启旁路直播 StartPublishInavAnother
*/
int VHPaasRoomImpl::StartConfigBroadCast(LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex,
   bool showBorder, std::string boradColor, std::string backGroundColor, 
   SetBroadCastEventCallback fun /*= nullptr*/) {
    LOGD("");
    int bRet = VhallLive_NO_PERMISSION;
    if (!mInavInfo.publish_inav_another) {
        LOGD("no permission ");
        return bRet;
    }
    
    //RtcSDKEventDelegate *obj = mRtcDelegate;
    /*int*/ 
    bRet = mpWebRtcSDKInterface->StartConfigBroadCast(layoutMode, profileIndex, 
       showBorder, boradColor, backGroundColor, fun
    //   [&, obj, layoutMode, profileIndex, fun ](const std::string& result, const std::string& msg)->void {
    //   LOGD("OnStartPublishInavAnother result:%s msg:%s", result.c_str(), msg.c_str());
    //   //if (obj) {
    //   //   obj->OnConfigBroadCast(layoutMode, profileIndex, result, msg);
    //   //}
    //   if (fun) {
    //      fun(result.c_str(), msg.c_str());//此处成功 通知pass 服务  GetPublishInavAnotherWithParam 接口
    //       //VHPaasRoomImpl* processer = this;
    //      ////先调用api,成功之后再调用底层接口。
    //      //HTTP_GET_REQUEST httpGet(GetPublishInavAnotherWithParam(mLiveRoomId, 1)/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    //      //if (mpHttpManagerInterface) {
    //      //    mpHttpManagerInterface->HttpGetRequest(httpGet, [&, processer, layoutMode, profileIndex, showBorder, boradColor, backGroundColor](std::string data, int code, const std::string userData)->void {
    //      //        LOGD("data:%s", data.c_str());
    //      //        if (processer) {
    //      //           OnStartPublishInavAnother(code, data, layoutMode, profileIndex, showBorder, boradColor, backGroundColor);
    //      //        }
    //      //    });
    //      //}  
    //   }
    //}
    );
    //if (bRet != VhallLive_OK) {
    //   mRtcDelegate->OnConfigBroadCast(layoutMode, profileIndex, RTC_FAILED, msg);
    //}
    return bRet;
}

void VHPaasRoomImpl::GetPublishInavAnotherWithParam()
{
   HTTP_GET_REQUEST httpGet(GetPublishInavAnotherWithParam(mLiveRoomId, 1)/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
         if (mpHttpManagerInterface) {
             mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
                 LOGD("data:%s", data.c_str());
                 //if (processer) {
                 //   OnStartPublishInavAnother(code, data, layoutMode, profileIndex, showBorder, boradColor, backGroundColor);
                 //}
             });
         }  
}
//void VHPaasRoomImpl::OnStartPublishInavAnother(int error_code, std::string res_msg, LayoutMode layoutMode, BroadCastVideoProfileIndex profileIndex, bool showBorder, std::string boradColor, std::string backGroundColor) {
//   if (mpWebRtcSDKInterface) {
//      LOGD("layoutMode:%d  profileIndex:%d", layoutMode, profileIndex);
//      int codeNum = 0;
//      string msg = "http send err";
//      rapidjson::Document doc;
//      doc.Parse<0>(res_msg.c_str());
//      if (doc.HasParseError()) {
//         LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
//         if (mbIsRuning && mRtcDelegate) {
//            mRtcDelegate->OnConfigBroadCast(layoutMode, profileIndex, RTC_FAILED, msg);
//         }
//         return;
//      }
//
//      if (doc.IsObject()) {
//         if (doc.HasMember("code") && doc["code"].IsInt()) {
//            codeNum = doc["code"].GetInt();
//         }
//         if (doc.HasMember("msg") && doc["msg"].IsString()) {
//            msg = doc["msg"].GetString();
//         }
//      }
//
//      if (codeNum == 200) {
//         RtcSDKEventDelegate *obj = mRtcDelegate;
//         int bRet = mpWebRtcSDKInterface->StartConfigBroadCast(layoutMode, profileIndex, showBorder, boradColor, backGroundColor,
//            [&, obj, layoutMode, profileIndex](const std::string& result, const std::string& msg)->void {
//            LOGD("OnStartPublishInavAnother result:%s msg:%s", result.c_str(), msg.c_str());
//            if (obj) {
//               obj->OnConfigBroadCast(layoutMode, profileIndex, result, msg);
//            }
//         });
//         if (bRet != VhallLive_OK) {
//            mRtcDelegate->OnConfigBroadCast(layoutMode, profileIndex, RTC_FAILED, msg);
//         }
//      }
//      else {
//         if (mbIsRuning && mRtcDelegate) {
//            mRtcDelegate->OnConfigBroadCast(layoutMode, profileIndex, RTC_FAILED, msg);
//         }
//      }
//   }
//}

/**
*   获取流ID
*/
std::string VHPaasRoomImpl::GetUserStreamID(const std::wstring user_id, VHStreamType streamType) {
    LOGD("user_id %s streamType:%d", user_id.c_str(), streamType);
    if (mpWebRtcSDKInterface == nullptr) {
        return std::string();
    }
    return mpWebRtcSDKInterface->GetUserStreamID(user_id, streamType);
}

int VHPaasRoomImpl::SetConfigBroadCastLayOut(LayoutMode mode, SetBroadCastEventCallback fun)
{
   int bRet = VhallLive_NO_OBJ;
   if (mpWebRtcSDKInterface) {
      if (!mpWebRtcSDKInterface->IsEnableConfigMixStream()) {
         LOGD("VhallLive_SERVER_NOT_READY");
         bRet = vlive::VhallLive_SERVER_NOT_READY;
      }
      else {
         bRet = mpWebRtcSDKInterface->SetConfigBroadCastLayOut(mode, fun);
      }
   }
   return bRet;
}

int VHPaasRoomImpl::SetMainView(std::string stream_id/*,int stream_type*/, SetBroadCastEventCallback fun/* = nullptr*/) {
   int bRet;
    LOGD("stream_id %s", stream_id.c_str());
    if (!mInavInfo.publish_inav_another) {
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    if (stream_id.length() == 0) {
        LOGD("VhallLive_PARAM_ERR");
        return VhallLive_PARAM_ERR;
    }
    if (mpWebRtcSDKInterface == nullptr) {
        return VhallLive_NO_OBJ;
    }
    if (!mpWebRtcSDKInterface->IsEnableConfigMixStream()) {
        LOGD("VhallLive_SERVER_NOT_READY");
        return vlive::VhallLive_SERVER_NOT_READY;
    }
    //WebRtcSDKEventInterface*obj = mRtcDelegate;
    bRet = mpWebRtcSDKInterface->SetMixLayoutMainView(stream_id, fun);

    //if (bRet != VhallLive_OK) {
    //   if (obj) {
    //      obj->OnSetMainView(stream_id, stream_type, RTC_FAILED, RTC_FAILED);
    //   }
    //}
    return bRet;
}

/*
 *  接口说明：开启旁路直播后，可通过此接口修复混流布局
 */
//int VHPaasRoomImpl::ChangeLayout(LayoutMode layoutMode, std::string custom) {
//   int bRet = VhallLive_OK;
//    //if (!mInavInfo.publish_inav_another) {
//    //    LOGD("no permission ");
//    //    return VhallLive_NO_PERMISSION;
//    //}
//    ////HTTP_GET_REQUEST httpGet(GetSetLayOut(layoutMode, custom), layoutMode, bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName);
//    ////if (mpHttpManagerInterface) {
//    ////    mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
//    ////        LOGD("data:%s", data.c_str());
//    ////        ParamRoomEvent(RoomEvent_SetLayOut, data, code, userData);
//    ////    });
//    ////}
//    //
//    //if (mpWebRtcSDKInterface) {
//    //   LOGD("layoutMode:%d", layoutMode);
//    //   RtcSDKEventDelegate *obj = mRtcDelegate;
//    //   bRet = mpWebRtcSDKInterface->SetConfigBroadCastLayOut(layoutMode, [&, obj, layoutMode](const std::string& result, const std::string& msg)->void {
//    //      LOGD("StartConfigBroadCast result:%s msg:%s", result.c_str(), msg.c_str());
//    //      if (obj) {
//    //         obj->OnChangeConfigBroadCastLayout(layoutMode, result, msg);
//    //      }
//    //   });
//    //   if (bRet != VhallLive_OK) {
//    //      if (obj) {
//    //         obj->OnChangeConfigBroadCastLayout(layoutMode, RTC_FAILED, "");
//    //      }
//    //   }
//    //}
//    return bRet;
//}

/*
*   停止旁路直播
*   live_room_id: 直播房间id
*/
int VHPaasRoomImpl::StopPublishInavAnother() {
    if (!mInavInfo.publish_inav_another) {
        LOGD("no permission ");
        return VhallLive_NO_PERMISSION;
    }
    //api状态上报
    VHPaasRoomImpl* processer = this;
    HTTP_GET_REQUEST httpGet(GetPushAnotherConfig(2));
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&, processer](std::string data, int code, const std::string userData)->void {
            LOGD("data:%s", data.c_str());
            if (processer) {
               processer->OnStopPublishInavAnother(code, data);
            }
        });
    }
    return 0;
}

void VHPaasRoomImpl::implementTask(int iTask)
{
   LOGD("data:%s", __FUNCTION__);
   CPaasTaskEvent task;
   task.mEventType = iTask;

   PostTaskEvent(task);
}

bool VHPaasRoomImpl::IsEnableConfigMixStream()
{
   bool bEnabele = false;
   if (mpWebRtcSDKInterface) {
      bEnabele = mpWebRtcSDKInterface->IsEnableConfigMixStream();
   }
   
   return bEnabele;
}

void VHPaasRoomImpl::SetEnableConfigMixStream(bool enable)
{
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->SetEnableConfigMixStream(false);
   }
}

DWORD __stdcall VHPaasRoomImpl::ThreadProcess(LPVOID p)
{
   LOGD("ThreadProcess");
   while (mStatic_bThreadRun) {
      if (mStatic_TaskEvent) {
         DWORD ret = WaitForSingleObject(mStatic_TaskEvent, 2000);
         if (p) {
            VHPaasRoomImpl* sdk = (VHPaasRoomImpl*)(p);
            if (sdk) {
               sdk->ProcessTask();
            }
         }
      }
   }

   if (mStatic_ThreadExitEvent) {
      ::SetEvent(mStatic_ThreadExitEvent);
   }

   LOGD("exit ThreadProcess");
   return 0;
}

void VHPaasRoomImpl::ProcessTask()
{
   CPaasTaskEvent event;
   {
      CThreadLockHandle lockHandle(&mStatic_taskListMutex);
      if (mStatic_taskEventList.size() > 0) {
         event = mStatic_taskEventList.front();
         mStatic_taskEventList.pop_front();
      }
      else {
         return;
      }
   }

   switch (event.mEventType)
   {
   case eTask_BroadCast: {
      GetPublishInavAnotherWithParam();
      break;
   }
   default:; break;
   }

}

bool VHPaasRoomImpl::IsSupportMediaFormat(CaptureStreamAVType type)
{
   LOGD("enter type:%d", type);
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->IsSupportMediaFormat(type);
   }
   return false;
}

void VHPaasRoomImpl::PostTaskEvent(CPaasTaskEvent task) {
   if (mStatic_bThreadRun) {
      CThreadLockHandle lockHandle(&mStatic_taskListMutex);
      mStatic_taskEventList.push_back(task);
      if (mStatic_TaskEvent) {
         ::SetEvent(mStatic_TaskEvent);
      }
   }
}

std::string VHPaasRoomImpl::GetLocalAuxiliaryId()
{
   std::string strRef;
   if ( mpWebRtcSDKInterface) {
      strRef = mpWebRtcSDKInterface->GetLocalAuxiliaryId();
   }

   return strRef;
}

std::string VHPaasRoomImpl::GetAuxiliaryId()
{
   std::string strRef;
   if (mpWebRtcSDKInterface)
   {
      strRef = mpWebRtcSDKInterface->GetAuxiliaryId();
   }
   return strRef;
}

VHStreamType VHPaasRoomImpl::CalcStreamType(const bool & bAudio, const bool & bVedio)
{
   VHStreamType eRef = VHStreamType_Count;
   if (mpWebRtcSDKInterface) {
      eRef = mpWebRtcSDKInterface->CalcStreamType(bAudio, bVedio);
   }

   return eRef;
}

int VHPaasRoomImpl::SetLocalCapturePlayerVolume(const int volume)
{
   int iRef = VhallLive_ERROR;
   if (mpWebRtcSDKInterface)
   {
      iRef = mpWebRtcSDKInterface->SetLocalCapturePlayerVolume(volume);
   }
   return iRef;
}

int VHPaasRoomImpl::StopPlayAudioFile()
{
   int nRet = VhallLive_NO_OBJ;
   if (mpWebRtcSDKInterface)
   {
      nRet = mpWebRtcSDKInterface->StopPlayAudioFile();
   }
   return nRet;
}

int VHPaasRoomImpl::GetPlayAudioFileVolum()
{
   int nRet = VhallLive_NO_OBJ;
   if (mpWebRtcSDKInterface)
   {
      nRet = mpWebRtcSDKInterface->GetPlayAudioFileVolum();
   }
   return nRet;
}

int VHPaasRoomImpl::PlayAudioFile(std::string fileName, int devIndex)
{
   int nRet = VhallLive_NO_OBJ;
   if (mpWebRtcSDKInterface)
   {
      nRet = mpWebRtcSDKInterface->PlayAudioFile(fileName, devIndex);
   }
   return nRet;
}

int VHPaasRoomImpl::ChangeNextCamera(VideoProfileIndex videoProfile)
{
   LOGD(" enter ");
   if (mpWebRtcSDKInterface) {
      std::list<vhall::VideoDevProperty> mCameraList;
      mpWebRtcSDKInterface->GetCameraDevices(mCameraList);
      int index = 0;
      std::string devId;
      mpWebRtcSDKInterface->GetCurrentMic(index, devId);

      //只有一个摄像头或者没有，不进行切换。
      if (mCameraList.size() == 0 || mCameraList.size() == 1) {
         LOGD(" only on return ");
         return -1;
      }
      std::string curCamerDevId;
      std::wstring playerCaptureId = mpWebRtcSDKInterface->GetCurrentDesktopPlayCaptureId();
      mpWebRtcSDKInterface->GetCurrentCamera(curCamerDevId);
      //当选中的摄像头还有下一个设备，则切换。
      int nCheckCount = 1;
      std::list<vhall::VideoDevProperty>::iterator iter = mCameraList.begin();

      LOGD("  The num of Camera is %d", mCameraList.size());
      while (iter != mCameraList.end()) {
         vhall::VideoDevProperty dev = *iter;
         LOGD("  The cur index of Camera is %d", nCheckCount);
         if (dev.mDevId == curCamerDevId || curCamerDevId.length() == 0) {
            //找到当前正在使用的摄像头。
            //1:如果还有下一个，则切换；
            if (nCheckCount < mCameraList.size() && iter->mDevId.size() > 0) {
               iter++;
               if (iter != mCameraList.end())
               {
                  vhall::VideoDevProperty nexDev = *iter;
                  StopLocalCapture();
                  mpWebRtcSDKInterface->SetUsedMic(index, devId, playerCaptureId);
                  mpWebRtcSDKInterface->StartLocalCapture(nexDev.mDevId, videoProfile, true);
               }
               else{
                  return 0;
               }
               //return ;
            }
            //2:已经是最后一个,选取第一个；
            else if (nCheckCount == mCameraList.size()) {
               iter = mCameraList.begin();
               if (iter->mDevId.size() > 0) {
                  vhall::VideoDevProperty nexDev = *iter;
                  StopLocalCapture();
                  mpWebRtcSDKInterface->SetUsedMic(index, devId, playerCaptureId);
                  mpWebRtcSDKInterface->StartLocalCapture(nexDev.mDevId, videoProfile, true);
                  //return;
               }
            }
         }
         nCheckCount++;
         iter++;
      }


   }
   LOGD(" leave ");
   return 0;
}

void VHPaasRoomImpl::GetCurrentCamera(std::string & devId)
{
   LOGD("enter devId:%s", devId.c_str());
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->GetCurrentCamera(devId);
   }
}

void VHPaasRoomImpl::GetCurrentMic(int & index, std::string & devId)
{
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->GetCurrentMic(index, devId);
      LOGD("enter index:%d", index);
   }
}

bool VHPaasRoomImpl::GetMuteAllAudio()
{
   bool bRef = false;
   if (mpWebRtcSDKInterface) {
      bRef = mpWebRtcSDKInterface->GetMuteAllAudio();
   }
   return bRef;
}

int VHPaasRoomImpl::SetUsedMic(int index, std::string devId, std::wstring desktopCaptureId)
{
   LOGD("enter index:%d", index);
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->SetUsedMic(index, devId, desktopCaptureId);
   }
   return 0;
}

bool VHPaasRoomImpl::StopRenderRemoteStream(const std::wstring & user, const std::string streamId, vlive::VHStreamType streamType)
{
   LOGD("enter user:%ws streamType:%d", user.c_str(), streamType);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StopRenderRemoteStream(user, streamId, streamType);
   }
   return bRet;
}

bool VHPaasRoomImpl::StartRenderRemoteStreamByInterface(const std::wstring & user, vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive)
{
   LOGD("enter user:%ws streamType:%d", user.c_str(), streamType);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StartRenderRemoteStreamByInterface(user, streamType, receive);
   }
   return bRet;
}

bool VHPaasRoomImpl::IsRemoteStreamIsExist(const std::wstring & user, vlive::VHStreamType streamType)
{
   LOGD("enter %s  streamType:%d", __FUNCTION__, streamType/*, receive, receive*/);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->IsRemoteStreamIsExist(user, streamType);
   }
   return bRet;
}

int VHPaasRoomImpl::GetSubScribeStreamFormat(const std::wstring & user, vlive::VHStreamType streamType, bool & hasVideo, bool & hasAudio)
{
   LOGD("enter  streamType:%d", streamType);
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->GetSubScribeStreamFormat(user, streamType, hasVideo, hasAudio);
   }
   LOGD("end streamType:%d", streamType);
   return -1;
}

double VHPaasRoomImpl::GetPushDesktopVideoLostRate()
{
   double dRef = 0.0;
   LOGD("enter  ");
   if (mpWebRtcSDKInterface) {
      dRef =  mpWebRtcSDKInterface->GetPushDesktopVideoLostRate();
   }
   LOGD("end");
   return dRef;
}

void VHPaasRoomImpl::OnStopPublishInavAnother(int error_code, std::string res_msg) {
   /*if (mpWebRtcSDKInterface) {
      LOGD("error_code:%d  res_msg:%s", error_code, res_msg.c_str());
      int codeNum = 0;
      string msg = "http send err";
      rapidjson::Document doc;
      doc.Parse<0>(res_msg.c_str());
      if (doc.HasParseError()) {
         LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
         if (mbIsRuning && mRtcDelegate) {
            mRtcDelegate->OnStopConfigBroadCast(RTC_FAILED, msg);
         }
         return;
      }
      if (doc.IsObject()) {
         if (doc.HasMember("code") && doc["code"].IsInt()) {
            codeNum = doc["code"].GetInt();
         }
         if (doc.HasMember("msg") && doc["msg"].IsString()) {
            msg = doc["msg"].GetString();
         }
      }
      if (codeNum == 200) {
         WebRtcSDKEventInterface *obj = mRtcDelegate;
         int bRet = mpWebRtcSDKInterface->StopBroadCast([&, obj](const std::string& result, const std::string& msg)->void {
            LOGD("OnStartPublishInavAnother result:%s msg:%s", result.c_str(), msg.c_str());
            if (obj) {
               obj->OnStopConfigBroadCast(result, msg);
            }
         });
      }
      else {
         if (mbIsRuning && mRtcDelegate) {
            mRtcDelegate->OnStopConfigBroadCast(RTC_FAILED, msg);
         }
      }
   }*/
}

bool VHPaasRoomImpl::ParamRoomEvent(RoomEvent event, std::string data, int code, std::string userData) {
    LOGD("ParamRoomEvent data:%s code:%d userData:%s ", data.c_str(), code, userData.c_str());
    int codeNum = -1;
    string msg = "http send err";
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnFailedEvent(event, codeNum, msg, userData);
        }
        return false;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            codeNum = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            msg = doc["msg"].GetString();
        }
    }
    if (codeNum == 200) {
		if (event == RoomEvent_Logout) {
         mbDisConnectWebSocket = true;
			mpVHWebSocketInterface->DisConnectWebSocket();
			mpVHWebSocketInterface->SyncDisConnectServer();
		}
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnSuccessedEvent(event, userData);
        }
        if (event == RoomEvent_Logout) {
            if (mpVHWebSocketInterface) {
                mbDisConnectWebSocket = true;
                mpVHWebSocketInterface->DisConnectWebSocket();
                mpVHWebSocketInterface->SyncDisConnectServer();
            }
        }
        return true;
    }
    else {
        if (mbIsRuning && mVHRoomDelegate) {
            mVHRoomDelegate->OnFailedEvent(event, codeNum, msg, userData);
        }
    }
    return false;
}

void VHPaasRoomImpl::CheckPermission(VHStreamType streamType, std::string data, int code, std::string context) {
    LOGD("ParamRoomEvent data:%s code:%d ", data.c_str(), code);
    int codeNum = code;
    string msg = "no define";
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("code") && doc["code"].IsInt()) {
            codeNum = doc["code"].GetInt();
        }
        if (doc.HasMember("msg") && doc["msg"].IsString()) {
            msg = doc["msg"].GetString();
        }
    }

    if (codeNum == 200) {
        if (doc.HasMember("data") && doc["data"].IsObject()) {
            rapidjson::Value& dataMsg = doc["data"];
            if (dataMsg.HasMember("has_permission") && dataMsg["has_permission"].IsInt()) {
                int permission = dataMsg["has_permission"].GetInt();
                if (permission == 1) {
                    //有权限
                    switch (streamType)
                    {
                    case VHStreamType_AVCapture:
                        if (mpWebRtcSDKInterface) {
                            mpWebRtcSDKInterface->StartPushLocalStream();
                        }
                        break;
                    case VHStreamType_Desktop:
                        if (mpWebRtcSDKInterface) {
                            mpWebRtcSDKInterface->StartPushDesktopStream(context);
                        }
                        break;
                    case VHStreamType_MediaFile:
                        if (mpWebRtcSDKInterface) {
                            mpWebRtcSDKInterface->StartPushMediaFileStream();
                        }
                        break;
                    case VHStreamType_Count:
                        break;
                    default:
                        break;
                    }

                }
                else {
                    if (mbIsRuning && mVHRoomDelegate) {
                       mRtcDelegate->OnPushStreamError("", streamType, permission, "no permission");
                    }
                }
            }
            
        }
    }
    else {
        if (mbIsRuning && mVHRoomDelegate) {
           mRtcDelegate->OnPushStreamError("", streamType, codeNum, msg);
        }
    }
}

void VHPaasRoomImpl::ParamRecvSocketMsg(const std::string msg) {
    if (mbIsRuning && mMsgDelegate) {
       mMsgDelegate->OnRecvMsgEvent(msg);
    }
}


void VHPaasRoomImpl::ParamRecvInavMsg(const std::string data) {
    LOGD("ParamRecvInavMsg data:%s", data.c_str());
    //http://doc.vhall.com/docs/show/1201
    int codeNum = 0;
    string msg = "no define";
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    if (doc.HasParseError()) {
        LOGE("VHTokenRespMsg ParseError%d\n", doc.GetParseError());
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("data") && doc["data"].IsObject()) {
            rapidjson::Value& resp = doc["data"];
            {
                //rapidjson::Value& resp = dataObj["data"];
                if (resp.HasMember("inav_event") && resp["inav_event"].IsString()) {
                    std::string inav_event = resp["inav_event"].GetString();
                    if (mbIsRuning && mVHRoomDelegate) {
                        if (inav_event.compare("apply_inav_publish") == 0) {
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                std::string third_party_user_id = resp["third_party_user_id"].GetString();
                                LOGD("mInavInfo.apply_inav_publish");
                                mVHRoomDelegate->OnRecvBaseApplyInavPublishMsg(String2WString(third_party_user_id));
                            }
                        }
                        else if (inav_event.compare("audit_inav_publish") == 0) {
                            std::string third_party_user_id;
                            int status = 0;
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                third_party_user_id = resp["third_party_user_id"].GetString();
                            }

                            if (resp.HasMember("status") && resp["status"].IsString()) {
                                char s[64] = { 0 };
                                memcpy(s, resp["status"].GetString(), resp["status"].GetStringLength());
                                //itoa(status, s, 10);
								         status = atoi(s);
                            }
                            else if (resp.HasMember("status") && resp["status"].IsInt()) {
                                status = resp["status"].GetInt();
                            }
                            mVHRoomDelegate->OnRecvAuditInavPublishMsg(String2WString(third_party_user_id), (AuditPublish)status);
                        }
                        else if (inav_event.compare("askfor_inav_publish") == 0) {
                            std::string third_party_user_id;
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                third_party_user_id = resp["third_party_user_id"].GetString();
                            }
                            mVHRoomDelegate->OnRecvBaseAskforInavPublishMsg(String2WString(third_party_user_id));
                        }
                        else if (inav_event.compare("kick_inav_stream") == 0) {
                            std::string third_party_user_id;
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                third_party_user_id = resp["third_party_user_id"].GetString();
                            }
                            mVHRoomDelegate->OnRecvKickInavStreamMsg(String2WString(third_party_user_id));
                        }
                        else if (inav_event.compare("kick_inav") == 0) {
                            std::string third_party_user_id;
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                third_party_user_id = resp["third_party_user_id"].GetString();
                            }
                            mVHRoomDelegate->OnRecvKickInavMsg(String2WString(third_party_user_id));
                        }
                        else if (inav_event.compare("user_publish_callback") == 0) {
                            std::string third_party_user_id;
                            std::string stream_id;
                            int status = 0;
                            if (resp.HasMember("third_party_user_id") && resp["third_party_user_id"].IsString()) {
                                third_party_user_id = resp["third_party_user_id"].GetString();
                            }
                            if (resp.HasMember("stream_id") && resp["stream_id"].IsString()) {
                                stream_id = resp["stream_id"].GetString();
                            }
                            if (resp.HasMember("status") && resp["status"].IsString()) {
                                char s[64] = { 0 };
                                memcpy(s, resp["status"].GetString(), resp["status"].GetStringLength());
                                itoa(status, s, 10);
                            }
                            else if (resp.HasMember("status") && resp["status"].IsInt()) {
                                status = resp["status"].GetInt();
                            }
                            mVHRoomDelegate->OnUserPublishCallback(String2WString(third_party_user_id), stream_id, (PushStreamEvent)status);
                        }
                        else if (inav_event.compare("inav_close") == 0) {
                            mVHRoomDelegate->OnRecvInavCloseMsg();
                        }
                    }
                }
            }

        }
    }
}

std::string VHPaasRoomImpl::GetLeaveInavtUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/leave-inav?") + baseInfo + std::string("&inav_id=") + mInavRoomId;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetApplyInavPublishUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/apply-inav-publish?") + baseInfo + std::string("&inav_id=") + mInavRoomId;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetAuditInavPublishUrl(const std::string& userid, int type) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/audit-inav-publish?") + baseInfo + std::string("&inav_id=") + mInavRoomId + string("&audit_user_id=") + userid + string("&audit_type=") + to_string(type);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetAskforInavPublishUrl(const std::string& userid) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/askfor-inav-publish?") + baseInfo + std::string("&inav_id=") + mInavRoomId + string("&askfor_third_user_id=") + userid;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetUserPublishCallbackUrl(const std::string& streamid, int type) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/user-publish-callback?") + baseInfo + std::string("&inav_id=") + mInavRoomId + std::string("&stream_id=") + streamid + std::string("&type=") + to_string(type);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetKickInavStreamUrl(const std::string& user_id, int type) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/kick-inav-stream?") + baseInfo + std::string("&inav_id=") + mInavRoomId + std::string("&kick_user_id=") + user_id + std::string("&type=") + to_string(type);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetKickInavUrl(const std::string& userid, int type) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/kick-inav?") + baseInfo + std::string("&inav_id=") + mInavRoomId + std::string("&kick_user_id=") + userid + std::string("&type=") + to_string(type);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetKickInavListUrl() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/get-kick-inav-list?") + baseInfo + std::string("&inav_id=") + mInavRoomId;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetAccessTokenPermission() {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/get-access-token-permission?") + baseInfo + std::string("&resourse_id=") + mInavRoomId + std::string("&permission=publish_inav_stream");
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetPublishInavAnother(const std::string& live_room_id,int start) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/publish-inav-another?") + baseInfo + string("&inav_id=") + mInavRoomId + string("&room_id=") + live_room_id + string("&type=") + to_string(start);// % 4").arg(baseInfo).arg(mInavRoomId).arg(QString::fromStdString(live_room_id)).arg(start);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetSetLayOut(std::string layoutMode,std::string custom) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + std::string("/inav/set-layout?") + baseInfo + string("&inav_id=") + mInavRoomId + string("&layout=") + layoutMode;
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetPublishInavAnotherWithParam(const std::string& live_room_id, int start) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string layouyt = GetLayoutDesc(mCurrentBroadCastInfo.layoutMode);
    std::string dip = mCurrentBroadCastInfo.dpiDesc;
    std::string stream_id = mCurrentBroadCastInfo.stream_id;
    std::string url = mDomain + std::string("/inav/publish-inav-another?") + baseInfo + string("&inav_id=") + mInavRoomId +
        string("&room_id=") + live_room_id + string("&type=") + to_string(start);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string VHPaasRoomImpl::GetPushAnotherConfig(int state) {
    std::string baseInfo = GetBaseUrlInfo();
    std::string url = mDomain + 
        std::string("/inav/push-another-config?") + baseInfo + 
        string("&inav_id=") + mInavRoomId + 
        string("&room_id=") + mCurrentBroadCastInfo.live_room_id + 
        string("&type=") + to_string(state);
    LOGD("urld:%s", url.c_str());
    return url;
}

std::string  VHPaasRoomImpl::GetLayoutDesc(LayoutMode layout) {
    switch (layout)
    {
    case CANVAS_LAYOUT_PATTERN_GRID_1:
        return "CANVAS_LAYOUT_PATTERN_GRID_1";
    case CANVAS_LAYOUT_PATTERN_GRID_2_H:
        return "CANVAS_LAYOUT_PATTERN_GRID_2_H";
    case CANVAS_LAYOUT_PATTERN_GRID_3_E:
        return "CANVAS_LAYOUT_PATTERN_GRID_3_E";
    case CANVAS_LAYOUT_PATTERN_GRID_3_D:
        return "CANVAS_LAYOUT_PATTERN_GRID_3_D";
    case CANVAS_LAYOUT_PATTERN_GRID_4_M:
        return "CANVAS_LAYOUT_PATTERN_GRID_4_M";
    case CANVAS_LAYOUT_PATTERN_GRID_5_D:
        return "CANVAS_LAYOUT_PATTERN_GRID_5_D";
    case CANVAS_LAYOUT_PATTERN_GRID_6_E:
        return "CANVAS_LAYOUT_PATTERN_GRID_6_E";
    case CANVAS_LAYOUT_PATTERN_GRID_9_E:
        return "CANVAS_LAYOUT_PATTERN_GRID_9_E";
    case CANVAS_LAYOUT_PATTERN_FLOAT_2_1DR:
        return "CANVAS_LAYOUT_PATTERN_FLOAT_2_1DR";
    case CANVAS_LAYOUT_PATTERN_FLOAT_2_1DL:
        return "CANVAS_LAYOUT_PATTERN_FLOAT_2_1DL";
    case CANVAS_LAYOUT_PATTERN_FLOAT_3_2DL:
        return "CANVAS_LAYOUT_PATTERN_FLOAT_3_2DL";
    case CANVAS_LAYOUT_PATTERN_FLOAT_6_5D:
        return "CANVAS_LAYOUT_PATTERN_FLOAT_6_5D";
    case CANVAS_LAYOUT_PATTERN_FLOAT_6_5T:
        return "CANVAS_LAYOUT_PATTERN_FLOAT_6_5T";
    case CANVAS_LAYOUT_PATTERN_TILED_5_1T4D:
        return "CANVAS_LAYOUT_PATTERN_TILED_5_1T4D";
    case CANVAS_LAYOUT_PATTERN_TILED_5_1D4T:
        return "CANVAS_LAYOUT_PATTERN_TILED_5_1D4T";
    case CANVAS_LAYOUT_PATTERN_TILED_5_1L4R:
        return "CANVAS_LAYOUT_PATTERN_TILED_5_1L4R";
    case CANVAS_LAYOUT_PATTERN_TILED_5_1R4L:
        return "CANVAS_LAYOUT_PATTERN_TILED_5_1R4L";
    case CANVAS_LAYOUT_PATTERN_TILED_6_1T5D:
        return "CANVAS_LAYOUT_PATTERN_TILED_6_1T5D";
    case CANVAS_LAYOUT_PATTERN_TILED_6_1D5T:
        return "CANVAS_LAYOUT_PATTERN_TILED_6_1D5T";
    case CANVAS_LAYOUT_PATTERN_TILED_9_1L8R :
        return "CANVAS_LAYOUT_PATTERN_TILED_9_1L8R";
    case CANVAS_LAYOUT_PATTERN_TILED_9_1R8L :
        return "CANVAS_LAYOUT_PATTERN_TILED_9_1R8L";
    case CANVAS_LAYOUT_PATTERN_TILED_13_1L12R :
        return "CANVAS_LAYOUT_PATTERN_TILED_13_1L12R";
    case CANVAS_LAYOUT_PATTERN_TILED_17_1TL16GRID :
        return "CANVAS_LAYOUT_PATTERN_TILED_17_1TL16GRID";
    case CANVAS_LAYOUT_PATTERN_TILED_9_1D8T:
        return "CANVAS_LAYOUT_PATTERN_TILED_9_1D8T";
    case CANVAS_LAYOUT_PATTERN_TILED_13_1TL12GRID:
        return "CANVAS_LAYOUT_PATTERN_TILED_13_1TL12GRID";
    case CANVAS_LAYOUT_PATTERN_TILED_17_1TL16GRID_E:
        return "CANVAS_LAYOUT_PATTERN_TILED_17_1TL16GRID_E";
    case CANVAS_LAYOUT_PATTERN_CUSTOM:
        return "CANVAS_LAYOUT_PATTERN_CUSTOM";
    default:
        break;
    }
    return std::string();
}

//void VHPaasRoomImpl::SetBroadCastCallBack(const std::string result, const std::string& msg) {
//    LOGD("msg:%s", msg.c_str());
//    if (result.compare("success") == 0) {
//        if (mbIsRuning && mVHRoomDelegate) {
//            LOGD("msg:%s", msg.c_str());
//            mVHRoomDelegate->OnSuccessedEvent(RoomEvent_Start_PublishInavAnother);
//        }
//        //api状态上报
//        HTTP_GET_REQUEST httpGet(GetPublishInavAnother(mCurrentBroadCastInfo.live_room_id,1), "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName);
//        if (mpHttpManagerInterface) {
//            mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
//                LOGD("data:%s", data.c_str());
//            });
//        }
//    }
//    else {
//        if (mbIsRuning && mVHRoomDelegate) {
//            LOGD("msg:%s", msg.c_str());
//            mVHRoomDelegate->OnFailedEvent(RoomEvent_Start_PublishInavAnother, -1, msg);
//        }
//    }
//}
//
//void VHPaasRoomImpl::SetBroadCastCallBackWithParam(const std::string result,const std::string& msg) {
//    //如果设置布局成功
//    if (result.compare("success") == 0) {
//        if (mbIsRuning && mVHRoomDelegate) {
//            LOGD("msg:%s", msg.c_str());
//            mVHRoomDelegate->OnSuccessedEvent(RoomEvent_Start_PublishInavAnother, result);
//        }
//    }
//    else {
//        if (mbIsRuning && mVHRoomDelegate) {
//            LOGD("msg:%s", msg.c_str());
//            mVHRoomDelegate->OnFailedEvent(RoomEvent_Start_PublishInavAnother, -1, msg);
//        }
//    }
//}
//
bool VHPaasRoomImpl::IsVHMediaConnected() {
    LOGD("IsVHMediaConnected");
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->IsWebRtcRoomConnected();
    }
    return bRet;
}

void VHPaasRoomImpl::GetCurrentCameraInfo(int &index, std::string& devId) {
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->GetCurrentPlayOut(index, devId);
      LOGD("enter index:%d", index);
   }
}

/**
* 摄像头画面预览，当预览结束之后需要停止预览，否则摄像头将被一直占用
*   回调监听：RtcSDKEventDelegate
*/
int VHPaasRoomImpl::PreviewCamera(void* wnd, const std::string& devGuid, VideoProfileIndex index, int micIndex /*= -1*/) {
   LOGD("IsVHMediaConnected");
   int bRet = 0;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->PreviewCamera(wnd, devGuid, index, micIndex);
   }
   return bRet;
}

int VHPaasRoomImpl::GetMicVolumValue()
{
   int nRet = 0;
   if (mpWebRtcSDKInterface) {
      nRet = mpWebRtcSDKInterface->GetMicVolumValue();
   }
   return nRet;
}

void VHPaasRoomImpl::ChangePreViewMic(int micIndex)
{
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->ChangePreViewMic(micIndex);
   }
}

int VHPaasRoomImpl::StopPreviewCamera() {
   LOGD("IsVHMediaConnected");
   int bRet = 0;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StopPreviewCamera();
   }
   return bRet;
}

/*  开始摄像头\麦克风采集
*   回调检测 OnStartLocalCapture OnCameraCaptureErr  OnLocalMicOpenErr
*/
int VHPaasRoomImpl::StartLocalCapture(const std::string devId,VideoProfileIndex index, bool doublePush) {
    LOGD("StartLocalCapture");
    int nRet = false;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->StartLocalCapture(devId,index, doublePush);
    }
    return nRet;
}

int VHPaasRoomImpl::StartLocalAuxiliaryCapture(const std::string devId, VideoProfileIndex index)
{
   LOGD("%s %d", __FUNCTION__, index);
   int nRet = false;
   if (mpWebRtcSDKInterface) {
      nRet = mpWebRtcSDKInterface->StartLocalAuxiliaryCapture(devId, index);
   }
   LOGD("StartLocalAuxiliaryCapture :%d", nRet);
   return nRet;
}

int VHPaasRoomImpl::StartLocalCapturePicture(const std::string filePath, VideoProfileIndex index, bool doublePush/* = false*/) {
    LOGD("StartLocalCapturePicture");
    int nRet = false;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->StartLocalCapturePicture(filePath, index, doublePush);
    }
    return nRet;
}

int VHPaasRoomImpl::StartLocalCapturePlayer(const int dev_index, const std::wstring dev_id, const int volume) {
    LOGD("StartLocalCapturePlayer");
    int nRet = false;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->StartLocalCapturePlayer(dev_index, dev_id, volume);
    }
    return nRet;
}
/*
*   停止桌面音频采集
*/
int VHPaasRoomImpl::StopLocalCapturePlayer() {
    LOGD("StopLocalCapturePlayer");
    int nRet = false;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->StopLocalCapturePlayer();
    }
    return nRet;
}


/*停止摄像头\麦克风采集*/
void VHPaasRoomImpl::StopLocalCapture() {
    LOGD("StopLocalCapture");
    //bool bRet = false;
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->StopLocalCapture();
    }
    //return bRet;
}

void VHPaasRoomImpl::StopLocalAuxiliaryCapture()
{
   LOGD("StopLocalAuxiliaryCapture");
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->StopLocalAuxiliaryCapture();
   }
   LOGD("StopLocalAuxiliaryCapture end");
   return;
}

/*开始摄像头数据推流  回调检测：OnStartPushLocalStream*/
int VHPaasRoomImpl::StartPushLocalStream() {
    LOGD("enter");
    if (mpWebRtcSDKInterface && !mpWebRtcSDKInterface->IsWebRtcRoomConnected()) {
        LOGD("VhallLive_ROOM_DISCONNECT ");
        return VhallLive_ROOM_DISCONNECT;
    }
    //推流之前判断自己是否拥有权限。
    HTTP_GET_REQUEST httpGet(GetAccessTokenPermission()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            CheckPermission(VHStreamType_AVCapture, data, code, "");
        });
    }
    return VhallLive_OK;
}

int VHPaasRoomImpl::StartPushLocalAuxiliaryStream(std::string context)
{
   LOGD("enter");
   int bRet = VhallLiveErrCode::VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StartPushLocalAuxiliaryStream(context);
   }
   LOGD("end");
   return bRet;
}

/*停止摄像头数据推流 回调检测：OnStopPushLocalStream*/
int VHPaasRoomImpl::StopPushLocalStream() {
    LOGD("enter");
	int bRet = VhallLiveErrCode::VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->StopPushLocalStream();
    }
    return bRet;
}

int VHPaasRoomImpl::StopPushLocalAuxiliaryStream()
{
   LOGD("enter");
   int bRet = VhallLiveErrCode::VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StopPushLocalAuxiliaryStream();
   }
   LOGD("end");
   return bRet;
}

bool VHPaasRoomImpl::IsPushingStream(VHStreamType streamType) {
    LOGD("enter");
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->IsPushingStream(streamType);
    }
    return bRet;
}

bool VHPaasRoomImpl::IsCapturingStream(VHStreamType streamType) {
    LOGD("enter");
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->IsCapturingStream(streamType);
    }
    return bRet;
}

///**
//*   当前互动房间是否存在桌面共享视频流
//*/
//bool VHPaasRoomImpl::IsUserPushingDesktopStream() {
//    LOGD("enter");
//    bool bRet = false;
//    if (mpWebRtcSDKInterface) {
//        bRet = mpWebRtcSDKInterface->IsPushingStream(VHStreamType_Desktop);
//    }
//    return bRet;
//}
///**
//*   当前互动房间是否存在插播视频流
//*/
//bool VHPaasRoomImpl::IsUserPushingMediaFileStream() {
//    LOGD("enter");
//    bool bRet = false;
//    if (mpWebRtcSDKInterface) {
//        bRet = mpWebRtcSDKInterface->IsPushingStream(VHStreamType_MediaFile);
//    }
//    return bRet;
//}

void VHPaasRoomImpl::SubScribeRemoteStream(const std::string &stream_id, int delayTimeOut) {
   LOGD("enter");
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->SubScribeRemoteStream(stream_id, delayTimeOut);
   }
}

bool VHPaasRoomImpl::ChangeSubScribeUserSimulCast(const std::wstring& user_id, vlive::VHStreamType streamType, VHSimulCastType type) {
   LOGD("enter");
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->ChangeSubScribeUserSimulCast(user_id, streamType, type);
   }
   return bRet;
}

/**开始渲染媒体数据流*/
bool VHPaasRoomImpl::StartRenderStream(void* wnd, vlive::VHStreamType streamType) {
    LOGD("enter streamType:%d", streamType);
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
       bRet = mpWebRtcSDKInterface->StartRenderLocalStream(streamType, wnd);
    }
    LOGD("end");
    return bRet;
}

bool VHPaasRoomImpl::StartRenderLocalStream(vlive::VHStreamType streamType, void * wnd)
{
   LOGD("enter  streamType:%d", streamType);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StartRenderLocalStream(streamType, wnd);
   }
   return bRet;
}

std::string VHPaasRoomImpl::GetSubScribeStreamId(const std::wstring& user, vlive::VHStreamType streamType) {
   LOGD("enter  streamType:%d", streamType);
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->GetSubScribeStreamId(user, streamType);
   }
   return std::string();
}

bool VHPaasRoomImpl::IsExitSubScribeStream(const vlive::VHStreamType & streamType)
{
   LOGD("enter");
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->IsExitSubScribeStream(streamType);
   }
   return bRet;
}

bool VHPaasRoomImpl::IsExitSubScribeStream(const std::string & id, const vlive::VHStreamType & streamType)
{
   LOGD("enter");
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->IsExitSubScribeStream(id, streamType);
   }
   return bRet;
}

void VHPaasRoomImpl::GetStreams(std::list<std::string>& streams)
{
   LOGD("enter");
  
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->GetStreams(streams);
   }

}

void VHPaasRoomImpl::ClearSubScribeStream()
{
   LOGD("enter  streamType:%d");
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->ClearSubScribeStream();
   }
}

bool VHPaasRoomImpl::StartRenderLocalStreamByInterface(vlive::VHStreamType streamType, std::shared_ptr<vhall::VideoRenderReceiveInterface> receive)
{
   LOGD("enter  streamType:%d", streamType/*, receive, receive*/);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StartRenderLocalStreamByInterface(streamType, receive);
   }
   return bRet;
}

bool VHPaasRoomImpl::IsStreamExit(std::string id)
{
   //LOGD("enter  streamType:%d", streamType/*, receive, receive*/);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->IsStreamExit(id);
   }
   return bRet;
}

bool VHPaasRoomImpl::StartRenderSubscribeStream(const std::wstring& user, vlive::VHStreamType streamType, void* wnd) {
   LOGD("enter user:%s streamType:%d", user.c_str(), streamType);
   bool bRet = false;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->StartRenderRemoteStream(user, streamType, wnd);
   }
   LOGD("end");
   return bRet;
}

std::vector<DesktopCaptureSource> VHPaasRoomImpl::GetDesktops(int streamType) {
   LOGD("enter streamType:%d ", streamType);
   std::vector<DesktopCaptureSource> sources;
   if (mpWebRtcSDKInterface) {
      sources = mpWebRtcSDKInterface->GetDesktops(streamType);
      //sources = mpWebRtcSDKInterface->GetDesktops(streamType == ScreenCaptureSource_DeskTop ? VHStreamType_Desktop : VHStreamType_SoftWare);
   }
   return sources;
}

/*开始桌面共享采集  回调检测：OnStartDesktopCaptureSuc OnStartDesktopCaptureErr*/
int VHPaasRoomImpl::StartDesktopCapture(int index, VideoProfileIndex profileIndex) {
    LOGD("enter index:%d profileIndex:%d", index, profileIndex);
    if (mpWebRtcSDKInterface) {
        return mpWebRtcSDKInterface->StartDesktopCapture(index, profileIndex);
    }
    return VhallLive_NO_OBJ;
}

/*停止桌面共享采集*/
void VHPaasRoomImpl::StopDesktopCapture() {
    LOGD("enter");
    if (mpWebRtcSDKInterface) {
        //return 
			mpWebRtcSDKInterface->StopDesktopCapture();
    }
    //return VhallLive_NO_OBJ;
}
/*开始桌面共享采集推流 回调检测：OnStartPushDesktopStream*/
int VHPaasRoomImpl::StartPushDesktopStream(std::string context/* = ""*/) {
    LOGD("enter");
    if (mpWebRtcSDKInterface && !mpWebRtcSDKInterface->IsWebRtcRoomConnected()) {
        LOGD("VhallLive_ROOM_DISCONNECT ");
        return VhallLive_ROOM_DISCONNECT;
    }
    //推流之前判断自己是否拥有权限。
    HTTP_GET_REQUEST httpGet(GetAccessTokenPermission()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&, context](std::string data, int code, const std::string userData)->void {
            CheckPermission(VHStreamType_Desktop, data, code, context);
        });
    }
    return VhallLive_OK;
}

/*停止桌面共享采集推流 */
int VHPaasRoomImpl::StopPushDesktopStream() {
    LOGD("enter");
    int bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->StopPushDesktopStream();
    }
    return bRet;
}

/*
* 设置选择的软件源
*/
int VHPaasRoomImpl::SelectSource(int64_t id){
   LOGD("enter");
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->SelectSource(id);
   }
   return bRet;
}
/*
* 停止软件源共享采集
*/
void VHPaasRoomImpl::StopSoftwareCapture(){
   LOGD("enter");
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->StopSoftwareCapture();
   }
   return;
}
/*
* 停止软件共享采集推流
*/
int VHPaasRoomImpl::StopPushSoftWareStream(){
   LOGD("enter");
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->StopPushSoftWareStream();
   }
   return bRet;
}
/*
* 开始软件共享采集推流
*/
int VHPaasRoomImpl::StartPushSoftWareStream(){
   LOGD("enter");
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->StartPushSoftWareStream();
   }
   return bRet;
};

/*开始插播文件*/
int VHPaasRoomImpl::StartPlayMediaFile(std::string filePath, VideoProfileIndex profileIndex, long long seekPos) {
    LOGD("enter file:%s", filePath.c_str());
    int bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        bRet = mpWebRtcSDKInterface->StartPlayMediaFile(filePath, profileIndex, seekPos);
    }
    return bRet;
}

bool VHPaasRoomImpl::IsPlayFileHasVideo() {
   if (mpWebRtcSDKInterface) {
      return  mpWebRtcSDKInterface->IsPlayFileHasVideo();
   }
   return false;
}

int VHPaasRoomImpl::MediaFileVolumeChange(int vol) {
   LOGD("enter vol:%d", vol);
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->MediaFileVolumeChange(vol);
   }
   return bRet;
}

void VHPaasRoomImpl::ChangeMediaFileProfile(VideoProfileIndex profileIndex) {
   LOGD("enter profileIndex:%d", profileIndex);
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->ChangeMediaFileProfile(profileIndex);
   }
}

int VHPaasRoomImpl::CheckPicEffectiveness(const std::string filePath) {
   LOGD("enter file:%s", filePath.c_str());
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->CheckPicEffectiveness(filePath);
   }
   return bRet;
}

int VHPaasRoomImpl::GetPlayMeidaFileWidthAndHeight(std::string filePath, int& srcWidth, int &srcHeight) {
   LOGD("enter file:%s", filePath.c_str());
   int bRet = VhallLive_ERROR;
   if (mpWebRtcSDKInterface) {
      bRet = mpWebRtcSDKInterface->GetPlayMeidaFileWidthAndHeight(filePath, srcWidth, srcHeight);
   }
   return bRet;
}

/*停止插播文件*/
void VHPaasRoomImpl::StopMediaFileCapture() {
    LOGD("enter ");
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->StopMediaFileCapture();
    }
}
/*开始插播文件推流 */
int VHPaasRoomImpl::StartPushMediaFileStream() {
    LOGD("enter ");
    if (mpWebRtcSDKInterface && !mpWebRtcSDKInterface->IsWebRtcRoomConnected()) {
        LOGD("VhallLive_ROOM_DISCONNECT ");
        return VhallLive_ROOM_DISCONNECT;
    }

    //推流之前判断自己是否拥有权限。
    HTTP_GET_REQUEST httpGet(GetAccessTokenPermission()/*, "", bIsEnableProxy, mProxyAddr, mProxyPort, mProxyUserPwd, mProxyUserName*/);
    if (mpHttpManagerInterface) {
        mpHttpManagerInterface->HttpGetRequest(httpGet, [&](std::string data, int code, const std::string userData)->void {
            CheckPermission(VHStreamType_MediaFile, data, code);
        });
        return VhallLive_OK;
    }
    else {
        return VhallLive_NO_OBJ;
    }
}
/*停止插播文件推流 回调检测：OnStopPushMediaFileStream*/
void VHPaasRoomImpl::StopPushMediaFileStream() {
    LOGD("enter ");
    //int bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        //bRet = 
			mpWebRtcSDKInterface->StopPushMediaFileStream();
    }
    //return bRet;
}
//插播文件暂停处理
void VHPaasRoomImpl::MediaFilePause() {
    LOGD("enter ");
    //nt bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
       // bRet = 
			mpWebRtcSDKInterface->MediaFilePause();
    }
    //return bRet;
}
//插播文件恢复处理
void VHPaasRoomImpl::MediaFileResume() {
    LOGD("enter ");
    //int bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        //bRet = 
			mpWebRtcSDKInterface->MediaFileResume();
    }
    //return bRet;
}
//插播文件快进处理
void VHPaasRoomImpl::MediaFileSeek(const unsigned long long& seekTimeInMs) {
    LOGD("enter ");
    //int bRet = VhallLive_ERROR;
    if (mpWebRtcSDKInterface) {
        //bRet = 
			mpWebRtcSDKInterface->MediaFileSeek(seekTimeInMs);
    }
    //return bRet;
}

//插播文件总时长
const long long VHPaasRoomImpl::MediaFileGetMaxDuration() {
    long long nRet = 0;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->MediaFileGetMaxDuration();
    }
    return nRet;
}
//插播文件当前时长
const long long VHPaasRoomImpl::MediaFileGetCurrentDuration() {
    long long nRet = 0;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->MediaFileGetCurrentDuration();
    }
    return nRet;
}
//插播文件的当前状态 
//返回值：播放状态  MEDIA_FILE_PLAY_STATE
int VHPaasRoomImpl::MediaGetPlayerState() {
    int nRet = 0;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->MediaGetPlayerState();
    }
    return nRet;
}
/**获取摄像头列表**/
void VHPaasRoomImpl::GetCameraDevices(std::list<vhall::VideoDevProperty> &cameras) {
    LOGD("enter ");
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->GetCameraDevices(cameras);
    }
}

/*
*  是否存在音频或视频设备。
*  返回值：只要存在一类设备 返回true, 如果音视频设备都没有则返回false
**/
bool VHPaasRoomImpl::HasVideoOrAudioDev() {
   LOGD("enter ");
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->HasVideoOrAudioDev();
   }
   return false;
}

bool VHPaasRoomImpl::HasVideoDev()
{
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->HasVideoDev();
   }
   return false;
}

bool VHPaasRoomImpl::HasAudioDev()
{
   if (mpWebRtcSDKInterface) {
      return mpWebRtcSDKInterface->HasAudioDev();
   }
   return false;
}

/**获取麦克风列表**/
void VHPaasRoomImpl::GetMicDevices(std::list<vhall::AudioDevProperty> &micDevList) {
    LOGD("enter ");
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->GetMicDevices(micDevList);
    }
}

/*
 *  获取摄像头列表详情
 */
int VHPaasRoomImpl::GetCameraDevDetails(std::list<CameraDetailsInfo> &cameraDetails) {
   LOGD("enter ");
   if (mpWebRtcSDKInterface) {
      mpWebRtcSDKInterface->GetCameraDevDetails(cameraDetails);
   }
   return 0;
}

/**获取扬声器列表**/
void VHPaasRoomImpl::GetPlayerDevices(std::list<vhall::AudioDevProperty> &playerDevList) {
    LOGD("enter ");
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->GetPlayerDevices(playerDevList);
    }
}
/*
*  设置使用的麦克风
*  index: GetMicDevices获取的列表中 VhallLiveDeviceInfo中的 devIndex
*/
void VHPaasRoomImpl::SetUseMicIndex(int index,std::string devId, std::wstring desktopCaptureId) {
    LOGD("enter index:%d", index);
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->SetUsedMic(index, devId, desktopCaptureId);
    }
}

/*
*  设置使用的扬声器
*  index: GetPlayerDevices获取的列表中 vhall::AudioDevProperty中的 devIndex
*/
void VHPaasRoomImpl::SetUsePlayer(int index, std::string devId) {
    LOGD("enter index:%d", index);
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->SetUsedPlay(index, devId);
    }
}
/**设置当前使用的麦克风音量**/
bool VHPaasRoomImpl::SetCurrentMicVol(int vol) {
    LOGD("enter vol:%d", vol);
    if (vol < 0 || vol > 100) {
        return false;
    }
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->SetCurrentMicVol(vol);
        return true;
    }
    return false;
}
/**获取当前使用的麦克风音量**/
int VHPaasRoomImpl::GetCurrentMicVol() {
    int nRet = 0;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->GetCurrentMicVol();
    }
    LOGD("enter nRet:%d", nRet);
    return nRet;
}
/**设置当前使用的扬声器音量**/
bool VHPaasRoomImpl::SetCurrentPlayVol(int vol) {
    if (vol < 0 || vol > 100) {
        return false;
    }
    if (mpWebRtcSDKInterface) {
        mpWebRtcSDKInterface->SetCurrentPlayVol(vol);
        LOGD("enter vol:%d", vol);
        return true;
    }
    return false;
}

/**获取当前使用的扬声器音量**/
int VHPaasRoomImpl::GetCurrentPlayVol() {
    int nRet = 0;
    if (mpWebRtcSDKInterface) {
        nRet = mpWebRtcSDKInterface->GetCurrentPlayVol();
    }
    LOGD("enter nRet:%d", nRet);
    return nRet;
}
//关闭摄像头
bool VHPaasRoomImpl::CloseCamera() {
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        LOGD("enter CloseCamera");
        bRet = mpWebRtcSDKInterface->CloseCamera();
    }
    return bRet;
}
//打开摄像头
bool VHPaasRoomImpl::OpenCamera() { 
    bool bRet = false;
    if (mpWebRtcSDKInterface) {
        LOGD("enter OpenCamera");
        bRet = mpWebRtcSDKInterface->OpenCamera();
    }
    return bRet;
}

//关闭摄像头
bool VHPaasRoomImpl::IsCameraOpen()
{
	bool isOpen = false;
	if (mpWebRtcSDKInterface)
	{
		isOpen = mpWebRtcSDKInterface->IsCameraOpen();
	}
	return isOpen;
}

bool VHPaasRoomImpl::CloseMic() {
    bool bRet = false;
    if (mpWebRtcSDKInterface)
    {
        bRet = mpWebRtcSDKInterface->CloseMic();
    }
    return bRet;
}
/*
*   打开麦克风
*/
bool VHPaasRoomImpl::OpenMic() {
    bool bRet = false;
    if (mpWebRtcSDKInterface)
    {
        bRet = mpWebRtcSDKInterface->OpenMic();
    }
    return bRet;
}

//打开摄像头
bool VHPaasRoomImpl::IsMicOpen()
{
	bool isOpen = false;
	if (mpWebRtcSDKInterface)
	{
		isOpen = mpWebRtcSDKInterface->IsMicOpen();
	}
	return isOpen;
}

int VHPaasRoomImpl::MuteAllSubScribeAudio(bool mute) {
    int nRet = VhallLive_NO_OBJ;
    if (mpWebRtcSDKInterface)
    {
        nRet = mpWebRtcSDKInterface->MuteAllSubScribeAudio(mute);
    }
    return nRet;
}

/*
*   打开、关闭远端用户本地视频
*/
int VHPaasRoomImpl::OperateRemoteUserVideo(std::wstring user_id, bool close /*= false*/) {
    int nRet = VhallLive_NO_OBJ;
    if (mpWebRtcSDKInterface)
    {
        nRet = mpWebRtcSDKInterface->OperateRemoteUserVideo(user_id, close);
    }
    return nRet;
}
/*
*   打开、关闭远端用户本地声音
*/
int VHPaasRoomImpl::OperateRemoteUserAudio(std::wstring user_id, bool close/* = false*/) {
    int nRet = VhallLive_NO_OBJ;
    if (mpWebRtcSDKInterface)
    {
        nRet = mpWebRtcSDKInterface->OperateRemoteUserAudio(user_id, close);
    }
    return nRet;
}

void VHPaasRoomImpl::UploadReportSDKLogin(std::string appid, std::string thrid_user_id) {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    unsigned long long timeId = currentTime.LowPart;
    char timeStamp[64] = { 0 };
    sprintf_s(timeStamp, "%lld", timeId);
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    std::string system = "windows";
    rapidjson::Value userSystemname(StringRef(system.c_str()));
    document.AddMember("imei", userSystemname, allocator);

    std::string systemPf = "6";
    rapidjson::Value userSystemPf(StringRef(systemPf.c_str()));
    document.AddMember("pf", userSystemPf, allocator);

    std::string ver = PAASSDK_VER;
    rapidjson::Value sdkVer(StringRef(ver.c_str()));
    document.AddMember("cv", sdkVer, allocator);

    rapidjson::Value sdkAppid(StringRef(appid.c_str()));
    document.AddMember("app_id", sdkAppid, allocator);


    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();
    LOGD("OnSocketOpen  SendMsg:%s", data.c_str());

    mpLogReport->SendLog(SDK_LOGIN, timeStamp, thrid_user_id, "1", data);
}

void VHPaasRoomImpl::UploadReportSDKInit() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    unsigned long long timeId = currentTime.LowPart;
    char timeStamp[64] = { 0 };
    sprintf_s(timeStamp, "%lld", timeId);
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    std::string system = "windows";
    rapidjson::Value userSystemname(StringRef(system.c_str()));
    document.AddMember("imei", userSystemname, allocator);

    std::string systemPf = "6";
    rapidjson::Value userSystemPf(StringRef(systemPf.c_str()));
    document.AddMember("pf", userSystemPf, allocator);

    std::string ver = PAASSDK_VER;
    rapidjson::Value sdkVer(StringRef(ver.c_str()));
    document.AddMember("cv", sdkVer, allocator);


    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string data = buffer.GetString();
    LOGD("OnSocketOpen  SendMsg:%s", data.c_str());
    mpLogReport->SendLog(SDK_INIT, timeStamp, "defualt", "1", data);
}

namespace vlive {
    VHPAASSDK_EXPORT VHPaasInteractionRoom* GetPaasSDKInstance() {
        std::unique_lock<std::mutex> lock(gSDKMutex);
        if (gPaasSDKInstance == nullptr) {
            gPaasSDKInstance = new VHPaasRoomImpl();
        }

        return (VHPaasInteractionRoom*)gPaasSDKInstance;
    }

    VHPAASSDK_EXPORT void DestoryPaasSDKInstance() {
        std::unique_lock<std::mutex> lock(gSDKMutex);
        if (gPaasSDKInstance) {
            delete gPaasSDKInstance;
            gPaasSDKInstance = nullptr;
        }
    }
}

std::string VHPaasRoomImpl::WString2String(const std::wstring& ws)
{
    std::string strLocale = setlocale(LC_ALL, "");
    const wchar_t* wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char *chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs(chDest, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[]chDest;
    setlocale(LC_ALL, strLocale.c_str());
    return strResult;
}
// string => wstring
std::wstring VHPaasRoomImpl::String2WString(const std::string& str)
{
    int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t *wide = new wchar_t[num];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
    std::wstring w_str(wide);
    delete[] wide;
    return w_str;
}
