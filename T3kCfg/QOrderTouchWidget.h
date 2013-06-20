#ifndef QORDERTOUCHWIDGET_H
#define QORDERTOUCHWIDGET_H

#include <QWidget>
#include <QPixmap>

class QOrderTouchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QOrderTouchWidget(QWidget *parent = 0);
    ~QOrderTouchWidget();

    void Init();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *evt);
    virtual void hideEvent(QHideEvent *);

protected:
    QPoint                      m_ptOrder;
    bool                        m_bShowMark;
    bool                        m_bOnScreen;
    bool                        m_bTouch;
    int                         m_nPercent;

    bool                        m_bFlickerMark;

    int                         m_nTimerFlickerMark;

    QPixmap                     m_ArrowImage[4];

    bool                        m_bHideCursor;

signals:

public slots:
    void onOrderTouch( bool bShowMark, bool bOnScreen, bool bTouch, short nX, short nY, int nPercent );
};

#endif // QORDERTOUCHWIDGET_H
