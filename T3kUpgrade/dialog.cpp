#include "dialog.h"
#include "ui_dialog.h"

#include <QPropertyAnimation>
#include <QMessageBox>
#include <quazipfile.h>
#include <QBriefingDialog.h>
#include "../common/QUtils.h"

#define RETRY_CONNECTION_INTERVAL       (3000)
#define WAIT_MODECHANGE_TIMEOUT         (60000)     // 60 secs

static const QString PartName[] = { "MM", "CM1", "CM2", "CM1-1", "CM2-1" };

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    m_TimerConnectDevice = 0;
    m_TimerRequestTimeout = 0;
    m_TimerRequestInformation = 0;
    m_TimerWaitModeChange = 0;
    m_nPacketId = -1;

    m_bWaitIAP = false;
    m_bWaitIAPCheckOK = false;

    m_bWaitAPP = false;
    m_bWaitAPPCheckOK = false;

    m_nStableCheck = 0;

    m_bIsStartRequestInformation = false;
    m_bIsStartFirmwareDownload = false;
    m_bIsInformationUpdated = false;

    m_strDownloadProgress.clear();

    memset( &m_TempSensorInfo, 0, sizeof(m_TempSensorInfo) );
    memset( &m_SensorInfo, 0, sizeof(m_SensorInfo) );
    m_FirmwareInfo.clear();;

    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    ui->stackedWidget->setCurrentIndex(0);
    connect( &m_Packet, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::QueuedConnection );
    connect( &m_Packet, SIGNAL(responseFromSensor(unsigned short)), this, SLOT(onResponseFromSensor(unsigned short)), Qt::QueuedConnection );

    ui->pushButtonUpgrade->setEnabled(false);
    displayInformation("Device is not connected.");
    m_strSensorInformation = "";

    stopAllFirmwareDownloadJobs();
    stopAllQueryInformationJobs();

    loadFirmwareFile();
    updateFirmwareInformation();
}

Dialog::~Dialog()
{
    FirmwareInfo* pFI;
    for (int i=0 ; i<m_FirmwareInfo.size() ; i++)
    {
        pFI = m_FirmwareInfo.at(i);
        if (pFI->pFirmwareBinary != 0)
        {
            free( pFI->pFirmwareBinary );
        }
        delete pFI;
    }
    m_FirmwareInfo.clear();

    m_Packet.close();
    delete ui;
}

bool Dialog::loadFirmwareFile()
{
    QString strPath = QCoreApplication::applicationDirPath();
    strPath = rstrip(strPath, "/\\");
    strPath += "/";

    QDir currentDir(strPath);
    QStringList files;
    QString fileName = "*.fwb";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    qDebug( "fwb: %d", files.size() );

    if (files.size() <= 0)
    {
        return false;
    }

    QString strFirmwareFilePathName = strPath;
    strFirmwareFilePathName += files.front();

    qDebug( "file: %s", (const char*)strFirmwareFilePathName.toLatin1() );

    QuaZip zip(strFirmwareFilePathName);
    zip.open( QuaZip::mdUnzip );

    QuaZipFile file(&zip);

#define MM_MODULE_NAME "nuribom mm"
#define CM_MODULE_NAME "nuribom cm"
    char ver_info[40];
#define NAME_OFFSET (0x200)
#define MAJ_VER_OFFSET (0x302)
#define MIN_VER_OFFSET (0x300)

    for ( bool more=zip.goToFirstFile(); more; more=zip.goToNextFile() )
    {
        file.open(QIODevice::ReadOnly);

        //qDebug( "actual filename: %s", (const char*)file.getActualFileName().toLatin1() );
        //qDebug( "filename: %s", (const char*)file.getFileName().toLatin1() );

        if ( file.size() > 0 )
        {
            FirmwareInfo* pFI = new FirmwareInfo;
            memset( pFI, 0, sizeof(FirmwareInfo) );
            pFI->pFirmwareBinary = (char*)malloc(sizeof(char) * file.size());
            int nReadBytes = file.read( pFI->pFirmwareBinary, file.size() );
            pFI->dwFirmwareSize = nReadBytes;
            if (nReadBytes != (int)file.size())
            {
                qDebug( "read error: %d/%d", nReadBytes, (int)file.size());
                free( pFI->pFirmwareBinary );
                delete pFI;
                file.close();
                continue;
            }

            memset( ver_info, 0, sizeof(ver_info) );
            memcpy( ver_info, pFI->pFirmwareBinary+NAME_OFFSET, sizeof(ver_info)-1 );

            if (!analysisFirmwareBinary(ver_info, pFI))
            {
                free( pFI->pFirmwareBinary );
                delete pFI;
                file.close();
                continue;
            }

            unsigned short major = ((unsigned short)(pFI->pFirmwareBinary[MAJ_VER_OFFSET+1]) << 8) | (unsigned char)pFI->pFirmwareBinary[MAJ_VER_OFFSET];
            unsigned short minor = ((unsigned short)(pFI->pFirmwareBinary[MIN_VER_OFFSET+1]) << 8) | (unsigned char)pFI->pFirmwareBinary[MIN_VER_OFFSET];
            pFI->dwFirmwareVersion = ((unsigned long)(major) << 16) | minor;

            if ((minor & 0x0f) != 0)
            {
                snprintf( pFI->szVersion, 256, "%x.%02x", major, minor );
            }
            else
            {
                snprintf( pFI->szVersion, 256, "%x.%x", major, minor );
            }

            qDebug( "> firmware binary[%s] %s %s: %ld bytes", pFI->type == TYPE_MM ? "mm" : "cm", pFI->szVersion, pFI->szModel, pFI->dwFirmwareSize );

            m_FirmwareInfo.push_back(pFI);
        }
        file.close();
    }
    zip.close();

    return m_FirmwareInfo.size() != 0 ? true : false;
}

FirmwareInfo* Dialog::findFirmware( FIRMWARE_TYPE type, unsigned short nModelNumber )
{
    for ( int f=0 ; f<m_FirmwareInfo.size() ; f++ )
    {
        FirmwareInfo* pFI = m_FirmwareInfo.at(f);
        if ((pFI->type == type) && (pFI->nModelNumber == nModelNumber))
        {
            return pFI;
        }
    }
    return NULL;
}

bool Dialog::analysisFirmwareBinary( const char* ver_info, FirmwareInfo* pFI )
{
    QString strVersionInfo = ver_info;
    int chkV = strVersionInfo.indexOf("nuribom");
    if ( chkV < 0 )
    {
        qDebug( "invalid firmware binary" );
        return false;
    }
    strVersionInfo = strVersionInfo.right(strVersionInfo.length()-8);
    if (strVersionInfo.indexOf("cm") == 0)
    {
        pFI->type = TYPE_CM; // CM
    }
    else if (strVersionInfo.indexOf("mm") == 0)
    {
        pFI->type = TYPE_MM; // MM
    }
    else
    {
        qDebug( "invalid type" );
        return false;
    }
    strVersionInfo = strVersionInfo.right(strVersionInfo.length()-3);

    if ( pFI->type == TYPE_MM )  // MM
    {
        if (strVersionInfo.compare("T3000") == 0)
        {
            snprintf(pFI->szModel, 256, "T3000");
            pFI->nModelNumber = 0x3000;
        }
        else if (strVersionInfo.compare("T3100") == 0)
        {
            snprintf(pFI->szModel, 256, "T3100");
            pFI->nModelNumber = 0x3100;
        }
        else if (strVersionInfo.compare("T3k_A") == 0)
        {
            snprintf(pFI->szModel, 256, "T3k A");
            pFI->nModelNumber = 0x3500;
        }
        else
        {
            qDebug( "unknown model mm" );
            return false;
        }
    }
    else
    {
        if (strVersionInfo.compare("T3000") == 0)
        {
            snprintf(pFI->szModel, 256, "C3000");
            pFI->nModelNumber = 0x3000;
        }
        else if (strVersionInfo.compare("T3100") == 0)
        {
            snprintf(pFI->szModel, 256, "C3100");
            pFI->nModelNumber = 0x3100;
        }
        else if (strVersionInfo.compare("T3500") == 0)
        {
            snprintf(pFI->szModel, 256, "C3400/C3500");
            pFI->nModelNumber = 0x3500;
        }
        else
        {
            qDebug( "unknown model cm" );
            return false;
        }
    }

    return true;
}

void Dialog::onDisconnected()
{
    qDebug( "disconnected" );

    ui->pushButtonUpgrade->setEnabled(false);
    displayInformation("Device is not connected.");
    m_strSensorInformation = "";
    m_bIsInformationUpdated = false;

    stopAllQueryInformationJobs();
    if (!m_bWaitIAP && !m_bWaitAPP)
    {
        stopAllFirmwareDownloadJobs();
        ui->stackedWidget->setCurrentIndex(0);
    }

    if (m_Packet.isOpen())
    {
        m_Packet.close();
    }
    connectDevice();
}

inline int getIndex(unsigned short which)
{
    switch (which)
    {
    case PKT_ADDR_MM:
        return IDX_MM;
    case PKT_ADDR_CM1:
        return IDX_CM1;
    case PKT_ADDR_CM2:
        return IDX_CM2;
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
        return IDX_CM1_1;
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
        return IDX_CM2_1;
    }
    return IDX_MM;
}

void Dialog::timerEvent(QTimerEvent *evt)
{
    if ( evt->type() == QEvent::Timer )
    {
        if ( evt->timerId() == m_TimerConnectDevice )
        {
            killTimer(m_TimerConnectDevice);
            m_TimerConnectDevice = 0;
            connectDevice();
        }
        else if (evt->timerId() == m_TimerRequestTimeout )
        {
            qDebug( "request timeout" );
            killRequestTimeoutTimer();
            // TODO: retry????
            if (m_CurrentJob.type == JOBF_QUERY_INFO)
            {
                if (m_CurrentJob.subStep == SUB_QUERY_IAP_REVISION)
                {
                    // iap revision timeout
                    qDebug( "iap revision timout" );
                    int nIndex = getIndex(m_CurrentJob.which);
                    if ( (m_TempSensorInfo[nIndex].nMode == MODE_MM_IAP) ||
                            (m_TempSensorInfo[nIndex].nMode == MODE_CM_IAP) )
                    {
                        snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "IAP Rev(Unknown)" );
                    }
                    else
                    {
                        snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "UPG Rev(Unknown)" );
                    }
                    m_CurrentJob.subStep = SUB_QUERY_FINISH;
                    executeNextJob();
                }
                else if (m_CurrentJob.subStep == SUB_QUERY_MODE)
                {
                    qDebug( "query mode timout" );
                    int nIndex = getIndex(m_CurrentJob.which);
                    m_TempSensorInfo[nIndex].nMode = MODE_UNKNOWN;
                    m_CurrentJob.subStep = SUB_QUERY_FINISH;
                    executeNextJob();
                }
                else
                {
                    executeNextJob( true );
                }
            }
            else if (m_CurrentJob.type == JOBF_WRITE)
            {
                qDebug( "write timeout!!!!" );
                QString strMessage = QString("[%1] Write Error").arg(PartName[getIndex(m_CurrentJob.which)]);
                addProgressText(strMessage, TM_NG);
                onFirmwareUpdateFailed();
            }
            else
            {
                executeNextJob( true );
            }
        }
        else if (evt->timerId() == m_TimerRequestInformation)
        {
            killTimer(m_TimerRequestInformation);
            m_TimerRequestInformation = 0;

            queryInformation();
        }
        else if (evt->timerId() == m_TimerWaitModeChange)
        {
            killWaitModeChangeTimer();
            qDebug( "Wait IAP/APP timeout!!!" );
            QString strMessage = QString("[%1] Wait timeout!").arg(PartName[getIndex(m_CurrentJob.which)]);
            addProgressText(strMessage, TM_NG);
            onFirmwareUpdateFailed();
        }
    }
}

void Dialog::startQueryInformation(bool bDelay)
{
    if (m_TimerRequestInformation)
    {
        killTimer(m_TimerRequestInformation);
        m_TimerRequestInformation = 0;
    }

    m_bIsStartRequestInformation = true;

    if (bDelay)
    {
        m_TimerRequestInformation = startTimer(500);
    }
    else
    {
        queryInformation();
    }
}

void Dialog::stopQueryInformation()
{
    if (m_TimerRequestInformation)
    {
        killTimer(m_TimerRequestInformation);
        m_TimerRequestInformation = 0;
    }

    killRequestTimeoutTimer();

    stopAllQueryInformationJobs();

    m_bIsStartRequestInformation = false;
}

void Dialog::startFirmwareDownload()
{
    ui->listWidget->clear();
    ui->progressBar->setValue(0);
    ui->labelPart->setText("");

    ui->labelMessage->setStyleSheet("color: rgb(203, 45, 5); font-weight: bold;");
    ui->labelMessage->setText("CAUTION: Do not unplug the device until the process is completed.");

    if (m_TimerRequestInformation)
    {
        killTimer(m_TimerRequestInformation);
        m_TimerRequestInformation = 0;
    }

    m_bIsStartFirmwareDownload = true;

    firmwareDownload();
}

void Dialog::stopFirmwareDownload()
{
    if (m_TimerRequestInformation)
    {
        killTimer(m_TimerRequestInformation);
        m_TimerRequestInformation = 0;
    }

    killRequestTimeoutTimer();

    stopAllFirmwareDownloadJobs();

    m_bWaitIAP = false;
    m_bWaitIAPCheckOK = false;
    m_bWaitAPP = false;
    m_bWaitAPPCheckOK = false;
    m_bIsStartFirmwareDownload = false;
}

void Dialog::firmwareDownload()
{
    stopAllFirmwareDownloadJobs();

    JobItem job;

    bool bIAPModeOK = true;
    // check iap mode
    for ( int i=IDX_MAX-1 ; i>=0 ; i-- )
    {
        if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
            continue;
        if ((m_SensorInfo[i].nMode != MODE_CM_IAP) && (m_SensorInfo[i].nMode != MODE_MM_IAP))
        {
            bIAPModeOK = false;
            break;
        }
    }

    if (!bIAPModeOK)
    {
        for ( int i=IDX_MAX-1 ; i>=0 ; i-- )
        {
            if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
                continue;
            job.type = JOBF_MARK_IAP;
            job.subStep = SUB_QUERY_FINISH;
            job.which = m_SensorInfo[i].nWhich;
            m_JobListForFirmwareDownload.append( job );
        }

        job.type = JOBF_RESET;
        job.subStep = SUB_QUERY_FINISH;
        job.which = PKT_ADDR_MM;
        m_JobListForFirmwareDownload.append( job );

        job.type = JOBF_WAIT_IAP_ALL;
        job.subStep = SUB_QUERY_FINISH;
        job.which = 0;
        m_JobListForFirmwareDownload.append( job );
    }

    for ( int i=IDX_MAX-1 ; i>=0 ; i-- )
    {
        if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
            continue;

        job.type = JOBF_ERASE;
        job.subStep = SUB_QUERY_FINISH;
        job.which = m_SensorInfo[i].nWhich;
        m_JobListForFirmwareDownload.append( job );

        job.type = JOBF_WRITE;
        job.subStep = SUB_QUERY_WRITE_PROGRESS;
        job.firmwareBinaryPos = 0;
        job.which = m_SensorInfo[i].nWhich;
        m_JobListForFirmwareDownload.append( job );

        job.type = JOBF_MARK_APP;
        job.subStep = SUB_QUERY_FINISH;
        job.which = m_SensorInfo[i].nWhich;
        m_JobListForFirmwareDownload.append( job );
    }

    job.type = JOBF_RESET;
    job.subStep = SUB_QUERY_FINISH;
    job.which = PKT_ADDR_MM;
    m_JobListForFirmwareDownload.append( job );

    job.type = JOBF_WAIT_APP_ALL;
    job.subStep = SUB_QUERY_FINISH;
    job.which = 0;
    m_JobListForFirmwareDownload.append( job );

    m_bIsExecuteJob = true;
    executeNextJob();
}

void Dialog::startRequestTimeoutTimer( int nTimeout )
{
    if (m_TimerRequestTimeout != 0)
        killTimer(m_TimerRequestTimeout);

    m_TimerRequestTimeout = startTimer(nTimeout);
}

void Dialog::killRequestTimeoutTimer()
{
    if (m_TimerRequestTimeout != 0)
    {
        killTimer(m_TimerRequestTimeout);
        m_TimerRequestTimeout = 0;
    }
}

void Dialog::startWaitModeChangeTimer()
{
    if (m_TimerWaitModeChange !=0)
        killTimer(m_TimerWaitModeChange);
    m_TimerWaitModeChange = startTimer(WAIT_MODECHANGE_TIMEOUT);
}

void Dialog::killWaitModeChangeTimer()
{
    if (m_TimerWaitModeChange !=0)
    {
        killTimer(m_TimerWaitModeChange);
        m_TimerWaitModeChange = 0;
    }
}

void Dialog::stopAllQueryInformationJobs()
{
    killRequestTimeoutTimer();
    m_JobListForRequestInformation.clear();
    if (!m_bIsStartFirmwareDownload)
        m_bIsExecuteJob = false;

    memset( &m_CurrentJob, 0, sizeof(JobItem) );
}

void Dialog::stopAllFirmwareDownloadJobs()
{
    killRequestTimeoutTimer();
    m_JobListForFirmwareDownload.clear();
    m_bIsExecuteJob = false;

    memset( &m_CurrentJob, 0, sizeof(JobItem) );
}

void Dialog::queryInformation()
{
    stopAllQueryInformationJobs();

    memset( &m_TempSensorInfo, 0, sizeof(m_TempSensorInfo) );

    JobItem job;
    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_MM;
    m_JobListForRequestInformation.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM1;
    m_JobListForRequestInformation.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM2;
    m_JobListForRequestInformation.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM_SUB | PKT_ADDR_CM1;
    m_JobListForRequestInformation.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM_SUB | PKT_ADDR_CM2;
    m_JobListForRequestInformation.append( job );

    m_bIsExecuteJob = true;
    executeNextJob();
}

void Dialog::onResponseFromSensor(unsigned short nPacketId)
{
    //qDebug( "responseFromSensor: %x", nPacketId );

    if ( nPacketId != m_nPacketId )
    {
        qDebug( "invalid packet id" );
        return;
    }

    killRequestTimeoutTimer();

    int nFWMode = 0;
    int nIndex = 0;
    QString strMessage;
    switch (m_CurrentJob.type)
    {
    default:
        qDebug( "invalid job type" );
        break;
    case JOBF_QUERY_INFO:
        nIndex = getIndex(m_CurrentJob.which);
        switch (m_CurrentJob.subStep)
        {
        case SUB_QUERY_MODE:
            nFWMode = m_Packet.getFirmwareMode();
            m_TempSensorInfo[nIndex].nMode = nFWMode;
            if ((nFWMode != MODE_MM_APP) && (nFWMode != MODE_CM_APP))
            {
                m_CurrentJob.subStep = SUB_QUERY_IAP_VERSION;
                break;
            }
            m_CurrentJob.subStep = SUB_QUERY_VERSION;
            break;
        case SUB_QUERY_VERSION:
            m_TempSensorInfo[nIndex].nModelNumber = m_Packet.getModelNumber();
            m_TempSensorInfo[nIndex].nVersionMajor = m_Packet.getVersionMajor();
            m_TempSensorInfo[nIndex].nVersionMinor = m_Packet.getVersionMinor();
            m_TempSensorInfo[nIndex].nWhich = m_CurrentJob.which;

            if ((m_TempSensorInfo[nIndex].nModelNumber == 0x3500) && (m_CurrentJob.which == PKT_ADDR_MM) )
            {
                snprintf( m_TempSensorInfo[nIndex].szModel, 256, "T3k A" );
            }
            else
            {
                snprintf( m_TempSensorInfo[nIndex].szModel, 256, "%c%04x", (m_CurrentJob.which == PKT_ADDR_MM) ? 'T' : 'C', m_TempSensorInfo[nIndex].nModelNumber );
            }

            if ((m_TempSensorInfo[nIndex].nVersionMinor & 0x0f) != 0)
            {
                snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "%x.%02x", m_TempSensorInfo[nIndex].nVersionMajor,
                    m_TempSensorInfo[nIndex].nVersionMinor );
            }
            else
            {
                snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "%x.%x", m_TempSensorInfo[nIndex].nVersionMajor,
                    m_TempSensorInfo[nIndex].nVersionMinor );
            }
            snprintf( m_TempSensorInfo[nIndex].szDateTime, 256, " %s, %s", m_Packet.getDate(), m_Packet.getTime() );
            m_CurrentJob.subStep = SUB_QUERY_FINISH;
            break;
        case SUB_QUERY_IAP_VERSION:
            m_TempSensorInfo[nIndex].nWhich = m_CurrentJob.which;
            m_TempSensorInfo[nIndex].nModelNumber = m_Packet.getModelNumber();
            if ((m_TempSensorInfo[nIndex].nModelNumber == 0x3500) && (m_CurrentJob.which == PKT_ADDR_MM) )
            {
                snprintf( m_TempSensorInfo[nIndex].szModel, 256, "T3k A" );
            }
            else
            {
                snprintf( m_TempSensorInfo[nIndex].szModel, 256, "%c%04x", (m_CurrentJob.which == PKT_ADDR_MM) ? 'T' : 'C', m_TempSensorInfo[nIndex].nModelNumber );
            }
            m_CurrentJob.subStep = SUB_QUERY_IAP_REVISION;
            break;
        case SUB_QUERY_IAP_REVISION:
            m_TempSensorInfo[nIndex].nIapRevision = m_Packet.getRevision();
            if ( (m_TempSensorInfo[nIndex].nMode == MODE_MM_IAP) ||
                    (m_TempSensorInfo[nIndex].nMode == MODE_CM_IAP) )
            {
                snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "IAP Rev(%04x)", m_TempSensorInfo[nIndex].nIapRevision );
            }
            else
            {
                snprintf( m_TempSensorInfo[nIndex].szVersion, 256, "UPG Rev(%04x)", m_TempSensorInfo[nIndex].nIapRevision );
            }
            m_CurrentJob.subStep = SUB_QUERY_FINISH;
            break;
        default:
            qDebug( "already finished..." );
            break;
        }
        break;
    case JOBF_MARK_IAP:
        strMessage = QString("[%1] Mark IAP OK").arg(PartName[getIndex(m_CurrentJob.which)]);
        addProgressText(strMessage, TM_OK);
        qDebug( "mark iap: %x ok", m_CurrentJob.which );
        break;
    case JOBF_MARK_APP:
        strMessage = QString("[%1] Mark APP OK").arg(PartName[getIndex(m_CurrentJob.which)]);
        addProgressText(strMessage, TM_OK);
        qDebug( "mark app: %x ok", m_CurrentJob.which );
        break;
    case JOBF_RESET:
        strMessage = QString("[%1] Reset OK").arg(PartName[getIndex(m_CurrentJob.which)]);
        addProgressText(strMessage, TM_OK);
        qDebug( "reset: %x ok", m_CurrentJob.which );
        break;
    case JOBF_ERASE:
        strMessage = QString("[%1] Erase OK").arg(PartName[getIndex(m_CurrentJob.which)]);
        addProgressText(strMessage, TM_OK);
        qDebug( "erase: %x ok", m_CurrentJob.which );
        break;
    case JOBF_WRITE:
        FirmwareInfo* pFI = findFirmware( m_CurrentJob.which == PKT_ADDR_MM ? TYPE_MM : TYPE_CM, m_SensorInfo[getIndex(m_CurrentJob.which)].nModelNumber );
        if (m_CurrentJob.firmwareBinaryPos == pFI->dwFirmwareSize)
        {
            qDebug( "write: %x finish", m_CurrentJob.which );
            m_CurrentJob.subStep = SUB_QUERY_FINISH;
            strMessage = QString("[%1] Write Firmware... Finish").arg(PartName[getIndex(m_CurrentJob.which)]);
            addProgressText(strMessage, TM_OK);
            ui->progressBar->setValue(100);
        }
        else
        {
            qDebug( "write: %x %ld/%ld", m_CurrentJob.which, m_CurrentJob.firmwareBinaryPos, pFI->dwFirmwareSize );
            ui->progressBar->setValue( (int)(m_CurrentJob.firmwareBinaryPos * 100 / pFI->dwFirmwareSize) );
        }
        break;
    }

    executeNextJob();
}

void Dialog::executeNextJob( bool bRetry/*=false*/ )
{
    if (!m_bIsExecuteJob)
        return;

    m_nPacketId = (unsigned short)-1;

    if (m_bIsStartRequestInformation)
    {
        if (!m_JobListForRequestInformation.isEmpty() || bRetry || ((m_CurrentJob.type == JOBF_QUERY_INFO) && (m_CurrentJob.subStep != SUB_QUERY_FINISH)) )
        {
            if ( !bRetry && ((m_CurrentJob.type != JOBF_QUERY_INFO) || ((m_CurrentJob.type == JOBF_QUERY_INFO) && (m_CurrentJob.subStep == SUB_QUERY_FINISH))) )
            {
                JobItem job = m_JobListForRequestInformation.front();
                m_JobListForRequestInformation.pop_front();

                m_CurrentJob = job;
            }

            switch (m_CurrentJob.type)
            {
            default:
                qDebug( "invalid job type" );
                break;
            case JOBF_QUERY_INFO:
                switch (m_CurrentJob.subStep)
                {
                case SUB_QUERY_MODE:
                    m_nPacketId = m_Packet.queryMode(m_CurrentJob.which);
                    break;
                case SUB_QUERY_VERSION:
                    m_nPacketId = m_Packet.queryVersion(m_CurrentJob.which);
                    break;
                case SUB_QUERY_IAP_VERSION:
                    m_nPacketId = m_Packet.queryIapVersion(m_CurrentJob.which);
                    break;
                case SUB_QUERY_IAP_REVISION:
                    m_nPacketId = m_Packet.queryIapRevision(m_CurrentJob.which);
                    break;
                default:
                    qDebug( "already finished..." );
                    break;
                }
                break;
            }

            if (m_nPacketId == (unsigned short)-1)
            {
                qDebug( "job failed" );
                stopAllQueryInformationJobs();
            }
            else
            {
                //qDebug("execute job");
                startRequestTimeoutTimer(200);
            }

            return;
        }

        onFinishAllRequestInformationJobs();
    }

    if (m_bIsStartFirmwareDownload)
    {
        if (!m_JobListForFirmwareDownload.isEmpty() || bRetry || ((m_CurrentJob.type == JOBF_WRITE) && (m_CurrentJob.subStep != SUB_QUERY_FINISH)) || m_bWaitAPP || m_bWaitIAP )
        {
            if ( (!bRetry && !m_bWaitIAP && !m_bWaitAPP && (m_CurrentJob.type != JOBF_WRITE)) ||
                 ((m_CurrentJob.type == JOBF_WRITE) && (m_CurrentJob.subStep == SUB_QUERY_FINISH)))
            {
                JobItem job = m_JobListForFirmwareDownload.front();
                m_JobListForFirmwareDownload.pop_front();

                m_CurrentJob = job;
            }

            QString strMessage;
            switch (m_CurrentJob.type)
            {
            default:
                if ( m_CurrentJob.type != JOBF_QUERY_INFO )
                    qDebug( "invalid job type" );
                break;
            case JOBF_MARK_IAP:
                strMessage = QString("[%1] Mark IAP...").arg(PartName[getIndex(m_CurrentJob.which)]);
                addProgressText(strMessage, TM_NORMAL);
                qDebug( "mark Iap: %x", m_CurrentJob.which );
                m_nPacketId = m_Packet.markIap(m_CurrentJob.which);
                break;
            case JOBF_MARK_APP:
                strMessage = QString("[%1] Mark APP...").arg(PartName[getIndex(m_CurrentJob.which)]);
                addProgressText(strMessage, TM_NORMAL);
                qDebug( "mark App: %x", m_CurrentJob.which );
                m_nPacketId = m_Packet.markApp(m_CurrentJob.which);
                break;
            case JOBF_RESET:
                strMessage = QString("[%1] Reset...").arg(PartName[getIndex(m_CurrentJob.which)]);
                addProgressText(strMessage, TM_NORMAL);
                qDebug( "reset: %x", m_CurrentJob.which );
                m_nPacketId = m_Packet.reset(m_CurrentJob.which);
                break;
            case JOBF_WAIT_IAP_ALL:
                strMessage = QString("Wait IAP Mode...");
                addProgressText(strMessage, TM_NORMAL);
                m_bWaitIAP = true;
                m_bWaitIAPCheckOK = false;
                m_nStableCheck = 0;
                startWaitModeChangeTimer();
                break;
            case JOBF_WAIT_APP_ALL:
                strMessage = QString("Wait APP Mode...");
                addProgressText(strMessage, TM_NORMAL);
                m_bWaitAPP = true;
                m_bWaitAPPCheckOK = false;
                m_nStableCheck = 0;
                startWaitModeChangeTimer();
                break;
            case JOBF_ERASE:
                strMessage = QString("[%1] Erase...").arg(PartName[getIndex(m_CurrentJob.which)]);
                addProgressText(strMessage, TM_NORMAL);
                qDebug( "erase: %x", m_CurrentJob.which );
                m_nPacketId = m_Packet.erase(m_CurrentJob.which);
                break;
            case JOBF_WRITE:
                if (m_CurrentJob.firmwareBinaryPos == 0)
                {
                    strMessage = QString("[%1] Write Firmware...").arg(PartName[getIndex(m_CurrentJob.which)]);
                    addProgressText(strMessage, TM_NORMAL);
                    ui->labelPart->setText(PartName[getIndex(m_CurrentJob.which)]);
                    ui->progressBar->setValue(0);
                }
                FirmwareInfo* pFI = findFirmware( m_CurrentJob.which == PKT_ADDR_MM ? TYPE_MM : TYPE_CM, m_SensorInfo[getIndex(m_CurrentJob.which)].nModelNumber);
                qDebug( "firmware info: %s %s %ld bytes", pFI->szModel, pFI->szVersion, pFI->dwFirmwareSize );

                unsigned short nWriteBytes = 0;
                if ((pFI->dwFirmwareSize - m_CurrentJob.firmwareBinaryPos) > TX_BUF_LEN)
                {
                    nWriteBytes = TX_BUF_LEN;
                }
                else
                {
                    nWriteBytes = (unsigned short)(pFI->dwFirmwareSize - m_CurrentJob.firmwareBinaryPos);
                }

                qDebug( "write %d bytes", nWriteBytes );

                m_nPacketId = m_Packet.write( m_CurrentJob.which, m_CurrentJob.firmwareBinaryPos, nWriteBytes, (unsigned char*)(pFI->pFirmwareBinary+m_CurrentJob.firmwareBinaryPos));
                if (m_nPacketId != (unsigned short)-1)
                    m_CurrentJob.firmwareBinaryPos += nWriteBytes;
                break;
            }

            if (!m_bWaitIAP && !m_bWaitAPP)
            {
                if (m_nPacketId == (unsigned short)-1)
                {
                    qDebug( "job failed" );
                    strMessage = QString("[%1] Failed...").arg(PartName[getIndex(m_CurrentJob.which)]);
                    addProgressText(strMessage, TM_NG);
                    onFirmwareUpdateFailed();
                }
                else
                {
                    //qDebug("execute job");
                    if ( m_CurrentJob.type == JOBF_ERASE )
                        startRequestTimeoutTimer( 10000 );
                    else
                        startRequestTimeoutTimer( 300 );
                }
            }
            else
            {
                if (!m_bIsStartRequestInformation)
                {
                    startQueryInformation(true);
                }
                else
                {
                    if (m_bWaitIAP)
                    {
                        qDebug( "check IAP" );
                        bool bIAPModeOK = true;
                        for (int i=0 ; i<IDX_MAX ; i++)
                        {
                            if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
                                continue;
                            if ( (m_SensorInfo[i].nMode != MODE_CM_IAP) && (m_SensorInfo[i].nMode != MODE_MM_IAP) )
                            {
                                bIAPModeOK = false;
                                break;
                            }
                        }
                        if (bIAPModeOK)
                        {
                            m_nStableCheck++;
                            if (m_nStableCheck > 2)
                            {
                                killWaitModeChangeTimer();
                                m_bWaitIAP = false;
                                m_bWaitIAPCheckOK = true;
                                stopQueryInformation();
                                qDebug( "check IAP OK" );
                                strMessage = QString("IAP Mode OK");
                                addProgressText(strMessage, TM_OK);
                                executeNextJob();
                            }
                        }
                    }
                    else if (m_bWaitAPP)
                    {
                        qDebug( "check APP" );
                        bool bAPPModeOK = true;
                        for (int i=0 ; i<IDX_MAX ; i++)
                        {
                            if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
                                continue;
                            if ( (m_SensorInfo[i].nMode != MODE_CM_APP) && (m_SensorInfo[i].nMode != MODE_MM_APP) )
                            {
                                bAPPModeOK = false;
                                break;
                            }
                        }
                        if (bAPPModeOK)
                        {
                            m_nStableCheck++;
                            if (m_nStableCheck > 2)
                            {
                                killWaitModeChangeTimer();
                                m_bWaitAPP = false;
                                m_bWaitAPPCheckOK = true;
                                stopQueryInformation();
                                qDebug( "check APP OK" );
                                strMessage = QString("APP Mode OK");
                                addProgressText(strMessage, TM_OK);
                                executeNextJob();
                            }
                        }
                    }
                }
            }

            return;
        }

        onFinishAllFirmwareDownloadJobs();
    }

    m_bIsExecuteJob = false;
}

void Dialog::onFinishAllRequestInformationJobs()
{
    qDebug( "request information job finish!" );
    if (m_bIsStartRequestInformation)
    {
        updateSensorInformation();
        if (m_TimerRequestInformation)
        {
            killTimer(m_TimerRequestInformation);
        }
        m_TimerRequestInformation = startTimer(2000);
    }
}

void Dialog::onFinishAllFirmwareDownloadJobs()
{
    m_bIsStartFirmwareDownload = false;
    qDebug( "firmware download job finish!" );

    QString strMessage = QString("Firmware Download OK!");
    addProgressText(strMessage, TM_OK);

    ui->labelMessage->setText("The firmware has been updated successfully.");
    ui->labelMessage->setStyleSheet("color: rgb(31, 160, 70); font-weight: bold;");

    ui->pushButtonCancel->setText("OK");
    ui->pushButtonCancel->setEnabled(true);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    show();
}

void Dialog::onFirmwareUpdateFailed()
{
    ui->labelMessage->setText("Failed to update firmware");
    ui->labelMessage->setStyleSheet("color: rgb(203, 45, 5); font-weight: bold;");
    stopFirmwareDownload();
    ui->pushButtonCancel->setText("Cancel");
    ui->pushButtonCancel->setEnabled(true);
}

void Dialog::displayInformation( const QString& strText )
{
    QString strInformation;
    strInformation = "<html><body>"
            "<table width=\"100%\" height=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
            "<tr>"
            "<td><div align=\"center\"><font size=\"3\" color=#880015>"
            + strText +
            "</font></div></td>"
            "</tr>"
            "</table>"
            "</body></html>";
    ui->textEditSensorInformation->setHtml(strInformation);
}

void Dialog::updateSensorInformation()
{
    memcpy( &m_SensorInfo, &m_TempSensorInfo, sizeof(m_TempSensorInfo) );

    QString strInformationHTML;
    QString strTableHeader =
        "<table width=\"100%\" cellspacing=\"0\" style=\"border-collapse:collapse;\"><tr>"
        "<td width=\"30%\" style=\"border-width:1px; border-color:black; border-style:solid;\" bgcolor=\"#d5dffb\">"
            "<p><font size=\"3\" color=#3f3f3f><b>Part</b></font></p>"
        "</td>"
        "<td width=\"30%\" style=\"border-width:1px; border-color:black; border-style:solid;\" bgcolor=\"#d5dffb\">"
            "<p><font size=\"3\" color=#3f3f3f><b>Version</b></font></p>"
        "</td>"
        "<td width=\"40%\" style=\"border-width:1px; border-color:black; border-style:solid;\" bgcolor=\"#d5dffb\">"
            "<p><font size=\"3\" color=#3f3f3f><b>Build</b></font></p>"
        "</td></tr>";
    QString strTableTail = "</table>";
    QString strRowStart = "<tr>";
    QString strRowEnd = "</tr>";
    QString strColumn1Start = "<td width=\"30%\" style=\"border-width:1px; border-color:black; border-style:solid;\"><p><font size=\"3\" color=#000000>";
    QString strColumn2Start = "<td width=\"30%\" style=\"border-width:1px; border-color:black; border-style:solid;\"><p><font size=\"3\" color=#000000>";
    QString strColumn2RedStart = "<td width=\"30%\" style=\"border-width:1px; border-color:black; border-style:solid;\"><p><font size=\"3\" color=#880015>";
    QString strColumn3Start = "<td width=\"40%\" style=\"border-width:1px; border-color:black; border-style:solid;\"><p><font size=\"3\" color=#000000>";
    QString strColumnEnd = "</font></p></td>";

    strInformationHTML = "<html>\n";
    strInformationHTML += "<body>\n";
    strInformationHTML += strTableHeader;

    int nMaxPart = 3;
    if ( m_SensorInfo[IDX_CM1_1].nMode != MODE_UNKNOWN ||
         m_SensorInfo[IDX_CM2_1].nMode != MODE_UNKNOWN ) {
        nMaxPart = 5;
    }
    for (int i=0 ; i<nMaxPart ; i++)
    {
        strInformationHTML += strRowStart;
        if ( m_SensorInfo[i].nMode == MODE_UNKNOWN )
            strInformationHTML += strColumn2RedStart;
        else
            strInformationHTML += strColumn1Start;
        strInformationHTML += PartName[i];
        strInformationHTML += strColumnEnd;
        if ( m_SensorInfo[i].nMode == MODE_UNKNOWN )
        {
            strInformationHTML += strColumn2RedStart;
            strInformationHTML += "Disconnected";
            strInformationHTML += strColumnEnd;

            strInformationHTML += strColumn2RedStart;
            strInformationHTML += "-";
            strInformationHTML += strColumnEnd;
        }
        else
        {
            strInformationHTML += strColumn2Start;
            strInformationHTML += QString(m_SensorInfo[i].szVersion) + " " + QString(m_SensorInfo[i].szModel);
            strInformationHTML += strColumnEnd;

            strInformationHTML += strColumn3Start;
            strInformationHTML += QString(m_SensorInfo[i].szDateTime);
            strInformationHTML += strColumnEnd;
        }
        strInformationHTML += strRowEnd;
    }

    strInformationHTML += strTableTail;
    strInformationHTML += "</body>\n</html>";

    QString strInformation = "";
    for (int i=0 ; i<nMaxPart ; i++)
    {
        strInformation += PartName[i];
        if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
        {
            strInformation += "Disconnected";
        }
        else
        {
            strInformation += QString(m_SensorInfo[i].szVersion) + QString(m_SensorInfo[i].szModel) + QString(m_SensorInfo[i].szDateTime);
        }
    }

    if (m_strSensorInformation != strInformation)
    {
        ui->textEditSensorInformation->setHtml(strInformationHTML);
        m_strSensorInformation = strInformation;
    }

    if ( !m_bIsInformationUpdated )
    {
        m_bIsInformationUpdated = true;
        ui->pushButtonUpgrade->setEnabled(true);
    }
}

void Dialog::updateFirmwareInformation()
{
    QString strInformation;
    if (m_FirmwareInfo.size() == 0)
    {
        QString strText = "Firmware file does not exist.";
        strInformation = "<html><body>"
                "<table width=\"100%\" height=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
                "<tr>"
                "<td><div align=\"center\"><font size=\"3\" color=#880015>"
                + strText +
                "</font></div></td>"
                "</tr>"
                "</table>"
                "</body></html>";
    }
    else
    {
        QString strMMFirmwares, strCMFirmwares;

        for (int i=0 ; i<m_FirmwareInfo.size() ; i++)
        {
            FirmwareInfo* pFI = m_FirmwareInfo.at(i);
            if (pFI->type == TYPE_MM)
            {
                if (!strMMFirmwares.isEmpty())
                    strMMFirmwares += ", ";
                strMMFirmwares += pFI->szVersion;
                strMMFirmwares += " ";
                strMMFirmwares += pFI->szModel;
            }
            else
            {
                if (!strCMFirmwares.isEmpty())
                    strCMFirmwares += ", ";
                strCMFirmwares += pFI->szVersion;
                strCMFirmwares += " ";
                strCMFirmwares += pFI->szModel;
            }
        }

        strInformation = "<html><body>"
                "<p style=\"line-height:50%; margin-top:0; margin-bottom:0;\"><b><font color=#7f7f7f>MM</font></b></p>"
                "<hr size=\"1\">"
                "<p style=\"line-height:80%; margin-top:0; margin-bottom:0;\"><font color=black>"
                + strMMFirmwares +
                "</font></p>"
                "<p style=\"line-height:150%; margin-top:0; margin-bottom:0;\">&nbsp;</p>"
                "<p style=\"line-height:50%; margin-top:0; margin-bottom:0;\"><b><font color=#7f7f7f>CM</font></b></p>"
                "<hr size=\"1\">"
                "<p style=\"line-height:80%; margin-top:0; margin-bottom:0;\"><font color=black>"
                + strCMFirmwares +
                "</font></p>"
                "</body></html>";
    }
    ui->textEditFirmwareInformation->setHtml(strInformation);
}

void Dialog::connectDevice()
{
    qDebug( "try connect..." );
    if (m_Packet.open())
    {
        qDebug( "connection ok" );
        displayInformation("Connected.");
        m_strSensorInformation = "";
        startQueryInformation(false);
    }
    else
    {
        qDebug( "connection fail" );
        m_TimerConnectDevice = startTimer(RETRY_CONNECTION_INTERVAL);
    }
}

void Dialog::showEvent(QShowEvent *evt)
{
    if ( evt->type() == QEvent::Show )
    {
        connectDevice();
    }

    QDialog::showEvent(evt);
}

void Dialog::closeEvent(QCloseEvent *evt)
{
    if ( evt->type() == QEvent::Close )
    {
        qDebug( "closeEvent" );
        stopQueryInformation();

        if (m_Packet.isOpen())
        {
            m_Packet.close();
        }
        if (m_TimerConnectDevice != 0)
        {
            killTimer(m_TimerConnectDevice);
            m_TimerConnectDevice = 0;
        }
    }

    QDialog::closeEvent(evt);
}

bool Dialog::verifyFirmware(QString& strMsg)
{
    if (m_SensorInfo[0].nMode == MODE_UNKNOWN )
    {
        qDebug( "invalid firmware mode" );
        strMsg = "invalid firmware mode";
        return false;
    }

    // mm
    FirmwareInfo* pFI = findFirmware( TYPE_MM, m_SensorInfo[IDX_MM].nModelNumber );

    if (!pFI)
    {
        qDebug( "%s firmware not found", m_SensorInfo[IDX_MM].szModel );

        strMsg = QString("%1 firmware not found")
                        .arg(m_SensorInfo[IDX_MM].szModel);
        return false;
    }

    // cm
    for ( int i=IDX_CM1 ; i<IDX_MAX ; i++ )
    {
        if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
            continue;
        FirmwareInfo* pFI = findFirmware( TYPE_CM, m_SensorInfo[i].nModelNumber );

        if (!pFI)
        {
            strMsg = QString("%1 firmware not found")
                            .arg(m_SensorInfo[i].szModel);
            return false;
        }
    }

    return true;
}

void Dialog::on_pushButtonUpgrade_clicked()
{
    ui->pushButtonCancel->setEnabled(false);
    stopQueryInformation();

    qDebug( "Stop Query Information" );

    QString strMsg;
    if (!verifyFirmware(strMsg))
    {
        QMessageBox msgBox;
        msgBox.setText("Error: Verify Firmware");
        msgBox.setInformativeText(strMsg);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QString strDetailHTML;

    QString strHead1 =
        "<html><body><font color=black>"
        "<table width=\"100%\" cellspacing=\"0\" bgcolor=\"#d5dffb\">"
            "<tr>"
                "<td width=\"15%\">"
                    "<p align=\"left\">Part</p>"
                "</td>"
                "<td width=\"15%\">"
                    "<p align=\"left\">Model</p>"
                "</td>"
                "<td width=\"40%\">"
                    "<p align=\"center\">from</p>"
                "</td>"
                "<td width=\"10%\">"
                    "<p align=\"center\">&nbsp;</p>"
                "</td>"
                "<td width=\"20%\">"
                    "<p align=\"center\">to</p>"
                "</td>"
            "</tr>"
        "</table>"
        "<hr>";

    QString strHead2 =
        "<table width=\"100%\" cellspacing=\"0\" align=\"center\">";

    QString strCMInfo = "";
    FirmwareInfo* pFI_CM = NULL;
    int nCMCount = 0;
    for (int i=IDX_CM1; i<IDX_MAX ; i++)
    {
        if (m_SensorInfo[i].nMode == MODE_UNKNOWN)
            continue;

        pFI_CM = findFirmware( TYPE_CM, m_SensorInfo[i].nModelNumber );
        QString strCMItem =
            "<tr>"
                "<td width=\"15%\">"
                    "<p align=\"left\">" + QString(PartName[i]) + "</p>"
                "</td>"
                "<td width=\"15%\">"
                    "<p align=\"left\">" + QString(m_SensorInfo[i].szModel) + "</p>"
                "</td>"
                "<td width=\"40%\">"
                "<p align=\"center\">" + QString(m_SensorInfo[i].szVersion) + "</p>"
                "</td>"
                "<td width=\"20%\">"
                    "<p align=\"center\">" + QString(pFI_CM->szVersion) + "</p>"
                "</td>"
            "</tr>";
        strCMInfo += strCMItem;
        nCMCount++;
    }

    FirmwareInfo* pFI_MM = findFirmware( TYPE_MM, m_SensorInfo[IDX_MM].nModelNumber );
    QString strMMInfo =
        "<tr>"
            "<td width=\"15%\">"
                "<p align=\"left\">MM</p>"
            "</td>"
            "<td width=\"15%\">"
                "<p align=\"left\">" + QString(m_SensorInfo[IDX_MM].szModel) + "</p>"
            "</td>"
            "<td width=\"40%\">"
                "<p align=\"center\">" + QString(m_SensorInfo[IDX_MM].szVersion) + "</p>"
            "</td>"
            "<td width=\"10%\" rowspan=" + QString("%1").arg(nCMCount+1) + " align=\"center\" valign=\"middle\">"
                "<p align=\"center\">=&gt;</p>"
            "</td>"
            "<td width=\"20%\">"
                "<p align=\"center\">" + QString(pFI_MM->szVersion) + "</p>"
            "</td>"
        "</tr>";



    QString strTail =
        "</table></font>"
        "</body></html>";

    strDetailHTML = strHead1 + strHead2 + strMMInfo + strCMInfo + strTail;

    QBriefingDialog briefDialog(strDetailHTML, this);
    int nResult = briefDialog.exec();

    if (nResult == QDialog::Accepted)
    {
        Qt::WindowFlags flags = windowFlags();
        flags &= ~Qt::WindowCloseButtonHint;
        setWindowFlags(flags);
        show();

        emit ui->stackedWidget->slideInNext();
        startFirmwareDownload();
    }
    else
    {
        startQueryInformation(false);
    }
}

void Dialog::on_pushButtonCancel_clicked()
{
    if (m_bIsStartFirmwareDownload)
    {
        // TODO: confirm cancel
    }
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    show();

    emit ui->stackedWidget->slideInPrev();
    stopFirmwareDownload();
    startQueryInformation(true);
}

void Dialog::addProgressText(QString& strMessage, TextMode tm)
{
    ui->listWidget->addItem(strMessage);
    int idx = ui->listWidget->count()-1;

    switch (tm)
    {
    case TM_NORMAL:
        ui->listWidget->item(idx)->setForeground(QColor(100, 100, 100));
        break;
    case TM_NG:
        ui->listWidget->item(idx)->setForeground(QColor(170, 0, 26));
        break;
    case TM_OK:
        ui->listWidget->item(idx)->setForeground(QColor(26, 136, 58));
        break;
    }

    ui->listWidget->scrollToBottom();
}
