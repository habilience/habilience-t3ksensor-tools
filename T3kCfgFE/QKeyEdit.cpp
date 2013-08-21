#include "QKeyEdit.h"

#include "QKeyMapStr.h"
#include "QLangManager.h"
#include <QKeyEvent>

#define RES_TAG "EDIT PROFILE ITEM"

QKeyEdit::QKeyEdit(QWidget *parent) :
    QLineEdit(parent)
{
    m_wKeyValue = 0x00;
    setAlignment( Qt::AlignVCenter );
    setInputMethodHints(Qt::ImhLatinOnly);

    setToNone();
}

void QKeyEdit::reset()
{
    setToNone();
}

void QKeyEdit::setToNone()
{
    QLangRes& res = QLangManager::getResource();
    setText( res.getResString(RES_TAG, "TEXT_NONE") );
}

void QKeyEdit::setKeyValue( unsigned short wKeyValue )
{
    unsigned char cV0 = (unsigned char)(wKeyValue >> 8);
    unsigned char cV1 = (unsigned char)(wKeyValue & 0xFF);

    /*
    if (cV0 & MM_MOUSE_KEY0_MOUSE)
    {
        setToNone();
        return;
    }
    */

    m_wKeyValue = wKeyValue;

    QString strKey;
    if (cV0 & MM_MOUSE_KEY0_CTRL)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Ctrl";
    }

    if (cV0 & MM_MOUSE_KEY0_ALT)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Alt";
    }

    if (cV0 & MM_MOUSE_KEY0_SHIFT)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Shift";
    }

    if (cV0 & MM_MOUSE_KEY0_WINDOW)
    {
        if (!strKey.isEmpty())
            strKey += "+";
#ifdef Q_OS_MAC
        strKey += "Cmd";
#else
        strKey += "Win";
#endif
    }

    int nScanCode;
    if (findScanCode(cV1, nScanCode))
    {
        int nVKey = scanCodeToVirtualKey(nScanCode);

        QString strK;
        strK = virtualKeyToString( nVKey );
        if (!strK.isEmpty())
        {
            if (!strKey.isEmpty())
                strKey += "+";
            strKey += strK;
        }
    }

    if (strKey.isEmpty())
        setToNone();
    else
        setText( strKey );
}

bool QKeyEdit::eventFilter(QObject *obj, QEvent *evt)
{
    return QLineEdit::eventFilter(obj, evt);
}

void QKeyEdit::keyPressEvent(QKeyEvent *evt)
{
    QString strKey;
    unsigned char cV0 = (unsigned char)(m_wKeyValue >> 8);
    unsigned char cV1;

    if (evt->key() == Qt::Key_Control)
    {
#ifdef Q_OS_MAC
        cV0 ^= MM_MOUSE_KEY0_WINDOW;
#else
        cV0 ^= MM_MOUSE_KEY0_CTRL;
#endif
    }

    if (evt->key() == Qt::Key_Alt)
    {
        cV0 ^= MM_MOUSE_KEY0_ALT;
    }

    if (evt->key() == Qt::Key_Shift)
    {
        cV0 ^= MM_MOUSE_KEY0_SHIFT;
    }

    if (evt->key() == Qt::Key_Meta)
    {
#ifdef Q_OS_MAC
        cV0 ^= MM_MOUSE_KEY0_CTRL;
#else
        cV0 ^= MM_MOUSE_KEY0_WINDOW;
#endif
    }

    if (cV0 & MM_MOUSE_KEY0_CTRL)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Ctrl";
    }

    if (cV0 & MM_MOUSE_KEY0_ALT)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Alt";
    }

    if (cV0 & MM_MOUSE_KEY0_SHIFT)
    {
        if (!strKey.isEmpty())
            strKey += "+";
        strKey += "Shift";
    }

    if (cV0 & MM_MOUSE_KEY0_WINDOW)
    {
        if (!strKey.isEmpty())
            strKey += "+";
#ifdef Q_OS_MAC
        strKey += "Cmd";
#else
        strKey += "Win";
#endif
    }

    if (!strKey.isEmpty())
        strKey += "+";

    int nVKey = evt->key();
    qDebug( "VKEY: %d", nVKey );
    int nScanCode = virtualKeyToScanCode(nVKey);

    if (nVKey == Qt::Key_Hangul)
    {
        cV1 = 0x90;
    }
    else if (nVKey == Qt::Key_Hangul_Hanja)
    {
        cV1 = 0x91;
    }
    else
    {
        if (!findUsageId(nScanCode, cV1))
        {
            cV1 = 0x00;
        }
    }

    switch (nScanCode)
    {
    case 0x52:	// Insert
        cV1 = 0x49;
        break;
    case 0x47:	// Home
        cV1 = 0x4A;
        break;
    case 0x49:	// Page Up
        cV1 = 0x4B;
        break;
    case 0x53:	// Delete
        cV1 = 0x4C;
        break;
    case 0x4F:	// End
        cV1 = 0x4D;
        break;
    case 0x51:	// Page Down
        cV1 = 0x4E;
        break;
    case 0x4D:	// Right Arrow
        cV1 = 0x4F;
        break;
    case 0x4B:	// Left Arrow
        cV1 = 0x50;
        break;
    case 0x50:	// Down Arrow
        cV1 = 0x51;
        break;
    case 0x48:	// Up Arrow
        cV1 = 0x52;
        break;
    }


    QString strK;
    strK = virtualKeyToString( nVKey );
    if (strK.isEmpty())
    {
        cV1 = 0x00;
    }

    m_wKeyValue = (cV0 << 8) | cV1;
    qDebug( "%04X", m_wKeyValue );

    strKey += strK;

    if (strKey.isEmpty() || m_wKeyValue == 0x0000 )
    {
        setToNone();
        emit keyEditValueChanged( this, 0x0000 );
    }
    else
    {
        setText( strKey );
        emit keyEditValueChanged( this, m_wKeyValue );
    }

    evt->accept();
}
