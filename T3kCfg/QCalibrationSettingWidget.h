#ifndef QCALIBRATIONSETTINGWIDGET_H
#define QCALIBRATIONSETTINGWIDGET_H

#include <QWidget>
#include "QT3kDeviceEventHandler.h"
#include "QCalibrationWidget.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"
#include "QTouchSettingWidget.h"

class QLineEdit;
class QAdvancedSettingWidget;

namespace Ui {
    class QCalibrationSettingWidget;
}

class QCalibrationSettingWidget : public QWidget, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QCalibrationSettingWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    ~QCalibrationSettingWidget();

    void SetDefault();
    void Refresh();

    void ChildClose();

protected:
    virtual void onChangeLanguage();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnSTT(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *status);

    void RequestSensorData( bool bDefault );
    void ShowCalibrationWindow( bool bShow, int nScreenMargin=-1, int nMacMargin=0 );
    float ChangeRangeValue( bool bDecrease, float fCurrentValue );
    void OnRangeLeftRight( bool bDecrease, QLineEdit* pEdit, int nRangeS, int nRangeE, int nRangeMax, const char* szCmdH );

protected:
    QCalibrationWidget*         m_pwndCalibration;
    float                       m_fScreenMargin;

    QTouchSettingWidget*        m_pTouchSettingWnd;
    QAdvancedSettingWidget*     m_pAdvancedWidget;

    QRequestHIDManager          m_RequestSensorData;

    float                       m_fMMVersion;
    int                         m_nUsbConfigMode;

private:
    Ui::QCalibrationSettingWidget *ui;

    QT3kDevice*&                m_pT3kHandle;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);

private slots:
    void on_BtnTouchSetting_clicked();
    void on_BtnPalmInc_clicked();
    void on_BtnPalmDec_clicked();
    void on_BtnTwoTouchInc_clicked();
    void on_BtnTwoTouchDec_clicked();
    void on_BtnDoubleClkInc_clicked();
    void on_BtnDoubleClkDec_clicked();
    void on_BtnSingleClkInc_clicked();
    void on_BtnSingleClkDec_clicked();
    void on_BtnMarginInc_clicked();
    void on_BtnMarginDec_clicked();
    void on_BtnCalibration_clicked();
    void on_BtnAdvanced_clicked();
};

#endif // QCALIBRATIONSETTINGWIDGET_H
