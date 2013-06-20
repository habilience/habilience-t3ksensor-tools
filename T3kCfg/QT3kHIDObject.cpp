#include "QT3kHIDObject.h"

#define CHECK_TIMEOUT_COUNT     3

QT3kHIDObject::QT3kHIDObject(T30xHandle*& pHandle, QObject *parent) :
    QObject(parent), m_pT3kHandle(pHandle)
{
    m_nSendCmdID = -1;
    m_nTimeOutCount = 0;

    m_bStart = false;
}

QT3kHIDObject::~QT3kHIDObject()
{
}

void QT3kHIDObject::OnChangeLanguage()
{

}

void QT3kHIDObject::StartAsyncTimeoutChecker( long lTimeOut )
{
    if( m_Timer.isActive() )
    {
        m_Timer.stop();
        disconnect( &m_Timer, SIGNAL(timeout()), NULL, NULL );
    }
    connect( &m_Timer, SIGNAL(timeout()), this, SLOT(on_AsyncTimeout()) );

    m_Timer.start( lTimeOut );
}

void QT3kHIDObject::StopAsyncTimeoutChecker()
{
    m_Timer.stop();

    m_nTimeOutCount = 0;

    disconnect( &m_Timer, SIGNAL(timeout()), NULL, NULL );
}

void QT3kHIDObject::on_AsyncTimeout()
{
    if( m_strSendCmd.isEmpty() )
        StopAsyncTimeoutChecker();
    else
    {
        m_nTimeOutCount++;
        if( m_nTimeOutCount < CHECK_TIMEOUT_COUNT )
            m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)m_strSendCmd.toUtf8().data(), true );
        else
        {
            StopAsyncTimeoutChecker();
            emit CheckTimeOut();
        }
    }
}
