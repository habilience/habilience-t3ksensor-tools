#ifndef QTIMEOUTCHECKER_H
#define QTIMEOUTCHECKER_H

#include <QObject>
#include <QVector>

class QTimeoutChecker : public QObject
{
    Q_OBJECT
public:
    explicit QTimeoutChecker(QObject *parent = 0);
    ~QTimeoutChecker();

    int Start( int nTimerout );
    void Stop( int nID = -1 ); // -1 : all

    bool IsRunning( int nID );

protected:
    virtual void timerEvent(QTimerEvent *);

protected:
    int             m_nTimerID;
    QVector<int>    m_vID;

signals:
    void TimeOut( int nID );

public slots:

};

#endif // QTIMEOUTCHECKER_H
