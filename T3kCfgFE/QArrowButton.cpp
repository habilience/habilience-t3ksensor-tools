#include "QArrowButton.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

QArrowButton::QArrowButton(QWidget *parent) :
    QPushButton(parent)
{
    m_bIsHovered = false;
    m_bIsFocused = false;
    m_bIsSelected = false;

    installEventFilter(this);
}

void QArrowButton::makeTriangle( const QRectF& rcBody, QPolygonF& polygonArrow, int offset )
{
    offset *= 2;
    switch (m_direction)
    {
    case DirectionLeft:
        polygonArrow.push_back(QPointF(rcBody.left()+offset, rcBody.center().y()));
        polygonArrow.push_back(QPointF(rcBody.right()-offset/2, rcBody.top()+offset));
        polygonArrow.push_back(QPointF(rcBody.right()-offset/2, rcBody.bottom()-offset));
        break;
    case DirectionUp:
        polygonArrow.push_back(QPointF(rcBody.center().x(), rcBody.top()+offset));
        polygonArrow.push_back(QPointF(rcBody.right()-offset, rcBody.bottom()-offset/2));
        polygonArrow.push_back(QPointF(rcBody.left()+offset, rcBody.bottom()-offset/2));
        break;
    case DirectionRight:
        polygonArrow.push_back(QPointF(rcBody.right()-offset, rcBody.center().y()));
        polygonArrow.push_back(QPointF(rcBody.left()+offset/2, rcBody.bottom()-offset));
        polygonArrow.push_back(QPointF(rcBody.left()+offset/2, rcBody.top()+offset));
        break;
    case DirectionDown:
        polygonArrow.push_back(QPointF(rcBody.center().x(), rcBody.bottom()-offset));
        polygonArrow.push_back(QPointF(rcBody.left()+offset, rcBody.top()+offset/2));
        polygonArrow.push_back(QPointF(rcBody.right()-offset, rcBody.top()+offset/2));
        break;
    }
}

void QArrowButton::paintEvent(QPaintEvent *)
{
    QRect rcBody(0, 0, width()-1, height()-1);

    rcBody.adjust( 2, 2, -2, -2 );

    QPainter p(this);

    p.save();
    p.setRenderHint(QPainter::Antialiasing);

    QColor clrActive = QColor(43, 140, 238);
    QColor clrText = QColor(100,100,100,200);
    QColor clrBorder = QColor(100,100,100);

    QRectF rcText = rcBody;
    switch (m_direction)
    {
    case DirectionLeft:
        rcText.setLeft( rcText.right() - rcText.width() * 3 / 4 );
        break;
    case DirectionUp:
        rcText.setTop( rcText.bottom() - rcText.height() * 3 / 4 );
        break;
    case DirectionRight:
        rcText.setRight( rcText.left() + rcText.width() * 3 / 4 );
        break;
    case DirectionDown:
        rcText.setBottom( rcText.top() + rcText.height() * 3 / 4 );
        break;
    }

    QPolygonF polygonArrow1;
    makeTriangle(rcBody, polygonArrow1, 0);
    m_polygonArrow = polygonArrow1;
    QPolygonF polygonArrow2;
    makeTriangle(rcBody, polygonArrow2, 1);
    QPolygonF polygonArrow3;
    makeTriangle(rcBody, polygonArrow3, 2);

    // draw background
    if (m_bIsHovered || m_bIsSelected || isChecked())
    {
        //clrActive.setAlpha( m_bIsSelected||isChecked() ? 100 : 50 );
        clrActive.setAlpha(0);
        p.setPen( Qt::NoPen );
        QRectF rcBound = polygonArrow1.boundingRect();
        QRadialGradient grad( rcBound.center(), rcBound.width()+rcBound.height() );
        grad.setColorAt(0, clrActive);
        clrActive.setAlpha(200);
        grad.setColorAt(1, clrActive);
        p.setBrush( grad );
        p.drawPolygon( polygonArrow1 );
    }

    // draw focus
    if (m_bIsFocused)
    {
        QPolygonF polygonArrow4;
        makeTriangle(rcBody, polygonArrow4, 4);

        p.setBrush( Qt::NoBrush );
        clrActive.setAlpha(5);
        p.setPen( QPen(clrActive, 8.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawPolygon( polygonArrow4 );
        clrActive.setAlpha(20);
        p.setPen( QPen(clrActive, 5.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawPolygon( polygonArrow3 );
        clrActive.setAlpha(80);
        p.setPen( QPen(clrActive, 3.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawPolygon( polygonArrow2 );
    }

    // draw border
    p.setBrush( Qt::NoBrush );
    if (m_bIsSelected)
    {
        clrBorder.setAlpha(10);
        p.setPen( QPen(clrBorder, 6.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawPolygon( polygonArrow3 );
        clrBorder.setAlpha(30);
        p.setPen( QPen(clrBorder, 3.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
        p.drawPolygon( polygonArrow2 );
    }
    clrBorder.setAlpha(100);
    p.setPen( QPen(clrBorder, 2.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) );
    p.drawPolygon( polygonArrow1 );

    if (m_bIsSelected)
        rcText.adjust( 1, 1, 1, 1 );

    QString strCaption = text();
    p.setPen( clrText );
    int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;
    p.drawText(rcText, flags, strCaption);

    p.restore();
}

bool QArrowButton::event(QEvent *evt)
{
    if (isEnabled())
    {
        switch (evt->type())
        {
        case QEvent::HoverEnter:
            {
                QHoverEvent* hoverEvt = (QHoverEvent*)evt;
                if (!m_bIsHovered && m_polygonArrow.containsPoint(hoverEvt->pos(), Qt::WindingFill))
                {
                    m_bIsHovered = true;
                    update();
                }
            }
            break;
        case QEvent::HoverLeave:
            {
                QHoverEvent* hoverEvt = (QHoverEvent*)evt;
                if (m_bIsHovered && !m_polygonArrow.containsPoint(hoverEvt->pos(), Qt::WindingFill))
                {
                    m_bIsHovered = false;
                    update();
                }
            }
            break;
        case QEvent::HoverMove:
            {
                QHoverEvent* hoverEvt = (QHoverEvent*)evt;
                bool bIsHovered;
                if (!m_polygonArrow.containsPoint(hoverEvt->pos(), Qt::WindingFill))
                {
                    bIsHovered = false;
                }
                else
                {
                    bIsHovered = true;
                }
                if (m_bIsHovered != bIsHovered)
                {
                    m_bIsHovered = bIsHovered;
                    update();
                }
            }
            break;
        case QEvent::FocusIn:
            m_bIsFocused = true;
            update();
            break;
        case QEvent::FocusOut:
            m_bIsFocused = false;
            update();
            break;
        default:
            break;
        }
    }
    else
    {
        m_bIsHovered = false;
        m_bIsFocused = false;
        m_bIsSelected = false;
    }

    return QPushButton::event(evt);
}

void QArrowButton::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        setFocus();
        m_bIsSelected = true;
        update();

        QPushButton::mousePressEvent(evt);
    }
}

void QArrowButton::mouseReleaseEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        m_bIsSelected = false;
        update();

        QPushButton::mouseReleaseEvent(evt);
    }
}

bool QArrowButton::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvt = (QMouseEvent*)evt;
        if (mouseEvt->button() == Qt::LeftButton)
        {
            if (!m_polygonArrow.containsPoint(mouseEvt->pos(), Qt::WindingFill))
            {
                return true;
            }
        }
    }

    return QPushButton::eventFilter(obj, evt);
}
