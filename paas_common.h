#ifndef __PAAS_COMMON_H__
#define __PAAS_COMMON_H__

#include <string>
#include <functional>
#include <iostream>

enum SocketIOMsgType {
    SocketIOMsgType_None = 0,
    SocketIOMsgType_CMD = 1,
    SocketIOMsgType_FLASH = 2,
};

//���湲��Դ��ȡ�б�����
enum ScreenCaptureSource {
   ScreenCaptureSource_DeskTop = 1,
   ScreenCaptureSource_Soft,
};

//��·ֱ������ʱ�ֱ���
enum BROAD_CAST_DPI {
   BROAD_CAST_DPI_1920_1080 = 0,//BROAD_CAST_DPI_HUHD = 0,        // "1920*1080"
   BROAD_CAST_DPI_1440_1080,
   BROAD_CAST_DPI_1280_960,
   BROAD_CAST_DPI_1280_720,//BROAD_CAST_DPI_UHD,             // "1280*720"
   BROAD_CAST_DPI_960_720,      //  
   BROAD_CAST_DPI_960_540,//BROAD_CAST_DPI_MHD,             // "960*540
   BROAD_CAST_DPI_640_480,//BROAD_CAST_DPI_HD,              // "640*480"
   BROAD_CAST_DPI_320_240,//BROAD_CAST_DPI_SD,              // "320*240"
};

//��·ֱ������֡��
enum FRAME_FPS {
    //֡�� 15, 20 (Ĭ��), 25 ֡
    FRAME_FPS_15 = 15,
    FRAME_FPS_20 = 20,
    FRAME_FPS_25 = 25,
};

//��·ֱ����������
enum BITRATE {
    //���� 800, 1000 (Ĭ��), 1200
    BITRATE_800 = 800,
    BITRATE_1000 = 1000,
    BITRATE_1200 = 1200,
};

enum RoomConnectState {
   RoomConnectState_Connected = 0,     //�������ӳɹ�
   RoomConnectState_Reconnecting = 1,  //������������
   RoomConnectState_DisConnect,        //���������ѶϿ� 
   RoomConnectState_NetWorkDisConnect,   //���类�ر� 
};

//api ��ؽӿڵ��ûص��¼�
enum RoomEvent
{
    RoomEvent_OnNetWork_Reconnecting = 0,    //�������ڽ�������
    RoomEvent_OnNetWork_Reconnect,           //���������ɹ�
    RoomEvent_Login,                         //��¼�¼�
    RoomEvent_Logout,                        //�ǳ��¼�
    RoomEvent_InavUserList,                  //����������Ա�б�
    RoomEvent_Apply_Push,                    //��������
    RoomEvent_AuditInavPublish,              //�����������
    RoomEvent_AskforInavPublish,             //��������
    RoomEvent_UserPublishCallback,           //��/��/�ܾ�����״̬��ִ
    RoomEvent_KickInavStream,                //�߳���/ȡ���߳���
    RoomEvent_KickInav,                      //�߳���������/ȡ���߳���������
    RoomEvent_GetKickInavList,               //��ȡ���߳������������б�
    RoomEvent_NoPushStreamPermission,        //�鿴AccessToken�Ƿ����ĳȨ��
    RoomEvent_GetPermission,                 //��ȡȨ��
    RoomEvent_Count
};

//�û�״̬ 
enum MemberStatus {
    MemberStatus_Pushing = 1,  // 1 ������
    MemberStatus_Watching,     // 2 �ۿ���
    MemberStatus_BeInvited,    // 3 ������
    MemberStatus_Application,  // 4 ������
    MemberStatus_KickOut       // 5 ���߳�
};

enum AuditPublish {
    AuditPublish_Accept = 1,  //ͬ������
    AuditPublish_Refused      //�ܾ�����
};

enum PushStreamEvent {
    //*type	int	��	1 ����(Ĭ��) 2 ���� 3 �ܾ�����
    PushStreamEvent_Upper = 1,
    PushStreamEvent_Lower,
    PushStreamEvent_Refused,
};

enum KickStream {
    KickStream_KickOutUser = 1,     //1 �߳�����Ĭ�ϣ�
    KickStream_CancelKickOutUser   // 2 ȡ���߳���
};

class VHRoomMember {
public:
    MemberStatus status;
    std::wstring thirdPartyUserId;
};

class Permission {
public:
    bool kick_inav = false;                 //�߳��������� / ȡ���߳���������
    bool kick_inav_stream = false;          //�߳�·�� / ȡ���߳���
    bool publish_inav_another = false;      //����·ֱ�� / ��������·ֱ��
    bool apply_inav_publish = false;        //��������
    bool publish_inav_stream = false;       //����
    bool askfor_inav_publish = false;       //��������
    bool audit_inav_publish = false;        //�����������

    void reset()
    {
       kick_inav = false;                 //�߳��������� / ȡ���߳���������
       kick_inav_stream = false;          //�߳�·�� / ȡ���߳���
       publish_inav_another = false;      //����·ֱ�� / ��������·ֱ��
       apply_inav_publish = false;        //��������
       publish_inav_stream = false;       //����
       askfor_inav_publish = false;       //��������
       audit_inav_publish = false;        //�����������

    }
};

#endif