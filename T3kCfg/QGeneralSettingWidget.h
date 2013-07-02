#ifndef QGENERALSETTINGWIDGET_H
#define QGENERALSETTINGWIDGET_H

#include "ui_QGeneralSettingWidget.h"

#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

class QGeneralSettingWidget : public QWidget, private Ui::QGeneralSettingWidget, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QGeneralSettingWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QGeneralSettingWidget();

    void SetDefault();
    void Refresh();

    virtual void OnChangeLanguage();

protected:
    void RequestGeneralSetting( bool bDefault );

    void ChangeRadioButtonOrientation( int nOrientation );

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);

    virtual void OnRSP(ResponsePart Part, short lTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

protected:
    int                         m_nInputModeV;
    int                         m_nInputMode;
    int                         m_nTimerAutoInputMode;

    QRequestHIDManager          m_RequestSensorData;

    int                         m_nDisplayOrientation;

private:
    T3kHandle*&                m_pT3kHandle;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void RegisterTrayIcon( bool bRegister );

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
};

#endif // QGENERALSETTINGWIDGET_H
