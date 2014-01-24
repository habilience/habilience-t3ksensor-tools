#ifndef QSENSORSETTINGWIDGET_H
#define QSENSORSETTINGWIDGET_H

#include <QWidget>
#include <QToolButton>

#include "QT3kDeviceREventHandler.h"
#include "QDiableTouchWidget.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

namespace Ui {
    class QSensorSettingWidget;
}

class QSensorSettingWidget : public QWidget, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QSensorSettingWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
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

    virtual void onChangeLanguage();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

private:
    Ui::QSensorSettingWidget *ui;

    QT3kDeviceR*&                m_pT3kHandle;

    QString                     m_strCaptionON;
    QString                     m_strCaptionOFF;

    QRequestHIDManager          m_RequestSensorData;

    int                         m_nChkUsbCfgMode;

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
