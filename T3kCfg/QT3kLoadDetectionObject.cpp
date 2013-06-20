#include "QT3kLoadDetectionObject.h"

#include <QTimer>

QT3kLoadDetectionObject::QT3kLoadDetectionObject(T30xHandle *&pHandle, QObject *parent) :
    QT3kHIDObject(pHandle, parent)
{
    m_bMainCamDetection = false;
}

QT3kLoadDetectionObject::~QT3kLoadDetectionObject()
{
    m_DetectionWaitTimer.stop();
    disconnect( &m_DetectionWaitTimer, SIGNAL(timeout()), NULL, NULL );

    InitDetectionData();
    m_vDetectionData.clear();
}

void QT3kLoadDetectionObject::InitCamDataForDetection()
{
    foreach( CMLogDataGroup* pCM, m_pStorageHandle->CM.values() )
    {
        if( !pCM )
        {
            pCM = new CMLogDataGroup;
            pCM->bNoCam = true;
        }
        pCM->strIRD.clear();
        pCM->strITD.clear();
    }
}

void QT3kLoadDetectionObject::InitDetectionData()
{
    for( int i=0; i<m_vDetectionData.size(); i++ )
    {
        m_vDetectionData[i].nIRD = 0;
        if( m_vDetectionData[i].pIRD )
           delete[] m_vDetectionData[i].pIRD;
        m_vDetectionData[i].pIRD = NULL;
        if( m_vDetectionData[i].pITD )
           delete[] m_vDetectionData[i].pITD;
        m_vDetectionData[i].pITD = NULL;
    }
}

bool QT3kLoadDetectionObject::Start(SensorLogData *pStorage)
{
    if( m_bStart ) return false;
    if( !pStorage ) return false;

    m_bStart = true;
    m_bMainCamDetection = false;
    m_DetectionWaitTimer.stop();

    m_pStorageHandle = pStorage;

    m_vDetectionData.resize( m_pStorageHandle->CM.size() );
    for( int i=0; i<m_vDetectionData.size(); i++ )
    {
        m_vDetectionData[i].nIRD = 0;
        m_vDetectionData[i].pIRD = NULL;
        m_vDetectionData[i].pITD = NULL;
    }
    InitCamDataForDetection();

    m_pT3kHandle->SetReportView( true );

    connect( &m_DetectionWaitTimer, SIGNAL(timeout()), this, SLOT(on_DetectionCheckTimer()) );
    m_DetectionWaitTimer.start( 4000 );

    return true;
}

void QT3kLoadDetectionObject::OnIRD(ResponsePart Part, short, const char *, int, int nCount, unsigned char *pIRD)
{
    if ( nCount <= 0 )
        return;

    Q_ASSERT( (Part == CM1) || (Part == CM2) || (Part == CM1_1) || (Part == CM2_1) );

    int nCamPart = (int)(Part-1);
    Q_ASSERT( nCamPart >= 0 && nCamPart < m_pStorageHandle->CM.size() );
    if( m_vDetectionData.size() < nCamPart ) return;

    if ( m_vDetectionData[nCamPart].nIRD == 0 )
        m_vDetectionData[nCamPart].nIRD = nCount;
    else if ( m_vDetectionData[nCamPart].nIRD != nCount )
    {
        if( m_vDetectionData[nCamPart].pIRD )
            delete[] m_vDetectionData[nCamPart].pIRD;
        if( m_vDetectionData[nCamPart].pITD )
            delete[] m_vDetectionData[nCamPart].pITD;

        m_vDetectionData[nCamPart].pIRD = NULL;
        m_vDetectionData[nCamPart].pITD = NULL;
        m_vDetectionData[nCamPart].nIRD = 0;

        return;
    }

    if( nCount > (int)(sizeof(uchar))*nCount )
        return;

    if( m_vDetectionData[nCamPart].pIRD == NULL )
    {
        m_vDetectionData[nCamPart].pIRD = new uchar[nCount];
        ::memset( m_vDetectionData[nCamPart].pIRD, 0, sizeof(uchar) * nCount );
    }

    ::memcpy( m_vDetectionData[nCamPart].pIRD, pIRD, nCount );
}

void QT3kLoadDetectionObject::OnITD(ResponsePart Part, short, const char *, int, int nCount, unsigned char *pITD)
{
    if ( nCount <= 0 )
        return;

    Q_ASSERT( (Part == CM1) || (Part == CM2) || (Part == CM1_1) || (Part == CM2_1) );

    int nCamPart = (int)(Part-1);
    Q_ASSERT( nCamPart >= 0 && nCamPart < m_pStorageHandle->CM.size() );
    if( m_vDetectionData.size() < nCamPart ) return;

    if ( m_vDetectionData[nCamPart].nIRD == 0 )
        m_vDetectionData[nCamPart].nIRD = nCount;
    else if ( m_vDetectionData[nCamPart].nIRD != nCount )
    {
        if( m_vDetectionData[nCamPart].pIRD )
            delete[] m_vDetectionData[nCamPart].pIRD;
        if( m_vDetectionData[nCamPart].pITD )
            delete[] m_vDetectionData[nCamPart].pITD;

        m_vDetectionData[nCamPart].pIRD = NULL;
        m_vDetectionData[nCamPart].pITD = NULL;
        m_vDetectionData[nCamPart].nIRD = 0;

        return;
    }

    if( m_vDetectionData[nCamPart].pITD == NULL )
    {
        m_vDetectionData[nCamPart].pITD = new uchar[nCount];
        ::memset( m_vDetectionData[nCamPart].pITD, 0, sizeof(uchar) * nCount );
    }

    ::memcpy( m_vDetectionData[nCamPart].pITD, pITD, nCount );
}

void QT3kLoadDetectionObject::on_DetectionCheckTimer()
{
    if( m_pStorageHandle->CM.size() > 2 && !m_bMainCamDetection )
    {
        bool bPassCam1 = m_pStorageHandle->CM.value( CM1 )->bNoCam || (m_vDetectionData[0].pIRD && m_vDetectionData[0].pITD);
        bool bPassCam2 = m_pStorageHandle->CM.value( CM2 )->bNoCam || (m_vDetectionData[1].pIRD && m_vDetectionData[1].pITD);

        if( bPassCam1 && bPassCam2 )
        {
            m_bMainCamDetection = true;

            m_pT3kHandle->SendCommand( (const char*)"cam1/sub/mode=detection", true );
            m_pT3kHandle->SendCommand( (const char*)"cam2/sub/mode=detection", true );

            QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_COMPLETE")) + "] " );
            strLog += QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_DETECTION")) + " M";
            emit PrintProgreeLog( strLog );
            return;
        }
    }
    bool bOK = true;
    for( int i=0;i<m_vDetectionData.size(); i++ )
    {
        if( !m_pStorageHandle->CM.value( CM1+i )->bNoCam && (!m_vDetectionData[i].pIRD || !m_vDetectionData[i].pITD) )  // sync?
        {
            //PrintProgreeLog( QString("...%1:false").arg(i), true );
            bOK = false;
            break;
        }
    }

    if( bOK || m_DetectionWaitTimer.interval() == 3000 )
    {
        m_DetectionWaitTimer.stop();

        m_pT3kHandle->SendCommand( (const char*)"cam1/mode=detection", true );
        m_pT3kHandle->SendCommand( (const char*)"cam2/mode=detection", true );

        m_pT3kHandle->SetReportView( false );

        QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_COMPLETE")) + "] " );
        strLog += QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_DETECTION")) + (m_bMainCamDetection ? " S" : " M");
        emit PrintProgreeLog( strLog );

        QString str;
        for( int j=0; j<m_vDetectionData.size(); j++ )
        {
            if( m_vDetectionData[j].pIRD )
            {
                for(int i=0;i<m_vDetectionData[j].nIRD;i++)
                {
                    m_pStorageHandle->CM.value( CM1+j )->strIRD += str.sprintf( "%02x", m_vDetectionData[j].pIRD[i] );
                }
            }
            if( m_vDetectionData[j].pITD )
            {
                for(int i=0;i<m_vDetectionData[j].nIRD;i++)
                {
                    m_pStorageHandle->CM.value( CM1+j )->strITD += str.sprintf( "%02x", m_vDetectionData[j].pITD[i] );
                }
            }
        }

        emit Complete( LDS_DETECTION );
        m_bStart = false;
    }
    else
    {
        m_DetectionWaitTimer.setInterval( 3000 );
    }
}
