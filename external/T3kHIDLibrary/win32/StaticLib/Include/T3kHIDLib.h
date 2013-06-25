#ifndef __T3K_HID_LIBRARY_H__
#define __T3K_HID_LIBRARY_H__

#if defined(OS_LINUX)		/* linux */
#  ifdef T3KHIDLIB_EXPORTS
#    define T3K_API	
#  else
#    define T3K_API 	
#  endif

#  define T3K_CALLBACK
#  define T3K_CDECL
#  include <stddef.h>	/* for size_t */

#elif defined(OS_MAC)		/* mac os x */
#  ifdef T3KHIDLIB_EXPORTS
#    define T3K_API	__attribute__((visibility("default")))
#  else
#    define T3K_API 	
#  endif

#  define T3K_CALLBACK
#  define T3K_CDECL
#  include <stddef.h>	/* for size_t */

#else						/* ms windows */
#  define T3K_CALLBACK __stdcall
#  define T3K_CDECL    __cdecl
#  ifndef USE_T3K_STATIC_LIBS
#    ifdef T3KHIDLIB_EXPORTS
#      define T3K_API __declspec(dllexport)
#    else
#      define T3K_API __declspec(dllimport)
#    endif
#  else
#    define T3K_API
#  endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef void *	T3K_HANDLE;
typedef void *	T3K_DEVICE_INFO;

#pragma pack(1)

/*********************************************************************/
/* error code */
/*********************************************************************/
enum
{
	T3K_SUCCESS						= 0,
	T3K_ERROR_NO_DEVICE				= -1,
	T3K_ERROR_FIRMWARE_DOWNLOAD		= -2,
	T3K_ERROR_TIMEOUT				= -3,
	T3K_ERROR_SYNC_OBJECT_OVERFLOW	= -4,
	T3K_ERROR_ALREADY_OPENED		= -5,
	T3K_ERROR_DEVICE_BUSY			= -6,
	T3K_ERROR_NOT_SUPPORT			= -7,
	T3K_ERROR_SEND_FAIL				= -8,
	T3K_ERROR_INVALID_PARAMETER		= -9,
	T3K_ERROR_ACCESS_DENIED			= -10,
	T3K_ERROR_UNKNOWN				= -20
};

/*********************************************************************/
/* touch point body */
/*********************************************************************/
typedef struct _t3ktouchpoint
{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
	unsigned short s;		/* 0:nodata 1:hover 2:touch */
} t3ktouchpoint;

/*********************************************************************/
/* constant of device type */
/*********************************************************************/
#define T3K_DEVICE_MOUSE     0x01
#define T3K_DEVICE_TOUCH     0x02
#define T3K_DEVICE_KEYBOARD  0x10

/*********************************************************************/
/* constant of mouse button */
/*********************************************************************/
#define T3K_LBUTTON		0x01
#define T3K_RBUTTON		0x02
#define T3K_MBUTTON		0x04

/*********************************************************************/
/* constant of combination keys */
/*********************************************************************/
#define T3K_CKEY_CTRL    0x01
#define T3K_CKEY_SHIFT   0x02
#define T3K_CKEY_ALT     0x04
#define T3K_CKEY_WINDOW  0x08

/*********************************************************************/
/* instant mode */
/*********************************************************************/
enum
{
	T3K_HID_MODE_MESSAGE       = 0x0001, /* 'M': MSG, TXT */
	T3K_HID_MODE_COMMAND       = 0x0002, /* 'C': CMD, RSP, RSC */
	T3K_HID_MODE_VIEW          = 0x0004, /* 'V': IRD, ITD, OBC, PRV */
	T3K_HID_MODE_OBJECT        = 0x0010, /* 'O': OBJ */
	T3K_HID_MODE_TOUCHPNT      = 0x0020, /* 'T': TPT */
	T3K_HID_MODE_GESTURE       = 0x0040, /* 'G': GST */
	T3K_HID_MODE_DEVICE        = 0x0080, /* 'D': DVC */
	
	T3K_HID_MODE_SINGLE_TOUCH  = 0x0100, /* 'S': single touch enable only */
	T3K_HID_MODE_MOUSE_DISABLE = 0x0200  /* 'X': disable mouse */
};

/*********************************************************************/
/* gesture action */
/*********************************************************************/
enum
{
	t3kgstNoAction				= 0x00,
	t3kgstSingleMove			= 0x01,
	t3kgstSingleTap				= 0x02,
	t3kgstSingleDoubleTap		= 0x03,
	t3kgstSingleLongTap			= 0x04,
	t3kgstFingersMove			= 0x11,
	t3kgstFingersTap			= 0x12,
	t3kgstFingersDoubleTap		= 0x13,
	t3kgstFingersLongTap		= 0x14,
	t3kgstPalmMove				= 0x21,
	t3kgstPalmTap				= 0x22,
	t3kgstPalmDoubleTap			= 0x23,
	t3kgstPalmLongTap			= 0x24,
	t3kgstPutAndTap				= 0x32,
	t3kgstPutAndDoubleTap		= 0x33,
	t3kgstPutAndLongTap			= 0x34,
	t3kgstZoom					= 0x80
};

#define T3K_GESTURE_ACTION_MASK		0x3f

/*********************************************************************/
/* feasible gesture mask */
/*********************************************************************/
enum
{
	t3kfgstSingleMove			= 0x0001,
	t3kfgstSingleTap			= 0x0002,
	t3kfgstSingleDoubleTap		= 0x0004,
	t3kfgstSingleLongTap		= 0x0008,
	t3kfgstSingleAll			= (t3kfgstSingleMove|t3kfgstSingleTap|t3kfgstSingleDoubleTap|t3kfgstSingleLongTap),
	t3kfgstFingersMove			= 0x0010,
	t3kfgstFingersTap			= 0x0020,
	t3kfgstFingersDoubleTap		= 0x0040,
	t3kfgstFingersLongTap		= 0x0080,
	t3kfgstFingersAll			= (t3kfgstFingersMove|t3kfgstFingersTap|t3kfgstFingersDoubleTap|t3kfgstFingersLongTap),
	t3kfgstPalmMove				= 0x0100,
	t3kfgstPalmTap				= 0x0200,
	t3kfgstPalmDoubleTap		= 0x0400,
	t3kfgstPalmLongTap			= 0x0800,
	t3kfgstPalmAll				= (t3kfgstPalmMove|t3kfgstPalmTap|t3kfgstPalmDoubleTap|t3kfgstPalmLongTap),
	t3kfgstActionZoom			= 0x1000,
	t3kfgstPutAndTap			= 0x2000,
	t3kfgstPutAndDoubleTap		= 0x4000,
	t3kfgstPutAndLongTap		= 0x8000,
	t3kfgstPutAndAll			= (t3kfgstPutAndTap|t3kfgstPutAndDoubleTap|t3kfgstPutAndLongTap)
};

/*********************************************************************/
/* feasible gesture extension mask */
/*********************************************************************/
enum
{
	t3kfgstePutAndTapOrDoubleTapOnly	= 0x020000,
	t3kfgsteHybridMultitouchDevice		= 0x040000,
	t3kfgsteInertialWheel				= 0x100000,
	t3kfgsteInvertWheel					= 0x200000,
	t3kfgsteAll							= (t3kfgstePutAndTapOrDoubleTapOnly|t3kfgsteHybridMultitouchDevice|t3kfgsteInertialWheel|t3kfgsteInvertWheel)
};

/*********************************************************************/
/* type of packet */
/*********************************************************************/
enum
{
	t3ktype_msg,			// message
	t3ktype_obj,			// touch object
	t3ktype_obc,			// touch object
	t3ktype_dtc,			// detection data for graph
	t3ktype_ird,			// ird for graph
	t3ktype_itd,			// itd for graph
	t3ktype_prv,			// preview data for sideview
	t3ktype_cmd,			// command
	t3ktype_rsp,			// response
	t3ktype_rse,			// response error
	t3ktype_stt,			// status
	t3ktype_dvc,			// device data
	t3ktype_tpt,			// touch point data
	t3ktype_gst,			// gesture data
	t3ktype_ver				// version information
};

/*********************************************************************/
/* packet structure */
/*********************************************************************/
#define T3K_MESSAGE_LENGTH			(256-2)
#define T3K_MAX_OBJ_COUNT			(10+1)
#define T3K_MAX_DTC_COUNT			(10)
#define T3K_GST_MESSAGE_LENGTH		(256-14)
#define T3K_COMMAND_LENGTH			(440)
#define T3K_VER_DATE_LEN  11
#define T3K_VER_TIME_LEN  8
typedef struct _t3kpacket
{
	short type;
	unsigned short ticktime;
	char	partid[8];
	unsigned short length;
	union _body
	{
		struct _msg
		{
			char message[T3K_MESSAGE_LENGTH];
		} msg;
		struct _obj
		{
			unsigned short count;
			float start_pos[T3K_MAX_OBJ_COUNT];
			float end_pos[T3K_MAX_OBJ_COUNT];
		} obj;
		struct _obc
		{
			unsigned short count;
			float start_pos;
			float end_pos;
		} obc;
		struct _dtc
		{
			unsigned short count;
			unsigned long start_pos[T3K_MAX_DTC_COUNT];
			unsigned long end_pos[T3K_MAX_DTC_COUNT];
		} dtc;
		struct _ixd
		{
			short total;
			short offset;
			short count;
			unsigned char data[1];
		} ixd;
		struct _prv
		{
			short height;
			short offset_y;
			short width;
			unsigned char image[1];
		} prv;
		struct _cmd
		{
			unsigned short id;
			char command[T3K_COMMAND_LENGTH];
		} cmd;
		struct _rsp
		{
			unsigned short id;
			unsigned char is_final;
			char command[T3K_COMMAND_LENGTH];
		} rsp;
		struct _stt
		{
			char status[T3K_COMMAND_LENGTH];
		} stt;
		struct _dvc
		{
			unsigned char flag;
			unsigned char touch_obj_cnt;
			unsigned char mouse_buttons;
			char mouse_wheel;
			unsigned short mouse_x;
			unsigned short mouse_y;
			float touch_x;
			float touch_y;
			unsigned char keyboard_modifier;
			unsigned char keyboard_reserved;
			unsigned char keyboard_key[6];
		} dvc;
		struct _tpt
		{
			short actual_touch;
			short touch_count;
			t3ktouchpoint points[1];
		} tpt;
		struct _gst
		{
			unsigned char action_group;
			unsigned char action;
			unsigned short feasibleness;
			unsigned short x;
			unsigned short y;
			unsigned short w;
			unsigned short h;
			float zoom;
			char message[T3K_GST_MESSAGE_LENGTH];
		} gst;
		struct _ver
		{
			unsigned short nv;
			unsigned short minor;
			unsigned short major;
			unsigned short model;
			unsigned char date[T3K_VER_DATE_LEN];
			unsigned char time[T3K_VER_TIME_LEN];
		} ver;
	} body;
} t3kpacket;

/*********************************************************************/
/* firmware mode */
/*********************************************************************/
enum
{
	MODE_UNKNOWN	= 0,
	MODE_CM_IAP		= 1,
	MODE_CM_APP		= 2,
	MODE_MM_IAP		= 3,
	MODE_MM_APP		= 4,
	MODE_MM_UPG		= 5,
	MODE_CM_UPG		= 6
};

/*********************************************************************/
/* address of packet: T3kQueryFirmwareVersion */
/*********************************************************************/
enum
{
	PKT_ADDR_MM     = 0x1000,
	PKT_ADDR_CM1    = 0x2000,
	PKT_ADDR_CM2    = 0x4000,
	PKT_ADDR_CM_SUB = 0x8000
};

#define sizeof_header_t3kpacket          ((size_t)&((t3kpacket *)0)->body)

#pragma pack()

/*********************************************************************/
/* event handlers */
/*********************************************************************/
typedef void (T3K_CALLBACK *FnOnT3kHandleEventsHandler)( T3K_HANDLE hDevice, void * pContext );
typedef void (T3K_CALLBACK *FnOnT3kDisconnectHandler)( T3K_HANDLE hDevice, void * pContext );
typedef int  (T3K_CALLBACK *FnOnT3kReceiveRawDataHandler)( T3K_HANDLE hDevice, unsigned char* pBuffer, unsigned short nBytes, void * pContext );
typedef void (T3K_CALLBACK *FnOnT3kDownloadingFirmwareHandler)( T3K_HANDLE hDevice, int bDownload, void * pContext );
typedef void (T3K_CALLBACK *FnOnT3kPacketHandler)( T3K_HANDLE hDevice, t3kpacket* packet, int bSync, void * pContext );

typedef struct _T3K_EVENT_NOTIFY
{
	void*								Context;
	FnOnT3kHandleEventsHandler			fnOnHandleEvents;
	FnOnT3kDisconnectHandler			fnOnDisconnect;
	FnOnT3kReceiveRawDataHandler		fnOnReceiveRawData;
	FnOnT3kDownloadingFirmwareHandler	fnOnDownloadingFirmware;
	FnOnT3kPacketHandler				fnOnPacket;
} T3K_EVENT_NOTIFY, *PT3K_EVENT_NOTIFY;

/*********************************************************************/
/* interface functions */
/*********************************************************************/
T3K_API int T3kGetDeviceCount( unsigned short nVendorID, unsigned short nProductID, unsigned short nMI );
T3K_API T3K_HANDLE T3kOpenDevice( unsigned short nVendorID, unsigned short nProductID, unsigned short nMI, unsigned short nDevIndex );
T3K_API void T3kSetEventNotify( T3K_HANDLE hDevice, PT3K_EVENT_NOTIFY pNotify );
T3K_API void T3kCloseDevice( T3K_HANDLE hDevice );

T3K_API int T3kSetInstantMode( T3K_HANDLE hDevice, int nMode, unsigned short nExpireTime, unsigned long dwFgstValue );
T3K_API int T3kSendCommand( T3K_HANDLE hDevice, const char * szCmd, int bAsync, unsigned short nTimeout );
T3K_API char* T3kGetResponse( T3K_HANDLE hDevice );

T3K_API T3K_DEVICE_INFO T3kGetDeviceInfo( unsigned short nVendorID, unsigned short nProductID, unsigned short nMI, unsigned short nDevIndex );
T3K_API T3K_DEVICE_INFO T3kGetDeviceInfoFromHandle( T3K_HANDLE hDevice );

T3K_API unsigned short T3kGetDevInfoVendorID( T3K_DEVICE_INFO devInfo );
T3K_API unsigned short T3kGetDevInfoProductID( T3K_DEVICE_INFO devInfo );
T3K_API unsigned short T3kGetDevInfoInterfaceNumber( T3K_DEVICE_INFO devInfo );
T3K_API char* T3kGetDevInfoPath( T3K_DEVICE_INFO devInfo );
T3K_API int T3kGetDevInfoDeviceIndex( T3K_DEVICE_INFO devInfo );

T3K_API int T3kSendBuffer( T3K_HANDLE hDevice, const unsigned char* pBuffer, unsigned short nBufferSize, int bAsync, unsigned short nTimeout );
T3K_API int T3kQueryFirmwareVersion( T3K_HANDLE hDevice, unsigned short wAddr, int* pnMode, int bAsync, unsigned short nTimeout );
T3K_API void T3kUseExclusiveMode( T3K_HANDLE hDevice, int bUse );

T3K_API void* T3kGetLowDeviceHandle( T3K_HANDLE hDevice );
T3K_API int T3kGetLastError(void);

T3K_API int T3kFindScanCode( char cUsageId );
T3K_API char T3kFindUsageId( int nScanCode );

T3K_API unsigned short T3kCalculateCRC( unsigned char *pkt_ptr, unsigned short pkt_len );
T3K_API unsigned short T3kCheckCRC( unsigned char *pkt_ptr, unsigned short pkt_len );

T3K_API void T3kLockFirmwareDownload( T3K_HANDLE hDevice );
T3K_API void T3kUnlockFirmwareDownload( T3K_HANDLE hDevice );

};

#ifdef __cplusplus
#endif	// extern "C"
#endif
