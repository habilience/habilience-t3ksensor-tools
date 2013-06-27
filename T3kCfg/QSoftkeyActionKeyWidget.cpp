#include "QSoftkeyActionKeyWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>

#include "stdInclude.h"
#include "T3kPacketDef.h"
#include "T3kSoftkeyDef.h"

#define CTRL                    0
#define ALT			1
#define SHIFT                   2
#define WIN			3
#define CLEAR                   4

#define UP			0
#define DOWN                    1

QSoftkeyActionKeyWidget::QSoftkeyActionKeyWidget(QWidget *parent) :
    QLabel(parent), m_KeyEditWidget(this), m_cbFuncKeyWidget(this, false), m_cbMouseWidget(this, false)
{
    m_pIconBtn[0][0] = m_pIconBtn[0][1] =
    m_pIconBtn[1][0] = m_pIconBtn[1][1] =
    m_pIconBtn[2][0] = m_pIconBtn[2][1] =
    m_pIconBtn[3][0] = m_pIconBtn[3][1] =
    m_pIconBtn[4][0] = m_pIconBtn[4][1] = NULL;

    m_wIconKeyValue[CTRL] = MM_MOUSE_KEY0_CTRL;
    m_wIconKeyValue[ALT] = MM_MOUSE_KEY0_ALT;
    m_wIconKeyValue[SHIFT] = MM_MOUSE_KEY0_SHIFT;
    m_wIconKeyValue[WIN] = MM_MOUSE_KEY0_WINDOW;
    m_wIconKeyValue[CLEAR] = 0;

    m_wKeyValue = 0x00;

    m_eInputMode = PIM_KEY;

    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    setAttribute( Qt::WA_Hover );

    installEventFilter( this );
    m_KeyEditWidget.installEventFilter( this );

    connect( &m_KeyEditWidget, SIGNAL(KeyPressSignal(ushort)), this, SLOT(onKeyPress_Signal(ushort)) );

    connect( &m_cbFuncKeyWidget, SIGNAL(ShowPopupSignal()), this, SLOT(on_FuncKey_ShowPopup()) );
    connect( &m_cbFuncKeyWidget, SIGNAL(HidePopupSignal()), this, SLOT(on_FuncKey_HidePopup()) );
    connect( &m_cbFuncKeyWidget, SIGNAL(FocusInSignal()), this, SLOT(on_FuncKey_FocusIn()) );
    connect( &m_cbFuncKeyWidget, SIGNAL(FocusOutSignal()), this, SLOT(on_FuncKey_FocusOut()) );
    connect( &m_cbFuncKeyWidget, SIGNAL(activated(int)), this, SLOT(on_FuncKey_activated(int)) );

    connect( &m_cbMouseWidget, SIGNAL(ShowPopupSignal()), this, SLOT(on_FuncKey_ShowPopup()) );
    connect( &m_cbMouseWidget, SIGNAL(HidePopupSignal()), this, SLOT(on_FuncKey_HidePopup()) );
    connect( &m_cbMouseWidget, SIGNAL(FocusInSignal()), this, SLOT(on_FuncKey_FocusIn()) );
    connect( &m_cbMouseWidget, SIGNAL(FocusOutSignal()), this, SLOT(on_FuncKey_FocusOut()) );
    connect( &m_cbMouseWidget, SIGNAL(activated(int)), this, SLOT(on_Mouse_activated(int)) );

    Init();
}

QSoftkeyActionKeyWidget::~QSoftkeyActionKeyWidget()
{
    for( int i=0 ; i<QICON_COUNT ; i++ )
    {
        if( m_pIconBtn[i][0] != NULL )
            delete m_pIconBtn[i][0];
        if( m_pIconBtn[i][1] != NULL )
            delete m_pIconBtn[i][1];
    }
}

void QSoftkeyActionKeyWidget::Init()
{
    m_pIconBtn[CTRL][UP]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CTRL_U" );
    m_pIconBtn[CTRL][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CTRL_S" );
    m_pIconBtn[ALT][UP]                 = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_ALT_U" );
    m_pIconBtn[ALT][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_ALT_S" );
    m_pIconBtn[SHIFT][UP]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_SHIFT_U" );
    m_pIconBtn[SHIFT][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_SHIFT_S" );
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    m_pIconBtn[WIN][UP]                 = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_WIN_U" );
    m_pIconBtn[WIN][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_WIN_S" );
#elif defined(Q_OS_MAC)
    m_pIconBtn[WIN][UP]                 = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_MAC_U" );
    m_pIconBtn[WIN][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_MAC_S" );
#endif
    m_pIconBtn[CLEAR][UP]               = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CLEAR_U" );
    m_pIconBtn[CLEAR][DOWN]             = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CLEAR_S" );

    m_nIconTotalWidth = 0;
    m_nIconTotalWidth += m_pIconBtn[CTRL][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[ALT][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[SHIFT][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[WIN][UP]->width();

    m_nIconTotalWidth += QICON_COUNT;

    m_cbFuncKeyWidget.setCursor( Qt::PointingHandCursor );
    m_cbFuncKeyWidget.setFocusPolicy( Qt::ClickFocus );
	
    m_cbMouseWidget.setCursor( Qt::PointingHandCursor );
    m_cbMouseWidget.setFocusPolicy( Qt::ClickFocus );

    SetRealGeometry();

    for( int i=1 ; i<=24 ; i++ )
        m_cbFuncKeyWidget.addItem( QString("F%1").arg(i) );

    m_cbFuncKeyWidget.SetZeroSet( true );
    m_cbFuncKeyWidget.setCurrentIndex( -1 );
    m_cbFuncKeyWidget.hide();

    OnChangeLanguage();

    m_KeyEditWidget.Reset();
}

void QSoftkeyActionKeyWidget::setFocus()
{
    m_KeyEditWidget.setFocus();
}

QString QSoftkeyActionKeyWidget::text()
{
    if( m_eInputMode == PIM_MOUSE )
    {
        QLangRes& Res = QLangManager::GetPtr()->GetResource();

        QString str( m_KeyEditWidget.text() );
        if( (m_wKeyValue & 0xFF00) == 0x0000 )
            return m_cbMouseWidget.itemText( m_cbMouseWidget.currentIndex() ) +
                    Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MOUSE") );
        else
            return str + m_cbMouseWidget.itemText( m_cbMouseWidget.currentIndex() ) +
                    Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MOUSE") );
    }
    return m_KeyEditWidget.text();
}

int QSoftkeyActionKeyWidget::GetFuncKeyCurrentIndex()
{
    return m_cbFuncKeyWidget.currentIndex();
}

void QSoftkeyActionKeyWidget::SetFuncKeyCurrentIndex(int nIndex)
{
    m_cbFuncKeyWidget.setCurrentIndex( nIndex );
}

void QSoftkeyActionKeyWidget::SetWidgetInputMode(PartInputMode eMode)
{
    m_eInputMode = eMode;
    bool bKeyMode = false;
    switch( m_eInputMode )
    {
    case PIM_KEY:
        m_cbFuncKeyWidget.hide();
        m_cbMouseWidget.hide();
        bKeyMode = true;
        break;
    case PIM_FUNC:
        m_cbMouseWidget.hide();
        m_cbFuncKeyWidget.setVisible( true );
        m_cbFuncKeyWidget.show();
        break;
    case PIM_MOUSE:
        m_cbFuncKeyWidget.hide();
        m_cbMouseWidget.show();
        break;
    default:
        break;
    }

    m_KeyEditWidget.SetNotKeyInputMode( !bKeyMode );
}

void QSoftkeyActionKeyWidget::SetRealGeometry()
{
    bool bR2L = QLangManager::GetPtr()->GetResource().IsR2L();
    QRect rc( bR2L ? m_pIconBtn[CLEAR][UP]->width()+4 : m_nIconTotalWidth, 0, width()-m_nIconTotalWidth-m_pIconBtn[CLEAR][UP]->width(), height() );
    m_KeyEditWidget.setGeometry( rc );
    m_cbFuncKeyWidget.setGeometry( rc );
    m_cbMouseWidget.setGeometry( bR2L ? 0 : rc.left(), rc.top(), rc.width()+m_pIconBtn[CLEAR][UP]->width(), rc.height() );
}

void QSoftkeyActionKeyWidget::Reset()
{
    m_cbMouseWidget.setCurrentIndex( 0 );
    m_cbFuncKeyWidget.setCurrentIndex( -1 );
    m_KeyEditWidget.SetKeyValue( 0x0000 );
    m_KeyEditWidget.Reset();
    m_wKeyValue = 0x0000;
    update();
}

void QSoftkeyActionKeyWidget::SetKeyValue( ushort wKeyValue )
{
    switch( m_eInputMode )
    {
    case PIM_KEY:
        m_wKeyValue = wKeyValue;
        break;
    case PIM_FUNC:
    case PIM_MOUSE:
        m_wKeyValue = (m_wKeyValue & 0xFF00) | (wKeyValue);
    default:
        break;
    }
    m_KeyEditWidget.SetKeyValue( m_wKeyValue );
    update();
}

void QSoftkeyActionKeyWidget::SetMouseValue(ushort wMouseValue)
{
    switch( wMouseValue )
    {
    case MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN:
        m_cbMouseWidget.setCurrentIndex( 0 );
        break;
    case MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN:
        m_cbMouseWidget.setCurrentIndex( 1 );
        break;
    case MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN:
        m_cbMouseWidget.setCurrentIndex( 2 );
        break;
    default:
        m_cbMouseWidget.setCurrentIndex( 0 );
        break;
    }

    m_wKeyValue = (m_wKeyValue & 0xFF00) | (wMouseValue);

    m_KeyEditWidget.SetKeyValue( m_wKeyValue & 0xFF00 );
}

ushort QSoftkeyActionKeyWidget::GetKeyValue()
{
    if( m_eInputMode == PIM_MOUSE )
        return (m_wKeyValue & 0xFF00) | ((ushort)m_cbMouseWidget.itemData( m_cbMouseWidget.currentIndex() ).toInt());
    return m_wKeyValue;
}

void QSoftkeyActionKeyWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    m_cbMouseWidget.clear();

    m_cbMouseWidget.addItem( Res.GetResString(QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MLEFT_DOWN")), MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN );
    m_cbMouseWidget.addItem( Res.GetResString(QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MRIGHT_DOWN")), MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN );
    m_cbMouseWidget.addItem( Res.GetResString(QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_MMIDDLE_DOWN")), MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN );

    SetRealGeometry();
}

void QSoftkeyActionKeyWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rc( 0, 0, width()-1, height() );

    dc.fillRect( rc, Qt::white );

    uchar cV0 = (uchar)(m_wKeyValue >> 8);

    bool bR2L = QLangManager::GetPtr()->GetResource().IsR2L();
    int nIconW = 0;
    int nIconH = rc.height(); //m_pIconBtn[0][0]->height();
    int nIconX = bR2L ? width()-m_pIconBtn[CTRL][UP]->width() : 0;
    int nIconY = rc.top() + (rc.height()-nIconH) / 2;
    for( int i=0 ; i<QICON_COUNT-1 ; i++ )
    {
        nIconW = m_pIconBtn[i][UP]->width();
        m_rectIconBtn[i] = QRect(nIconX, nIconY, nIconW, nIconH);
        dc.drawImage( m_rectIconBtn[i], *m_pIconBtn[i][cV0 & m_wIconKeyValue[i] ? DOWN : UP] );
        bR2L ? nIconX -= m_pIconBtn[i+1][UP]->width()+1 : nIconX += nIconW+1;
    }

    if( m_eInputMode == PIM_MOUSE ) return;
    nIconW = m_pIconBtn[CLEAR][UP]->width();
    m_rectIconBtn[CLEAR] = QRect(bR2L ? 0 : rc.right()-nIconW, nIconY, nIconW, nIconH);
    dc.drawImage( m_rectIconBtn[CLEAR], *m_pIconBtn[CLEAR][DOWN] );

    dc.end();
}

void QSoftkeyActionKeyWidget::mousePressEvent(QMouseEvent *evt)
{
    qDebug( "Usr Define Mouse Press" );
    if( evt->button() == Qt::LeftButton )
    {
        if( m_rectIconBtn[CLEAR].contains( evt->pos() ) )
        {
            if( m_eInputMode == PIM_KEY || m_eInputMode == PIM_FUNC )
            {
                Reset();
                m_KeyEditWidget.SetKeyValue( 0x0000 );
                m_wKeyValue = 0;
            }
        }
        else
        {
            for( int i=0 ; i<QICON_COUNT-1 ; i++ )
            {
                if( m_rectIconBtn[i].contains(evt->pos()) )
                {
                    uchar cV0 = (uchar)(m_wKeyValue >> 8);
                    uchar cV1 = (uchar)(m_wKeyValue);
                    cV0 ^= m_wIconKeyValue[i];

                    m_wKeyValue = cV0 << 8 | cV1;
                    break;
                }
            }
        }
        m_KeyEditWidget.Reset();
        m_KeyEditWidget.SetKeyValue( m_wKeyValue );
        update();
        if( m_eInputMode == PIM_MOUSE )
            MousekeyPress( m_wKeyValue );
        else
            KeyPressSignal( m_wKeyValue );
        m_KeyEditWidget.setFocus();
    }

    QLabel::mousePressEvent(evt);
}

void QSoftkeyActionKeyWidget::resizeEvent(QResizeEvent *evt)
{
    int nNW = evt->size().width();
    int nNH = evt->size().height();

    int nEditW = nNW - (m_pIconBtn[0][0]->width() + m_pIconBtn[1][0]->width() +
                        m_pIconBtn[2][0]->width() + m_pIconBtn[3][0]->width() + m_pIconBtn[4][0]->width()) - 8;

    m_KeyEditWidget.setGeometry( m_KeyEditWidget.x(), m_KeyEditWidget.y(), nEditW, nNH );
    m_cbFuncKeyWidget.setGeometry( m_KeyEditWidget.x(), m_KeyEditWidget.y(), nEditW, nNH );
    m_cbMouseWidget.setGeometry( QLangManager::GetPtr()->GetResource().IsR2L() ? 0 : m_cbFuncKeyWidget.x(), m_cbFuncKeyWidget.y(), m_cbFuncKeyWidget.width()+m_pIconBtn[CLEAR][UP]->width(), m_cbFuncKeyWidget.height() );

    QLabel::resizeEvent(evt);
}

void QSoftkeyActionKeyWidget::showEvent(QShowEvent *evt)
{
    m_KeyEditWidget.setFocus();
    QLabel::showEvent(evt);
}

bool QSoftkeyActionKeyWidget::eventFilter(QObject *target, QEvent *evt)
{
    if( target == this && evt->type() == QEvent::FocusIn )
    {
        m_KeyEditWidget.setFocus();
        evt->accept();
        return true;
    }

    if( evt->type() == QEvent::FocusOut )
    {
        //if( !QRect(0,0,width(),height()).contains( mapFromGlobal( QCursor::pos() ) ) && m_bHoverMode )
            FocusOutKeyEdit();
    }

    if( evt->type() == QEvent::HoverEnter || evt->type() == QEvent::HoverMove )
    {
        bool bRet = false;

        for( int i=0; i<QICON_COUNT; i++ )
        {
            if( m_rectIconBtn[i].contains( mapFromGlobal(QCursor::pos()) ) )
            {
                bRet = true;
                break;
            }
        }
        if( bRet )
            setCursor( Qt::PointingHandCursor );
        else
            setCursor( Qt::ArrowCursor );
    }
    if( evt->type() == QEvent::HoverLeave )
    {
        setCursor( Qt::ArrowCursor );
    }

    return QLabel::eventFilter(target,evt);
}

void QSoftkeyActionKeyWidget::OnKeyValueChange(ushort wKeyValue)
{
    m_wKeyValue = wKeyValue;
    update();

    KeyPressSignal( m_wKeyValue );
}

void QSoftkeyActionKeyWidget::onKeyPress_Signal(ushort nValue)
{
    if( m_eInputMode != PIM_KEY ) return;
    OnKeyValueChange( nValue );
    KeyPressSignal( nValue );

    ItemChanged( this );
}

void QSoftkeyActionKeyWidget::on_FuncKey_ShowPopup()
{
    FuncKey_ShowPopup_S();
}

void QSoftkeyActionKeyWidget::on_FuncKey_HidePopup()
{
    FuncKey_HidePopup_S();
}

void QSoftkeyActionKeyWidget::on_FuncKey_FocusIn()
{
    FuncKey_FocusIn_S();
}

void QSoftkeyActionKeyWidget::on_FuncKey_FocusOut()
{
    FuncKey_FocusOut_S();
}

void QSoftkeyActionKeyWidget::on_FuncKey_activated(int nIndex)
{
    FuncKey_activated(nIndex);
}

void QSoftkeyActionKeyWidget::on_Mouse_activated(int nIndex)
{
    Mouse_activated(nIndex);
}
