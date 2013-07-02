#ifndef QCALIBRATIONSETTINGWIDGET_H
#define QCALIBRATIONSETTINGWIDGET_H

#include <QWidget>
#include "QTPDPEventMultiCaster.h"
#include "QCalibrationWidget.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"
#include "QTouchSettingWidget.h"

class QLineEdit;

namespace Ui {
    class QCalibrationSettingWidget;
}

class QCalibrationSettingWidget : public QWidget, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QCalibrationSettingWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QCalibrationSettingWidget();

    void SetDefault();
    void Refresh();

    void ChildClose();

protected:
    virtual void OnChangeLanguage();
    void RequestSensorData( bool bDefault );
    void ShowCalibrationWindow( bool bShow, int nScreenMargin=-1, int nMacMargin=0 );
    float ChangeRangeValue( bool bDecrease, float fCurrentValue );
    void OnRangeLeftRight( bool bDecrease, QLineEdit* pEdit, int nRangeS, int nRangeE, int nRangeMax, const char* szCmdH );

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void OnRSP(ResponsePart Part, short lTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);
    virtual void OnSTT(ResponsePart Part, short nTickTime, const char *sPartId, const char *pStatus);

protected:
    QCalibrationWidget*         m_pwndCalibration;
    float			m_fScreenMargin;

    QTouchSettingWidget*        m_pTouchSettingWnd;

    QRequestHIDManager          m_RequestSensorData;

    float                       m_fMMVersion;
    int                         m_nUsbConfigMode;

private:
    Ui::QCalibrationSettingWidget *ui;

    T3kHandle*&                m_pT3kHandle;

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
};

#endif // QCALIBRATIONSETTINGWIDGET_H
