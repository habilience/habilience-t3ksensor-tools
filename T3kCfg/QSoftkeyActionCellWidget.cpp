#include "stdInclude.h"
#include "QSoftkeyActionCellWidget.h"

#include "QKeyMapStr.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QtEvents>
#include <QPainter>
#include <QApplication>


QSoftkeyActionCellWidget::QSoftkeyActionCellWidget(QWidget *parent, int nRowIndex, int nColumnIndex) :
    QLabel(parent), m_nRowIndex(nRowIndex), m_nColumnIndex(nColumnIndex)
{
    m_bFocusIn = false;
    m_nTimer = 0;
    m_bHover = false;
    m_bShowWidget = false;

    m_bLoad = false;

    setFocusPolicy( Qt::NoFocus );
    setAttribute( Qt::WA_Hover );

    m_cbActionWidget.setParent( this );
    m_cbActionWidget.hide();
    m_cbActionWidget.SetUseHover( true );
    m_editKeyActionWidget.setParent( this );
    m_editKeyActionWidget.hide();

    if( m_nRowIndex%2 == 1 )
        m_cbActionWidget.SetDrawBackColor( true );

    m_cbActionWidget.setCursor( Qt::PointingHandCursor );

    connect( &m_cbActionWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(on_Softkey_Action_Changed(int)) );
    connect( &m_cbActionWidget, SIGNAL(activated(int)), this, SLOT(on_Softkey_Action_Activated(int)) );
    connect( &m_cbActionWidget, SIGNAL(FocusInCB()), this, SLOT(on_Child_FocusIn()) );
    connect( &m_cbActionWidget, SIGNAL(HidePopupSignal()), this, SLOT(on_m_cbKeyActionWidget_hidePopup()) );

    connect( &m_editKeyActionWidget, SIGNAL(KeyPressSignal(ushort)), this, SLOT(on_Softkey_Press(ushort)) );
    connect( &m_editKeyActionWidget, SIGNAL(MousekeyPress(ushort)), this, SLOT(on_Softkey_MousePress(ushort)) );
    connect( &m_editKeyActionWidget, SIGNAL(FuncKey_ShowPopup_S()), this ,SLOT(on_Child_FocusIn()) );
    connect( &m_editKeyActionWidget, SIGNAL(FuncKey_HidePopup_S()), this ,SLOT(on_m_cbKeyActionWidget_hidePopup()) );
    connect( &m_editKeyActionWidget, SIGNAL(FuncKey_FocusIn_S()), this ,SLOT(on_Child_FocusIn()) );
    connect( &m_editKeyActionWidget, SIGNAL(FuncKey_activated(int)), this, SLOT(on_m_cbFuncKeyWidget_activated(int)) );
    connect( &m_editKeyActionWidget, SIGNAL(Mouse_activated(int)), this, SLOT(on_m_cbMouse_activated(int)) );
    connect( &m_editKeyActionWidget, SIGNAL(FocusOutKeyEdit()), this, SLOT(on_Edit_FocusOut()) );

    installEventFilter( this );
    m_editKeyActionWidget.installEventFilter( this );

    onChangeLanguage();
}

void QSoftkeyActionCellWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    m_cbActionWidget.clear();

    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DISABLED") ), 0x0000 );

    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ), 0xFFFF );
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FUNCTION_KEY") ), 0xFFFF );
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MOUSE") ), 0xFFFF );

    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_ESCAPE") ), 0x0029 );

#if defined(Q_OS_MAC)
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH1") ), 0x082B );
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_EXPLORER") ), 0x0811 );
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DESKTOP") ), 0x0044 );
#else
#if defined(Q_OS_WIN)
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH2") ), 0x092B );
#endif
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH1") ), 0x052B );
#if defined(Q_OS_WIN)
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_MANAGER") ), 0x0329 );
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_EXPLORER") ), 0x0808 );
#endif
    m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DESKTOP") ), 0x0807 );
#endif



#ifdef Q_OS_WIN
    wchar_t wszATM[MAX_PATH];
    ZeroMemory( wszATM, sizeof(wchar_t)*MAX_PATH );
    QString str("T3kCfg");
    str.toWCharArray( wszATM );

    ATOM atm = GlobalAddAtom( wszATM );
    Q_ASSERT( atm );
    if( !RegisterHotKey( NULL, atm, MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_F10 ) )
        m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_VKBOARD") ), 0x0743 );
    else
        UnregisterHotKey( NULL, atm );

    if( !RegisterHotKey( NULL, atm, MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_F12 ) )
        m_cbActionWidget.addItem( Res.getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_T3KCFG") ), 0x0745 );
    else
        UnregisterHotKey( NULL, atm );

    atm = GlobalDeleteAtom( atm );
    Q_ASSERT( !atm );
#endif

    ResizeWidget( size() );
}

void QSoftkeyActionCellWidget::paintEvent(QPaintEvent *evt)
{
    QPainter dc;
    dc.begin( this );
    QRect rc( 0, 0, width()-1, height()-1 );

    dc.fillRect( rc, Qt::white );

    if( m_bHover )
    {
        dc.fillRect( QRect(rc.left(),rc.top(),rc.width(),rc.height()/2), QColor(228,244,252) );
        dc.fillRect( QRect(rc.left(),rc.top()+rc.height()/2,rc.width(),rc.height()/2), QColor(180,230,250) );
        dc.setPen( QColor(60,130,180) );
        dc.drawRoundedRect( rc, 2., 2. );
    }
    else
    {
        int nIndex = m_cbActionWidget.currentIndex();
        if( nIndex > 0 && nIndex < 4 )
        {
            if( m_nRowIndex%2 == 1 )
            {
                rc.setHeight( rc.height()+1 );
                dc.fillRect( rc, QColor(219,228,242) );
            }
        }
    }

    if( !m_bShowWidget )
        setText( " " + GetText() );

    dc.end();
    QLabel::paintEvent(evt);
}

void QSoftkeyActionCellWidget::resizeEvent(QResizeEvent *evt)
{
    QSize szNewSize( evt->size() );

    ResizeWidget( szNewSize );

    QLabel::resizeEvent(evt);
}

void QSoftkeyActionCellWidget::ResizeWidget(QSize szNewSize)
{
    int nKeyComboW = szNewSize.width();

    int nIdx = GetTypeIndex();
    int nOffset = szNewSize.width()/3;
    if( nIdx > 0 && nIdx < 4  )
    {
        nKeyComboW = nOffset;
    }
    else
    {
        nKeyComboW = szNewSize.width();
    }
    bool bR2L = QLangManager::instance()->getResource().isR2L();
    m_cbActionWidget.setGeometry( bR2L ? szNewSize.width()-nKeyComboW : 0, 0, nKeyComboW, szNewSize.height() );
    m_editKeyActionWidget.setGeometry( bR2L ? 0 : nOffset+1, 0, szNewSize.width()-nOffset-1, szNewSize.height() );

    qDebug( "Resize Item -> %d:%d-%d:%d", m_nRowIndex, m_nColumnIndex, szNewSize.width(), szNewSize.height() );
}

void QSoftkeyActionCellWidget::timerEvent(QTimerEvent *evt)
{
    if( m_nTimer == evt->timerId() )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;

        if( !m_bFocusIn )
        {
            int nIndex = m_cbActionWidget.currentIndex();
            if( (nIndex >0 && nIndex < 4) && m_editKeyActionWidget.GetKeyValue() == 0x0000 )
            {
                m_cbActionWidget.setCurrentIndex( 0 );
                m_cbActionWidget.show();
            }
            else
            {
                if( nIndex >0 && nIndex < 4 )
                    m_cbActionWidget.hide();
                m_editKeyActionWidget.hide();
            }
            m_bFocusIn = false;
            m_bShowWidget = false;
            m_bHover = false;
            update();
        }
    }

    QLabel::timerEvent(evt);
}

bool QSoftkeyActionCellWidget::eventFilter(QObject *target, QEvent *evt)
{
    if( isVisible() && target == this )
    {
        if( evt->type() == QEvent::HoverEnter )
        {
            setCursor( Qt::PointingHandCursor );
            m_bHover = true;
            update();
        }

        if( evt->type() == QEvent::HoverLeave )
        {
            setCursor( Qt::ArrowCursor );
            m_bHover = false;
            update();
        }
    }

    if( target == &m_editKeyActionWidget )
    {
        if( evt->type() == QEvent::FocusIn )
        {
            m_bFocusIn = true;
        }
    }

    return QLabel::eventFilter(target,evt);
}

void QSoftkeyActionCellWidget::on_Clicked_Show( int nIndex )
{
    if( nIndex < 0 || nIndex >= m_cbActionWidget.count() ) return;

    switch( nIndex )
    {
    case 0:
    default:
        m_cbActionWidget.show();
        m_editKeyActionWidget.hide();
        break;
    case 1:
        m_cbActionWidget.show();
        m_editKeyActionWidget.SetWidgetInputMode( PIM_KEY );
        m_editKeyActionWidget.show();
        break;
    case 2:
        m_cbActionWidget.show();
        m_editKeyActionWidget.SetWidgetInputMode( PIM_FUNC );
        m_editKeyActionWidget.show();
        break;
    case 3:
        m_cbActionWidget.show();
        m_editKeyActionWidget.SetWidgetInputMode( PIM_MOUSE );
        m_editKeyActionWidget.show();
        break;
    }

    m_bShowWidget = true;
    update();

    m_editKeyActionWidget.setFocus();
}

void QSoftkeyActionCellWidget::on_Softkey_Press(ushort nValue)
{
    KeyPressSignal( this, nValue );
    ItemChanged( this, 1 );
}

void QSoftkeyActionCellWidget::on_Softkey_MousePress(ushort nValue)
{
    MousekeyPress( this, nValue );
    ItemChanged( this, 2 );
}

void QSoftkeyActionCellWidget::on_Softkey_Action_Changed(int nIndex)
{
    ChangeWidgetMode( nIndex, (nIndex >= 0 && nIndex <= 3) ? true : false );
}

void QSoftkeyActionCellWidget::on_Edit_FocusOut()
{
    if( !m_nTimer )
    {
        m_bFocusIn = false;
        m_nTimer = startTimer( 1 );
    }
}

void QSoftkeyActionCellWidget::on_Softkey_Action_Activated(int nIndex)
{
    m_bFocusIn = true;
    if( nIndex < 0 || nIndex >= m_cbActionWidget.count() ) return;

    if( !m_bLoad )
    {
        m_bLoad = true;
        ResizeWidget( QSize(width(),height()) );
    }

    m_editKeyActionWidget.Reset();
    m_editKeyActionWidget.SetKeyValue( 0x0000 );

    if( nIndex == 0 || nIndex > 3 )
    {
        m_cbActionWidget.show();
        m_editKeyActionWidget.hide();
        KeyPressSignal( this, m_cbActionWidget.itemData( nIndex ).toUInt() );
    }
    else
    {
        m_cbActionWidget.show();
        m_editKeyActionWidget.show();
        ChangeWidgetMode( nIndex, true );

        if( nIndex == 2 )
        {
            //on_m_cbFuncKeyWidget_activated( 0 );
            m_editKeyActionWidget.SetFuncKeyCurrentIndex( -1 );
            KeyPressSignal( this, m_editKeyActionWidget.GetKeyValue() );
        }
        else if( nIndex == 3 )
        {
            on_m_cbMouse_activated( 0 );
        }

        ItemChanged( this, nIndex );
    }

    m_editKeyActionWidget.setFocus();
}

void QSoftkeyActionCellWidget::on_m_cbFuncKeyWidget_activated(int nIndex)
{
    if( nIndex < 0 ) return;
    uchar cKey = 0;
    if( findUsageId( virtualKeyToScanCode( Qt::Key_F1+nIndex ), cKey ) )
    {
        m_editKeyActionWidget.SetKeyValue( cKey );
        KeyPressSignal( this, m_editKeyActionWidget.GetKeyValue() );
    }
    m_bFocusIn = true;
}

void QSoftkeyActionCellWidget::on_m_cbMouse_activated(int nIndex)
{
    if( nIndex < 0 ) return;

    m_editKeyActionWidget.SetMouseValue( 0x0001 << nIndex );
    MousekeyPress( this, m_editKeyActionWidget.GetKeyValue() );

    m_bFocusIn = true;
}

void QSoftkeyActionCellWidget::on_m_cbKeyActionWidget_hidePopup()
{
    m_editKeyActionWidget.setFocus();
}

void QSoftkeyActionCellWidget::on_Child_FocusIn()
{
    m_bFocusIn = true;
}

ushort QSoftkeyActionCellWidget::GetVaule()
{
    return m_editKeyActionWidget.GetKeyValue();
}

int QSoftkeyActionCellWidget::SetKeyAction(const char cConditionKey, const char cKey)
{
    ushort nValue = ((ushort)cConditionKey << 8) | (ushort)cKey;

    m_editKeyActionWidget.Reset();
    m_editKeyActionWidget.SetKeyValue( 0x0000 );

    bool bFound = false;
    int nI = 0;

    if( nValue == 0x0000 )
    {
        switch( m_editKeyActionWidget.GetWidgetInputMode() )
        {
        case PIM_KEY:
            m_cbActionWidget.setCurrentIndex( 1 );
            nI = 1;
            break;
        case PIM_FUNC:
            m_cbActionWidget.setCurrentIndex( 2 );
            nI = 2;
            break;
        case PIM_MOUSE:
            Q_ASSERT(false);
            break;
        }

        if( !m_bLoad )
        {
            m_bLoad = true;
            ResizeWidget( QSize(width(),height()) );
        }

        return nI;
    }

    for( nI = 1 ; nI < m_cbActionWidget.count() ; nI ++ )
    {
        ushort wItemValue = (ushort)m_cbActionWidget.itemData(nI).toInt();
        if( wItemValue == nValue )
        {
            m_cbActionWidget.setCurrentIndex( nI );
            m_editKeyActionWidget.SetKeyValue( wItemValue );
            bFound = true;
            break;
        }
    }

    if( !bFound )
    {
        bool bFunctionKey = false;
        int nVk = 0;
        if( !(nValue >> 8 & 0x80) )
        {
            int nScanCode;
            if( findScanCode( nValue & 0xFF, nScanCode ) )
            {
                nVk = scanCodeToVirtualKey( nScanCode );
                if( nVk >= Qt::Key_F1 && nVk <= Qt::Key_F24 )
                {
                    bFunctionKey = true;
                }
            }
        }
        if( bFunctionKey )
        {
            //m_editKeyActionWidget.Reset();
            m_editKeyActionWidget.SetKeyValue( nValue );
            int nFuncIndex = nVk-Qt::Key_F1;
            if( nFuncIndex >= 0 )
                m_editKeyActionWidget.SetFuncKeyCurrentIndex( nFuncIndex );

            m_cbActionWidget.setCurrentIndex( 2 ); /// !!! Check

            nI = 2;
        }
        else
        {
            //m_editKeyActionWidget.Reset();
            m_editKeyActionWidget.SetKeyValue( nValue );

            m_cbActionWidget.setCurrentIndex( 1 );

            nI = 1;
        }
    }

    if( !m_bLoad )
    {
        m_bLoad = true;
        ResizeWidget( QSize(width(),height()) );
    }

    return nI;
}

void QSoftkeyActionCellWidget::SetMouseAction(const char cConditionKey, const uchar cMouse)
{
    m_editKeyActionWidget.Reset();
    m_editKeyActionWidget.SetMouseValue( ((ushort)cConditionKey << 8) | (ushort)cMouse );

    m_cbActionWidget.setCurrentIndex( 3 );

    if( !m_bLoad )
    {
        m_bLoad = true;
        ResizeWidget( QSize(width(),height()) );
    }
}

bool QSoftkeyActionCellWidget::IsMouseType()
{
    return m_cbActionWidget.currentIndex() == 3 ? true : false;
}

void QSoftkeyActionCellWidget::ChangeWidgetMode(int nIndex, bool bShowKeyCombo)
{
    switch( nIndex )
    {
    case 1: // User Defined Key
        {
            ResizeWidget( size() );
            m_cbActionWidget.SetUseHover( false );
            m_editKeyActionWidget.show();
            m_editKeyActionWidget.SetWidgetInputMode( PIM_KEY );
            if( bShowKeyCombo )
            {
                m_cbActionWidget.show();
                m_bShowWidget = false;
            }
            else
            {
                m_cbActionWidget.hide();
            }
        }
        break;

    case 2: // FunctionKey
        {
            ResizeWidget( size() );
            m_cbActionWidget.SetUseHover( false );
            m_editKeyActionWidget.show();
            m_editKeyActionWidget.SetWidgetInputMode( PIM_FUNC );
            if( bShowKeyCombo )
            {
                m_cbActionWidget.show();
                m_bShowWidget = false;
            }
            else
            {
                m_cbActionWidget.hide();
            }

            m_editKeyActionWidget.setFocus();
        }
        break;

    case 3: // Mouse
        {
            ResizeWidget( size() );
            m_cbActionWidget.SetUseHover( false );
            m_editKeyActionWidget.show();
            m_editKeyActionWidget.SetWidgetInputMode( PIM_MOUSE );
            if( bShowKeyCombo )
            {
                m_cbActionWidget.show();
                m_bShowWidget = false;
            }
            else
            {
                m_cbActionWidget.hide();
            }

            m_editKeyActionWidget.setFocus();
        }
        break;
    case 0:
    default: // None
        m_cbActionWidget.setGeometry( 0, 0, width(), height() );
        m_cbActionWidget.SetUseHover( true );
        m_editKeyActionWidget.hide();
        if( nIndex >= 0 )
        {
            m_cbActionWidget.show();
            m_bShowWidget = false;
        }
        else
        {
            m_cbActionWidget.hide();
        }
        break;
    }

    m_bHover = false;
    update();
}

QString QSoftkeyActionCellWidget::GetText()
{
    int nIndex = m_cbActionWidget.currentIndex();
    if( nIndex == 0 || nIndex == 1 || nIndex == 2 )
        return m_editKeyActionWidget.text();
    else if( nIndex == 3 )
        return m_editKeyActionWidget.text();
    else if( nIndex > 1 && nIndex < m_cbActionWidget.count() )
        return m_cbActionWidget.itemText( nIndex );

    return "";
}

void QSoftkeyActionCellWidget::SetWidgetMode(uint nI)
{
    switch( nI )
    {
    case 0:
        on_Softkey_Action_Changed( 0 );
        m_cbActionWidget.setCurrentIndex( 0 );
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        break;
    }
}
