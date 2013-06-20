#ifndef __T30x_ERROR_CODE_H__
#define __T30x_ERROR_CODE_H__

#define T30ERROR_NONE							(0x00000000)
#define T30ERROR_HID_INTERNAL_ERROR				(0x80004001)			// ���� ����
#define T30ERROR_HID_INVALID_PARAMETER			(0x80004002)			// �Ķ���� ����
#define T30ERROR_HID_OPEN_FAIL					(0x80001001)			// HID ���� ���� 
#define T30ERROR_HID_WRITE_FAIL					(0x80001002)			// HID Write ����

#define T30ERROR_HID_TIMEOUT					(0x80002001)			// Sync-Command Timeout
#define T30ERROR_HID_FIRMWARE_DOWNLOAD			(0x80002002)			// Firmware �ٿ���...
#define T30ERROR_HID_CREATE_INTERNALWND_FAIL	(0x80002003)			// ���� Window���� ����
#define T30ERROR_HID_NOT_CONNECTED				(0x80002004)			// T3000 ������� ����
#define T30ERROR_HID_TOO_MANY_OBJECTS			(0x80002005)			// HID �ִ� ��ü�� �ʰ�(255)
#define T30ERROR_HID_ALREADY_OPENED				(0x80002007)			// HID ��������.

#endif //__T30x_ERROR_CODE_H__