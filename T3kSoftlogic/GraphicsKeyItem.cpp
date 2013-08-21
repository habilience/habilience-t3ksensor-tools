#include "GraphicsKeyItem.h"

#include "T3kCommonData.h"

#include "KeyDesignWidget.h"

#include <QPainter>
#include <QCursor>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#ifdef _DEBUG
#include <QDebug>
#endif


QGraphicsKeyItem::QGraphicsKeyItem(QKeyDesignWidget *pDesign, QGraphicsItem *parent) :
    QGraphicsItem(parent), m_pDesigner(pDesign)
{
    setCursor(Qt::ArrowCursor);
    m_bPress = false;
    m_bShow = true;
    m_bInvert = false;

    m_dWidth = 0;
    m_dHeight = 0;

    setFlags( ItemIsMovable|ItemIsSelectable );
    setAcceptedMouseButtons( Qt::NoButton );

    m_clrKeyBorder = qRgb(240, 240, 240);

    m_penBorder.setStyle( Qt::SolidLine );
    m_penBorder.setColor( m_clrKeyBorder );
    m_penBorder.setWidth( 1 );
    m_brushKey.setStyle( Qt::SolidPattern );
    m_brushKey.setColor( qRgb(180, 180, 180) );

    setAcceptHoverEvents( true );
}

QGraphicsKeyItem::QGraphicsKeyItem(QKeyDesignWidget *pDesign, bool bShow, qreal dW, qreal dH, int nFontHeight, QGraphicsItem *parent) :
    QGraphicsItem(parent), m_pDesigner(pDesign)
{
    setCursor(Qt::ArrowCursor);

    m_bShow = bShow;
    m_nFontHeight = nFontHeight;

    m_bPress = false;
    m_bInvert = false;

    m_dWidth = dW;
    m_dHeight = dH;

    setFlags( ItemIsMovable|ItemIsSelectable );
    setAcceptedMouseButtons( Qt::NoButton );

    m_clrKeyBorder = qRgb(240, 240, 240);

    m_penBorder.setStyle( Qt::SolidLine );
    m_penBorder.setColor( m_clrKeyBorder );
    m_penBorder.setWidth( 1 );
    m_brushKey.setStyle( Qt::SolidPattern );
    m_brushKey.setColor( qRgb(180, 180, 180) );

    setAcceptHoverEvents( true );
}

QGraphicsKeyItem::~QGraphicsKeyItem()
{

}

void QGraphicsKeyItem::setEnableMoving(bool bEnable)
{
    if( bEnable )
    {
        setAcceptedMouseButtons( Qt::LeftButton|Qt::RightButton );
    }
    else
    {
        setAcceptedMouseButtons( Qt::NoButton );
    }
}

QRectF QGraphicsKeyItem::boundingRect() const
{
    return QRectF( 0, 0, m_dWidth, m_dHeight );
}

void QGraphicsKeyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QRect rcKey( boundingRect().toRect() );

    qDebug() << QString("paint : %1,%2,%3,%4").arg(rcKey.left()).arg(rcKey.top()).arg(rcKey.width()).arg(rcKey.height());
    painter->setPen( m_penBorder );
    painter->setBrush( m_brushKey );

    //painter->fillRect( rcKey,  );
    painter->drawRect( rcKey );
    if( !m_bShow )
    {
        painter->save();

        QBrush brhHidden( Qt::BDiagPattern );
        brhHidden.setColor( m_bInvert ? Qt::white : Qt::black );
        painter->setBrush( brhHidden );
        painter->drawRect( rcKey );

        painter->restore();
    }

    if( option->state & QStyle::State_Selected )
    {
        painter->save();

        painter->setCompositionMode( QPainter::RasterOp_NotDestination ); // ?

        QRect rcSelected( rcKey );
        rcSelected.adjust( 5,5,-5,-5 );
        QPen penSelected( Qt::DashLine );
        penSelected.setColor( qRgb(77, 243, 109) );
        penSelected.setWidth( 2 );
        painter->setPen( penSelected );
        painter->setBrush( Qt::NoBrush );
        painter->drawRect( rcSelected );

        painter->restore();
    }

    rcKey.adjust( 5, 5, -5, -5 );
    if ( !m_strName.isEmpty() )
    {
        QFont fntText;
        if ( m_nFontHeight > 11 )
        {
            painter->save();

            fntText.setFamily( "Arial" );
            fntText.setPixelSize( m_nFontHeight );

            painter->setFont( fntText );
            QRect rcText( rcKey );
            rcText.setTop( rcKey.top() + rcKey.height()/2 );
            QTextOption option( Qt::AlignCenter );
            QFontMetrics fm( fntText );
            drawOutlineText( painter, qRgb(50,50,50), qRgb(255, 255, 255), fm.elidedText( m_strName, Qt::ElideRight, rcText.width(), Qt::TextSingleLine ), rcText, option );
            rcKey.setBottom ( rcText.top() );

            painter->restore();
        }
    }

    if ( m_nFontHeight > 12 )
    {
        QString strCaption = QString("%1").arg(m_nID+1);
        QTextOption option( Qt::AlignCenter );
        QFont ft( painter->font() );
        ft.setBold( true );
        painter->setFont( ft );
        drawOutlineText( painter, qRgb(0,0,0), qRgb(255, 255, 255), strCaption, rcKey, option );
    }

    painter->restore();
}

void QGraphicsKeyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_ptPrevPos = event->scenePos();

    update();

    QGraphicsItem::mousePressEvent(event);
}

void QGraphicsKeyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();

    QGraphicsItem::mouseReleaseEvent(event);
}

void QGraphicsKeyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pt( event->scenePos().x() - m_ptPrevPos.x(), event->scenePos().y() - m_ptPrevPos.y() );

//    m_pDesigner->onHideSelection( pt );

    m_ptPrevPos = event->scenePos();

    update();

    QGraphicsItem::mouseMoveEvent(event);
}

void QGraphicsKeyItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::PointingHandCursor);

    QGraphicsItem::hoverEnterEvent(event);
}

void QGraphicsKeyItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverLeaveEvent(event);
}

void QGraphicsKeyItem::drawOutlineText( QPainter* painter, QRgb dwTextColor, QRgb dwOutlineColor, QString strText, QRect rc, QTextOption nFormat )
{
    painter->save();

    painter->setPen( dwOutlineColor );

    QRect rcText( rc );

    rcText.adjust( -1, 0, -1, 0 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( +2, 0, +2, 0 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( -1, -1, -1, -1 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( 0, +2, 0, +2 );
    painter->drawText( rcText, strText, nFormat );

    painter->setPen( dwTextColor );
    painter->drawText( rc, strText, nFormat );

    painter->restore();
}

void QGraphicsKeyItem::onInvertDrawing(bool bInvert)
{
    m_bInvert = bInvert;
    m_penBorder.setColor( bInvert ? qRgb(56, 93, 138) : qRgb(240, 240, 240) );

    update();
}

void QGraphicsKeyItem::onUpdaterGeometry(qreal dW, qreal dH, int nFontHeight)
{
    if( nFontHeight > 0 )
        m_nFontHeight = nFontHeight;

    prepareGeometryChange();

    m_dWidth = dW;
    m_dHeight = dH;

    update();
}

void QGraphicsKeyItem::onUpdateEnable(bool bShow)
{
    m_bShow = bShow;

    update();
}

void QGraphicsKeyItem::onUpdateColor(QRgb color)
{
    m_brushKey.setColor( color );

    update();
}
