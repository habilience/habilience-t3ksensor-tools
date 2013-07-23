#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QT3kDeviceEventHandler.h>
#include "QLangManager.h"

namespace Ui {
class Dialog;
}

class QSideviewDialog;
class QDetectionDialog;
class QBentAdjustmentDialog;
class QTouchSettingDialog;
class QSelectDeviceDialog;
class Dialog : public QDialog, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

private:
    QString m_strModelName;
    QString m_strFirmwareVersion;

    bool    m_bFirmwareDownload;
    bool    m_bIsConnected;

    bool    m_bInvalidFirmwareVersion;

    int     m_nDeviceCount;

    int     m_TimerReconnect;
    int     m_TimerRefreshInfo;
    int     m_TimerCheckDevice;

    QFont   m_fntErrorTitle;
    QFont   m_fntErrorDetail;

    bool    m_bEnterTurnOffCheck;

    QSideviewDialog*        m_pDlgSideview;
    QDetectionDialog*       m_pDlgDetection;
    QBentAdjustmentDialog*  m_pDlgBentAdjustment;
    QTouchSettingDialog*    m_pDlgTouchSetting;

    QSelectDeviceDialog*    m_pDlgSelectDevice;

    struct SensorAppInfo {
        bool            bIsValid;
        unsigned short  nVersionMajor;
        unsigned short  nVersionMinor;
        char            szVersion[256];
        char            szModel[256];
    };

#define IDX_MM      (0)
#define IDX_CM1     (1)
#define IDX_CM2     (2)
#define IDX_CM1_1   (3)
#define IDX_CM2_1   (4)
#define IDX_MAX     (5)
    SensorAppInfo      m_SensorAppInfo[IDX_MAX];
    SensorAppInfo      m_TempSensorAppInfo[IDX_MAX];

    void versionStringToSensorInfo(SensorAppInfo *info, const QString& strVer);
    void updateVersionInformation();

protected:
    void onCreate();
    void onDestroy();
    void onInitDialog();

    virtual void showEvent(QShowEvent *evt);
    virtual void paintEvent(QPaintEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

    void drawSafeMode(QPainter& p);

    enum SelectMenu
    {
        MenuSideview,
        MenuDetection,
        MenuBentAdjustment,
        MenuTouchSetting
    };

    void switchMenu( SelectMenu menu );
    void closeAllSubMenuDialogs();

    void refreshInfo();
    bool openDevice();
    void checkDeviceStatus();

    void updateResetButton();

    void onDeviceConnected();
    void onDeviceDisconnected();

    void loadSensorDefaultSettings( bool bQuestion = true );
    void saveSensorDefaultSettings();
    bool isExistSensorDefaultValue( const QString& strSensorCmd );

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected( T3K_DEVICE_INFO devInfo );
    virtual void TPDP_OnDownloadingFirmware( T3K_DEVICE_INFO devInfo, bool bIsDownload );
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                             const char* partid, int id, bool bFinal, const char * szCmd );

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    const QString& getModelName() { return m_strModelName; }
    const QString& getFirmwareVersion() { return m_strFirmwareVersion; }

    bool isExistSubCamera();
    bool isAvailableSubCameraModel();

    void onCloseMenu();
    
private slots:
    void on_btnSelectSensor_clicked();

    void on_btnSideview_clicked();

    void on_btnDetection_clicked();

    void on_btnBentAdjustment_clicked();

    void on_btnTouchSetting_clicked();

    void on_btnReset_clicked();

    void on_btnTouchMark_clicked();

    void on_btnExit_clicked();

    void on_chkSafeMode_clicked(bool checked);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
