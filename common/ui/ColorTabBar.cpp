#include "ColorTabBar.h"

#include <QPainter>
#include <QtEvents>


static const QRgb g_TabDefaultColor[5] =
{
    qRgba(237, 28, 36, 200),
    qRgba(255, 127, 39, 200),
    qRgba(34, 177, 36, 200),
    qRgba(0, 162, 232, 200),
    qRgba(63, 72, 204, 200)
};

QColorTabBar::QColorTabBar(QWidget *parent) :
    QTabBar(parent)
{
    m_bNotify = false;
    m_nNotifyIndex = -1;
    m_nActiveIndex = -1;
    m_nHoverIndex = -1;
    m_nBlinkCount = 0;

    m_bHorz = true;

    setMouseTracking( true );
    setAttribute( Qt::WA_Hover );

    installEventFilter( this );

    connect( this, &QTabBar::currentChanged, this, &QColorTabBar::onCurrentChanged );
}

QColorTabBar::~QColorTabBar()
{

}

void QColorTabBar::paintEvent(QPaintEvent *)
{
    //QTabBar::paintEvent(evt);

    QPainter painter;
    painter.begin( this );

    int nDiameter = 3 << 1;
    for( int i=0 ;i<count(); i++ )
    {
        QRect rectTab( tabRect(i).adjusted( 0, 0, -1, 0 ) );

        bool bActive = (m_nActiveIndex == i);
        if( bActive ) rectTab.adjust( 0, 0, 0, 1 );
        bool bHover = (m_nHoverIndex == i);
        bool bNotify = (m_nNotifyIndex == i);

        QPainterPath tabPath;
        QPainterPath tabS;

        QRectF arcRect( 0, 0, nDiameter, nDiameter );

        if (m_bHorz)
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
            //tabS.closeSubpath();
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
            //tabS.closeSubpath();
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

        painter.fillPath( tabPath, QBrush(colorBody) );

        QColor colorStart = bActive ? g_TabDefaultColor[i] : (bHover ? QColor(255, 190, 60, 200) : QColor(255, 255, 255, 200));
        QColor colorEnd(255, 255, 255, 200);
        QRectF rectTabTip;
        rectTabTip = tabS.boundingRect();
        QLinearGradient gradTip;
        if (m_bHorz)
        {
            gradTip.setStart(rectTabTip.center().x(), rectTabTip.top());
            gradTip.setFinalStop(rectTabTip.center().x(), rectTabTip.bottom());
        }
        else
        {
            gradTip.setStart(rectTabTip.left(), rectTabTip.center().y());
            gradTip.setFinalStop(rectTabTip.right(), rectTabTip.center().y());
        }
        gradTip.setColorAt( 0, colorStart );
        gradTip.setColorAt( 1.f, colorEnd );

        painter.setBrush(Qt::NoBrush);
        painter.setPen( QPen(QColor(160,160,160,100), 2.f) );
        painter.drawPath( tabPath );
        painter.setPen( QPen(QColor(160,160,160)) );
        painter.drawPath( tabPath );
        painter.setPen( Qt::white );
        if( bActive )
            painter.drawLine( rectTab.bottomLeft(), rectTab.bottomRight() );

        painter.fillPath( tabS, QBrush(gradTip) );
        if (bActive || bHover)
        {
            painter.setPen( colorStart );
            painter.drawPath( tabS );
        }

        QRectF rectText;

        float fTextOffset = 0.f;

        if (m_bHorz)
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

        QFont fnt( font() );

        fnt.setBold(bActive);

        painter.setFont( fnt );

        int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;
        painter.setPen( QColor(80,80,80) );
        painter.drawText( rectText, flags, tabText(i) );
    }

    painter.end();
}

bool QColorTabBar::eventFilter(QObject *target, QEvent *evt)
{
    if( target == this )
    {
        if( evt->type() == QEvent::HoverMove )
        {
            for( int i=0; i<count(); i++ )
            {
                QMouseEvent* pEvt = (QMouseEvent*)evt;
                if( tabRect(i).contains(pEvt->pos()) )
                {
                    m_nHoverIndex = i;
                    setCursor( Qt::PointingHandCursor );
                    break;
                }
            }
        }
        else if( evt->type() == QEvent::HoverLeave )
        {
            m_nHoverIndex = -1;
            setCursor( Qt::ArrowCursor );
        }

        update();
    }

    return QTabBar::eventFilter(target, evt);
}

void QColorTabBar::onNotifyTab(int index)
{
    if( m_nNotifyIndex == index )
        return;


}

void QColorTabBar::onCurrentChanged(int index)
{
    m_nActiveIndex = index;
}
