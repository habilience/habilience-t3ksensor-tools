#include "QKeyEditWidget.h"

#include "KeyMapStr.h"
#include <QKeyEvent>

#include <QCoreApplication>
#include <QLocale>

#include "QLangManager.h"

QKeyEditWidget::QKeyEditWidget(QWidget *parent) :
    QLineEdit(parent)
{
    m_wKeyValue = 0x00;
    m_bNotInput = false;
    installEventFilter( this );
    setAlignment( Qt::AlignVCenter );

    setInputMethodHints( Qt::ImhDigitsOnly );
}

void QKeyEditWidget::SetNotKeyInputMode(bool bNotInput)
{
    m_bNotInput = bNotInput;
    setReadOnly( bNotInput );
}

void QKeyEditWidget::Reset()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NONE") ) );
}

void QKeyEditWidget::SetKeyValue( ushort wKeyValue )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    uchar cV0 = (uchar)(wKeyValue >> 8);
    uchar cV1 = (uchar)(wKeyValue & 0xFF);

    if( cV0 & MM_MOUSE_KEY0_MOUSE )
    {
        setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NONE") ) );
        return;
    }

    m_wKeyValue = wKeyValue;

    QString strKey;
    if( cV0 & MM_MOUSE_KEY0_CTRL )
    {
        if( !strKey.isEmpty() )
            strKey += "+";
        strKey += "Ctrl";
    }

    if( cV0 & MM_MOUSE_KEY0_ALT )
    {
        if( !strKey.isEmpty() )
            strKey += "+";
        strKey += "Alt";
    }

    if( cV0 & MM_MOUSE_KEY0_SHIFT )
    {
        if( !strKey.isEmpty() )
            strKey += "+";
        strKey += "Shift";
    }

    if( cV0 & MM_MOUSE_KEY0_WINDOW )
    {
        if( !strKey.isEmpty() )
            strKey += "+";
#ifdef Q_OS_MAC
        strKey += "Cmd";
#else
        strKey += "Win";
#endif
    }

    if( !strKey.isEmpty() )
        strKey += "+";

    int nScanCode;
    if( FindScanCode(cV1, nScanCode) )
    {
        int nVKey = FromMapVirtualKey(nScanCode);

        QString strK;
        strK = VirtualKeyToString( nVKey );

        strKey += strK;
    }

    if( strKey.isEmpty() )
        Reset();
    else
        setText( strKey );
}

bool QKeyEditWidget::eventFilter(QObject *target, QEvent *evt)
{
    if( target == this )
    {
        if( evt->type() == QEvent::KeyPress && !m_bNotInput )
        {
            QKeyEvent* pEvt = (QKeyEvent*)evt;
            if( pEvt->key() == Qt::Key_Tab )
            {
                emit keyPressEvent(pEvt);
                evt->accept();
                return true;
            }

            if( pEvt->key() == Qt::Key_Return )
            {
                emit keyPressEvent(pEvt);
                evt->accept();
                return true;
            }
        }
    }

    return QLineEdit::eventFilter(target, evt);
}

void QKeyEditWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QKeyEvent::KeyPress && isVisible() && !m_bNotInput )
    {
        QString strKey;
        uchar cV0 = (uchar)(m_wKeyValue >> 8);
        uchar cV1;
        if( evt->key() == Qt::Key_Control )
        {
#ifdef Q_OS_MAC
            cV0 ^= MM_MOUSE_KEY0_WINDOW;
#else
            cV0 ^= MM_MOUSE_KEY0_CTRL;
#endif
        }

        if( evt->key() == Qt::Key_Alt )
        {
            cV0 ^= MM_MOUSE_KEY0_ALT;
        }

        if( evt->key() == Qt::Key_Shift )
        {
            cV0 ^= MM_MOUSE_KEY0_SHIFT;
        }

        if( evt->key() == Qt::Key_Meta )
        {
#ifdef Q_OS_MAC
            cV0 ^= MM_MOUSE_KEY0_CTRL;
#else
            cV0 ^= MM_MOUSE_KEY0_WINDOW;
#endif
        }

        if( cV0 & MM_MOUSE_KEY0_CTRL )
        {
            if( !strKey.isEmpty() )
                strKey += "+";
            strKey += "Ctrl";
        }

        if( cV0 & MM_MOUSE_KEY0_ALT )
        {
            if( !strKey.isEmpty() )
                strKey += "+";
            strKey += "Alt";
        }

        if( cV0 & MM_MOUSE_KEY0_SHIFT )
        {
            if( !strKey.isEmpty() )
                strKey += "+";
            strKey += "Shift";
        }

        if( cV0 & MM_MOUSE_KEY0_WINDOW )
        {
            if( !strKey.isEmpty() )
                strKey += "+";
#ifdef Q_OS_MAC
            strKey += "Cmd";
#else
            strKey += "Win";
#endif
        }

        if( !strKey.isEmpty() )
            strKey += "+";

        int nVKey = evt->key();
        qDebug( "VKEY: %d\r\n", nVKey );
        int nScanCode = FindScanCodeFromVK(nVKey);

        if( nVKey == Qt::Key_Hangul )
        {
            cV1 = 0x90;
        }
        else if( nVKey == Qt::Key_Hangul_Hanja )
        {
            cV1 = 0x91;
        }
        else
        {
            if( !FindUsageId( nScanCode, cV1 ) )
            {
                cV1 = 0x00;
            }
        }

        switch( nScanCode )
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
        strK = VirtualKeyToString( nVKey );
        if( strK.isEmpty() )
        {
            cV1 = 0x00;
        }

        m_wKeyValue = cV0 << 8 | cV1;
        qDebug( "%04X\r\n", m_wKeyValue );

        strKey += strK;

        if( strKey.isEmpty() || m_wKeyValue == 0x0000 )
        {
            KeyPressSignal( 0x0000 );
            setText( QLangManager::GetPtr()->GetResource().GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NONE") ) );
        }
        else
        {
            KeyPressSignal( m_wKeyValue );
            setText( strKey );
        }
        evt->accept();
        return;
    }

    QLineEdit::keyPressEvent(evt);
}

