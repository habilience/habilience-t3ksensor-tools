#include "QUserDefinedKeyWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>

#include "stdInclude.h"
#include "../common/QKeyMapStr.h"
#include "QLangManager.h"

#define CTRL        0
#define ALT			1
#define SHIFT       2
#define WIN			3
#define CLEAR       4

#define UP			0
#define DOWN        1

QUserDefinedKeyWidget::QUserDefinedKeyWidget(QWidget *parent) :
    QLabel(parent), m_KeyEditWidget(this), m_cbFuncKeyWidget(this, false)
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
    m_bFuncKeyMode = false;

    m_cbFuncKeyWidget.SetZeroSet( true );
    m_cbFuncKeyWidget.setCursor( Qt::PointingHandCursor );

    connect( &m_KeyEditWidget, SIGNAL(KeyPressSignal(ushort)), this, SLOT(onKeyPress_Signal(ushort)) );
    connect( &m_cbFuncKeyWidget, SIGNAL(activated(int)), this, SLOT(on_FuncKey_activated(int)) );

    setAttribute( Qt::WA_Hover );
    installEventFilter( this );

    Init();
}

QUserDefinedKeyWidget::~QUserDefinedKeyWidget()
{
    for( int i=0 ; i<QICON_COUNT ; i++ )
    {
        if( m_pIconBtn[i][0] != NULL )
            delete m_pIconBtn[i][0];
        if( m_pIconBtn[i][1] != NULL )
            delete m_pIconBtn[i][1];
    }
}

void QUserDefinedKeyWidget::Init()
{
    m_pIconBtn[CTRL][UP]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CTRL_U" );
    m_pIconBtn[CTRL][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CTRL_S" );
    m_pIconBtn[ALT][UP]         = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_ALT_U" );
    m_pIconBtn[ALT][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_ALT_S" );
    m_pIconBtn[SHIFT][UP]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_SHIFT_U" );
    m_pIconBtn[SHIFT][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_SHIFT_S" );
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    m_pIconBtn[WIN][UP]         = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_WIN_U" );
    m_pIconBtn[WIN][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_WIN_S" );
#elif defined(Q_OS_MAC)
    m_pIconBtn[WIN][UP]         = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_MAC_U" );
    m_pIconBtn[WIN][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_MAC_S" );
#endif
    m_pIconBtn[CLEAR][UP]       = new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CLEAR_U" );
    m_pIconBtn[CLEAR][DOWN]		= new QImage( ":/T3kCfgRes/Resources/PNG_ICON_CLEAR_S" );

    m_nIconTotalWidth = 0;
    m_nIconTotalWidth += m_pIconBtn[CTRL][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[ALT][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[SHIFT][UP]->width();
    m_nIconTotalWidth += m_pIconBtn[WIN][UP]->width();

    m_nIconTotalWidth += QICON_COUNT;

    SetRealGeometry();

    QString strFn;
    int nIdx;
    for( int i=1 ; i<=24 ; i++ )
    {
        strFn.sprintf( "F%d", i );
        m_cbFuncKeyWidget.addItem( strFn );
        nIdx = m_cbFuncKeyWidget.count()-1;
        ulong nScanCode = virtualKeyToScanCode( Qt::Key_F1+i-1 );
        uchar cUsageId;
        if( findUsageId( nScanCode, cUsageId ) )
        {
            m_cbFuncKeyWidget.setItemData( nIdx, cUsageId );
        }
    }

    m_cbFuncKeyWidget.setCurrentIndex( -1 );
    m_cbFuncKeyWidget.hide();
}

void QUserDefinedKeyWidget::SetRealGeometry()
{
    bool bR2L = QLangManager::GetPtr()->GetResource().IsR2L();
    QRect rc( bR2L ? m_pIconBtn[CLEAR][UP]->width()+6 : m_nIconTotalWidth, 0, width()-m_nIconTotalWidth-m_pIconBtn[CLEAR][UP]->width(), height() );
    m_KeyEditWidget.setGeometry( rc );
    m_KeyEditWidget.setAlignment( bR2L ? Qt::AlignRight : Qt::AlignLeft );
    m_cbFuncKeyWidget.setGeometry( rc );
}

void QUserDefinedKeyWidget::SetUseFuncKey(bool bFuncKey)
{
    m_bFuncKeyMode = bFuncKey;
    if( m_bFuncKeyMode )
        m_cbFuncKeyWidget.show();
    else
        m_cbFuncKeyWidget.hide();
}

void QUserDefinedKeyWidget::SetFuncKeyCurrentIndex(int nIndex)
{
    m_cbFuncKeyWidget.setCurrentIndex( nIndex );
}

int QUserDefinedKeyWidget::GetFuncKeyCurrentIndex()
{
    return m_cbFuncKeyWidget.currentIndex();
}

int QUserDefinedKeyWidget::GetFuncKeyCount()
{
    return m_cbFuncKeyWidget.count();
}

QVariant QUserDefinedKeyWidget::GetItemData(int nIndex) const
{
    return m_cbFuncKeyWidget.itemData( nIndex );
}

void QUserDefinedKeyWidget::Reset()
{
    m_cbFuncKeyWidget.setCurrentIndex( -1 );
    m_KeyEditWidget.Reset();
    m_KeyEditWidget.SetKeyValue( 0x0000 );
    m_wKeyValue = 0x0000;
    update();
}

void QUserDefinedKeyWidget::SetKeyValue( ushort wKeyValue )
{
    if( m_bFuncKeyMode )
        m_wKeyValue = (m_wKeyValue & 0xFF00) | (wKeyValue);
    else
        m_wKeyValue = wKeyValue;

    m_KeyEditWidget.SetKeyValue( m_wKeyValue );
    update();
}

ushort QUserDefinedKeyWidget::GetKeyValue()
{
    return m_wKeyValue;
}

void QUserDefinedKeyWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rc( 0, 0, width()-1, height()-1 );

    uchar cV0 = (uchar)(m_wKeyValue >> 8);

    bool bR2L = QLangManager::GetPtr()->GetResource().IsR2L();
    int nIconW = 0;
    int nIconH = m_pIconBtn[0][0]->height();
    int nIconX = bR2L ? width()-m_pIconBtn[CTRL][UP]->width() : 0;
    int nIconY = rc.top() + (rc.height()-nIconH) / 2;
    for( int i=0 ; i<QICON_COUNT-1 ; i++ )
    {
        nIconW = m_pIconBtn[i][UP]->width();
        m_rectIconBtn[i] = QRect(nIconX, nIconY, nIconW, nIconH);
        dc.drawImage( m_rectIconBtn[i], *m_pIconBtn[i][cV0 & m_wIconKeyValue[i] ? DOWN : UP] );
        bR2L ? nIconX -= m_pIconBtn[i+1][UP]->width()+1 : nIconX += nIconW+1;
    }

    nIconW = m_pIconBtn[CLEAR][UP]->width();
    m_rectIconBtn[CLEAR] = QRect(bR2L ? 0 : rc.right()-nIconW, nIconY, nIconW, nIconH);
    dc.drawImage( m_rectIconBtn[CLEAR], *m_pIconBtn[CLEAR][DOWN] );

    dc.end();
}

void QUserDefinedKeyWidget::mousePressEvent(QMouseEvent *ev)
{
    if( ev->type() == QEvent::MouseButtonPress )
    {
        if( ev->button() == Qt::LeftButton )
        {
            if( m_rectIconBtn[CLEAR].contains( ev->pos() ) )
            {
                Reset();
                m_KeyEditWidget.SetKeyValue( 0x0000 );
                m_wKeyValue = 0;
                KeyPressSignal( m_wKeyValue );
                m_KeyEditWidget.setFocus();
            }
            else
            {
                for( int i=0 ; i<QICON_COUNT-1 ; i++ )
                {
                    if( m_rectIconBtn[i].contains(ev->pos()) )
                    {
                        uchar cV0 = (uchar)(m_wKeyValue >> 8);
                        uchar cV1 = (uchar)(m_wKeyValue);
                        cV0 ^= m_wIconKeyValue[i];

                        m_wKeyValue = cV0 << 8 | cV1;
                        m_KeyEditWidget.SetKeyValue( m_wKeyValue );
                        update();
                        KeyPressSignal( m_wKeyValue );
                        m_KeyEditWidget.setFocus();
                    }
                }
            }
        }
    }

    QLabel::mousePressEvent(ev);
}

bool QUserDefinedKeyWidget::eventFilter(QObject *target, QEvent *evt)
{
    if( target == this )
    {
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
    }

    return QLabel::eventFilter(target,evt);
}

void QUserDefinedKeyWidget::OnKeyValueChange(ushort wKeyValue)
{
    m_wKeyValue = wKeyValue;
    update();

    KeyPressSignal( m_wKeyValue );
}

void QUserDefinedKeyWidget::onKeyPress_Signal(ushort nValue)
{
    OnKeyValueChange( nValue );
}

void QUserDefinedKeyWidget::on_FuncKey_activated(int nIndex)
{
    SetKeyValue( m_cbFuncKeyWidget.itemData( nIndex ).toInt() );
    FuncKey_activated(nIndex);
}

void QUserDefinedKeyWidget::setFocus()
{
    m_KeyEditWidget.setFocus();
}
