#ifndef QARROWBUTTON_H
#define QARROWBUTTON_H

#include <QPushButton>

class QArrowButton : public QPushButton
{
    Q_OBJECT
public:
    enum Direction { DirectionLeft, DirectionUp, DirectionRight, DirectionDown };
private:
    Direction   m_direction;

    bool m_bIsHovered;
    bool m_bIsFocused;
    bool m_bIsSelected;

    QPolygonF   m_polygonArrow;
protected:
    void makeTriangle( const QRectF& rcBody, QPolygonF& polygonArrow, int offset );
public:
    explicit QArrowButton(QWidget *parent = 0);

    void setDirection(Direction dir) { m_direction = dir; }
    Direction direction() const { return m_direction; }

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual bool event(QEvent *evt);
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseReleaseEvent(QMouseEvent *evt);
    virtual bool eventFilter(QObject *obj, QEvent *evt);
    
signals:
    
public slots:
    
};

#endif // QARROWBUTTON_H
