#ifndef QDETECTIONDIALOG_H
#define QDETECTIONDIALOG_H

#include <QDialog>
#include <QT3kDeviceEventHandler.h>
#include "QLangManager.h"
#include "t3kcomdef.h"

namespace Ui {
class QDetectionDialog;
}

class Dialog;
class QBorderStyleEdit;
class QDetectionDialog : public QDialog, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
private:
    enum DetectionMode	{ DetectionModeNone, DetectionModeMain, DetectionModeSub };
    DetectionMode   m_detectionMode;

    bool    m_bEnterAutoRangeSetting;
    int     m_nAutoRangeStep;
    bool    m_bTouchOK;
    bool    m_bCamTouch;
    unsigned long m_dwTickTouch;
    int     m_nCamTouchCount[2];
    QPoint  m_ptCamTouchObj[2][4];
    int     m_nCamTouchMax[2];

    long    m_lCam1Left;
    long    m_lCam1Right;
    long    m_lCam2Left;
    long    m_lCam2Right;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual bool eventFilter(QObject *obj, QEvent *evt);

    virtual void reject();
    virtual void accept();

    // override QT3kDeviceEventHandler::IListener
    virtual void onChangeLanguage();

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };
    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorRefresh();
    void sensorWriteToFactoryDefault();

    void resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault=true);

    void enableAllControls( bool bEnable );

    enum BuzzerType { BuzzerEnterCalibration, BuzzerCancelCalibration, BuzzerCalibrationSucces, BuzzerClick, BuzzerNextPoint };
    void playBuzzer( BuzzerType type );

    int getIndexFromPart(ResponsePart Part);

    // override QLangManager::ILangChangeNotify
    virtual void TPDP_OnDTC(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char *layerid, unsigned long *start_pos, unsigned long *end_pos, int cnt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    void onFinishAutoRange();

    void enterAutoRangeSetting();
    void leaveAutoRangeSetting();

    void setDetectionMode( DetectionMode mode );

    void showArrow();
    void hideArrow();

    void analysisTouchObj();

public:
    explicit QDetectionDialog(Dialog *parent = 0);
    ~QDetectionDialog();

    bool canClose();
    
private slots:
    void on_btnClose_clicked();

    void on_btnMain_clicked();

    void on_btnSub_clicked();

    void on_btnReset_clicked();

    void on_btnAutoRangeSetting_clicked();

    void on_btnRefresh_clicked();

    void on_btnSave_clicked();

    void on_chkAutoDetectionLine_clicked();

    void on_chkSimpleDetection_clicked();

    void on_chkInvertDetection_clicked();

private:
    Dialog* m_pMainDlg;
    Ui::QDetectionDialog *ui;
};

#endif // QDETECTIONDIALOG_H
