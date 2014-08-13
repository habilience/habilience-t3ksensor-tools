#ifndef QGENERALSETTINGWIDGET_H
#define QGENERALSETTINGWIDGET_H

#include "ui_QGeneralSettingWidget.h"

#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

class QGeneralSettingWidget : public QWidget, private Ui::QGeneralSettingWidget, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QGeneralSettingWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    ~QGeneralSettingWidget();

    void SetDefault();
    void Refresh();

    virtual void onChangeLanguage();

protected:
    void RequestGeneralSetting( bool bDefault );

    void ChangeRadioButtonOrientation( int nOrientation );

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
#ifdef Q_OS_MAC
    virtual void TPDP_OnGST(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char cActionGroup, unsigned char cAction, unsigned short wFeasibleness, unsigned short x, unsigned short y, unsigned short w, unsigned short h, float fZoom, const char *msg);
#endif

protected:
    int                         m_nInputModeV;
    int                         m_nInputMode;
    int                         m_nTimerAutoInputMode;

    int                         m_nChkUsbCfgMode;

    QRequestHIDManager          m_RequestSensorData;

    int                         m_nDisplayOrientation;

    bool                        m_bOSXGesture;

private:
    QT3kDevice*&                m_pT3kHandle;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void RegisterTrayIcon( bool bRegister );
    void enableMacOSXGesture(bool bEnable);

private slots:
    void on_RBtnLandscape_clicked();
    void on_RBtnPortrait_clicked();
    void on_RBtnLandscapeF_clicked();
    void on_RBtnPortraitF_clicked();

    void on_RBMultiTouchWin7_clicked();
    void on_RBMouse_clicked();
    void on_ChkInputModeAutoSelect_clicked(bool clicked);
    void on_CBLLanguage_activated(int index);
    void on_chkTrayIcon_toggled(bool checked);
    void on_chkOSXGesture_toggled(bool checked);

public slots:
    void onConnectedDevice();
};

#endif // QGENERALSETTINGWIDGET_H
