#include "GraphicsButtonItem.h"

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

QGraphicsButtonItem::QGraphicsButtonItem(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    m_penBorder.setStyle( Qt::SolidLine );
    m_penBorder.setColor( qRgb(200, 200, 200) );
    m_penBorder.setWidth( 1 );

    m_brushFill.setStyle( Qt::SolidPattern );
    m_brushFill.setColor( qRgb(180, 180, 180) );

    setCursor( Qt::PointingHandCursor );
    setAcceptedMouseButtons( Qt::LeftButton );

    m_bPress = false;
    m_bInvert = false;

    m_clrMark = qRgb(0, 0, 0);
}

QGraphicsButtonItem::~QGraphicsButtonItem()
{
}

QRectF QGraphicsButtonItem::boundingRect() const
{
    return m_boundingRect;
}

void QGraphicsButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QRectF rc( boundingRect() );
    rc = rc.normalized();

    painter->setPen( m_penBorder );
    painter->setBrush( m_brushFill );

    painter->drawRoundRect( rc, 7, 7 );

    QPoint ptC( rc.center().toPoint() );
    int nWH = rc.width() / 4;
    int nC = 4;
    QPainterPath path;
    path.moveTo( ptC.x()-nWH+nC, ptC.y()-nWH );
    path.lineTo( ptC.x(), ptC.y()-nC );
    path.lineTo( ptC.x()+nWH-nC, ptC.y()-nWH );
    path.lineTo( ptC.x()+nWH, ptC.y()-nWH+nC );
    path.lineTo( ptC.x()+nC, ptC.y() );
    path.lineTo( ptC.x()+nWH, ptC.y()+nWH-nC );
    path.lineTo( ptC.x()+nWH-nC, ptC.y()+nWH );
    path.lineTo( ptC.x(), ptC.y()+nC );
    path.lineTo( ptC.x()-nWH+nC, ptC.y()+nWH );
    path.lineTo( ptC.x()-nWH, ptC.y()+nWH-nC );
    path.lineTo( ptC.x()-nC, ptC.y() );
    path.lineTo( ptC.x()-nWH, ptC.y()-nWH+nC );
    path.closeSubpath();

    painter->setBrush( QBrush( m_clrMark ) );
    painter->drawPath( path );

    painter->restore();
}

void QGraphicsButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_brushFill.setColor( qRgb(237, 28, 36) );
    update();

    m_bPress = true;

    event->setAccepted( true );
    //QGraphicsItem::mousePressEvent(event);
}

void QGraphicsButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_brushFill.setColor( m_bInvert ? qRgb(230, 230, 230) : qRgb(100, 100, 100) );
    update();

    if( m_bPress )
        emit Clicked();

    m_bPress = false;

    event->setAccepted( true );
    //QGraphicsItem::mouseReleaseEvent(event);
}

void QGraphicsButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_penBorder.setColor( qRgb(237, 28, 36) );
    update();

    event->setAccepted( true );
    //QGraphicsItem::hoverEnterEvent(event);
}

void QGraphicsButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_penBorder.setColor( qRgb(200, 200, 200) );
    update();

    event->setAccepted( true );
    //QGraphicsItem::hoverLeaveEvent(event);
}

void QGraphicsButtonItem::onUpdateSize(QRectF rc)
{
    prepareGeometryChange();

    m_boundingRect = rc;

    update();
}

void QGraphicsButtonItem::onUpdateInvert(bool bInvert)
{
    m_bInvert = bInvert;

    m_brushFill.setColor( m_bInvert ? qRgb(230, 230, 230) : qRgb(100, 100, 100) );

    m_clrMark = bInvert ? qRgb(100, 100, 100) : qRgb(0, 0, 0);
}
