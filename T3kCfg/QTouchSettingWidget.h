#ifndef QTOUCHSETTINGWIDGET_H
#define QTOUCHSETTINGWIDGET_H

#include <QDialog>
#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

namespace Ui{
    class QTouchSettingWidget;
}

class QTouchSettingWidget : public QDialog, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QTouchSettingWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QTouchSettingWidget();

protected:
    void RequestSensorData( bool bDefault );

    void SendTimeTap( int nTime );
    void SendTimeLongTap( int nTime );
    void SendSensWheel( int nSensitivity );
    void SendSensZoom( int nSensitivity );

    virtual void OnChangeLanguage();

    virtual void showEvent(QShowEvent *evt);

    virtual void OnRSP(ResponsePart Part, short lTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

protected:
    QTimer*                     m_pTimerSendData;

    QRequestHIDManager          m_RequestSensorData;

private:
    Ui::QTouchSettingWidget* ui;
    T3kHandle*&                m_pT3kHandle;

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
