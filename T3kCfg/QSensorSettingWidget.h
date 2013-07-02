#ifndef QSENSORSETTINGWIDGET_H
#define QSENSORSETTINGWIDGET_H

#include <QWidget>
#include <QToolButton>

#include "QTPDPEventMultiCaster.h"
#include "QWarningWidget.h"
#include "QDiableTouchWidget.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

namespace Ui {
    class QSensorSettingWidget;
}

class QSensorSettingWidget : public QWidget, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QSensorSettingWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QSensorSettingWidget();

    enum BuzzerType { BT_ERROR, BT_CLICK, BT_KEYTONE, BT_CALIBRATION, BT_SENSORATTACH, BT_USBATTACH, BT_PENPAIRING, MaxBuzzer };

    void SetDefault();
    void Refresh();

protected:
    void Init();

    void RequestGeneralSetting( bool bDefault );
    void ParseBuzzerSetting( const char* szCmd );
    void OnBuzzerPlay(unsigned int nIndex);
    void OnBnClickedCheckBuzzer( QToolButton* pBtn, unsigned int nIndex );

    virtual void OnChangeLanguage();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void OnRSP(ResponsePart Part, short lTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

private:
    Ui::QSensorSettingWidget *ui;

    T3kHandle*&                m_pT3kHandle;

    QString                     m_strCaptionON;
    QString                     m_strCaptionOFF;

    QRequestHIDManager          m_RequestSensorData;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);

private slots:
    void on_BtnChkTouch_clicked();
    void on_BtnDiagnostics_clicked();
    void on_CBLTouchKeyPresses_activated(int index);
    void on_CBLTouchEnableKey_activated(int index);
    void on_CBLCalibrationKeyPresses_activated(int index);
    void on_CBLCalibrationKey_activated(int index);
    void on_BtnChkPenPairing_clicked();
    void on_BtnChkUSBAttach_clicked();
    void on_BtnChkSensor_clicked();
    void on_BtnChkCalibration_clicked();
    void on_BtnChkKeytone_clicked();
    void on_BtnChkSingleClk_clicked();
    void on_BtnChkError_clicked();
    void on_BtnSoundPenPairing_clicked();
    void on_BtnSoundSensor_clicked();
    void on_BtnSoundKeytone_clicked();
    void on_BtnSoundError_clicked();
    void on_BtnSoundCalibration_clicked();
    void on_BtnSoundUSB_clicked();
    void on_BtnSoundSingle_clicked();
};

#endif // QSENSORSETTINGWIDGET_H
