#ifndef QCOLORTABBAR_H
#define QCOLORTABBAR_H

#include <QTabBar>

class QColorTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit QColorTabBar(QWidget *parent = 0);
    virtual ~QColorTabBar();

    void BlinkTab( int nIndex );

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);

protected:
    int                 m_nBlinkIndex;
    bool                m_bBlinkFalg;
    int                 m_nTimerBlink;

signals:

public slots:

};

#endif // QCOLORTABBAR_H
