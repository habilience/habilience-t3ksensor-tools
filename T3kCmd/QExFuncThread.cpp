#include "QExFuncThread.h"

#include <QCoreApplication>

QExFuncThread::QExFuncThread(QObject *parent, FnExternalFunction fn, void* pFnExFuncContext) :
    QThread(parent),
    m_fnExFunc(fn)
{
    m_bTerminateFlag = false;

    if( pFnExFuncContext )
        m_pFnExFuncContext = pFnExFuncContext;
    else
        m_pFnExFuncContext = (void*)this;
}

void QExFuncThread::run()
{
    if( m_fnExFunc )
        m_fnExFunc(m_pFnExFuncContext);

    QMutexLocker Lock( &m_Mutex );
    m_bTerminateFlag = false;
}

void QExFuncThread::onStart()
{
    start();
}

void QExFuncThread::onStop()
{
    QMutexLocker Lock( &m_Mutex );
    m_bTerminateFlag = true;
}

void QExFuncThread::onExit()
{
    qApp->quit();
}
