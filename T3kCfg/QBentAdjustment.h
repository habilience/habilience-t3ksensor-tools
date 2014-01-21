#ifndef QBENTADJUSTMENT_H
#define QBENTADJUSTMENT_H

#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"
#include "fe/QPointClipper.h"
#include "QRequestHIDManager.h"

#include <QDialog>
#include <QThread>
#include <QVector>

#define ADJUSTMENT_STEP	(7+6)

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
    QBentProgressDialog*    pCalcPosWnd;
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

class QBentAdjustment : public QObject
        , public QT3kDeviceREventHandler::IListener
        , public QLangManager::ILangChangeNotify
{
    Q_OBJECT
friend class QCalcCamPosThread;
private:
    int     m_nMonitorOrientation;

    bool    m_bEnterAdjustmentMode;
    int     m_nAdjustmentStep;
    bool    m_bIsValidTouch;
    int     m_nValidTouchCount;
    bool    m_bBentAdjustmentPerformed;
    int     m_nBentProgress;

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
    int     m_TimerDrawWaitTimeout;

    QVector<BentItem> m_BentItemArray;
    QPointClipper     m_PointClipper;

    QT3kDeviceR*         m_pT3kHandle;

    QRect               m_rcBody;

    QWidget*            m_pTargetWidget;

    QRequestHIDManager  m_T3kRequestManager;

    bool                m_bCheckFinishBent;

    int                 m_nTimerSaveCmd;
    int                 m_nSendCmdID;
    QString             m_strSendCmd;

    QVector<QString>    m_vSendCmd;
    QVector<int>        m_vSendCmdID;

    QMutex              m_Mutex;

public:
    static QPoint PosToDCC( float x, float y, const QRect rcClient );
    static QPointF PosToTPos( float x, float y, float o = 1.f );
    static QPoint TPosToDCC( float x, float y, const QRect rcClient );
protected:
    virtual void timerEvent(QTimerEvent *);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    void drawAdjustmentGrid( QPainter& p, QRect rcBody, QPoint* pPtCursor );
    void drawCameraLocations( QPainter& p, QRect rcBody );

    void drawCursor( QPainter& p, int nx, int ny, int nc );
    void drawErrorText( QPainter& p, int nx, int ny, int nc );
    void drawWaitTime( QPainter& p, QRect rcWait );
    void drawTouchLines( QPainter& p, QRect rcBody );

    void calculateCameraPosition( float* fObcS, float* fObcE );
    static bool calculateCameraValues( BentItem& item );

    void onAdjustmentFinish();

    void checkTouchPoints( bool bTouch );
    void setInvalidTouch();
    bool checkValidTouch();

    void sendSaveCommand();

    // QT3kDeviceREventHandler::IListener
    virtual void TPDP_OnOBJ(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char *layerid, float *start_pos, float *end_pos, int cnt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

public:
    explicit QBentAdjustment(QWidget* targetWidget, QObject *parent = 0);
    ~QBentAdjustment();

    void start();

    void enterAdjustmentMode();
    void leaveAdjustmentMode( bool bSuccess );

    void draw(QPainter& painter, QRect rcBody);

    bool canClose();
    void onClose();

signals:
    void updateWidget();
    void updateWidgetRect(QRect rc);
    void finishBentAdjustment();

private slots:
    void onFinishRequestCommand();
};

#endif // QBENTADJUSTMENT_H
