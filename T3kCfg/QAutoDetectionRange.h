#ifndef QAUTODETECTIONRANGE_H
#define QAUTODETECTIONRANGE_H

#include <QObject>
#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"
//#include "t3kcomdef.h"

class QBorderStyleEdit;
class QAutoDetectionRange : public QObject
        , public TPDPEventMultiCaster::ITPDPEventListener
        , public QLangManager::ILangChangeNotify
{
    Q_OBJECT
private:
    bool    m_bEnterAutoRangeSetting;
    int     m_nAutoRangeStep;
    bool    m_bTouchOK;
    bool    m_bCamTouch;
    unsigned long m_dwTickTouch;
    int     m_nCamTouchCount[2];
    QPoint  m_ptCamTouchObj[2][4];
    int     m_nCamTouchMax[2];

    int     m_nOldMargins[4];

    long    m_lCam1Left;
    long    m_lCam1Right;
    long    m_lCam2Left;
    long    m_lCam2Right;

    int     m_TimerBlinkArrow;

    bool    m_bToggleArrow;     // true: show, false: hide
    int     m_nTouchProgress;   // for Auto Range Setting
    QRect   m_rcArrow[4];
    QRect   m_rcProgress[4];

    QPixmap*        m_pPixmap;

    QRequestHIDManager  m_RequestHIDManager;

    T3kHandle*          m_pT3kHandle;

    bool                m_bChekcFinish;

protected:
    virtual void timerEvent(QTimerEvent *);

    void drawMonitor(QPainter& p, QRect rcBody);
    void drawArrow(QPainter& p, QRect rcTouchArea);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };
    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorWriteToFactoryDefault();

    void resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault=true);

    // override QT3kDeviceEventHandler::IListener
    virtual void OnDTC(ResponsePart, ushort, const char *, int, T3kRangeF *, unsigned short);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);
    virtual void OnRSE(ResponsePart, ushort, const char *, long, bool, const char *);

    void onFinishAutoRange();

    void showArrow();
    void hideArrow();
    void updateRect(QRect rc);

    void analysisTouchObj();

public:
    explicit QAutoDetectionRange(QObject *parent = 0);
    ~QAutoDetectionRange();

    void enterAutoRangeSetting();
    void leaveAutoRangeSetting();

    void draw(QPainter &painter, QRect rcBody);

signals:
    void updateWidget();
    void updateWidgetRect(QRect rc);
    void finishDetectionRange(bool bRet);

    void showProgressDialog();

private slots:
    void onRequestFinish();
    void onShowProgressDialog();
};

#endif // QAUTODETECTIONRANGE_H
