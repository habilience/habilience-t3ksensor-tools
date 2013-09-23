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
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void timerEvent(QTimerEvent *);

protected:
    bool            m_bNotify;
    int             m_nNotifyIndex;
    int             m_nActiveIndex;
    int             m_nHoverIndex;
    int             m_nBlinkCount;

    bool            m_bHorz;

    int             m_nBlinkIndex;
    bool            m_bBlinkFalg;
    int             m_nTimerBlink;

signals:

public slots:
    void onNotifyTab(int index);
    void onCurrentChanged(int index);

};

#endif // QCOLORTABBAR_H
