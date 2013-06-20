#ifndef QSIMPLEPROGRESSWIDGET_H
#define QSIMPLEPROGRESSWIDGET_H

#include <QProgressBar>

class QSimpleProgressWidget : public QProgressBar
{
    Q_OBJECT
public:
    explicit QSimpleProgressWidget(QWidget *parent = 0);

    void SetPos( int nPos );
    void SetRange( int nMin, int nMax ) { m_nMin = nMin; m_nMax = nMax; }

protected:
    virtual void paintEvent(QPaintEvent *evt);

protected:
    int		m_nMin;
    int		m_nMax;
    int		m_nPos;

signals:

public slots:

};

#endif // QSIMPLEPROGRESSWIDGET_H
