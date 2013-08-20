#include "QKeyMapStr.h"

#include <QKeyEvent>

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

const short s_ScanCodeToHIDUsageID[] =
{	
	0x00, 0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E, 0x2A, 0x2B, 0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C,
	0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30, 0x28, 0xE0, 0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x35, 0xE1, 0x31,
	0x1D, 0x1B, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xE5, 0x55, 0xE2, 0x2C, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x53, 0x47, 0x5F, 0x60, 0x61, 0x56, 0x5C, 0x5D, 0x5E, 0x57, 0x59, 0x5A, 0x5B, 0x62, 0x63, 0x46/*0x00*/, 0x00, 0x64,
	0x44, 0x45, 0x67, 0x00, 0x00, 0x8C, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
	0x71, 0x72, 0x00, 0x88, 0x00, 0x00, 0x87, 0x00, 0x00, 0x73, 0x93, 0x92, 0x8A, 0x00, 0x8B, 0x00, 0x89, 0x85, 0x00  
};

const unsigned long s_ScanCodeToQtVirtualKey[] =
{
    0x00000000, 0x01000000, 0x00000031, 0x00000032, 0x00000033, 0x00000034, 0x00000035, 0x00000036, 0x00000037, 0x00000038,
    0x00000039, 0x00000030, 0x0000002D, 0x0000003D, 0x01000003, 0x01000001, 0x00000051, 0x00000057, 0x00000045, 0x00000052,
    0x00000054, 0x00000059, 0x00000055, 0x00000049, 0x0000004F, 0x00000050, 0x0000005B, 0x0000005D, 0x01000004, 0x01000021,
    0x00000041, 0x00000053, 0x00000044, 0x00000046, 0x00000047, 0x00000048, 0x0000004A, 0x0000004B, 0x0000004C, 0x0000003B,
    0x00000027, 0x00000060, 0x01000020, 0x0000005C,/* 0x00000000,*/ 0x0000005A, 0x00000058, 0x00000043, 0x00000056, 0x00000042, 0x0000004E,
    0x0000004D, 0x0000002C, 0x0000002E, 0x0000002F, 0x01000020, 0x0000002A, 0x01000023, 0x00000020, 0x01000024, 0x01000030,
    0x01000031, 0x01000032, 0x01000033, 0x01000034, 0x01000035, 0x01000036, 0x01000037, 0x01000038, 0x01000039, 0x01000025,
    0x01000026,/* 0x01000014, 0x01000012, 0x01000015, 0x01000013 */ 0x01000010, 0x01000013, 0x01000016, 0x0000002D, 0x01000012, 0x0100000B, 0x01000014, 0x0000002B, 0x01000011,
    0x01000015, 0x01000017, 0x01000006, 0x01000007, 0x00000000, 0x00000000, 0x00000000, 0x0100003A, 0x0100003B, 0x01000005,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0100003C, 0x0100003D, 0x0100003E, 0x0100003F, 0x01000040, 0x01000041, 0x01000042, 0x01000043, 0x01000044, 0x01000045,
    0x01000046, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01000047, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

bool findUsageId( int nScanCode, unsigned char& cUsageId )
{
    if( nScanCode >= 0 && (unsigned int)nScanCode < sizeof(s_ScanCodeToHIDUsageID) / sizeof(short) )
    {
        cUsageId = (uchar)s_ScanCodeToHIDUsageID[nScanCode];
        return true;
    }

    return false;
}

unsigned long scanCodeToVirtualKey( int nScanCode )
{
    return s_ScanCodeToQtVirtualKey[nScanCode];
}

int virtualKeyToScanCode( unsigned long nVk )
{
    switch ( nVk )
    {
    case Qt::Key_Left:
        return 75;
    case Qt::Key_Right:
        return 77;
    case Qt::Key_Up:
        return 72;
    case Qt::Key_Down:
        return 80;
    }
    for ( unsigned int i=0 ; i<sizeof(s_ScanCodeToQtVirtualKey)/sizeof(ulong) ; i++ )
    {
        if( s_ScanCodeToQtVirtualKey[i] == nVk )
        {
            return i;
        }
    }
    return -1;
}

bool findScanCode( unsigned char cUsageId, int& nScanCode )
{
    switch( cUsageId )
    {
    case 0x50:
        nScanCode = 75;
        return true;
    case 0x4F:
        nScanCode = 77;
        return true;
    case 0x52:
        nScanCode = 72;
        return true;
    case 0x51:
        nScanCode = 80;
        return true;
    case 0x49:
        nScanCode = 82;
        return true;
    case 0x4A:
        nScanCode = 71;
        return true;
    case 0x4B:
        nScanCode = 73;
        return true;
    case 0x4C:
        nScanCode = 83;
        return true;
    case 0x4D:
        nScanCode = 79;
        return true;
    case 0x4E:
        nScanCode = 81;
        return true;
    }
    for ( unsigned int i=0 ; i<sizeof(s_ScanCodeToHIDUsageID)/sizeof(short) ; i++ )
    {
        if( s_ScanCodeToHIDUsageID[i] == cUsageId )
        {
            nScanCode = i;
            return true;
        }
    }
    return false;
}

QString compositeKeyToString( unsigned char cV )
{
    switch (cV)
	{
	case MM_MOUSE_KEY0_CTRL:
        return "Ctrl";
		break;
	case MM_MOUSE_KEY0_SHIFT:
        return "Shift";
		break;
	case MM_MOUSE_KEY0_ALT:
        return "Alt";
		break;
	case MM_MOUSE_KEY0_WINDOW:
#ifdef Q_WS_MAC
        return "Cmd";
#else
        return "Win";
#endif
		break;
	}
    return "";
}

QString virtualKeyToString( unsigned long nVk )
{
    switch( nVk )
    {
    case Qt::Key_Backspace:
        return "Backspace";
    case Qt::Key_Tab:
        return "Tab";
    case Qt::Key_Clear:
        return "";
    case Qt::Key_Pause:
        return "Pause";
    case Qt::Key_Hangul:
        return "Lang";
    case Qt::Key_Hangul_Hanja:
        return "Hanja";
    case Qt::Key_Escape:
        return "ESC";
    case Qt::Key_Space:
        return "Space";
    case Qt::Key_PageUp:
        return "PgUp";
    case Qt::Key_PageDown:
        return "PgDn";
    case Qt::Key_End:
        return "End";
    case Qt::Key_Home:
        return "Home";
    case Qt::Key_Left:
        return "Left";
    case Qt::Key_Up:
        return "Up";
    case Qt::Key_Right:
        return "Right";
    case Qt::Key_Down:
        return "Down";
    case Qt::Key_Print:
        return "Print";
    case Qt::Key_Insert:
        return "Insert";
    case Qt::Key_Delete:
        return "Delete";
    case Qt::Key_Enter:
        return "Enter";
    case Qt::Key_Return:
        return "Enter";
    case Qt::Key_CapsLock:
        return "Caps Lock";
    case Qt::Key_NumLock:
        return "Num Lock";
    case Qt::Key_ScrollLock:
        return "Scrl Lock";
    case Qt::Key_Asterisk:
        return "Num *";
    case Qt::Key_Plus:
        return "Num +";
    case Qt::Key_Minus:
        return "Num -";
    case Qt::Key_Slash:
        return "/";
    case Qt::Key_Equal:
        return "=";
    case Qt::Key_BracketLeft:
        return "[";
    case Qt::Key_BracketRight:
        return "]";
    case Qt::Key_Backslash:
        return "\\";
    case Qt::Key_Semicolon:
        return ";";
    case Qt::Key_Apostrophe:
        return "'";
    case Qt::Key_Comma:
        return ",";
    case Qt::Key_Period:
        return ".";
    case Qt::Key_division:
        return "/";
    case Qt::Key_QuoteLeft:
        return "`";
    }
    if ( (nVk >= Qt::Key_F1) && (nVk <= Qt::Key_F24) )
	{
        QString strK = QString("F%1").arg(nVk-Qt::Key_F1+1);
		return strK;
	}

	if( (nVk >= '0' && nVk <= '9') ||
		(nVk >= 'A' && nVk <= 'Z') )
	{
        QString strK = QString("%1").arg((char)nVk);
		return strK;
	}

    return "";
}

QString mouseKeyToString( unsigned char cMouseKey )
{
    switch( cMouseKey )
    {
    case MM_MOUSE_KEY1_NOACTION:
    default:
        return "";
        break;
    case MM_MOUSE_KEY1_MOUSE_L_MOVE:
        return "Left\r\nDrag";
    case MM_MOUSE_KEY1_MOUSE_L_CLICK:
        return "Left\r\nClick";
    case MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK:
        return "Left\r\nDouble Click";
    case MM_MOUSE_KEY1_MOUSE_R_MOVE:
        return "Right\r\nDrag";
    case MM_MOUSE_KEY1_MOUSE_R_CLICK:
        return "Right\r\nClick";
    case MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK:
        return "Right\r\nDouble Click";
    case MM_MOUSE_KEY1_MOUSE_M_MOVE:
        return "Middle\r\nDrag";
    case MM_MOUSE_KEY1_MOUSE_M_CLICK:
        return "Middle\r\nClick";
    case MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK:
        return "Middle\r\nDouble Click";
    case MM_MOUSE_KEY1_MOUSE_WHEEL:
        return "Wheel";
    case MM_MOUSE_KEY1_MOUSE_MOVE:
        return "Move";
    }
    return "";
}

QString getMappingStr( unsigned char cV0, unsigned char cV1 )
{
    QString strText;
    QString strMouseKey;
    int nScanCode;

    if (cV0 & MM_MOUSE_KEY0_MOUSE)
        strMouseKey = mouseKeyToString(cV1);

    short nCompositKey = cV0 & (~MM_MOUSE_KEY0_MOUSE);

    if (nCompositKey & MM_MOUSE_KEY0_CTRL)
    {
        if( !strText.isEmpty() )
            strText += "+";
        strText += compositeKeyToString( MM_MOUSE_KEY0_CTRL );
    }

    if (nCompositKey & MM_MOUSE_KEY0_ALT)
    {
        if( !strText.isEmpty() )
            strText += "+";
        strText += compositeKeyToString( MM_MOUSE_KEY0_ALT );
    }

    if (nCompositKey & MM_MOUSE_KEY0_SHIFT)
    {
        if( !strText.isEmpty() )
            strText += "+";
        strText += compositeKeyToString( MM_MOUSE_KEY0_SHIFT );
    }

    if (nCompositKey & MM_MOUSE_KEY0_WINDOW)
    {
        if( !strText.isEmpty() )
            strText += "+";
        strText += compositeKeyToString( MM_MOUSE_KEY0_WINDOW );
    }

    if (strMouseKey.isEmpty() && findScanCode(cV1, nScanCode))
    {
        QString strK;
        ulong nVKey = scanCodeToVirtualKey(nScanCode);

        strK = virtualKeyToString( nVKey );

        if (!strK.isEmpty())
        {
            if (!strText.isEmpty())
                strText += "+";
            strText += strK;
        }
    }

    if (!strMouseKey.isEmpty())
    {
        if (!strText.isEmpty())
            strText += "+";

        strText += strMouseKey;
    }
    return strText;
}
