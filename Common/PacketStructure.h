#ifndef PACKETSTRUCTURE_H
#define PACKETSTRUCTURE_H


#pragma pack( push, 1 )

#ifndef MAX_NAME
#define MAX_NAME                    50
#endif

#define RAWDATA_BLOCK               64                              // 64 byte
#define MAX_RAWDATA_BLOCKCOUNT      10                              // 1280 byte
#define MAX_RAWDATA_BLOCK           640                             // 1280 byte
#define MAX_CHAT_MSG_BLOCK          60

#define RemoteClientPortDef         35000
#define RemoteCtrlrPortDef          35001

enum Sender {
    Server                          = 0x1000,
    Client                          = 0x2000,
    CtrlManager                     = 0x3000,
    CtrlDriver                      = 0x4000
};

enum Type {
	// Request
    ReqClientList                   = 0x0001,
    ReqClientInfo                   = 0x0002,
    ReqConnectionInfo               = 0x0003,                       // Driver
    ReqAssignedID                   = 0x0004,
    ReqClientEnv                    = 0x0005,

	// Response
    ResConnectionInfo               = 0x0011,                       // Driver : RCtrlConnectInfo
    ResClientList                   = 0x0012,
    ResClientCount                  = 0x0013,
    ResClientInfo                   = 0x0014,
    ResNoClient                     = 0x0015,

	// Transmit
    TranSensorRawData               = 0x0021,                       // Send, Recv : RRawDataPkt
    TranChatMessage                 = 0x0022,
    TranInstantMode                 = 0x0023,
    TranClientEnv                   = 0x0024,

	// Notify
    NotifyFWDownloading             = 0x0031,
    NotifySensorDisconnected        = 0x0032,
    NotifyPairConnected             = 0x0033,
    NotifyPairDisconnected          = 0x0034,
    NotifyUpdateClientList          = 0x0035,
    NotifyClientID                  = 0x0036,
    NotifyManagerID                 = 0x0037,
    NotifyRemoteFinish              = 0x0038,

	// Etc..
    SideviewMode                    = 0x0051,
    OrderTouch                      = 0x0052,
    KeepAlive                       = 0x0053,

    Error                           = 0x0061
};

// Data
#define RDisconnected               0
#define RConnected                  1
#define ROtherConnected             2

typedef struct _RHeaderPkt
{
    unsigned short                  nPktSize;                       // packet total length
    short                           nType;
} RHeaderPkt;

typedef struct _RRawDataPkt
{
    RHeaderPkt                      Header;                         // size : sizeof(RRawDataPkt) + ((nTotalBlockCount-1)*64)
    short                           nTotalBlockCount;
    char                            data[RAWDATA_BLOCK];
} RRawDataPkt, *LPRRawDataPkt;

typedef struct _RInstantMode
{
    RHeaderPkt                      Header;
    short                           nMode;
    unsigned short                  nExpireTime;
    unsigned long                   dwGstFlag;
} RInstantMode;

typedef struct _RChatMessage
{
    RHeaderPkt                      Header;
    char                            szString[1];                    // variable
} RChatMessage;

typedef struct _RClientEnvironments
{
    RHeaderPkt                      Header;
    short                           nID;
    unsigned short                  nDisplayCnt;
    unsigned short                  nDPrimaryIdx;
    unsigned short                  nOrientation;
	bool							bWindows;
    bool                            bTabletInput;
    unsigned long                   dwHIDState;
    char                            szIPOSString[1];                  // variable : IP\0OS
} RClientEnvironments;

typedef struct _RAdditionalPkt
{
    RHeaderPkt                      Header;
    union _uoData                                                   // fixed Length : 64 byte
	{
		struct _RClientConnectInfo
		{
            short					nPrevID;                        // ID : 1 ~ 3000
            unsigned short          nModel;
            char                    szUserName[MAX_NAME];
		} RClientConnectInfo;

		struct _RCtrlConnectInfo
		{
            short                   nConnectID;                     // ID : 1 ~ 3000
            short                   nManagerID;
		} RCtrlConnectInfo;

		struct _RClientCount
		{
            short                   nTotalCount;
		} RClientCount;

		struct _RClientInfo
		{
            char                    bConnected;                     // Disconnect : 0, Connect : 1, Other connected : 2
            short                   nID;                            // ID : 1 ~ 3000
            unsigned short          nModel;
            char                    szUserName[MAX_NAME];           // UserName:ComputerName (8:8)
		} RClientInfo;

		struct _RClientID
		{
            short                   nID;
		} RClientID;

		struct _RManagerID
		{
            short                   nID;
		} RManagerID;

		struct _RStateFWDown
		{
            char                    bDownloading;                   // 0, 1
		} RStateFWDown;

		struct _RSideviewMode
		{
            char					bShow;                          // 0, 1
		} RSideviewMode;

		struct _ROrderTouch
		{
            char                    bShowMark;                      // 0, 1    // show/hide cross mark
            char                    bOnScreen;                      // 0, 1    // on/off screen pointer (auto range/bent/cali mark)
            char                    bTouchOK;                       // 0, 1    //
            short                   nX;                             // 32k
            short                   nY;                             // 32k
            char                    nPercent;						// 0 ~ 100
		} ROrderTouch;

		struct _RErrorPkt
		{
            short                   nErrorCode;                     // RemoteErrorCode.h
		} RErrorPkt;

        char                        szDummy[RAWDATA_BLOCK];
	} uoData;
} RAdditionalPkt;

#pragma pack( pop )

#endif // PACKETSTRUCTURE_H

