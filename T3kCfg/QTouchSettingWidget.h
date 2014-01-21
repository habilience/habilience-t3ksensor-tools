#ifndef QTOUCHSETTINGWIDGET_H
#define QTOUCHSETTINGWIDGET_H

#include <QDialog>
#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

namespace Ui{
    class QTouchSettingWidget;
}

class QTouchSettingWidget : public QDialog, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QTouchSettingWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QTouchSettingWidget();

protected:
    void RequestSensorData( bool bDefault );

    void SendTimeTap( int nTime );
    void SendTimeLongTap( int nTime );
    void SendSensWheel( int nSensitivity );
    void SendSensZoom( int nSensitivity );

    // QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    // QDialog
    virtual void showEvent(QShowEvent *evt);

    // QT3kDeviceREventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

protected:
    QTimer*                     m_pTimerSendData;

    QRequestHIDManager          m_RequestSensorData;

private:
    Ui::QTouchSettingWidget* ui;
    QT3kDeviceR*&                m_pT3kHandle;

private slots:
    void on_SldZoom_valueChanged(int value);
    void on_SldWheel_valueChanged(int value);
    void on_SldLongTap_valueChanged(int value);
    void on_SldTap_valueChanged(int value);
    void on_BtnDefault_clicked();
    //void on_BtnRefresh_clicked();
    void OnTimer();
};

#endif // QTOUCHSETTINGWIDGET_H
