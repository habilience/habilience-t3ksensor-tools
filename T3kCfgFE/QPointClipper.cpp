#include "QPointClipper.h"

#define EDGE_LEFT   (0)
#define EDGE_RIGHT  (1)
#define EDGE_BOTTOM (2)
#define EDGE_TOP    (3)

QPointClipper::QPointClipper(QObject *parent) :
    QObject(parent)
{
}

bool QPointClipper::ptInBound( float x, float y, int side )
{
    switch (side)
    {
    case EDGE_LEFT:
        if (x > m_rcBound.left()) return true;
        break;
    case EDGE_TOP:
        if (y > m_rcBound.top()) return true;
        break;
    case EDGE_RIGHT:
        if (x < m_rcBound.right()) return true;
        break;
    case EDGE_BOTTOM:
        if (y < m_rcBound.bottom()) return true;
        break;
    }
    return false;
}

int QPointClipper::getCondition( float x1, float y1, float x2, float y2, int side )
{
    bool c1 = ptInBound(x1, y1, side);
    bool c2 = ptInBound(x2, y2, side);
    if (!c1 && c2) return 1;
    if (c1 && c2) return 2;
    if (c1 && !c2) return 3;
    return 4;
}

QPointF QPointClipper::getIntersection( int side, float x1, float y1, float x2, float y2 )
{
    float m = 0.f;
    QPointF point;
    if (x2 != x1)
        m = (y2-y1) / (x2-x1);

    switch (side)
    {
    case EDGE_LEFT:
        point.setX( m_rcBound.left() );
        point.setY( y1 + m*(point.x()-x1) );
        break;
    case EDGE_RIGHT:
        point.setX( m_rcBound.right() );
        point.setY( y1 + m*(point.x()-x1) );
        break;
    case EDGE_BOTTOM:
        point.setY( m_rcBound.bottom() );
        if ( x1 != x2 )
            point.setX( x1 + (1/m) * (point.y() - y1) );
        else
            point.setX( x1 );
        break;
    case EDGE_TOP:
        point.setY( m_rcBound.top() );
        if ( x1 != x2 )
            point.setX( x1 + (1/m) * (point.y() - y1) );
        else
            point.setX( x1 );
        break;
    }
    return point;
}

void QPointClipper::addToClipped( QPointF point )
{
    m_ClippedPolygon.push_back(point);
}

void QPointClipper::reset()
{
    m_Polygon.clear();
    m_ClippedPolygon.clear();
}

void QPointClipper::trimToClipped()
{
    if (m_Polygon.isEmpty())
        return;
    QPointF pt = m_Polygon.at(0);
    addToClipped(pt);
    QPointF ptPrev = pt;
    for( int i=1 ; i<m_Polygon.size();i++ ) {
        pt = m_Polygon.at(i);
        if (ptPrev == pt)
            continue;
        addToClipped(pt);
        ptPrev = pt;
    }
    m_Polygon.clear();
}

void QPointClipper::addPoint(QPointF point)
{
    m_Polygon.push_back(point);
}

void QPointClipper::closePolygon()
{
    const QPointF& ptLast = m_Polygon.last();
    const QPointF& ptFirst = m_Polygon.first();
    if (ptLast == ptFirst)
        return;
    QPointF ptNew(ptFirst.x(), ptFirst.y());
    m_Polygon.push_back(ptNew);
}

bool QPointClipper::clip( QRectF rc )
{
    m_rcBound = rc;
    m_ClippedPolygon.clear();

    for ( int side=EDGE_LEFT ; side<=EDGE_TOP ; side++ )
    {
        int count = m_Polygon.size();
        QPointF pointInterX;
        float x1, y1, x2, y2;
        int condition;

        for ( int i=0 ; i<count-1 ; i++ )
        {
            x1 = m_Polygon.at(i).x();
            y1 = m_Polygon.at(i).y();
            x2 = m_Polygon.at(i+1).x();
            y2 = m_Polygon.at(i+1).y();
            condition = getCondition(x1, y1, x2, y2, side);
            switch ( condition )
            {
            case 1:
                pointInterX = getIntersection(side, x1, y1, x2, y2);
                addToClipped(pointInterX);
                addToClipped(x2, y2);
                break;
            case 2:
                addToClipped(x1, y1);
                addToClipped(x2, y2);
                break;
            case 3:
                pointInterX = getIntersection(side, x1, y1, x2, y2);
                addToClipped(x1, y1);
                addToClipped(pointInterX);
                break;
            case 4:
                break;
            }
        }

        if ( m_ClippedPolygon.size() > 0 )
        {
            addToClipped(m_ClippedPolygon.at(0).x(), m_ClippedPolygon.at(0).y());
        }
        m_Polygon.clear();
        for (int i=0 ; i<m_ClippedPolygon.size() ; i++)
            m_Polygon.push_back( m_ClippedPolygon.at(i) );
        m_ClippedPolygon.clear();
    }

    trimToClipped();

    return true;
}

int QPointClipper::getClippedPointCount()
{
    return m_ClippedPolygon.size();
}

QPointF QPointClipper::getClippedPoint(int i)
{
    return m_ClippedPolygon.at(i);
}
