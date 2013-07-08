#ifndef QEXFUNCTHREAD_H
#define QEXFUNCTHREAD_H

#include <QThread>
#include <QMutex>

class QExFuncThread : public QThread
{
    Q_OBJECT
public:
    typedef int (*FnExternalFunction)(void* pContext);

    explicit QExFuncThread(QObject *parent = 0, FnExternalFunction fn = NULL, void* pFnExFuncContext = NULL);

    bool TerminateFlag() { QMutexLocker Lock( &m_Mutex ); return m_bTerminateFlag; }

protected:
    virtual void run();

protected:
    FnExternalFunction          m_fnExFunc;
    void*                       m_pFnExFuncContext;

    QMutex                      m_Mutex;
    volatile bool               m_bTerminateFlag;

signals:

public slots:
    void onStart();
    void onStop();
    void onExit();
};

#endif // QEXFUNCTHREAD_H
