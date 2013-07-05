#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFWDPacket.h>
#include <QList>

namespace Ui {
class Dialog;
}

struct SensorInfo {
    unsigned short  nMode;
    unsigned short  nModelNumber;
    unsigned short  nIapRevision;
    unsigned short  nVersionMajor;
    unsigned short  nVersionMinor;
    char            szVersion[256];
    char            szDateTime[256];
    char            szModel[256];
};

enum FIRMWARE_TYPE { TYPE_MM=0, TYPE_CM=1 };
struct FirmwareInfo {
    char*           pFirmwareBinary;
    unsigned long   dwFirmwareSize;
    unsigned long   dwFirmwareVersion;
    unsigned short  nModelNumber;
    char            szVersion[256];
    char            szModel[256];
    FIRMWARE_TYPE   type;
};

class Dialog : public QDialog
{
    Q_OBJECT
    
private:
    QFWDPacket  m_Packet;
    int         m_TimerConnectDevice;
    int         m_TimerRequestTimeout;
    int         m_TimerRequestInformation;

    enum QueryInfoStep {
        SUB_QUERY_MODE = 0,
        SUB_QUERY_VERSION,
        SUB_QUERY_IAP_VERSION,
        SUB_QUERY_IAP_REVISION,
        SUB_QUERY_FINISH
    };

    enum JobItemType {
        JOBF_NONE = 0,
        JOBF_QUERY_INFO,
        JOBF_MARK_IAP,
        JOBF_MARK_APP,
        JOBF_RESET,
        JOBF_ERASE,
        JOBF_WRITE
    };

    struct JobItem {
        JobItemType     type;
        QueryInfoStep   subStep;
        unsigned short  which;
    };

    QList<JobItem>  m_JobList;
    bool            m_bIsExecuteJob;
    bool            m_bIsStartRequestInformation;
    JobItem         m_CurrentJob;
    unsigned short  m_nPacketId;

    bool            m_bIsInformationUpdated;

#define IDX_MM      (0)
#define IDX_CM1     (1)
#define IDX_CM2     (2)
#define IDX_CM1_1   (3)
#define IDX_CM2_1   (4)
#define IDX_MAX     (5)
    SensorInfo      m_SensorInfo[IDX_MAX];
    SensorInfo      m_TempSensorInfo[IDX_MAX];
    QList<FirmwareInfo*> m_FirmwareInfo;

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    QString m_strSensorInformation;

protected:
    virtual void timerEvent(QTimerEvent *evt);
    virtual void showEvent(QShowEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    void connectDevice();

    void startRequestTimeoutTimer();
    void killRequestTimeoutTimer();

    void startQueryInformation();
    void stopQueryInformation();

    void queryInformation();
    void stopAllJobs();

    void executeNextJob( bool bRetry=false );

    void onFinishAllJobs();

    void updateSensorInformation();
    void updateFirmwareInformation();
    void displayInformation( const char* szText );

    bool loadFirmwareFile();

    bool analysisFirmwareBinary( const char* ver_info, FirmwareInfo* pFI );

    bool verifyFirmware(QString& strMsg);

    FirmwareInfo* findFirmware( FIRMWARE_TYPE type, unsigned short nModelNumber );
    
private slots:
    void on_pushButtonUpgrade_clicked();

    void on_pushButtonCancel_clicked();

    void onDisconnected();
    void onResponseFromSensor(unsigned short nPacketId);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
