#include "dialog.h"
#include "ui_dialog.h"

#include <QPropertyAnimation>
#include <QMessageBox>

#define RETRY_CONNECTION_INTERVAL        (3000)
#define REQUEST_TIMEOUT                  (400)

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    m_TimerConnectDevice = 0;
    m_TimerRequestTimeout = 0;
    m_nPacketId = -1;

    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    setWindowIcon( QIcon(":/T3kUpgradeRes/resources/T3kUpgrade.png") );

    ui->stackedWidget->setCurrentIndex(0);
    connect( &m_Packet, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::QueuedConnection );
    connect( &m_Packet, SIGNAL(responseFromSensor(unsigned short)), this, SLOT(onResponseFromSensor(unsigned short)), Qt::QueuedConnection );

    stopAllJobs();
}

Dialog::~Dialog()
{
    m_Packet.close();
    delete ui;
}

void Dialog::onDisconnected()
{
    qDebug( "disconnected" );

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
void Dialog::onResponseFromSensor(unsigned short nPacketId)
{
    qDebug( "responseFromSensor" );

    if ( nPacketId != m_nPacketId )
    {
        qDebug( "invalid packet id" );
        return;
    }

    killRequestTimeroutTimer();

    int nFWMode = 0;
    int nIndex = 0;
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
            m_SensorInfo[nIndex].nMode = nFWMode;
            if ((nFWMode != MODE_MM_APP) && (nFWMode != MODE_CM_APP))
            {
                m_CurrentJob.subStep = SUB_QUERY_IAP_VERSION;
                break;
            }
            m_CurrentJob.subStep = SUB_QUERY_VERSION;
            break;
        case SUB_QUERY_VERSION:
            m_SensorInfo[nIndex].nModelNumber = m_Packet.getModelNumber();
            m_SensorInfo[nIndex].nVersionMajor = m_Packet.getVersionMajor();
            m_SensorInfo[nIndex].nVersionMinor = m_Packet.getVersionMinor();

            if ((m_SensorInfo[nIndex].nVersionMinor & 0x0f) != 0)
            {
                snprintf( m_SensorInfo[nIndex].szVersion, 256, "%x.%02x", m_SensorInfo[nIndex].nVersionMajor,
                    m_SensorInfo[nIndex].nVersionMinor );
            }
            else
            {
                snprintf( m_SensorInfo[nIndex].szVersion, 256, "%x.%x", m_SensorInfo[nIndex].nVersionMajor,
                    m_SensorInfo[nIndex].nVersionMinor );
            }
            snprintf( m_SensorInfo[nIndex].szDateTime, 256, "%s, %s", m_Packet.getDate(), m_Packet.getTime() );
            m_CurrentJob.subStep = SUB_QUERY_FINISH;
            break;
        case SUB_QUERY_IAP_VERSION:
            m_SensorInfo[nIndex].nModelNumber = m_Packet.getModelNumber();
            m_CurrentJob.subStep = SUB_QUERY_IAP_REVISION;
            break;
        case SUB_QUERY_IAP_REVISION:
            m_SensorInfo[nIndex].nIapRevision = m_Packet.getRevision();
            if ( (m_SensorInfo[nIndex].nMode == MODE_MM_IAP) ||
                    (m_SensorInfo[nIndex].nMode == MODE_CM_IAP) )
            {
                snprintf( m_SensorInfo[nIndex].szVersion, 256, "IAP Rev(%04x)", m_SensorInfo[nIndex].nIapRevision );
            }
            else
            {
                snprintf( m_SensorInfo[nIndex].szVersion, 256, "UPG Rev(%04x)", m_SensorInfo[nIndex].nIapRevision );
            }
            m_CurrentJob.subStep = SUB_QUERY_FINISH;
            break;
        default:
            qDebug( "already finished..." );
            break;
        }
        break;
    case JOBF_MARK_IAP:

        break;
    case JOBF_MARK_APP:

        break;
    case JOBF_RESET:

        break;
    case JOBF_ERASE:

        break;
    case JOBF_WRITE:
        // TODO: write fwb
        break;
    }

    executeNextJob();
    if (m_bIsExecuteJob)
        startRequestTimeoutTimer();
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
            killRequestTimeroutTimer();
            // TODO: 재전송????
            if (m_CurrentJob.type == JOBF_QUERY_INFO)
            {
                if (m_CurrentJob.subStep == SUB_QUERY_IAP_REVISION)
                {
                    // iap revision timeout
                    qDebug( "iap revision timout" );
                    int nIndex = getIndex(m_CurrentJob.which);
                    if ( (m_SensorInfo[nIndex].nMode == MODE_MM_IAP) ||
                            (m_SensorInfo[nIndex].nMode == MODE_CM_IAP) )
                    {
                        snprintf( m_SensorInfo[nIndex].szVersion, 256, "IAP Rev(Unknown)" );
                    }
                    else
                    {
                        snprintf( m_SensorInfo[nIndex].szVersion, 256, "UPG Rev(Unknown)" );
                    }
                    m_CurrentJob.subStep = SUB_QUERY_FINISH;
                    executeNextJob();
                    if (m_bIsExecuteJob)
                        startRequestTimeoutTimer();
                }
                else if (m_CurrentJob.subStep == SUB_QUERY_MODE)
                {
                    qDebug( "query mode timout" );
                    int nIndex = getIndex(m_CurrentJob.which);
                    m_SensorInfo[nIndex].nMode = MODE_UNKNOWN;
                    m_CurrentJob.subStep = SUB_QUERY_FINISH;
                    executeNextJob();
                    if (m_bIsExecuteJob)
                        startRequestTimeoutTimer();
                }
            }
        }
    }
}

void Dialog::startRequestTimeoutTimer()
{
    if (m_TimerRequestTimeout != 0)
        killTimer(m_TimerRequestTimeout);

    m_TimerRequestTimeout = startTimer(REQUEST_TIMEOUT);
}

void Dialog::killRequestTimeroutTimer()
{
    if (m_TimerRequestTimeout != 0)
    {
        killTimer(m_TimerRequestTimeout);
        m_TimerRequestTimeout = 0;
    }
}

void Dialog::stopAllJobs()
{
    killRequestTimeroutTimer();
    m_JobList.clear();
    m_bIsExecuteJob = false;

    memset( &m_CurrentJob, 0, sizeof(JobItem) );
}

void Dialog::queryInformation()
{
    stopAllJobs();

    JobItem job;
    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_MM;
    m_JobList.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM1;
    m_JobList.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM2;
    m_JobList.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM_SUB | PKT_ADDR_CM1;
    m_JobList.append( job );

    job.type = JOBF_QUERY_INFO;
    job.subStep = SUB_QUERY_MODE;
    job.which = PKT_ADDR_CM_SUB | PKT_ADDR_CM2;
    m_JobList.append( job );

    m_bIsExecuteJob = true;
    executeNextJob();
}

void Dialog::executeNextJob()
{
    if (!m_bIsExecuteJob)
        return;

    m_nPacketId = (unsigned short)-1;

    if (!m_JobList.isEmpty())
    {
        if ( (m_CurrentJob.type != JOBF_QUERY_INFO) || ((m_CurrentJob.type == JOBF_QUERY_INFO) && (m_CurrentJob.subStep == SUB_QUERY_FINISH)) )
        {
            JobItem job = m_JobList.front();
            m_JobList.pop_front();

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
        case JOBF_MARK_IAP:
            m_nPacketId = m_Packet.markIap(m_CurrentJob.which);
            break;
        case JOBF_MARK_APP:
            m_nPacketId = m_Packet.markApp(m_CurrentJob.which);
            break;
        case JOBF_RESET:
            m_nPacketId = m_Packet.reset(m_CurrentJob.which);
            break;
        case JOBF_ERASE:
            m_nPacketId = m_Packet.erase(m_CurrentJob.which);
            break;
        case JOBF_WRITE:
            // TODO: write fwb
            break;
        }

        if (m_nPacketId == (unsigned short)-1)
        {
            qDebug( "job failed" );
            stopAllJobs();
        }
        else
        {
            qDebug("execute job");
        }

        return;
    }
    qDebug( "job finish!" );
    m_bIsExecuteJob = false;
}

void Dialog::connectDevice()
{
    qDebug( "try connect..." );
    if (m_Packet.open())
    {
        qDebug( "connection ok" );
        queryInformation();
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

void Dialog::on_pushButtonUpgrade_clicked()
{
    emit ui->stackedWidget->slideInNext();
}

void Dialog::on_pushButtonCancel_clicked()
{
    // TODO: confirm cancel

    emit ui->stackedWidget->slideInPrev();
}
