#include "QT3kDeviceEventHandler.h"
#include "../common/T3kConstStr.h"

#define PARTID_LENGTH		(8)
ResponsePart getResponsePartFromPartId( const char* partid, const char* cmd )
{
    ResponsePart Part = MM;

    const char* szCmd = cmd;

    int nPartIdIdx = (int)strlen( partid )-1;

    if( partid[nPartIdIdx-1] == '-' )
    {
        nPartIdIdx -= 2;
    }

    switch( partid[nPartIdIdx] )
    {
    default:
        if( szCmd != NULL )
        {
            if ( strstr(szCmd, cstrCam1) == szCmd )
            {
                szCmd += (sizeof(cstrCam1)-1);
                Part = CM1;
                break;
            }

            if ( strstr(szCmd, cstrCam2) == szCmd )
            {
                szCmd += (sizeof(cstrCam2)-1);
                Part = CM2;
                break;
            }

            // TODO : sub ?
            const char cstrSub[] = "sub/";
            if( strstr(szCmd, cstrSub) == szCmd )
            {
                break;
            }
        }
        else
        {
            Part = MM;
        }
        break;
    case '1':
        {
            int nCamSub = 0;
            if( PARTID_LENGTH > nPartIdIdx+2 )
                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');

            if( nCamSub > 0 )
                Part = CM1_1;
            else
                Part = CM1;
        }
        break;
    case '2':
        {
            int nCamSub = 0;
            if( PARTID_LENGTH > nPartIdIdx+2 )
                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');

            if( nCamSub > 0 )
                Part = CM2_1;
            else
                Part = CM2;
        }
        break;
    }

    return Part;
}

QT3kDeviceEventHandler::_gc::_gc() {}
QT3kDeviceEventHandler::_gc::~_gc()
{
    if (QT3kDeviceEventHandler::m_pThis)
        delete QT3kDeviceEventHandler::m_pThis;
    QT3kDeviceEventHandler::m_pThis = NULL;
}
QT3kDeviceEventHandler::_gc s_gc;



QT3kDeviceEventHandler* QT3kDeviceEventHandler::m_pThis = NULL;
QT3kDeviceEventHandler::QT3kDeviceEventHandler(QObject *parent) :
    QObject(parent)
{
}

QT3kDeviceEventHandler::~QT3kDeviceEventHandler()
{
    m_EventListener.clear();
}

QT3kDeviceEventHandler* QT3kDeviceEventHandler::getPtr()
{
    if (!m_pThis)
    {
        m_pThis = new QT3kDeviceEventHandler();
    }
    return m_pThis;
}

void QT3kDeviceEventHandler::addListener(IListener* l)
{
    for (int i=0 ; i<m_EventListener.size() ; i++ )
    {
        if (m_EventListener.at(i) == l)
            return;
    }
    m_EventListener.push_back(l);
}

void QT3kDeviceEventHandler::removeListener(IListener* l)
{
    for (int i=0 ; i<m_EventListener.size() ; i++ )
    {
        if (m_EventListener.at(i) == l)
        {
            m_EventListener.remove(i);
            return;
        }
    }
}

