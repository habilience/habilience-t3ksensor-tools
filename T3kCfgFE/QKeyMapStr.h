#ifndef __KEY_MAP_STR_H__
#define __KEY_MAP_STR_H__

#include <QString>

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_CKEYS   (MM_MOUSE_KEY0_CTRL | MM_MOUSE_KEY0_SHIFT | MM_MOUSE_KEY0_ALT | MM_MOUSE_KEY0_WINDOW)
#define MM_MOUSE_KEY0_MOUSE   0x80

QString getMappingStr( unsigned char cV0, unsigned char cV1 );
QString mouseKeyToString( unsigned char cMouseKey );
QString virtualKeyToString( unsigned long nVk );
bool findScanCode( unsigned char cUsageId, int& nScanCode );
bool findUsageId( int nScanCode, unsigned char& cUsageId );

unsigned long scanCodeToVirtualKey( int nScanCode );
int virtualKeyToScanCode( unsigned long nVk );

#endif
