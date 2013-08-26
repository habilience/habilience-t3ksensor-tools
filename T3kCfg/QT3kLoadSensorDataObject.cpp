#include "QT3kLoadSensorDataObject.h"

#include "../common/T3kConstStr.h"

#include "CfgCustomCmdDef.h"
#include "QT3kUserData.h"

static const char cstrCam1Mode[] = "cam1/mdoe=";
static const char cstrCam2Mode[] = "cam2/mode=";
static const char cstrCam1_1Mode[] = "cam1/sub/mode=";
static const char cstrCam2_1Mode[] = "cam2/sub/mode=";
static const char cstrMode[] = "mode=";
static const char cstrSytaxError[] = "systax error";


QT3kLoadSensorDataObject::QT3kLoadSensorDataObject(T3kHandle*& pHandle, QObject *parent) :
    QT3kHIDObject(pHandle, parent)
{
    m_bFactoryCmd = false;
    m_nNVIndex = -1;
    m_eDataPart = DP_END;

    m_pStorageHandle = NULL;
}

QT3kLoadSensorDataObject::~QT3kLoadSensorDataObject()
{

}

bool QT3kLoadSensorDataObject::Start( SensorLogData* pStorage )
{
    if( !pStorage ) return false;
    if( !pStorage->CM.size() ) return false;

    StopAsyncTimeoutChecker();
    m_strSendCmd.clear();
    m_nSendCmdID = -1;

    m_bStart = false;
    m_pStorageHandle = pStorage;

    int nIdx = 0;
    for( nIdx = 0; nIdx<m_pStorageHandle->CM.size(); nIdx++ )
    {
        if( !m_pStorageHandle->CM.value( CM1+nIdx )->bNoCam )
            break;
    }

    if( nIdx < m_pStorageHandle->CM.size() )
        m_eDataPart = (eDataPart)((int)DP_CM1+nIdx);
    else
        m_eDataPart = DP_MM;

    bool bSub = false;
    short nModel = QT3kUserData::GetInstance()->GetModel();
    if( nModel != 0x0000 && nModel != 0x3000 && nModel != 0x3100 )
        bSub = true;
    int nMode = 0;
    if( m_pT3kHandle->QueryFirmwareVersion( PKT_ADDR_MM, &nMode ) )
    {
        if ( m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM1, &nMode) && bSub )
        {
            m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM1 | PKT_ADDR_CM_SUB, &nMode);
        }
        if ( m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM2, &nMode) && bSub )
        {
            m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM2 | PKT_ADDR_CM_SUB, &nMode);
        }
    }

    LoadNextCamData( m_eDataPart );

    return true;
}

void QT3kLoadSensorDataObject::LoadNextCamData(eDataPart /*ePart*/)
{
    m_bFactoryCmd = true;
    m_nNVIndex = -1;

    if( m_eDataPart < DP_MM )
    {
        int nCam = m_eDataPart-1;
        int nSub = (int)(nCam/2);
        m_strSendCmd = QString("cam%1/").arg(nCam%2+1);
        for( int i=0; i<nSub; i++ )
            m_strSendCmd += "sub/";
    }
    m_strSendCmd += "get_nv=-1";
    m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)m_strSendCmd.toUtf8().data(), true );

    StartAsyncTimeoutChecker( 1000 );
}

void QT3kLoadSensorDataObject::NextCommand()
{
    if( m_bFactoryCmd )
        m_nNVIndex--;
    else
        m_nNVIndex++;

    switch( m_eDataPart )
    {
    case DP_MM:
        m_strSendCmd = QString("get_nv=%1").arg(m_nNVIndex);
        break;
    default:
        {
            int nCam = m_eDataPart-1;
            int nSub = (int)(nCam/2);
            m_strSendCmd = QString("cam%1/").arg(nCam%2+1);
            for( int i=0; i<nSub; i++ )
                m_strSendCmd += "sub/";
            m_strSendCmd += QString("get_nv=%1").arg(m_nNVIndex);
        }
        break;
    }

    m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)m_strSendCmd.toUtf8().data(), true );
    Q_ASSERT( m_nSendCmdID >= 0 );

    StartAsyncTimeoutChecker( 1000 );
}

void QT3kLoadSensorDataObject::Push_Data(ResponsePart Part, PairRSP &stRSP, const char* sCmd)
{
    switch( Part )
    {
    case MM:
        {
            if( (strstr( sCmd, cstrFactorialScreenMargin ) == sCmd) ||
               (strstr( sCmd, cstrFactoryCalibration ) == sCmd) ||
               (strstr( sCmd, cstrFactorialSoftkeyCal ) == sCmd) ||
               (strstr( sCmd, cstrFactorialSoftkeyBind ) == sCmd) )
            {
                m_pStorageHandle->MM.FactoryMM.push_back( stRSP );
            }
            else if( (strstr( sCmd, cstrFactorialSoftkey ) == sCmd) ||
                     (strstr( sCmd, cstrFactorialSoftlogic ) == sCmd) )
            {
                stRSP.strData.replace( '\\', 'p' );
                m_pStorageHandle->MM.FactoryMM.push_back( stRSP );
            }
            else if( m_bFactoryCmd )
            {
                if( strstr( sCmd, cstrAdminSerial ) == sCmd )
                {
                    stRSP.strData.replace( '\"', "\\" );
                    stRSP.strData.replace( '\\', "\\\\" );
                    stRSP.strData.replace( '\/', "\\/" );
                    stRSP.strData.replace( '\b', "\\b" );
                    stRSP.strData.replace( '\f', "\\f" );
                    stRSP.strData.replace( '\n', "\\n" );
                    stRSP.strData.replace( '\r', "\\r" );
                    stRSP.strData.replace( '\t', "\\t" );
                    //stRSP.strData.replace( '\u', '\\"u' );
                }
                m_pStorageHandle->MM.FactoryMM.push_back( stRSP );
            }
            else if( (strstr( sCmd, cstrSoftkey ) == sCmd) ||
                     (strstr( sCmd, cstrSoftlogic ) == sCmd) )
            {
                stRSP.strData.replace( '\\', 'p' );
                m_pStorageHandle->MM.UserMM.push_back( stRSP );
            }
            else
                m_pStorageHandle->MM.UserMM.push_back( stRSP );
        }
        break;

    default:
        {
            if( m_pStorageHandle->CM.find( Part ) == m_pStorageHandle->CM.end() ) break;
            if( m_bFactoryCmd )
            {
                if( strstr( sCmd, cstrAdminSerial ) == sCmd )
                {
                    stRSP.strData.replace( '\"', "\\" );
                    stRSP.strData.replace( '\\', "\\\\" );
                    stRSP.strData.replace( '\/', "\\/" );
                    stRSP.strData.replace( '\b', "\\b" );
                    stRSP.strData.replace( '\f', "\\f" );
                    stRSP.strData.replace( '\n', "\\n" );
                    stRSP.strData.replace( '\r', "\\r" );
                    stRSP.strData.replace( '\t', "\\t" );
                    //stRSP.strData.replace( '\u', '\\"u' );
                }
                m_pStorageHandle->CM.value( Part )->FactoryCM.push_back( stRSP );
            }
            else
                m_pStorageHandle->CM.value( Part )->UserCM.push_back( stRSP );
        }
        break;
    }
}

void QT3kLoadSensorDataObject::OnRSP(ResponsePart Part, ushort, const char * /*sPartId*/, long lId, bool, const char *sCmd)
{
    if( m_nSendCmdID != lId ) return;

    if( strstr( sCmd, cstrInstantMode ) == sCmd ||
        strstr( sCmd, cstrMode ) == sCmd ||
        strstr( sCmd, cstrCam1Mode ) == sCmd ||
        strstr( sCmd, cstrCam2Mode ) == sCmd ||
        strstr( sCmd, cstrCam1_1Mode ) == sCmd ||
        strstr( sCmd, cstrCam2_1Mode ) == sCmd )
    {
        return;
    }

    StopAsyncTimeoutChecker();
    m_strSendCmd.clear();
    m_nSendCmdID = -1;

    const char* pszData = strchr( sCmd, '=' );
    if( !pszData ) return;

    QString strCmd( sCmd );
    QString strKey( strCmd.left(strCmd.indexOf( '=' )) );

    pszData++;

    PairRSP stRSP;
    stRSP.strKey = strKey;
    stRSP.strData = pszData;

    Push_Data( Part, stRSP, sCmd );

    NextCommand();
}

void QT3kLoadSensorDataObject::OnRSE(ResponsePart Part, ushort, const char * sPartId, long lId, bool, const char *sCmd)
{
    if( m_nSendCmdID != lId ) return;

    if( strstr( sCmd, cstrNoCam ) == sCmd )
    {
        if( Part != (ResponsePart)m_eDataPart ) return;

        CMLogDataGroup* pCM = m_pStorageHandle->CM.value( Part );
        if( !pCM )
            pCM = new CMLogDataGroup;
        pCM->bNoCam = true;
        m_pStorageHandle->RSE.push_back( QString("%1:%2").arg(sPartId).arg(sCmd) );
        return;
    }
    else if( strstr( sCmd, cstrSytaxError ) == sCmd )
    {
        m_pStorageHandle->RSE.push_back( QString("%1:%2 - %3").arg(sPartId).arg(sCmd).arg(m_strSendCmd));
        NextCommand();
        return;
    }

    StopAsyncTimeoutChecker();
    m_strSendCmd.clear();
    m_nSendCmdID = -1;

    const char* pszData = strchr( sCmd, '=' );
    if( !pszData )
    {
        if( sCmd[0] == 0 )
        {
            if( m_bFactoryCmd )
            {
                m_bFactoryCmd = false;
                m_nNVIndex = 0;

                switch( m_eDataPart )
                {
                case DP_MM:
                    m_strSendCmd = "get_nv=0";
                    break;
                default:
                    {
                        int nCam = m_eDataPart-1;
                        int nSub = (int)(nCam/2);
                        m_strSendCmd = QString("cam%1/").arg(nCam%2+1);
                        for( int i=0; i<nSub; i++ )
                            m_strSendCmd += "sub/";

                        m_strSendCmd += "get_nv=0";
                    }
                    break;
                }

                m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)m_strSendCmd.toUtf8().data(), true );
                Q_ASSERT( m_nSendCmdID >= 0 );

                StartAsyncTimeoutChecker( 1000 );
            }
            else
            {
                if( m_eDataPart == DP_MM )
                {
                    QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_COMPLETE")) + "] " );
                    strLog += QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_DATA"));
                    emit PrintProgreeLog( strLog );

                    emit Complete( LDS_SENSORDATA );
                    m_bStart = false;
                }
                else
                {
                    do
                    {
                        m_eDataPart = (eDataPart)(m_eDataPart+1);

                        if( m_pStorageHandle->CM.size() > 2 )
                        {
                            if( (int)m_eDataPart > (int)DP_CM2_1 )
                            {
                                m_eDataPart = DP_MM;
                                break;
                            }
                        }
                        else
                        {
                            if( (int)m_eDataPart > (int)DP_CM2 )
                            {
                                m_eDataPart = DP_MM;
                                break;
                            }
                        }

                        if( !m_pStorageHandle->CM.value( m_eDataPart )->bNoCam )
                            break;
                    } while( true );

                    LoadNextCamData( m_eDataPart );
                    // Next CM : m_nTimerLoadData = startTimer( 1 );
                }
            }
        }
        return;
    }

    m_pStorageHandle->RSE.push_back( sCmd );
}

void QT3kLoadSensorDataObject::OnVER(ResponsePart Part, ushort, const char * /*sPartId*/, T3kVER &VER)
{
    switch( Part )
    {
    case MM:
        {
            m_pStorageHandle->MM.VerInfo.strNV = QString("%1").arg(VER.nv);
            m_pStorageHandle->MM.VerInfo.strVer = QString("%1.%2").arg(VER.major, 0, 16).arg(VER.minor, 0, 16);
            m_pStorageHandle->MM.VerInfo.strModel = QString("%1").arg(VER.model, 0, 16);
            m_pStorageHandle->MM.VerInfo.strDateTime = QString("%1 %2").arg(QString((const char*)VER.date)).arg(QString((const char*)VER.time));
        }
        break;
    default:
        {
            Q_ASSERT( (int)Part-1 >= 0 );
            if( m_pStorageHandle->CM.find( Part ) == m_pStorageHandle->CM.end() ) break;
            m_pStorageHandle->CM.value( Part )->VerInfo.strNV = QString("%1").arg(VER.nv);
            m_pStorageHandle->CM.value( Part )->VerInfo.strVer = QString("%1.%2").arg(VER.major, 0, 16).arg(VER.minor, 0, 16);
            m_pStorageHandle->CM.value( Part )->VerInfo.strModel = QString("%1").arg(VER.model, 0, 16);
            m_pStorageHandle->CM.value( Part )->VerInfo.strDateTime = QString("%1 %2").arg(QString((const char*)VER.date)).arg(QString((const char*)VER.time));
        }
        break;
    }
}
