#ifndef QPOINTCLIPPER_H
#define QPOINTCLIPPER_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QRectF>

class QPointClipper : public QObject
{
    Q_OBJECT
private:
    QVector<QPointF>    m_Polygon;
    QVector<QPointF>    m_ClippedPolygon;
    QRectF              m_rcBound;

protected:
    bool ptInBound( float x, float y, int side );
    int getCondition( float x1, float y1, float x2, float y2, int side );
    QPointF getIntersection( int side, float x1, float y1, float x2, float y2 );
    void addToClipped( float x, float y ) { addToClipped(QPointF(x,y)); }
    void addToClipped( QPointF point );
    void trimToClipped();
public:
    explicit QPointClipper(QObject *parent = 0);

    void addPoint(float x, float y) { addPoint(QPointF(x, y)); }
    void addPoint(QPointF point);
    void closePolygon();
    bool clip( float left, float top, float right, float bottom ) { return clip(QRectF(left, top, right-left, bottom-top)); }
    bool clip( QRectF rc );

    void reset();

    int getClippedPointCount();
    QPointF getClippedPoint(int i);
    const QPointF* getClippedPoints() const { return m_ClippedPolygon.constData(); }
    
signals:
    
public slots:
    
};

#endif // QPOINTCLIPPER_H
