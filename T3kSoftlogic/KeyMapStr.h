#ifndef __KEY_MAP_STR_H__
#define __KEY_MAP_STR_H__

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_CKEYS   (MM_MOUSE_KEY0_CTRL | MM_MOUSE_KEY0_SHIFT | MM_MOUSE_KEY0_ALT | MM_MOUSE_KEY0_WINDOW)
#define MM_MOUSE_KEY0_MOUSE   0x80

CString GetMappingStr( BYTE cV0, BYTE cV1 );
CString MouseKeyToString( BYTE cMouseKey );
CString VirtualKeyToString( UINT nVk );
BOOL FindScanCode( BYTE cUsageId, int& nScanCode );
BOOL FindUsageId( int nScanCode, BYTE& cUsageId );

#endif