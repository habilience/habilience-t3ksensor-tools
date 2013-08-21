#include "ResizingGraphicsItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>


QResizingGraphicsItem::QResizingGraphicsItem(int nW, int nH, QEdgeResizeGrabber::EdgePosType type, int nGrabberHorSize, int nGrabberVerSize, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    m_rcBoundary.setRect( 0, 0, nW, nH );
    m_nGrabberHorSize = nGrabberHorSize;
    m_nGrabberVerSize = nGrabberVerSize;

    m_bSelected = false;

    setAcceptHoverEvents( true );

    for( int i=0; i<8; i++ )
    {
        QEdgeResizeGrabber::EdgePosType t = (QEdgeResizeGrabber::EdgePosType)(type & (0x00000001<<(i*4)));
        if( t == 0 ) continue;

        QEdgeResizeGrabber* grabber = new QEdgeResizeGrabber( t, 5, 5, this );
        connect( this, &QResizingGraphicsItem::Selected, grabber, &QEdgeResizeGrabber::onShow, Qt::QueuedConnection );
        connect( this, &QResizingGraphicsItem::UpdatePos, grabber, &QEdgeResizeGrabber::onUpdatePos );
        m_listResizeGrabber.push_back( grabber );
    }
}

QRectF QResizingGraphicsItem::boundingRect() const
{
    return m_rcBoundary;
}

void QResizingGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    //if( m_bSelected )
    {
        //painter->setCompositionMode( QPainter::RasterOp_SourceXorDestination );

        QPen SelectPen( Qt::SolidLine );
        SelectPen.setWidth( 5 );
        SelectPen.setColor( qRgb(255, 255, 180) );  /*PS_GEOMETRIC*/
        //CBrush SelectBrush( qRgb(112, 154, 209) );

        painter->setPen( SelectPen );
        painter->setBrush( Qt::NoBrush );

        QRectF rc( boundingRect() );
        rc.adjust( 4,4,-4,-4 );

        painter->drawRect( rc );
    }

    painter->restore();
}

void QResizingGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    for( int i=0; i<m_listResizeGrabber.size(); i++ )
    {
        QEdgeResizeGrabber* grabber = m_listResizeGrabber.at(i);
        if( grabber )
            grabber->installSceneEventFilter( this );
    }
}

void QResizingGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    for( int i=0; i<m_listResizeGrabber.size(); i++ )
    {
        QEdgeResizeGrabber* grabber = m_listResizeGrabber.at(i);
        if( grabber )
            grabber->removeSceneEventFilter( this );
    }
}

void QResizingGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF rc( m_rcBoundary );
    rc.adjust( 6,6,-6,-6 );
    if( rc.contains( event->pos() ) )
        event->setAccepted( false );
    else
        event->setAccepted( true );
}

void QResizingGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF rc( m_rcBoundary );
    rc.adjust( 6,6,-6,-6 );
    if( rc.contains( event->pos() ) )
        event->setAccepted( false );
    else
        event->setAccepted( true );
}

void QResizingGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF rc( m_rcBoundary );
    rc.adjust( 6,6,-6,-6 );
    if( rc.contains( event->pos() ) )
        event->setAccepted( false );
    else
        event->setAccepted( true );
}

void QResizingGraphicsItem::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted( false );
}

void QResizingGraphicsItem::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted( false );
}
#include <QDebug>
bool QResizingGraphicsItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    QEdgeResizeGrabber* grabber = dynamic_cast<QEdgeResizeGrabber*>(watched);
    if( grabber == NULL ) return false;

    QGraphicsSceneMouseEvent* mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL )
        return false;

    switch( event->type() )
    {
        case QEvent::GraphicsSceneMousePress:
            {
                grabber->setMouseState(QEdgeResizeGrabber::stateMDown);
                grabber->mouseDownX = mevent->pos().x();
                grabber->mouseDownY = mevent->pos().y();

                m_rcOldBoundary = sceneBoundingRect();;
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                grabber->setMouseState(QEdgeResizeGrabber::stateMRelease);

                emit UpdateSize(m_rcOldBoundary.adjusted(5,5,-5,-5), sceneBoundingRect().adjusted(5,5,-5,-5));
            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                grabber->setMouseState(QEdgeResizeGrabber::stateMMove);
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
            break;
    }


    if( grabber->getMouseState() == QEdgeResizeGrabber::stateMMove )
    {
        qreal x = mevent->pos().x(), y = mevent->pos().y();

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( grabber->getPos() )
        {
        case QEdgeResizeGrabber::edgeLeftTop:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;

        case QEdgeResizeGrabber::edgeRightTop:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;

        case QEdgeResizeGrabber::edgeRightBottom:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;

        case QEdgeResizeGrabber::edgeLeftBottom:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;
        case QEdgeResizeGrabber::edgeLeftMiddle:
            {
                XaxisSign = +1;
            }
            break;
        case QEdgeResizeGrabber::edgeTopMiddle:
            {
                YaxisSign = +1;
            }
            break;
        case QEdgeResizeGrabber::edgeRightMiddle:
            {
                XaxisSign = -1;
            }
            break;
        case QEdgeResizeGrabber::edgeBottomMiddle:
            {
                YaxisSign = -1;
            }
            break;
        }

        int xMoved = grabber->mouseDownX - x;
        int yMoved = grabber->mouseDownY - y;

        int newWidth = m_rcBoundary.width() + ( XaxisSign * xMoved);
        if ( newWidth < 40 ) newWidth  = 40;

        int newHeight = m_rcBoundary.height() + (YaxisSign * yMoved) ;
        if ( newHeight < 40 ) newHeight = 40;

        int deltaWidth  =   newWidth - m_rcBoundary.width() ;
        int deltaHeight =   newHeight - m_rcBoundary.height() ;

        //prepareGeometryChange();

        m_rcBoundary.setWidth( m_rcBoundary.width() + deltaWidth );
        m_rcBoundary.setHeight( m_rcBoundary.height() + deltaHeight );

        qDebug() << "Mouse 2: " + QString("%1,%2").arg(m_rcBoundary.width()).arg(m_rcBoundary.height());

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        switch( grabber->getPos() )
        {
        case QEdgeResizeGrabber::edgeLeftTop:
            {
                setPos(pos().x() + deltaWidth, pos().y() + deltaHeight);
            }
            break;

        case QEdgeResizeGrabber::edgeRightTop:
            {
                setPos(pos().x(), pos().y() + deltaHeight);
            }
            break;

        case QEdgeResizeGrabber::edgeRightBottom:
            {
            }
            break;

        case QEdgeResizeGrabber::edgeLeftBottom:
            {
                setPos(pos().x() + deltaWidth, pos().y());
            }
            break;
        case QEdgeResizeGrabber::edgeLeftMiddle:
            {
                setPos(pos().x() + deltaWidth, pos().y());
            }
            break;
        case QEdgeResizeGrabber::edgeTopMiddle:
            {
                setPos(pos().x(), pos().y() + deltaHeight);
            }
            break;
        case QEdgeResizeGrabber::edgeRightMiddle:
            {
            }
            break;
        case QEdgeResizeGrabber::edgeBottomMiddle:
            {
            }
            break;
        }

        emit UpdatePos( m_rcBoundary.adjusted( 5,5,-5,5 ) );

        update();
    }

    return true;
}

void QResizingGraphicsItem::onSelected(bool bSelected)
{
    m_bSelected = bSelected;

    emit Selected(m_bSelected);

    update();
}

void QResizingGraphicsItem::onUpdateSize(int nW, int nH)
{
    prepareGeometryChange();

    m_rcBoundary.setWidth( nW );
    m_rcBoundary.setHeight( nH );

    emit UpdatePos( m_rcBoundary.adjusted( 5,5,-5,-5 ).toRect() );

    update();
}
