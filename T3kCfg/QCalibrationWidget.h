#ifndef QCALIBRATIONWIDGET_H
#define QCALIBRATIONWIDGET_H

#include <QWidget>
#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"

#define QCALI_POINTS                (9)
#define UNDER_VER_CALI_PNTS         (9)
#define SUPPORT_VER_CALI_PNTS       (4)

class QCalibrationWidget : public QWidget, public QLangManager::LangChangeNotify, public QTPDPEventMultiCaster::ITPDPEventListener
{
    Q_OBJECT
public:
    explicit QCalibrationWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QCalibrationWidget();

    bool ShowWindow( bool nShow, int nUsbConfigMode=0, float fScreenMargin=0, int nMacMargin=0, float fMMVersion=0.0f );

protected:
    void EscapeCalibrationMode();

    virtual void OnMSG(ResponsePart Part, ushort /*nTickTime*/, const char *sPartId, const char *sTxt);

    virtual void OnChangeLanguage();

    virtual void keyPressEvent(QKeyEvent *evt);
    virtual void showEvent(QShowEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *evt);
    virtual void focusOutEvent(QFocusEvent *evt);
    virtual void timerEvent(QTimerEvent *);

protected:
    T3kHandle*&                m_pT3kHandle;
    float                       m_fScreenMargin;

    bool*                       m_paryCaliPoints;

    QPoint                      m_ptCur;
    QString                     m_strTitle;

    bool                        m_bUnderVersion;

    int                         m_bDigitizerMode;
    int                         m_nManualCaliPointX;
    int                         m_nManualCaliPointY;

    uint                        m_nTimerUpdate;
    bool                        m_bUpdateFlag;
};

#endif // QCALIBRATIONWIDGET_H
