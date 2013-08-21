#ifndef GRAPHICSBUTTONITEM_H
#define GRAPHICSBUTTONITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QPen>


class QGraphicsButtonItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    QGraphicsButtonItem(QGraphicsItem* parent=0);
    virtual ~QGraphicsButtonItem();

    virtual QRectF boundingRect() const;

protected:
    // QGraphicsItem
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

protected:
    QRectF              m_boundingRect;
    QPen                m_penBorder;
    QBrush              m_brushFill;

    bool                m_bPress;
    bool                m_bInvert;

    QRgb                m_clrMark;

signals:
    void Clicked();

public slots:
    void onUpdateSize(QRectF rc);
    void onUpdateInvert(bool bInvert);
};

#endif // GRAPHICSBUTTONITEM_H
