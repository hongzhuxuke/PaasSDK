#pragma once

#include <string>
#include <functional>
#include <iostream>

typedef std::function<void(const std::string&, const std::string&)> WebSocketEventCallback;

class WebSocketCallBackInterface {
public:
    virtual void OnOpen() = 0;
    virtual void OnFail() = 0;
    virtual void OnReconnecting() = 0;
    virtual void OnReconnect(unsigned, unsigned) = 0;
    virtual void OnClose() = 0;
    virtual void OnSocketOpen(std::string const& nsp) = 0;
    virtual void OnSocketClose(std::string const& nsp) = 0;
	 virtual void OnServiceMsg(std::string type, const std::wstring user_id, const std::wstring nickname, const std::string rolename, bool is_banned, int devType = -1, int uv = 0) = 0;
    virtual void OnRecvAllMsg(const char* ,int length) = 0;
};

class VHWebSocketInterface {
public:
    VHWebSocketInterface() {};

    virtual ~VHWebSocketInterface() {};

    virtual void RegisterCallback(WebSocketCallBackInterface* obj) = 0;
    /**
    * url: websocket���ӵ�ַ��eg. ws://t-msg01-open.e.vhall.com
    * token: ��֤token
    * obj: �ص���������
    **/
    virtual bool ConnectServer(const char* url, const char* token) = 0;

    virtual bool DisConnectServer() = 0;
    //websocket����
    virtual bool ConnectWebSocket(const std::string url, const std::string room_id) = 0;

    virtual void DisConnectWebSocket() = 0;

    virtual bool SyncDisConnectServer() = 0;
    /**
    * ���ӷ���ǰ��Ӽ����¼�
    */
    virtual void On(const std::string& event_name, const WebSocketEventCallback &callback_func) = 0;

    virtual bool SendMsg(std::string name, std::string msg) = 0;
};


#ifdef  VHALL_WEBSOCKET_SDK_EXPORT
#define VHALL_WEBSOCKET_SDK_EXPORT     __declspec(dllimport)
#else
#define VHALL_WEBSOCKET_SDK_EXPORT     __declspec(dllexport)
#endif

VHALL_WEBSOCKET_SDK_EXPORT VHWebSocketInterface* CreateVHWebSocket();
VHALL_WEBSOCKET_SDK_EXPORT  void DestroyCreateVHWebSocket(VHWebSocketInterface* obj);