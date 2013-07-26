#include "QCmdAsyncManagerCtrl.h"

#include <QPainter>
#include "QLogSystem.h"
#include "QT3kDevice.h"
#include <QTimerEvent>
#include <QCloseEvent>
#include "QSensorInitDataCfg.h"

QCmdAsyncManagerCtrl::QCmdAsyncManagerCtrl(QWidget *parent) :
    QWidget(parent)
{
    m_pDevice = NULL;
    m_TimerAsyncTimeout = 0;
    m_TimerStart = 0;
    m_TimerTimeout = 0;
    m_bIsStarted = false;
    m_nAsyncId = -1;
    m_bSetSensorCfgMode = false;

    m_bLastResult = false;
    m_nLastResultReason = 0;
}

void QCmdAsyncManagerCtrl::startAsyncCheckTimer()
{
    if (m_TimerAsyncTimeout)
        killTimer(m_TimerAsyncTimeout);
    m_TimerAsyncTimeout = startTimer(1500);
}

void QCmdAsyncManagerCtrl::setResult( bool bResult, int nReason )
{
    m_bLastResult = bResult;
    m_nLastResultReason = nReason;

    emit asyncFinished( bResult, nReason );
}

void QCmdAsyncManagerCtrl::onFinish()
{
    if (isStarted())
        stop();

    setResult(true, 0);
}

void QCmdAsyncManagerCtrl::TPDP_OnDisconnected(T3K_DEVICE_INFO /*devInfo*/)
{
    bool bNotify = false;
    if (isStarted())
    {
        bNotify = true;
        stop();
    }

    resetCommands();

    if (bNotify)
    {
        setResult( false, ID_RESULT_DEVICE_DISCONNECT );
    }
}

void QCmdAsyncManagerCtrl::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int id, bool /*bFinal*/, const char *cmd)
{
//    if (!isWindow())
//        return;

    if (m_nAsyncId == id)
    {
        if (m_TimerAsyncTimeout)
        {
            killTimer(m_TimerAsyncTimeout);
            m_TimerAsyncTimeout = 0;
        }

        if (m_bSetSensorCfgMode)
        {
            QString strPrefix = "";
            switch (Part)
            {
            default:
                break;
            case CM1:
                strPrefix = "cam1/";
                break;
            case CM2:
                strPrefix = "cam2/";
                break;
            case CM1_1:
                strPrefix = "cam1/sub/";
                break;
            case CM2_1:
                strPrefix = "cam2/sub/";
                break;
            }
            QString strRSP = strPrefix;
            strRSP += QString(cmd);
            int nSP = strRSP.indexOf('=');
            if (nSP >= 0)
            {
                QString strCmd = strRSP.left(nSP);
                QString strValue = strRSP.right(strRSP.size() - nSP - 1);
                QSensorInitDataCfg::instance()->setValue( strCmd, strValue );
            }
        }

        nextCommand();
    }
}

void QCmdAsyncManagerCtrl::nextCommand()
{
goto_recheck:
    if (m_DataStorage.isEmpty())
    {
        onFinish();
    }
    else
    {
        startAsyncCheckTimer();
        m_strLastCmd = m_DataStorage.at(0);
        m_DataStorage.removeAt(0);

        if (m_bSetSensorCfgMode)
        {
            switch (m_strLastCmd.at(0).toLatin1())
            {
            case '#':
                QSensorInitDataCfg::instance()->setValue( m_strLastCmd, "" );
                goto goto_recheck;
            case '{':
                {
                    int nSP = m_strLastCmd.indexOf('=');
                    if (nSP>=0)
                    {
                        QString strCmd, strValue;
                        strCmd = m_strLastCmd.left( nSP );
                        strValue = m_strLastCmd.right( m_strLastCmd.size() - nSP - 1 );
                        QSensorInitDataCfg::instance()->setValue( strCmd, strValue );
                    }
                }
                goto goto_recheck;
            }
        }

        LOG_C( "-> %s", (const char*)m_strLastCmd.toLatin1() );
        m_nAsyncId = m_pDevice->sendCommand( m_strLastCmd.toLatin1(), true );

        setProgress( m_nTotal - m_DataStorage.size());
        qDebug( "progress %d/%d", m_nTotal-m_DataStorage.size(), m_nTotal);

    }
}

void QCmdAsyncManagerCtrl::TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO /*devInfo*/, bool bDownload)
{
    if (bDownload)
    {
        bool bNotify = false;
        if (isStarted())
        {
            bNotify = true;
            stop();
        }

        resetCommands();

        if (bNotify)
        {
            setResult( false, ID_RESULT_FIRMWARE_DOWNLOAD );
        }
    }
}

void QCmdAsyncManagerCtrl::paintEvent(QPaintEvent */*evt*/)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);

    p.save();

    if (!m_bIsStarted)
    {
        p.fillRect( rcBody, Qt::white );

        QPen pen( QColor(128, 128, 128) );
        p.setPen( pen );

        p.drawLine( rcBody.left(), rcBody.center().y(), rcBody.right(), rcBody.center().y() );
    }
    else
    {
        // draw progress
        drawProgress( p );
    }


    p.restore();
}

void QCmdAsyncManagerCtrl::drawProgress(QPainter& p)
{
    // with m_nTotal, m_nTotal-m_DataStorage.size()
}


void QCmdAsyncManagerCtrl::setProgress( int pos )
{

}

void QCmdAsyncManagerCtrl::timerEvent(QTimerEvent *evt)
{
    if (evt->type() != QEvent::Timer )
        return;

    if ( evt->timerId() == m_TimerStart )
    {
        killTimer(m_TimerStart);
        m_TimerStart = 0;

        setProgress(1);
        nextCommand();
    }
    else if (evt->timerId() == m_TimerAsyncTimeout)
    {
        if (m_strLastCmd.isEmpty())
        {
            killTimer(m_TimerAsyncTimeout);
            m_TimerAsyncTimeout = 0;
        }
        else
        {
            qDebug( "RETRY ----> %s", (const char*)m_strLastCmd.toLatin1() );
        }

        startAsyncCheckTimer();
        LOG_I( "Timeout: %s", (const char*)m_strLastCmd.toLatin1() );
        LOG_C( "-> %s", (const char*)m_strLastCmd.toLatin1() );
        m_nAsyncId = m_pDevice->sendCommand( m_strLastCmd.toLatin1(), true );
    }
    else if (evt->timerId() == m_TimerTimeout)
    {
        if (isStarted())
            stop();

        resetCommands();

        setResult( false, ID_RESULT_TIMEOUT );
    }
}

void QCmdAsyncManagerCtrl::closeEvent(QCloseEvent *evt)
{
    if ( evt->type() == QEvent::Close )
    {
        stop();
    }
}

void QCmdAsyncManagerCtrl::insertCommand(const QString &strCommand )
{
    m_DataStorage.push_back( strCommand );
}

void QCmdAsyncManagerCtrl::resetCommands()
{
    m_DataStorage.clear();
}

void QCmdAsyncManagerCtrl::start( unsigned int nTimeout )
{
    m_bIsStarted = true;

    if (m_bSetSensorCfgMode)
    {
        QSensorInitDataCfg::instance()->reset();
    }

    m_nTotal = m_DataStorage.size();
    setProgress(0);

    m_TimerStart = startTimer( 10 );
    if (nTimeout != (unsigned int)-1)
    {
        m_TimerTimeout = startTimer(nTimeout);
    }
    update();
}

void QCmdAsyncManagerCtrl::stop()
{
    if (m_TimerTimeout)
    {
        killTimer(m_TimerTimeout);
        m_TimerTimeout = 0;
    }

    if (m_TimerStart)
    {
        killTimer(m_TimerStart);
        m_TimerStart = 0;
    }
    if (m_TimerAsyncTimeout)
    {
        killTimer(m_TimerAsyncTimeout);
        m_TimerAsyncTimeout = 0;
    }

    m_bIsStarted = false;
    update();
}

bool QCmdAsyncManagerCtrl::isStarted()
{
    return m_bIsStarted;
}

bool QCmdAsyncManagerCtrl::getLastResult(int *reason/*=NULL*/)
{
    if (reason != NULL)
        *reason = m_nLastResultReason;
    return m_bLastResult;
}
