#pragma once

#include <T3kHIDLib.h>

namespace t3k_hid_library {

typedef struct _HID_DEVICE_INFO {
	char				DevicePath[512];
	unsigned short		VendorID;
	unsigned short		ProductID;
	unsigned short		MI;
	int					DeviceIndex;
	unsigned short		UsagePage;
	unsigned short		Usage;
	void*			Extra;
} HID_DEVICE_INFO, *PHID_DEVICE_INFO;

typedef void (T3K_CALLBACK *FnOnDisconnect)( void* pContext );
typedef void (T3K_CALLBACK *FnOnReceive)( unsigned char* pData, unsigned long dwBytes, void* pContext );

typedef struct _HID_NOTIFY
{
	void*			Context;
	FnOnDisconnect	fnOnDisconnect;
	FnOnReceive		fnOnReceive;
} HID_NOTIFY, *PHID_NOTIFY;

class CHIDAdaptor;
class T3K_API CHID
{
public:
	CHID(void);
	~CHID(void);
protected:
	CHIDAdaptor*		m_pAdaptor;
public:
	int GetDeviceCount( unsigned short nVendorID, unsigned short nProductID, int nMI );
	PHID_DEVICE_INFO GetDeviceInfo( unsigned short nVendorID, unsigned short nProductID, int nMI, int nDevIndex );
	PHID_DEVICE_INFO GetDeviceInfo();

	bool IsOpenDevice();
	void SetNotify( PHID_NOTIFY pHIDNotify );

	bool Open( unsigned short nVendorID, unsigned short nProductID, int nMI, int nDevIndex );
	void Close();

	bool Write( const unsigned char* pBuffer, unsigned long dwBytesToWrite );

	bool GetFeature( void* pBuffer, const unsigned long dwBufferSize );
	bool SetFeature( void* pBuffer, const unsigned long nBufferSize );

	void* GetLowDeviceHandle();

	bool GetSerialNumberString( void* pBuffer, const unsigned long dwBufferLength );
};

} // t3k_hid_library
