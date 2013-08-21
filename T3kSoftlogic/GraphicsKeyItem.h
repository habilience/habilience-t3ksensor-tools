#ifndef QGraphicsKeyItem_H
#define QGraphicsKeyItem_H

#include <QGraphicsItem>
#include <QTextOption>
#include <QPen>
#include <QBrush>

class QKeyDesignWidget;
class QGraphicsKeyItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    explicit QGraphicsKeyItem(QKeyDesignWidget* pDesign, QGraphicsItem *parent = 0);
    QGraphicsKeyItem(QKeyDesignWidget* pDesign, bool bShow, qreal dW, qreal dH, int nFontHeight, QGraphicsItem *parent = 0);
    virtual ~QGraphicsKeyItem();

    virtual QRectF boundingRect() const;

    void setEnableMoving(bool bEnable);

    short getID() { return m_nID; }
    void setID(int nID) { m_nID = nID; }

    QString getName() { return m_strName; }
    void setName(QString strName) { m_strName = strName; }

    bool isGroup() { return m_bGroup; }

protected:
    // QGraphicsItem
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    // QObject



    void drawOutlineText( QPainter* painter, QRgb dwTextColor, QRgb dwOutlineColor, QString strText, QRect rc, QTextOption nFormat );

protected:
    QKeyDesignWidget*       m_pDesigner;
    short       m_nID;
    QString     m_strName;
    bool        m_bPress;

    bool        m_bShow;
    bool        m_bInvert;

    qreal       m_dWidth;
    qreal       m_dHeight;
    QRgb        m_clrKeyBorder;

    QPen        m_penBorder;
    QBrush      m_brushKey;
    int         m_nFontHeight;

    bool        m_bGroup;

    QPointF     m_ptPrevPos;

signals:
    void Editing(bool bEditing);

public slots:
    void onUpdaterGeometry(qreal nW, qreal nH, int nFontHeight = 0);
    void onInvertDrawing(bool bInvert);
    void onUpdateEnable(bool bShow);
    void onUpdateColor(QRgb color);
};

#endif // QGraphicsKeyItem_H
