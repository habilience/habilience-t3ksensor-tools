#ifndef QBENTADJUSTMENTDIALOG_H
#define QBENTADJUSTMENTDIALOG_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "QEventRedirection.h"
#include <QVector>
#include "QPointClipper.h"
#include <QThread>
#include <QSoundEffect>

namespace Ui {
class QBentAdjustmentDialog;
}

#define ADJUSTMENT_STEP	(7+6+4)

class QBentProgressDialog;
class QCalcCamPosThread;
struct BentItem
{
    int		nCameraIndex;

    float	fLastTouchPos;
    float   fLastTouchPosS;
    float   fLastTouchPosE;
    float	fTouchPosS[T3K_MAX_DTC_COUNT];
    float	fTouchPosE[T3K_MAX_DTC_COUNT];
    int		nTouchCnt;

    float	fObcS[ADJUSTMENT_STEP];
    float	fObcE[ADJUSTMENT_STEP];
    float	fObcCenter[ADJUSTMENT_STEP];
    float	fDistortion[ADJUSTMENT_STEP];

    float	fCamA;
    float	fCamB;
    float	fCamC;
    float	fCamD;
    float	fCamE;
    float	fCamF;

    unsigned char mode;

    // for debug
    float	fCamX;
    float	fCamY;
    float	fCamS;
    float	fCamR;

    // processing
    int     nCalcPosDataCnt;
    float   fDistortionMax;
    int     nCalcPosProg;
    //HANDLE  hThread;
    QCalcCamPosThread* pThread;
    QBentProgressDialog* pCalcPosWnd;
    bool    bCalcPos;

    bool	bDataValid;
    int		nAveCount;
    long	lPrevTouch;
    long	lAveDiff;
};

class QCalcCamPosThread : public QThread
{
private:
    BentItem& m_item;
protected:
    virtual void run();
public:
    QCalcCamPosThread(BentItem& item);
};

class QKeyWatcher : public QObject
{
    Q_OBJECT
private:
    bool m_bStopGUI;
public:
    QKeyWatcher(bool& bStopGUI) : m_bStopGUI(bStopGUI)
    {
    }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *evt);

};

class Dialog;
class QBorderStyleEdit;
class QBentAdjustmentDialog : public QWidget
        , public QT3kDeviceEventHandler::IListener
        , public QLangManager::ILangChangeNotify
        , public QEventRedirection::IEventListener
{
    Q_OBJECT
friend class QCalcCamPosThread;
private:
    bool    m_bIsModified;

    int     m_nMonitorOrientation;

    bool    m_bEnterAdjustmentMode;
    int     m_nAdjustmentStep;
    bool    m_bIsValidTouch;
    int     m_nValidTouchCount;
    bool    m_bBentAdjustmentPerformed;
    int     m_nBentProgress;
    bool    m_bCalibrationWarning;

    bool    m_bCheckCamTouch[2];        // cam1, cam2
    int     m_nTouchCount;
    int     m_nMaxTouchCount;
    bool    m_bIsTouchOK;
    bool    m_bIsTouchLift;
    long    m_lClickArea;

    bool    m_bDrawWaitTimeout;
    int     m_nWaitCountDown;
    QRect   m_rcWaitTime;

    bool    m_bOldTouchState;

#define FLAG_CAM1    0x01
#define FLAG_CAM2    0x02
#define FLAG_CAM1_1  0x10
#define FLAG_CAM2_1  0x20
    unsigned char m_cError;
    unsigned char m_cNG;

    bool    m_bShowCursor;
    QRect   m_rcCursor;

    int     m_TimerReCheckPoint;
    int     m_TimerBlinkCursor;
    int     m_TimerShowButtons;
    int     m_TimerHideButtons;
    int     m_TimerDrawWaitTimeout;

    QVector<BentItem> m_BentItemArray;
    QPointClipper     m_PointClipper;

    int     m_nAutoOffset;

    QString m_errorCalibrationCamera;

    // bent sound
    QSoundEffect m_BentStepEffect;

public:
    static QPoint PosToDCC( float x, float y, const QRect rcClient );
    static QPointF PosToTPos( float x, float y, float o = 1.f );
    static QPoint TPosToDCC( float x, float y, const QRect rcClient );
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    // override QEventRedirection::IEventListener
    virtual bool onKeyPress(QKeyEvent *evt);
    virtual bool onKeyRelease(QKeyEvent *evt);
    virtual bool onMouseWheel(QWheelEvent *evt);
    virtual void onRButtonClicked();
    virtual bool onRButtonDblClicked();

    bool isShortcutWidget(QWidget* widget);
    void focusChangeToNextShortcutWidget(bool bDirection);

    void enableAllControls(bool bEnable);

    bool loadDefaultSettingValues();
    void updateData( bool bSaveAndValidate );

    void enterAdjustmentMode();
    void leaveAdjustmentMode( bool bSuccess );

    void drawAdjustmentGrid( QPainter& p, QRect rcBody, QPoint* pPtCursor );
    void drawCameraLocations( QPainter& p, QRect rcBody );

    void drawCursor( QPainter& p, int nx, int ny, int nc );
    void drawErrorText( QPainter& p, int nx, int ny, int nc );
    void drawWaitTime( QPainter& p, QRect rcWait );
    void drawTouchLines( QPainter& p, QRect rcBody );

    void onRangeChange(QBorderStyleEdit* pEdit, int nMin, int nMax, int nStep);

    void calculateCameraPosition( float* fObcS, float* fObcE );
    static bool calculateCameraValues( BentItem& item );

    void showArrowButtons( bool bShow );
    void showAllButtons( bool bShow );

    void onAdjustmentFinish();

    void checkTouchPoints( bool bTouch );
    void setInvalidTouch();
    bool checkValidTouch();

    QPoint getRemoteCursorPos( int nAdjustmentStep );
    void remoteCursor( bool bShow, bool bTouchOK=false, int nx=0, int ny=0, int nProgress=0 );

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnOBJ(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char *layerid, float *start_pos, float *end_pos, int cnt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };
    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorRefresh();
    void sensorWriteToFactoryDefault();

    void setViewMode( bool bViewMode ); // instant_mode 'O'
public:
    explicit QBentAdjustmentDialog(Dialog* pPanretWidget, QWidget *parent = 0);
    ~QBentAdjustmentDialog();

    bool canClose();
    void onClose();

private slots:
    virtual void reject();
    virtual void accept();

    void onEditModified(QBorderStyleEdit* pEdit, int nValue, double dValue);
    void onCmdAsyncMngrFinished(bool, int);

    void on_btnClose_clicked();
    void on_btnSave_clicked();
    void on_btnReset_clicked();

    void on_btnMarginLeftDec_clicked();
    void on_btnMarginLeftInc_clicked();
    void on_btnMaringUpDec_clicked();
    void on_btnMaringUpInc_clicked();
    void on_btnMarginRightDec_clicked();
    void on_btnMarginRightInc_clicked();
    void on_btnMarginDownDec_clicked();
    void on_btnMarginDownInc_clicked();
    void on_btnLeft_clicked();
    void on_btnUp_clicked();
    void on_btnRight_clicked();
    void on_btnDown_clicked();

private:
    Dialog* m_pMainDlg;
    Ui::QBentAdjustmentDialog *ui;
    QEventRedirection m_EventRedirect;
};

#endif // QBENTADJUSTMENTDIALOG_H
