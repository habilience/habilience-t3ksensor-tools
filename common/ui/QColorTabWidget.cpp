#include "./QColorTabWidget.h"
#include <QPainter>
#include "../QUtils.h"
#include "../QGUIUtils.h"
#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimerEvent>


QColorTabWidget::QColorTabWidget(QWidget *parent) :
    QWidget(parent)
  , m_sizeTabHeader(90,40)
  , m_rcChildArea(0,0,0,0)
{
    m_bIsHovered = false;
    m_bIsMouseDown = false;
    m_bEnableTabChange = true;

    m_bNotifyTab = false;

    m_nBlinkCount = 0;
    m_TimerBlink = 0;
    m_nBarOffset = 0;

    m_eTabDirection = TabDirectionHorzLeftTop;

    m_nActiveTabIndex = -1;
    m_nHoverTabIndex = -1;

    m_pExtraWidget = NULL;
    m_nVisibleCount = 0;
    m_nMoveCount = 0;

    m_btnLeftTab.setParent( this );
    m_btnLeftTab.setVisible( false );
    m_btnLeftTab.setAutoRaise( true );
    m_btnRightTab.setParent( this );
    m_btnRightTab.setVisible( false );
    m_btnRightTab.setAutoRaise( true );

    m_fntTab = getSystemFont(this);
    m_fntTab.setPixelSize(11);

    setMouseTracking(true);     // for mousemove event
    installEventFilter( this ); // for mousemove event!
}

void QColorTabWidget::setTabDirection( TabDirection dir, int nBarSize, int nBarOffset/*=0*/ )
{
    m_eTabDirection = dir;
    m_nBarOffset = nBarOffset;

    if( isHorzTab() )
    {
        m_sizeTabHeader.setHeight(nBarSize);
        m_btnLeftTab.setFixedSize( nBarOffset-2, nBarSize-4 );
        m_btnRightTab.setFixedSize( nBarOffset-2, nBarSize-4 );

        QPixmap pm(":/T3kCfgRes/resources/PNG_SPIN_ARROW_H.png");
        m_btnLeftTab.setIcon( QIcon( pm.copy(0,0,pm.width()/2,pm.height()) ) );
        m_btnRightTab.setIcon( QIcon( pm.copy(pm.width()/2,0,pm.width()/2,pm.height()) ) );
    }
    else
    {
        m_sizeTabHeader.setWidth(nBarSize);
        m_btnLeftTab.setFixedSize( nBarSize-4, nBarOffset-2 );
        m_btnRightTab.setFixedSize( nBarSize-4, nBarOffset-2 );

        QPixmap pm(":/T3kCfgRes/resources/PNG_SPIN_ARROW_V.png");
        m_btnLeftTab.setIcon( QIcon( pm.copy(0,0,pm.width(),pm.height()/2) ) );
        m_btnRightTab.setIcon( QIcon( pm.copy(0,pm.height()/2,pm.width(),pm.height()/2) ) );
    }

    disconnect( &m_btnLeftTab, &QToolButton::clicked, NULL, NULL );
    disconnect( &m_btnLeftTab, &QToolButton::clicked, NULL, NULL );

    switch( m_eTabDirection )
    {
    case TabDirectionHorzLeftTop:
    case TabDirectionVertLeftTop:
        connect( &m_btnLeftTab, &QToolButton::clicked, this, &QColorTabWidget::onClickedBtnPrev );
        connect( &m_btnRightTab, &QToolButton::clicked, this, &QColorTabWidget::onClickedBtnNext );
        m_btnLeftTab.setEnabled( false );
        break;
    case TabDirectionHorzRightTop:
    case TabDirectionVertLeftBottom:
        connect( &m_btnLeftTab, &QToolButton::clicked, this, &QColorTabWidget::onClickedBtnNext );
        connect( &m_btnRightTab, &QToolButton::clicked, this, &QColorTabWidget::onClickedBtnPrev );
        m_btnRightTab.setEnabled( false );
        break;
    }
}

void QColorTabWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.save();

    p.setRenderHint(QPainter::Antialiasing);

    QRect rcBody(0, 0, width()-1, height()-1);
    rcBody.adjust( 1, 1, -1, -1 );

    drawTabs(p, rcBody);

    p.restore();
}

void QColorTabWidget::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerBlink)
    {
        if ( m_nBlinkCount < 20 )
        {
            m_nBlinkCount++;
            update();
        }
    }
}

bool QColorTabWidget::event(QEvent *evt)
{
    if (isEnabled())
    {
        switch (evt->type())
        {
        case QEvent::DragEnter:
        case QEvent::Enter:
        case QEvent::HoverEnter:
            m_bIsHovered = true;
            update();
            break;
        case QEvent::DragLeave:
        case QEvent::Leave:
        case QEvent::HoverLeave:
            m_bIsHovered = false;
            m_nHoverTabIndex = -1;
            setCursor(Qt::ArrowCursor);
            update();
            break;
        default:
            break;
        }
    }
    else
    {
        m_bIsHovered = false;
    }

    return QWidget::event(evt);
}

void QColorTabWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (m_bIsHovered)
        {
            m_bIsHovered = false;
        }

        int nCnt = m_nVisibleCount < m_aryTabInfo.size() ? m_nVisibleCount : m_aryTabInfo.size();
        for (int i=0 ; i<nCnt ; i++)
        {
            const TabInfo& ti = m_aryTabInfo.at(i);
            if (ti.rcArea.contains(e->pos()))
            {
                selectTab(i);
                break;
            }
        }

        m_bIsMouseDown = true;
        update();
    }

    QWidget::mousePressEvent(e);
}

void QColorTabWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_bIsMouseDown = false;
        int nCnt = m_nVisibleCount < m_aryTabInfo.size() ? m_nVisibleCount : m_aryTabInfo.size();
        for (int i=0 ; i<nCnt ; i++)
        {
            const TabInfo& ti = m_aryTabInfo.at(i);
            if (ti.rcArea.contains(e->pos()))
            {
                m_bIsHovered = true;
                m_nHoverTabIndex = i;
                QWidget::mouseReleaseEvent(e);
                return;
            }
        }

        m_bIsHovered = false;
        m_nHoverTabIndex = -1;
        setCursor(Qt::ArrowCursor);
        update();
    }
    QWidget::mouseReleaseEvent(e);
}

void QColorTabWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_bIsMouseDown)
    {
        int nCnt = m_nVisibleCount < m_aryTabInfo.size() ? m_nVisibleCount : m_aryTabInfo.size();
        for (int i=0 ; i<nCnt ; i++)
        {
            const TabInfo& ti = m_aryTabInfo.at(i);
            if (ti.rcArea.contains(e->pos()))
            {
                if (m_nHoverTabIndex != i)
                {
                    m_nHoverTabIndex = i;
                    setCursor(Qt::PointingHandCursor);
                    update();
                }
                break;
            }
        }
        if (m_nHoverTabIndex != -1)
        {
            m_nHoverTabIndex = -1;
            setCursor(Qt::ArrowCursor);
            update();
        }
    }
    else
    {
        if (m_bIsHovered)
        {
            int nCnt = m_nVisibleCount < m_aryTabInfo.size() ? m_nVisibleCount : m_aryTabInfo.size();
            for (int i=0 ; i<nCnt ; i++)
            {
                const TabInfo& ti = m_aryTabInfo.at(i);
                if (ti.rcArea.contains(e->pos()))
                {
                    if (m_nHoverTabIndex != i)
                    {
                        m_nHoverTabIndex = i;
                        setCursor(Qt::PointingHandCursor);
                        update();
                    }
                    return;
                }
            }
            if (m_nHoverTabIndex != -1)
            {
                m_nHoverTabIndex = -1;
                update();
            }
            setCursor(Qt::ArrowCursor);
        }
    }
    QWidget::mouseMoveEvent(e);
}

void QColorTabWidget::resizeEvent(QResizeEvent *evt)
{
    if ( evt->size().width() <= 0 || evt->size().height() <= 0 ) return;

    QRect rc(0, 0, evt->size().width()-1, evt->size().height()-1);
    rc.adjust( 1, 1, -1, -1 );

    if ( isHorzTab() )
    {
        m_rcChildArea.setLeft(rc.left());
        m_rcChildArea.setTop(rc.top()+m_sizeTabHeader.height());
        m_rcChildArea.setRight(rc.right());
        m_rcChildArea.setBottom(rc.bottom());

        m_btnLeftTab.move( rc.left(), rc.top()+2 );
        m_btnRightTab.move( rc.right()-m_btnRightTab.width(), rc.top()+2 );
    }
    else
    {
        m_rcChildArea.setLeft(rc.left() + m_sizeTabHeader.width());
        m_rcChildArea.setTop(rc.top());
        m_rcChildArea.setRight(rc.right());
        m_rcChildArea.setBottom(rc.bottom());

        m_btnLeftTab.move( rc.left(), rc.top()+2 );
        m_btnRightTab.move( rc.right()-m_btnRightTab.width(), rc.top()+2 );
    }

    if( m_pExtraWidget )
    {
        switch( m_eTabDirection )
        {
        case TabDirectionHorzLeftTop:
            m_pExtraWidget->move( rc.right() - m_nBarOffset - m_pExtraWidget->width(), rc.top() + (m_sizeTabHeader.height() - m_pExtraWidget->height())/2 );
            break;
        case TabDirectionHorzRightTop:
            m_pExtraWidget->move( rc.left() + m_nBarOffset, rc.top() + (m_sizeTabHeader.height() - m_pExtraWidget->height())/2 );
            break;
        case TabDirectionVertLeftTop:
            m_pExtraWidget->move( rc.left() + (m_sizeTabHeader.width() - m_pExtraWidget->width())/2, rc.top() + m_nBarOffset );
            break;
        case TabDirectionVertLeftBottom:
            m_pExtraWidget->move( rc.left() + (m_sizeTabHeader.width() - m_pExtraWidget->width())/2, rc.bottom() + m_pExtraWidget->height() + m_nBarOffset );
            break;
        default:
            Q_ASSERT( false );
            break;
        }
    }

    m_rcChildArea.adjust( 1, 1, -1, -1 );

    if (m_nActiveTabIndex >= 0)
    {
        TabInfo& ti = m_aryTabInfo[m_nActiveTabIndex];
        if (ti.pChildWidget)
        {
            ti.pChildWidget->move( m_rcChildArea.topLeft() );
            ti.pChildWidget->resize( m_rcChildArea.size() );
            if (!ti.pChildWidget->isVisible())
            {
                ti.pChildWidget->setVisible(true);
            }
        }
    }
}

void QColorTabWidget::drawTabs( QPainter& p, QRect rc )
{
#define TAB_INTERVAL	(3)
#define TAB_MARGIN		(10)
    QRect rectTab;

    switch (m_eTabDirection)
    {
    case TabDirectionHorzLeftTop:
        rectTab.setY(rc.top());
        rectTab.setX(rc.left() + m_nBarOffset);
        break;
    case TabDirectionHorzRightTop:
        rectTab.setY(rc.top());
        rectTab.setX(rc.right() - m_nBarOffset + TAB_INTERVAL);
        break;
    case TabDirectionVertLeftTop:
        rectTab.setX(rc.left());
        rectTab.setY(rc.top() + m_nBarOffset);
        break;
    case TabDirectionVertLeftBottom:
        rectTab.setX(rc.left());
        rectTab.setY(rc.bottom() - m_nBarOffset + TAB_INTERVAL);
        break;
    }

    QSize szTab = calcTabSize( p, m_aryTabInfo.at(0) );
    if (isHorzTab())
    {
        m_sizeTabHeader.setWidth(szTab.width());
        rectTab.setHeight(m_sizeTabHeader.height());
    }
    else
    {
        m_sizeTabHeader.setHeight(szTab.height());
        rectTab.setWidth(m_sizeTabHeader.width());
    }

    int nMaxTabBarSize = rc.width() - m_nBarOffset * 2;
    nMaxTabBarSize -= m_pExtraWidget != NULL ? (isHorzTab() ? m_pExtraWidget->width() : m_pExtraWidget->height()) + TAB_MARGIN*2 : 0;
    int nTabBarSize = 0;

    m_nVisibleCount = 0;
    for (int nI = m_nMoveCount; nI < m_aryTabInfo.size() ; nI ++)
    {
        TabInfo& ti = m_aryTabInfo[nI];
        szTab = calcTabSize( p, ti );

        if (m_eTabDirection == TabDirectionHorzRightTop)
        {
            rectTab.setX( rectTab.x() - (szTab.width() + TAB_INTERVAL + TAB_MARGIN) );
        }
        else if (m_eTabDirection == TabDirectionVertLeftBottom)
        {
            rectTab.setY( rectTab.y() - (szTab.height() + TAB_INTERVAL + TAB_MARGIN) );
        }

        if (isHorzTab())
        {
            rectTab.setWidth(szTab.width() + TAB_MARGIN);   // margin x: 5px
            nTabBarSize += rectTab.width();
        }
        else
        {
            rectTab.setHeight(szTab.height() + TAB_MARGIN);
            nTabBarSize += rectTab.height();
        }

        if( nTabBarSize <= nMaxTabBarSize )
        {
            drawTab( p, rectTab, ti,
                m_nHoverTabIndex == nI ? true : false,
                m_nActiveTabIndex == nI ? true : false, isNotifyTab(nI) < 0 ? false : true );

            m_nVisibleCount++;
        }

        ti.rcArea = rectTab;

        if (m_eTabDirection == TabDirectionHorzLeftTop)
        {
            rectTab.setX( rectTab.x() + (rectTab.width() + TAB_INTERVAL) );
        }
        else if (m_eTabDirection == TabDirectionVertLeftTop)
        {
            rectTab.setY( rectTab.y() + (rectTab.height() + TAB_INTERVAL) );
        }
    }

    if( m_nVisibleCount < m_aryTabInfo.size() )
    {
        if( !m_btnLeftTab.isVisible() )
        {
            m_btnLeftTab.setVisible( true );
            m_btnRightTab.setVisible( true );
        }
    }
    else
    {
        if( m_btnLeftTab.isVisible() )
        {
            m_btnLeftTab.setVisible( false );
            m_btnRightTab.setVisible( false );
        }
    }

    QPen bodyPen( QColor(160, 160, 160), 1.f);
    QPen bodyShadowPen( QColor(160, 160, 160, 150), 2.f);

    p.setPen(bodyShadowPen);
    if (isHorzTab())
    {
        int nTop = rectTab.bottom();
        if (m_nActiveTabIndex >= 0 && (m_nActiveTabIndex >= m_nMoveCount && m_nActiveTabIndex < (m_nMoveCount+m_nVisibleCount)))
        {
            const TabInfo& ti = m_aryTabInfo.at(m_nActiveTabIndex);
            p.drawLine( rc.left(), nTop, ti.rcArea.left(), nTop );
            p.drawLine( ti.rcArea.right(), nTop, rc.right(), nTop );
        }
        else
        {
            p.drawLine( rc.left(), nTop, rc.right(), nTop );
        }

        if (m_nActiveTabIndex >= 0)
            p.fillRect( rc.left(), nTop, rc.width(), rc.bottom()-nTop, Qt::white );

        p.drawLine( rc.right(), nTop, rc.right(), rc.bottom() );
        p.drawLine( rc.right(), rc.bottom(), rc.left(), rc.bottom() );
        p.drawLine( rc.left(), rc.bottom(), rc.left(), nTop );
    }
    else
    {
        int nLeft = rectTab.right();
        if (m_nActiveTabIndex >= 0 && (m_nActiveTabIndex >= m_nMoveCount && m_nActiveTabIndex < (m_nMoveCount+m_nVisibleCount)))
        {
            const TabInfo& ti = m_aryTabInfo.at(m_nActiveTabIndex);
            p.drawLine( nLeft, rc.top(), nLeft, ti.rcArea.top() );
            p.drawLine( nLeft, ti.rcArea.bottom(), nLeft, rc.bottom() );
        }
        else
        {
            p.drawLine( nLeft, rc.top(), nLeft, rc.bottom() );
        }

        if (m_nActiveTabIndex >= 0)
            p.fillRect( nLeft, rc.top(), rc.right()-nLeft, rc.height(), Qt::white );

        p.drawLine( nLeft, rc.bottom(), rc.right(), rc.bottom() );
        p.drawLine( rc.right(), rc.bottom(), rc.right(), rc.top() );
        p.drawLine( rc.right(), rc.top(), nLeft, rc.top() );
    }

    p.setPen( bodyPen );

    if (isHorzTab())
    {
        int nTop = rectTab.bottom();
        if (m_nActiveTabIndex >= 0 && (m_nActiveTabIndex >= m_nMoveCount && m_nActiveTabIndex < (m_nMoveCount+m_nVisibleCount)))
        {
            const TabInfo& ti = m_aryTabInfo.at(m_nActiveTabIndex);
            p.drawLine( rc.left(), nTop, ti.rcArea.left(), nTop );
            p.drawLine( ti.rcArea.right(), nTop, rc.right(), nTop );
        }
        else
        {
            p.drawLine( rc.left(), nTop, rc.right(), nTop );
        }

        p.drawLine( rc.right(), nTop, rc.right(), rc.bottom() );
        p.drawLine( rc.right(), rc.bottom(), rc.left(), rc.bottom() );
        p.drawLine( rc.left(), rc.bottom(), rc.left(), nTop );
    }
    else
    {
        int nLeft = rectTab.right();
        if (m_nActiveTabIndex >= 0 && (m_nActiveTabIndex >= m_nMoveCount && m_nActiveTabIndex < (m_nMoveCount+m_nVisibleCount)))
        {
            const TabInfo& ti = m_aryTabInfo.at(m_nActiveTabIndex);
            p.drawLine( nLeft, rc.top(), nLeft, ti.rcArea.top() );
            p.drawLine( nLeft, ti.rcArea.bottom(), nLeft, rc.bottom() );
        }
        else
        {
            p.drawLine( nLeft, rc.top(), nLeft, rc.bottom() );
        }

        p.drawLine( nLeft, rc.bottom(), rc.right(), rc.bottom() );
        p.drawLine( rc.right(), rc.bottom(), rc.right(), rc.top() );
        p.drawLine( rc.right(), rc.top(), nLeft, rc.top() );
    }
}

void QColorTabWidget::drawTab( QPainter& p, QRect& rectTab, const TabInfo& ti, bool bHover, bool bActive, bool bNotify )
{
    QPainterPath tabPath;
    QPainterPath tabS;

    int nDiameter = 3 << 1;
    QRectF arcRect( 0, 0, nDiameter, nDiameter );

    if (isHorzTab())
    {
        // Horz-Tab
        tabPath.moveTo( rectTab.bottomLeft() );
        tabPath.lineTo( rectTab.left(), rectTab.top()+nDiameter/2);
        tabS.moveTo( rectTab.left(), rectTab.top()+nDiameter/2 );
        arcRect.moveTo( rectTab.topLeft() );
        tabPath.arcTo( arcRect, 180, -90 );
        tabS.arcTo( arcRect, 180, -90 );
        tabPath.lineTo( rectTab.right()-nDiameter/2, rectTab.top() );
        tabS.lineTo( rectTab.right()-nDiameter/2, rectTab.top() );
        arcRect.moveTo( rectTab.right()-nDiameter, rectTab.top() );
        tabPath.arcTo( arcRect, 90, -90 );
        tabS.arcTo( arcRect, 90, -90 );
        tabPath.lineTo( rectTab.bottomRight() );
        tabS.closeSubpath();
        //tabPath.closeSubpath();
    }
    else
    {
        // Vert-Tab
        tabPath.moveTo( rectTab.right(), rectTab.y() );
        tabPath.lineTo( rectTab.x()+nDiameter, rectTab.y() );
        tabS.moveTo( rectTab.x()+nDiameter, rectTab.y() );
        arcRect.moveTo(rectTab.topLeft());
        tabPath.arcTo( arcRect, -270, 90 );
        tabS.arcTo( arcRect, -270, 90 );
        arcRect.moveTo(rectTab.x(), rectTab.bottom()-nDiameter);
        tabPath.arcTo( arcRect, -180, 90 );
        tabS.arcTo( arcRect, -180, 90 );
        tabPath.moveTo( rectTab.left()+nDiameter, rectTab.bottom() );
        tabPath.lineTo( rectTab.right(), rectTab.bottom() );
        tabS.closeSubpath();
        //tabPath.closeSubpath();
    }

    QColor colorBody;

    if (bNotify && (m_nBlinkCount % 2 == 0))
    {
        colorBody = QColor(252, 209, 211);
    }
    else
    {
        if (bActive)
            colorBody = QColor(255, 255, 255);
        else
            colorBody = QColor(0xF5, 0xF5, 0xF5);
    }

    p.fillPath( tabPath, QBrush(colorBody) );

    QColor colorStart = bActive ? ti.clrTab : (bHover ? QColor(255, 190, 60, 200) : QColor(255, 255, 255, 200));
    QColor colorEnd(255, 255, 255, 200);
    QRectF rectTabTip;
    rectTabTip = tabS.boundingRect();
    QLinearGradient gradTip;
    if (!isHorzTab())
    {
        gradTip.setStart(rectTabTip.left(), rectTabTip.center().y());
        gradTip.setFinalStop(rectTabTip.right(), rectTabTip.center().y());
    }
    else
    {
        gradTip.setStart(rectTabTip.center().x(), rectTabTip.top());
        gradTip.setFinalStop(rectTabTip.center().x(), rectTabTip.bottom());
    }
    gradTip.setColorAt( 0, colorStart );
    gradTip.setColorAt( 1.f, colorEnd );

    p.setBrush(Qt::NoBrush);
    p.setPen( QPen(QColor(160,160,160,100), 2.f) );
    p.drawPath( tabPath );
    p.setPen( QPen(QColor(160,160,160)) );
    p.drawPath( tabPath );

    p.fillPath( tabS, QBrush(gradTip) );
    if (bActive || bHover)
    {
        p.setPen( colorStart );
        p.drawPath( tabS );
    }

    QRectF rectText;

    float fTextOffset = ti.pIconImage ? ti.pIconImage->width()+5.f : 0.f;

    if (isHorzTab())
    {
        rectText.setX((float)rectTab.x() + fTextOffset);
        rectText.setY((float)rectTab.y() + nDiameter/2);
        rectText.setWidth((float)rectTab.width() - fTextOffset);
        rectText.setHeight((float)rectTab.height() - nDiameter/2);
    }
    else
    {
        rectText.setX((float)rectTab.x() + nDiameter/2 + fTextOffset);
        rectText.setY((float)rectTab.y());
        rectText.setWidth((float)rectTab.width() - nDiameter/2 - fTextOffset);
        rectText.setHeight((float)rectTab.height());
    }

    m_fntTab.setBold(bActive);

    p.setFont( m_fntTab );

    if (ti.pIconImage)
    {
        int nIW = ti.pIconImage->width();
        int nIH = ti.pIconImage->height();
        p.drawImage( (int)(rectText.x()-fTextOffset), (int)(rectText.y()), *ti.pIconImage, 0, 0, nIW, nIH );
    }

    int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;
    p.setPen( QColor(80,80,80) );
    p.drawText( rectText, flags, ti.strCaption );
}

QSize QColorTabWidget::calcTabSize( QPainter& p, const TabInfo& ti )
{
    p.setFont(m_fntTab);

    QRectF rectText(0,0,0,0);

    if( isHorzTab() )
    {
        rectText.setHeight((float)m_sizeTabHeader.height());
    }
    else
    {
        rectText.setWidth((float)m_sizeTabHeader.width());
    }

    QString strCaption = ti.strCaption;
    int flags = Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine;

    QRectF rectBB = p.boundingRect( rectText, flags, strCaption );
    //p.drawText( rectText, flags, strCaption, &rectBB );

    float fImageOffset = 0.f;
    if( ti.pIconImage )
    {
        fImageOffset = ti.pIconImage->width() + 5.f;
    }

    if( isHorzTab() )
    {
        rectBB.setWidth( rectBB.width() + fImageOffset + 15.f );
    }
    else
    {
        rectBB.setWidth( rectBB.width() + fImageOffset );
        rectBB.setHeight( rectBB.height() + 15.f );
    }


    return QSize( (int)(rectBB.width() + .5f), (int)(rectBB.height() + .5f) );
}

int QColorTabWidget::isNotifyTab( int tabIndex )
{
    for( int i=0 ; i<m_aryNotifyTabIndex.size() ; i++ )
    {
        if( m_aryNotifyTabIndex.at(i) == tabIndex )
            return i;
    }
    return -1;
}

int QColorTabWidget::addTab( const QString& strCaption, QWidget* pChildWidget, QColor clrTab/*=QColor(255,60,60,200)*/, QImage* pIconImage/*=NULL*/)
{
    pIconImage = pIconImage;    // TODO: icon
    TabInfo ti;
    ti.pChildWidget = pChildWidget;
    ti.strCaption = strCaption;
    ti.pIconImage = NULL;
    ti.clrTab = clrTab;
    m_aryTabInfo.push_back(ti);

    if (pChildWidget)
    {
        QSize sizeChild = pChildWidget->minimumSize();
        sizeChild.setWidth( sizeChild.width() + (!isHorzTab() ? m_sizeTabHeader.width() : 0) + 4 + 1 + 2 );
        sizeChild.setHeight( sizeChild.height() + (isHorzTab() ? m_sizeTabHeader.height() : 0) + 4 + 1 + 2 );
        QSize sizeParent = minimumSize();

        QSize sizeNewMinimum;
        sizeNewMinimum.setWidth( sizeParent.width() < sizeChild.width() ? sizeChild.width() : sizeParent.width() );
        sizeNewMinimum.setHeight( sizeParent.height() < sizeChild.height() ? sizeChild.height() : sizeParent.height() );

        setMinimumSize( sizeNewMinimum );
    }

    return m_aryTabInfo.size();
}

void QColorTabWidget::selectTab( int tabIndex )
{
    if (!m_bEnableTabChange)
        return;

    if (m_nActiveTabIndex != tabIndex)
    {
        if (m_bNotifyTab)
        {
            int nN = 0;
            if ((nN = isNotifyTab(tabIndex)) >= 0)
            {
                m_aryNotifyTabIndex.remove(nN);
            }
            if( m_aryNotifyTabIndex.size() == 0 )
            {
                m_bNotifyTab = false;
                if (m_TimerBlink)
                {
                    killTimer(m_TimerBlink);
                    m_TimerBlink = 0;
                }
            }
        }

        emit tabSelectChange(this, tabIndex);

        if (m_nActiveTabIndex >= 0)
        {
            TabInfo& tiOld = m_aryTabInfo[m_nActiveTabIndex];
            if( tiOld.pChildWidget )
            {
                tiOld.pChildWidget->setVisible(false);
            }
        }
        m_nActiveTabIndex = tabIndex;
        TabInfo& ti = m_aryTabInfo[m_nActiveTabIndex];
        if( ti.pChildWidget )
        {
            if (!m_rcChildArea.isEmpty())
            {
                ti.pChildWidget->move(m_rcChildArea.topLeft());
                ti.pChildWidget->resize(m_rcChildArea.size());
                ti.pChildWidget->setVisible(true);
            }
        }

        emit tabSelectChanged(this, tabIndex);
        update();
    }
}

void QColorTabWidget::setTabText( int tabIndex, const QString& text )
{
    if ( m_aryTabInfo.size() <= tabIndex || tabIndex < 0 )
    {
        return;
    }

    m_aryTabInfo[tabIndex].strCaption = text;
}

void QColorTabWidget::notifyTab( int tabIndex )
{
    if( tabIndex < 0 || tabIndex >= m_aryTabInfo.size() )
        return;
    if( m_nActiveTabIndex == tabIndex )
        return;

    for( int i=0 ; i<m_aryNotifyTabIndex.size() ; i++ )
    {
        if( m_aryNotifyTabIndex.at(i) == tabIndex )
            return;
    }

    m_bNotifyTab = true;
    m_aryNotifyTabIndex.push_back(tabIndex);
    m_nBlinkCount = 0;

    if (m_TimerBlink)
        killTimer(m_TimerBlink);
    m_TimerBlink = startTimer(500);
}

void QColorTabWidget::resetNotify()
{
    m_bNotifyTab = false;
    m_aryNotifyTabIndex.clear();
    if (m_TimerBlink)
    {
        killTimer(m_TimerBlink);
        m_TimerBlink = 0;
    }
    update();
}

void QColorTabWidget::enableTabChange( bool bEnable )
{
    m_bEnableTabChange = bEnable;
}

void QColorTabWidget::onClickedBtnPrev(bool /*checked*/)
{
    m_nMoveCount--;
    if( m_nMoveCount < 1 )
    {
        m_nMoveCount = 0;
        switch( m_eTabDirection )
        {
        case TabDirectionHorzLeftTop:
        case TabDirectionVertLeftTop:
            m_btnLeftTab.setEnabled( false );
            break;
        case TabDirectionHorzRightTop:
        case TabDirectionVertLeftBottom:
            m_btnRightTab.setEnabled( false );
            break;
        }
    }

    switch( m_eTabDirection )
    {
    case TabDirectionHorzLeftTop:
    case TabDirectionVertLeftTop:
        if( !m_btnRightTab.isEnabled() )
            m_btnRightTab.setEnabled( true );
        break;
    case TabDirectionHorzRightTop:
    case TabDirectionVertLeftBottom:
        if( !m_btnLeftTab.isEnabled() )
            m_btnLeftTab.setEnabled( true );
        break;
    }

    update();
}

void QColorTabWidget::onClickedBtnNext(bool /*checked*/)
{
    m_nMoveCount++;
    if( m_nMoveCount + m_nVisibleCount >= m_aryTabInfo.size() )
    {
        switch( m_eTabDirection )
        {
        case TabDirectionHorzLeftTop:
        case TabDirectionVertLeftTop:
            m_btnRightTab.setEnabled( false );
            break;
        case TabDirectionHorzRightTop:
        case TabDirectionVertLeftBottom:
            m_btnLeftTab.setEnabled( false );
            break;
        }
    }

    switch( m_eTabDirection )
    {
    case TabDirectionHorzLeftTop:
    case TabDirectionVertLeftTop:
        if( !m_btnLeftTab.isEnabled() )
            m_btnLeftTab.setEnabled( true );
        break;
    case TabDirectionHorzRightTop:
    case TabDirectionVertLeftBottom:
        if( !m_btnRightTab.isEnabled() )
            m_btnRightTab.setEnabled( true );
        break;
    }

    update();
}
