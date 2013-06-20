#ifndef KEYMAPSTR_H
#define KEYMAPSTR_H

#include <QObject>

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_MOUSE   0x80
#define MM_MOUSE_KEY1_NOACTION             0x0
#define MM_MOUSE_KEY1_MOUSE_L_MOVE         0x01
#define MM_MOUSE_KEY1_MOUSE_L_CLICK        0x02
#define MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK  0x03
#define MM_MOUSE_KEY1_MOUSE_R_MOVE         0x04
#define MM_MOUSE_KEY1_MOUSE_R_CLICK        0x05
#define MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK  0x06
#define MM_MOUSE_KEY1_MOUSE_M_MOVE         0x07
#define MM_MOUSE_KEY1_MOUSE_M_CLICK        0x08
#define MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK  0x09
#define MM_MOUSE_KEY1_MOUSE_WHEEL          0xfe
#define MM_MOUSE_KEY1_MOUSE_MOVE           0xff

QString GetMappingStr( uchar cV0, uchar cV1 );
QString MouseKeyToString( uchar cMouseKey );
QString VirtualKeyToString( ulong nVk );
bool FindScanCode( uchar cUsageId, int& nScanCode );
bool FindUsageId( int nScanCode, uchar& cUsageId );

ulong FromMapVirtualKey( int nScanCode );
int FindScanCodeFromVK( ulong dwVK );

#endif // KEYMAPSTR_H
