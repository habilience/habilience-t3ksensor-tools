#include "EdgeResizeGrabber.h"

#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QCursor>

QEdgeResizeGrabber::QEdgeResizeGrabber(EdgePosType ePos, int nW, int nH, QGraphicsItem *parent) :
    QGraphicsItem(parent), m_nW(nW), m_nH(nH)
{
    setParentItem( parent );

    m_ePos = ePos;

    setAcceptHoverEvents( true );

    switch( m_ePos )
    {
    case edgeLeftTop:
        setCursor( Qt::SizeFDiagCursor );
        break;
    case edgeRightTop:
        setCursor( Qt::SizeBDiagCursor );
        break;
    case edgeRightBottom:
        setCursor( Qt::SizeFDiagCursor );
        break;
    case edgeLeftBottom:
        setCursor( Qt::SizeBDiagCursor );
        break;
    case edgeLeftMiddle:
        setCursor( Qt::SizeHorCursor );
        break;
    case edgeTopMiddle:
        setCursor( Qt::SizeVerCursor );
        break;
    case edgeRightMiddle:
        setCursor( Qt::SizeHorCursor );
        break;
    case edgeBottomMiddle:
        setCursor( Qt::SizeVerCursor );
        break;
    default:
        setCursor( Qt::ArrowCursor );
        break;
    }

    if( parent )
    {
        QRectF rc( parent->boundingRect() );
        int nR = rc.width()-nW/2;
        int nB = rc.height()-nH/2;
        switch( m_ePos )
        {
        default:
        case edgeLeftTop:
            setPos( rc.x(), rc.y() );
            break;
        case edgeRightTop:
            setPos( nR, rc.y() );
            break;
        case edgeRightBottom:
            setPos( nR, nB );
            break;
        case edgeLeftBottom:
            setPos( rc.x(), nB );
            break;
        case edgeLeftMiddle:
            setPos( rc.x(), rc.center().y() );
            break;
        case edgeTopMiddle:
            setPos( rc.center().x(), rc.y() );
            break;
        case edgeRightMiddle:
            setPos( nR, rc.center().y() );
            break;
        case edgeBottomMiddle:
            setPos( rc.center().x(), nB );
            break;
        }
    }
}

QRectF QEdgeResizeGrabber::boundingRect() const
{
    return QRectF(0,0,m_nW,m_nH);
}

void QEdgeResizeGrabber::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->save();

    QPen penBorder( Qt::SolidLine );
    penBorder.setWidth( 1 );
    penBorder.setColor( Qt::white );
    penBorder.setCapStyle(Qt::SquareCap);

    QBrush brushFill( Qt::SolidPattern );
    brushFill.setColor( Qt::black );

    painter->setPen( penBorder );
    painter->setBrush( Qt::black );

    painter->fillRect( 0, 0, m_nW, m_nH, Qt::black );
    painter->drawRect( 0, 0, m_nW, m_nH );

    painter->restore();
}

void QEdgeResizeGrabber::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted( false );
}

void QEdgeResizeGrabber::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted( true );
}

void QEdgeResizeGrabber::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted( false );
}

void QEdgeResizeGrabber::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted( false );
}

void QEdgeResizeGrabber::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted( false );
}

void QEdgeResizeGrabber::onShow(bool bShow)
{
    setVisible( bShow );
}


void QEdgeResizeGrabber::onUpdatePos(QRectF rcItem)
{
    qreal nL = rcItem.left();
    qreal nT = rcItem.top();
    qreal nR = rcItem.right();
    qreal nB = rcItem.bottom();
    qreal nCX = rcItem.center().x();
    qreal nCY = rcItem.center().y();

    switch( m_ePos )
    {
    default:
    case edgeLeftTop:
        setPos( nL, nT );
        break;
    case edgeRightTop:
        setPos( nR, nT );
        break;
    case edgeRightBottom:
        setPos( nR, nB );
        break;
    case edgeLeftBottom:
        setPos( nL, nB );
        break;
    case edgeLeftMiddle:
        setPos( nL, nCY );
        break;
    case edgeTopMiddle:
        setPos( nCX, nT );
        break;
    case edgeRightMiddle:
        setPos( nR, nCY );
        break;
    case edgeBottomMiddle:
        setPos( nCX, nB );
        break;
    }
}
