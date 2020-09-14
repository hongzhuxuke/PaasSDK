#include "stdafx.h"
#include "VHWebSocketImpl.h"
#include <string>
#include "message_wrapper.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

#include "lib/rapidjson/include/rapidjson/rapidjson.h"
#include "lib/rapidjson/include/rapidjson/stringbuffer.h"
#include "lib/rapidjson/include/rapidjson/writer.h"
#include "lib/rapidjson/include/rapidjson/reader.h"
#include "lib/rapidjson/include/rapidjson/document.h"


HANDLE VHWebSocketImpl::gTaskEvent = nullptr;
HANDLE VHWebSocketImpl::gThreadExitEvent = nullptr;
std::atomic_bool VHWebSocketImpl::bThreadRun = true;
WebSocketCallBackInterface* VHWebSocketImpl::mpCallback = nullptr;

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;

    if (NULL != tv) {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz) {
        if (!tzflag) {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

static int ratelimit_connects(unsigned int *last, unsigned int secs) {
    struct timeval tv;

    ::gettimeofday(&tv, NULL);
    if (tv.tv_sec - (*last) < secs)
        return 0;

    *last = tv.tv_sec;

    return 1;
}

static const char HEX[] = "0123456789abcdef";

char hex_encode(unsigned char val) {
    return (val < 16) ? HEX[val] : '!';
}
const unsigned char URL_UNSAFE = 0x1; // 0-33 "#$%&+,/:;<=>?@[\]^`{|} 127
const unsigned char ASCII_CLASS[128] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,3,1,1,1,3,2,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,3,1,3,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,
};

bool hex_decode(char ch, unsigned char* val) {
    if ((ch >= '0') && (ch <= '9')) {
        *val = ch - '0';
    }
    else if ((ch >= 'A') && (ch <= 'Z')) {
        *val = (ch - 'A') + 10;
    }
    else if ((ch >= 'a') && (ch <= 'z')) {
        *val = (ch - 'a') + 10;
    }
    else {
        return false;
    }
    return true;
}

size_t url_encode(char * buffer, size_t buflen,
    const char * source, size_t srclen) {
    if (NULL == buffer)
        return srclen * 3 + 1;
    if (buflen <= 0)
        return 0;

    size_t srcpos = 0, bufpos = 0;
    while ((srcpos < srclen) && (bufpos + 1 < buflen)) {
        unsigned char ch = source[srcpos++];
        if ((ch < 128) && (ASCII_CLASS[ch] & URL_UNSAFE)) {
            if (bufpos + 3 >= buflen) {
                break;
            }
            buffer[bufpos + 0] = '%';
            buffer[bufpos + 1] = hex_encode((ch >> 4) & 0xF);
            buffer[bufpos + 2] = hex_encode((ch) & 0xF);
            bufpos += 3;
        }
        else {
            buffer[bufpos++] = ch;
        }
    }
    buffer[bufpos] = '\0';
    return bufpos;
}


size_t url_decode(char * buffer, size_t buflen,
    const char * source, size_t srclen) {
    if (NULL == buffer)
        return srclen + 1;
    if (buflen <= 0)
        return 0;

    unsigned char h1, h2;
    size_t srcpos = 0, bufpos = 0;
    while ((srcpos < srclen) && (bufpos + 1 < buflen)) {
        unsigned char ch = source[srcpos++];
        if (ch == '+') {
            buffer[bufpos++] = ' ';
        }
        else if ((ch == '%')
            && (srcpos + 1 < srclen)
            && hex_decode(source[srcpos], &h1)
            && hex_decode(source[srcpos + 1], &h2))
        {
            buffer[bufpos++] = (h1 << 4) | h2;
            srcpos += 2;
        }
        else {
            buffer[bufpos++] = ch;
        }
    }
    buffer[bufpos] = '\0';
    return bufpos;
}

void VHWebSocketImpl::ParseWebsocketData(char* dataMsg, size_t srcLen) {
    rapidjson::Document doc;
    doc.Parse<0>(dataMsg);
    if (doc.HasParseError()) {
        return;
    }
    if (doc.IsObject()) {
        if (doc.HasMember("text") && doc["text"].IsString()) {
            std::string textData = doc["text"].GetString();
            char dstBuffer[4096] = { 0 };
            size_t dstLen = 4096;
            url_decode(dstBuffer, dstLen, textData.c_str(), textData.length());

            rapidjson::Document docText;
            docText.Parse<0>(dstBuffer);
            if (docText.HasParseError()) {
                return;
            }
            if (VHWebSocketImpl::mpCallback) {
               VHWebSocketImpl::mpCallback->OnRecvAllMsg(dstBuffer,strlen(dstBuffer));
            }
            if (docText.IsObject()) {
                if (docText.HasMember("service_type") && docText["service_type"].IsString()) {
                    std::string service_type = docText["service_type"].GetString();
                    std::wstring id,userName;
                    std::string nickname, role_name;
                    bool is_banned = false;
                    if (service_type == "service_online") {
                        if (docText.HasMember("sender_id") && docText["sender_id"].IsString()) {
                            std::string third_party_user_id = docText["sender_id"].GetString();
                            id = String2WString(third_party_user_id);
                        }

                        if (docText.HasMember("data") && docText["data"].IsString()) {
                            std::string data = docText["data"].GetString();
                            rapidjson::Document dataDoc;
                            dataDoc.Parse<0>(data.c_str());
                            if (dataDoc.HasParseError()) {
                                return;
                            }
                            if (dataDoc.HasMember("type") && dataDoc["type"].IsString()) {
                                std::string type = dataDoc["type"].GetString();
                                if (type == "Leave" || type == "Join") {
                                    if (docText.HasMember("context") && docText["context"].IsString()) {
                                        std::string datacontextMsg = docText["context"].GetString();
                                        rapidjson::Document datacontextObj;
                                        datacontextObj.Parse<0>(datacontextMsg.c_str());
                                        if (datacontextObj.HasParseError()) {
                                            return;
                                        }
                                        if (type == "Leave") {
                                           if (datacontextObj.HasMember("audience")) {
                                              //包含此字段，表示是关闭了聊天过滤或者关闭了观看页面。所以不下麦
                                              return;
                                           }
                                        }
                                        if (datacontextObj.HasMember("nickname") && datacontextObj["nickname"].IsString()) {
                                            nickname = datacontextObj["nickname"].GetString();
                                            userName = String2WString(nickname);
                                        }
                                        if (datacontextObj.HasMember("role_name")) {
                                            if (datacontextObj["role_name"].IsString()) {
                                                role_name = datacontextObj["role_name"].GetString();
                                            }
                                            else {
                                                role_name = std::to_string(datacontextObj["role_name"].GetInt());
                                            }
                                        }

                                        int devType = 2;
                                        if (datacontextObj.HasMember("device_type")) {
                                            if (datacontextObj["device_type"].IsString()) {
                                                devType = atoi(datacontextObj["device_type"].GetString());
                                            }
                                            else {
                                                devType = datacontextObj["device_type"].GetInt();
                                            }
                                        }
                                        if (datacontextObj.HasMember("is_banned") && datacontextObj["is_banned"].IsBool()) {
                                            is_banned = datacontextObj["is_banned"].GetBool();
                                        }
                                        int uv = 0; 
                                        if (docText.HasMember("uv") && docText["uv"].IsString()) {
                                           uv = atoi(docText["uv"].GetString());
                                        }
                                        else if (docText.HasMember("uv") && docText["uv"].IsInt()) {
                                           uv = docText["uv"].GetInt();
                                        }

                                        if (VHWebSocketImpl::mpCallback) {
                                            VHWebSocketImpl::mpCallback->OnServiceMsg(type, id, userName, role_name, is_banned, devType, uv);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}


static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user,
    void *in, size_t len) {
    if (len == 0) {
        return 0;
    }

    lwsl_notice("callback_echo %lu %s\r\n\r\n", len, (char *)in);
    char* s = (char *)in;
    switch (reason) {
    case LWS_CALLBACK_CLIENT_RECEIVE:
        VHWebSocketImpl::ParseWebsocketData(s, len);
        break;
    default:
        break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "",		/* name - can be overridden with -e */
        callback_echo,
        0,	/* per_session_data_size */
        1024 * 20,
    }
    , NULL
};

typedef std::function<void(const std::string&, const std::string&)> EventCallback;
VHWebSocketImpl::VHWebSocketImpl()
{
    mSocketIoImpl = new sio::client;
}


VHWebSocketImpl::~VHWebSocketImpl()
{
    DisConnectWebSocket();
    SyncDisConnectServer();
    if (mSocketIoImpl) {
        delete mSocketIoImpl;
        mSocketIoImpl = nullptr;
    }
}

void VHWebSocketImpl::RegisterCallback(WebSocketCallBackInterface* obj) {
    mpCallback = obj;
}

bool VHWebSocketImpl::ConnectServer(const char* url, const char* token) {
    if (mSocketIoImpl && url != nullptr && token != nullptr) {

        std::map<std::string, std::string> http_extra_headers;
        http_extra_headers["host"] = url; 

        std::map<std::string, std::string> query;
        query["token"] = token;

        std::string connectUrl = std::string(url);
        mSocketIoImpl->set_reconnect_attempts(0x7FFFFFFF);

        mSocketIoImpl->close();
        mSocketIoImpl->clear_con_listeners();
        mSocketIoImpl->clear_socket_listeners();


        mSocketIoImpl->set_open_listener([&]()->void {
            if (mpCallback) {
                mpCallback->OnOpen();
            }
        });
        mSocketIoImpl->set_close_listener([&](sio::client::close_reason const& reason)->void {
            if (mpCallback) {
                mpCallback->OnClose();
            }
        });
        mSocketIoImpl->set_fail_listener([&]()->void {
            if (mpCallback) {
                mpCallback->OnFail();
            }
        });
        mSocketIoImpl->set_socket_open_listener([&](const std::string& nsp)->void {
            if (mpCallback) {
                mpCallback->OnSocketOpen(nsp);
            }
        });
        mSocketIoImpl->set_socket_close_listener([&](const std::string& nsp)->void {
            if (mpCallback) {
                mpCallback->OnSocketClose(nsp);
            }
        });
        mSocketIoImpl->set_reconnect_listener([&](unsigned a, unsigned b)->void {
            if (mpCallback) {
                mpCallback->OnReconnect(a, b);
            }
        });
        mSocketIoImpl->set_reconnecting_listener([&]()->void {
            if (mpCallback) {
                mpCallback->OnReconnecting();
            }
        });
        mSocketIoImpl->connect(connectUrl.c_str(), query, http_extra_headers);
        return true;
    }
    return false;
}


bool VHWebSocketImpl::ConnectWebSocket(const std::string url, const std::string room_id) {
    mWebSocketUrl = url;
    mRoomId = room_id;
    DisConnectWebSocket();
    bThreadRun = true;
    mHThread = CreateThread(NULL, 0, VHWebSocketImpl::WebSocketThreadProcess, this, 0, &threadId);
    if (mHThread) {
        gTaskEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        gThreadExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    return true;
}

void VHWebSocketImpl::DisConnectWebSocket() {
    bThreadRun = false;
    if (gTaskEvent) {
        ::SetEvent(gTaskEvent);
    }
    if (gThreadExitEvent) {
        WaitForSingleObject(gThreadExitEvent, 10000);
        ::CloseHandle(gThreadExitEvent);
        gThreadExitEvent = nullptr;
    }
}

bool VHWebSocketImpl::DisConnectServer() {
    if (mSocketIoImpl) {
        mSocketIoImpl->close();
        return true;
    }
    return false;
}

bool VHWebSocketImpl::SyncDisConnectServer() {
    if (mSocketIoImpl) {
        mSocketIoImpl->sync_close();
        return true;
    }
    return false;
}

void VHWebSocketImpl::On(const std::string& event_name, const WebSocketEventCallback &callback_func) {
    if (mSocketIoImpl) {
        mSocketIoImpl->socket()->on(event_name, [callback_func](sio::event& event)->void {
            callback_func(event.get_name(), MessageWrapper::FromMessage(event.get_message()));
        });
    }
}

bool VHWebSocketImpl::SendMsg(std::string name, std::string msg) {
    if (mSocketIoImpl) {
        mSocketIoImpl->socket()->emitMsg(name, MessageWrapper::ToMessage(msg));
        return true;
    }
    return false;
}



static uint64_t os_gettime_ms(void)
{
   LARGE_INTEGER currentTime;
   LARGE_INTEGER clockFreq;
   QueryPerformanceFrequency(&clockFreq);
   QueryPerformanceCounter(&currentTime);

   uint64_t timeVal = currentTime.QuadPart;
   timeVal *= 1000;
   timeVal /= clockFreq.QuadPart;

   return timeVal;
}

unsigned long long GetCurrentTimeMsec()
{
#ifdef _WIN32
   struct timeval tv;
   time_t clock;
   struct tm tm;
   SYSTEMTIME wtm;

   GetLocalTime(&wtm);
   tm.tm_year = wtm.wYear - 1900;
   tm.tm_mon = wtm.wMonth - 1;
   tm.tm_mday = wtm.wDay;
   tm.tm_hour = wtm.wHour;
   tm.tm_min = wtm.wMinute;
   tm.tm_sec = wtm.wSecond;
   tm.tm_isdst = -1;
   clock = mktime(&tm);
   tv.tv_sec = clock;
   tv.tv_usec = wtm.wMilliseconds * 1000;
   return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#else
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#endif
}

void VHWebSocketImpl::ProcessTask() {
    static struct lws *wsi_mirror = NULL;
    struct lws_context_creation_info creation_info;
    struct lws_client_connect_info connect_info;
    struct lws_context *context;

    memset(&creation_info, 0, sizeof creation_info);
    memset(&connect_info, 0, sizeof(connect_info));


    connect_info.port = 80;
    connect_info.address = mWebSocketUrl.c_str();

    LONGLONG curTime = GetCurrentTimeMsec();
    char path[512] = { 0 };
    _snprintf_s(path, 512, "/ws/%s?_=%lld&tag=0&time=&eventid=", mRoomId.c_str(), curTime);
    connect_info.path = path;

    creation_info.port = CONTEXT_PORT_NO_LISTEN;
    creation_info.protocols = protocols;
    creation_info.gid = -1;
    creation_info.uid = -1;
    creation_info.ws_ping_pong_interval = 0;


    context = lws_create_context(&creation_info);
    if (context == NULL) {
        fprintf(stderr, "Creating libwebsocket context failed\n");
        //return 1;
    }

    connect_info.context = context;
    connect_info.ssl_connection = 0;
    connect_info.host = connect_info.address;
    connect_info.origin = connect_info.address;
    connect_info.ietf_version_or_minus_one = -1;

    while (bThreadRun) {
        if (gTaskEvent) {
            //DWORD ret = WaitForSingleObject(gTaskEvent, 1000);
            unsigned int rl_mirror = 0;
            if (!wsi_mirror && ratelimit_connects(&rl_mirror, 2u)) {
                lwsl_notice("mirror: connecting\n");
                connect_info.pwsi = &wsi_mirror;
                wsi_mirror = lws_client_connect_via_info(&connect_info);
            }
            lws_service(context, 3000);
        }
    }
    lwsl_err("Exiting\n");
    lws_context_destroy(context);
    wsi_mirror = NULL;
}

DWORD WINAPI VHWebSocketImpl::WebSocketThreadProcess(LPVOID p) {
    if (bThreadRun) {
        if (p) {
            VHWebSocketImpl* sdk = (VHWebSocketImpl*)(p);
            if (sdk) {
                sdk->ProcessTask();
            }
        }
    }
    if (gThreadExitEvent) {
        ::SetEvent(gThreadExitEvent);
    }
    return 0;
}

std::string VHWebSocketImpl::WString2String(const std::wstring& ws)
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
std::wstring VHWebSocketImpl::String2WString(const std::string& str)
{
    int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t *wide = new wchar_t[num];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
    std::wstring w_str(wide);
    delete[] wide;
    return w_str;
}

VHALL_WEBSOCKET_SDK_EXPORT VHWebSocketInterface* CreateVHWebSocket() {
    return new VHWebSocketImpl;
}

VHALL_WEBSOCKET_SDK_EXPORT  void DestroyCreateVHWebSocket(VHWebSocketInterface* obj) {
    if (obj) {
        delete obj;
        obj = nullptr;
    }
}