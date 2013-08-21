#ifndef EDGERESIZEGRABBER_H
#define EDGERESIZEGRABBER_H

#include <QGraphicsItem>

class QEdgeResizeGrabber : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum EdgePosType { edgeLeftTop=0x00000001, edgeRightTop=0x00000010, edgeRightBottom=0x00000100, edgeLeftBottom=0x0000001000,
                   edgeLeftMiddle=0x00010000, edgeTopMiddle=0x00100000, edgeRightMiddle=0x01000000, edgeBottomMiddle=0x10000000 };

    QEdgeResizeGrabber(EdgePosType ePos, int nW = 5, int nH = 5, QGraphicsItem* parent=0);

    enum MouseState { stateMRelease = 0, stateMDown, stateMMove };

    EdgePosType getPos() { return m_ePos; }
    MouseState getMouseState() { return m_eMouseState; }
    void setMouseState(MouseState eState) { m_eMouseState = eState; }
    //QPoint getPoint

    qreal mouseDownX;
    qreal mouseDownY;

protected:
    EdgePosType         m_ePos;
    MouseState      m_eMouseState;

    const int       m_nW;
    const int       m_nH;

private:
    // QGraphicsItem
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

signals:

public slots:
    void onShow(bool bShow);
    void onUpdatePos(QRectF rcItem);
};

#endif // EDGERESIZEGRABBER_H
