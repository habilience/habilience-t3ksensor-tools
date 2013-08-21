#ifndef RESIZINGGRAPHICSITEM_H
#define RESIZINGGRAPHICSITEM_H

#include "EdgeResizeGrabber.h"

#include <QList>

class QResizingGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    QResizingGraphicsItem(int nW, int nH, QEdgeResizeGrabber::EdgePosType type = (QEdgeResizeGrabber::EdgePosType)0x11111111, int nGrabberHorSize = 5, int nGrabberVerSize = 5, QGraphicsItem* parent=0);

    // QGraphicsItem
    virtual QRectF boundingRect() const;

protected:
    // QGraphicsItem
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);

protected:
    QList<QEdgeResizeGrabber*>      m_listResizeGrabber;

    QRectF                          m_rcBoundary;
    QRectF                          m_rcOldBoundary;

    int                             m_nGrabberHorSize;
    int                             m_nGrabberVerSize;

    bool                            m_bSelected;

signals:
    void Selected(bool bSelected);
    void UpdateSize(QRectF rcOld, QRectF rcNew);
    void UpdatePos(QRectF rcNew);

public slots:
    void onSelected(bool bSelected);
    void onUpdateSize(int nW, int nH);
};

#endif // RESIZINGGRAPHICSITEM_H
