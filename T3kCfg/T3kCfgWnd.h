#ifndef T3KCFGWND_H
#define T3KCFGWND_H

#include <QMainWindow>

class QMainMenuWidget;
class QMouseSettingWidget;
class QCalibrationSettingWidget;
class QSensorSettingWidget;
class QGeneralSettingWidget;
class QMenuStripWidget;
class QSoftKeySettingWidget;
class QMenu;
class QSettings;

#include <QTimer>

#include "stdInclude.h"
#include "TPDPEventMultiCaster.h"

#include "QMySystemTrayIcon.h"
#include "QLangManager.h"

#include "PacketStructure.h"


namespace Ui {
    class T3kCfgWnd;
}

class T3kCfgWnd : public QMainWindow, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit T3kCfgWnd(QWidget *parent = 0);
    ~T3kCfgWnd();

    void ExitApplication();

    bool IsRegisterTrayIcon() { return m_bRegisterTrayIcon; }

    virtual void keyPressEvent(QKeyEvent *evt);

protected:
    virtual void closeEvent(QCloseEvent *evt);
    virtual void paintEvent(QPaintEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);
    virtual void showEvent(QShowEvent *);

    virtual void onChangeLanguage();

    bool CreateTrayIcon();
    void CreateAction();
    void ShowTrayMenu();

    void SetTrayIconIamge( int nProfileIndex );
    void EnableTrayProfile( bool bEnalble );

    void ShowContentsMenu();
    void HideContentsMenu();

    void FlatMenuToLeft();
    void FlatMenuToRight();

    void LoadCompany();
    QString GetStringFromVariant( QVariant& var );

    bool OpenT30xHandle();

    void ExceptionFirmwareVer( QString str );

    virtual void OnOpenT3kDevice(T3K_HANDLE);
    virtual void OnCloseT3kDevice(T3K_HANDLE);
    virtual void OnFirmwareDownload( bool bDownload );
    virtual void OnRSP( ResponsePart Part, ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd );

private:
    void Init();
    void ShowMainMenu( bool bShow );

private:
    Ui::T3kCfgWnd *ui;

    QMainMenuWidget*            m_pMainWidget;
    QMouseSettingWidget*        m_pMouseSettingWidget;
    QCalibrationSettingWidget*  m_pCaliSettingWidget;
    QSensorSettingWidget*       m_pSensorSettingWidget;
    QGeneralSettingWidget*      m_pGeneralSettingWidget;
    QSoftKeySettingWidget*      m_pSoftkeySettingWidget;

    QMenuStripWidget*           m_pMenuWidget;

    QMySystemTrayIcon*          m_pTrayIcon;
    QMenu*                      m_pTrayMenu;
    bool                        m_bRegisterTrayIcon;

    QList<QAction*>             m_listProfilesQAction;
    QAction*                    m_pOpenQAction;
    QAction*                    m_pExitQAction;

    T3kHandle*                  m_pT3kHandle;
    int                         m_nProfileIndex;

    bool                        m_bIsConnect;
    bool                        m_bFirmwareDownloading;

    int                         m_nTimerExit;
    int                         m_nTimerObserver;

    int                         m_nTimerChkTrayDoubleClk;
    QPoint                      m_ptTrayMenu;

//    int                         m_nTimerChkRemovableDrive;

    bool                        m_bSoftkey;

    int                         m_nCurInputMode;
    int                         m_nProfileIndexData;

    int                         m_nSendCmdID;

    bool                        m_bRunOtherTool;
    bool                        m_bPrevShowed;

    QString                     m_strTitle;
    QString                     m_strCompanyUrl;
    QString                     m_strProgInfo;

    int                         m_nTimerChkHIDCom;

    int                         m_nActiveWndTimer;
    QString                     m_strFirmwareVerError;

    bool                        m_bCustomLogo;
    bool                        m_bCustomLink;

    bool                        m_bInit;

    // Custom Define
protected slots:
    void OnTrayChangeProfile( QAction* pAction );
    void OnTrayOpenT3kCfg();
    void OnTrayIconActions( QSystemTrayIcon::ActivationReason TrayReason );
    void OnFinishAnimationMenu();
    void onShowMenuEvent( int nMenu );
    bool onRegisterTrayIcon( bool bRegister );
    void onByPassKeyPressEvent( QKeyEvent *evt );

    void OnShowErrorMsgBox(QString str);

    void onConnectedRemote();
    void onDisconnectedRemote();

    void CoercionExit();

    // UI
private slots:
    void on_BtnLogo_clicked();
    void on_BtnMainDefault_clicked();
    void on_BtnMainLink_clicked();
    void on_BtnMainExit_clicked();
};

#endif // T3KCFGWND_H
