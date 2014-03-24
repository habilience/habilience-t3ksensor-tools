#ifndef QDETECTIONDIALOG_H
#define QDETECTIONDIALOG_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "t3kcomdef.h"
#include "QEventRedirection.h"

namespace Ui {
class QDetectionDialog;
}

class Dialog;
class QBorderStyleEdit;
class QDetectionDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
        , public QLangManager::ILangChangeNotify
        , public QEventRedirection::IEventListener
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
    struct TchPoint {
        long s, e;
        long sc, ec;
        long width;
    }       m_ptCamTouchObj[2][4];
    int     m_nCamTouchMax[2];

    int     m_nOldMargins[4];

    long    m_lCam1Left;
    long    m_lCam1Right;
    long    m_lCam2Left;
    long    m_lCam2Right;

    int     m_TimerRefreshAutoOffset;
    int     m_TimerUpdateGraph;
    int     m_TimerBlinkArrow;

    bool    m_bToggleArrow;     // true: show, false: hide
    int     m_nTouchProgress;   // for Auto Range Setting
    QRect   m_rcArrow[4];
    QRect   m_rcProgress[4];

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *);

    void drawArrow(QPainter& p);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    void setCheckAutoOffset(bool bCheck);

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };
    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorRefresh();
    void sensorWriteToFactoryDefault();

    void resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault=true);

    void enableAllControls( bool bEnable );

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDTC(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char *layerid, unsigned long *start_pos, unsigned long *end_pos, int cnt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnMSG(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *txt);

    // override QEventRedirection::IEventListener
    virtual bool onKeyPress(QKeyEvent *evt);
    virtual bool onKeyRelease(QKeyEvent *evt);
    virtual void onRButtonClicked();
    virtual bool onRButtonDblClicked();

    void onFinishAutoRange();

    void enterAutoRangeSetting();
    void leaveAutoRangeSetting();

    void setDetectionMode( DetectionMode mode );

    void showArrow();
    void hideArrow();
    void updateRect(QRect rc);

    void analysisTouchObj();

    void setViewMode( bool bViewMode );     // instant_mode 'V'

public:
    explicit QDetectionDialog(Dialog *parent = 0);
    ~QDetectionDialog();

    bool canClose();
    void onClose();

signals:
    void showCrack(bool bShow);
    
private slots:
    virtual void reject();
    virtual void accept();

    void onCmdAsyncMngrFinished(bool, int);

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
    QEventRedirection m_EventRedirect;
};

#endif // QDETECTIONDIALOG_H
