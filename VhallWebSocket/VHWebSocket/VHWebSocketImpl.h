#pragma once
#include "VHWebSocketInterface.h"
#include "src/sio_client.h"
#include "libwebsockets.h"
#include <atomic>

class VHWebSocketImpl:public VHWebSocketInterface
{
public:
    VHWebSocketImpl();

    virtual ~VHWebSocketImpl();
    virtual void RegisterCallback(WebSocketCallBackInterface* obj);
    virtual bool ConnectServer(const char* url, const char* token) ;
    virtual bool ConnectWebSocket(const std::string url, const std::string room_id);
    virtual bool DisConnectServer();
    virtual void DisConnectWebSocket();
    virtual bool SyncDisConnectServer();
    virtual void On(const std::string& event_name, const WebSocketEventCallback &callback_func);
    virtual bool SendMsg(std::string name, std::string msg);


    void ProcessTask();
    static DWORD WINAPI WebSocketThreadProcess(LPVOID p);
    static void ParseWebsocketData(char* data, size_t srcLen);

private:
    static std::string WString2String(const std::wstring& ws);
    static std::wstring String2WString(const std::string& s);
private:
    sio::client* mSocketIoImpl = nullptr;
    static WebSocketCallBackInterface* mpCallback;

    HANDLE mHThread = nullptr;
    DWORD  threadId = 0;

    static HANDLE gTaskEvent;
    static HANDLE gThreadExitEvent;
    static std::atomic_bool bThreadRun;

    std::string mWebSocketUrl;
    std::string mRoomId;
};

