#include "QEventRedirection.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

QEventRedirection::QEventRedirection(QObject *parent) :
    QObject(parent)
{
    m_pListener = NULL;
    m_TimerSingleClickCheck = 0;
    m_bRButtonDblClicked = false;
}

bool QEventRedirection::eventFilter(QObject */*obj*/, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvt = (QKeyEvent*)evt;
        if (m_pListener)
        {
            //qDebug( "key press: %s", (const char*)obj->objectName().toLatin1() );
            if (m_pListener->onKeyPress(keyEvt))
                return true;
        }
    }
    else if (evt->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvt = (QKeyEvent*)evt;
        if (m_pListener)
        {
            //qDebug( "key release: %s", (const char*)obj->objectName().toLatin1() );
            if (m_pListener->onKeyRelease(keyEvt))
                return true;
        }
    }
    else if (evt->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvt = (QWheelEvent*)evt;
        if (m_pListener)
        {
            //qDebug( "wheel: %s", (const char*)obj->objectName().toLatin1() );
            if (m_pListener->onMouseWheel(wheelEvt))
                return true;
        }
    }
    else if (evt->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvt = (QMouseEvent*)evt;
        if (mouseEvt->button() == Qt::RightButton)
        {
            //qDebug( "r release: %s", (const char*)obj->objectName().toLatin1() );
            if (m_bRButtonDblClicked)
            {
                m_bRButtonDblClicked = false;
            }
            else
            {
                if (m_TimerSingleClickCheck)
                {
                    killTimer(m_TimerSingleClickCheck);
                }
                m_TimerSingleClickCheck = startTimer(300);
            }
        }
    }
    else if (evt->type() == QEvent::MouseButtonDblClick)
    {
        if (m_TimerSingleClickCheck)
        {
            killTimer(m_TimerSingleClickCheck);
            m_TimerSingleClickCheck = 0;
        }
        QMouseEvent* mouseEvt = (QMouseEvent*)evt;
        if (mouseEvt->button() == Qt::RightButton)
        {
            if (m_pListener)
            {
                //qDebug( "r dblclk: %s", (const char*)obj->objectName().toLatin1() );
                m_bRButtonDblClicked = true;
                m_pListener->onRButtonDblClicked();
            }
        }
    }

    return false;
}

void QEventRedirection::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerSingleClickCheck)
    {
        killTimer(m_TimerSingleClickCheck);
        m_TimerSingleClickCheck = 0;
        if (m_pListener)
        {
            //qDebug( "r clk" );
            m_pListener->onRButtonClicked();
        }
    }
}
