#ifndef QCMDASYNCMANAGERCTRL_H
#define QCMDASYNCMANAGERCTRL_H

#include <QWidget>
#include <QQueue>
#include <QString>

#include "QT3kDeviceEventHandler.h"

#define ID_RESULT_TIMEOUT				(1)
#define ID_RESULT_DEVICE_DISCONNECT		(2)
#define ID_RESULT_FIRMWARE_DOWNLOAD		(3)

class QT3kDevice;
class QCmdAsyncManagerCtrl : public QWidget, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT
public:
    explicit QCmdAsyncManagerCtrl(QWidget *parent = 0);

private:
    QT3kDevice* m_pDevice;
    bool        m_bSetSensorCfgMode;
    QQueue<QString> m_DataStorage;

    int         m_TimerAsyncTimeout;
    int         m_TimerStart;
    int         m_TimerTimeout;
    QString     m_strLastCmd;
    int         m_nAsyncId;

    bool        m_bIsStarted;

    int         m_nTotal;

    bool        m_bLastResult;
    int         m_nLastResultReason;

protected:
    void startAsyncCheckTimer();
    void onFinish();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO devInfo, bool bDownload);

    virtual void paintEvent(QPaintEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    void drawProgress(QPainter& p);

    void setProgress( int pos );

    void nextCommand();

    void setResult( bool bResult, int nReason );
public:
    void insertCommand( const QString& strCommand );
    void resetCommands();
    void setSensorCfgMode( bool bSetMode ) { m_bSetSensorCfgMode = bSetMode; }
    void setT3kDevice( QT3kDevice* pDevice ) { m_pDevice = pDevice; }

    void start(unsigned int nTimeout);
    void stop();
    bool isStarted();

    bool getLastResult(int *reason=NULL);

    int getCount() { return (int)m_DataStorage.size(); }
    
signals:
    void asyncFinished( bool bResult, int nReason );
    
public slots:
    
};

#endif // QCMDASYNCMANAGERCTRL_H
