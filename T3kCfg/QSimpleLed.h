#ifndef QSIMPLELED_H
#define QSIMPLELED_H

#include <QLabel>
#include <QPixmap>

class QSimpleLed : public QLabel
{
    Q_OBJECT
public:
    explicit QSimpleLed(QWidget *parent = 0);

    enum eLEDStatus { SConnectON, SConnectOFF, SDisconnect };

    void SetStatus( eLEDStatus eStatus );

protected:
    virtual void paintEvent(QPaintEvent *);

protected:
    QPixmap             m_pixImage;
    eLEDStatus          m_eLEDStatus;

signals:

public slots:

};

#endif // CSIMPLELED_H
