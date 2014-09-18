#ifndef T3KSOFTLOGICDLG_H
#define T3KSOFTLOGICDLG_H

#include "QT3kDeviceEventHandler.h"
#include "T3kCommonData.h"

#include "TabPanelWidget.h"
#include "TabKeyDesignWidget.h"
#include "TabLogicDesignWidget.h"
#include "TabCalibrationWidget.h"
#include "SelectDeviceWidget.h"

#include "KeyDesignWidget.h"

#include <QDialog>
#include <QStackedLayout>

namespace Ui {
class T3kSoftlogicDlg;
}

class T3kSoftlogicDlg : public QDialog, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT
    
public:
    explicit T3kSoftlogicDlg(QWidget *parent = 0, QString strLoadMoel = "");
    ~T3kSoftlogicDlg();

//    bool isConnected() { return m_bIsConnected; }
//    bool isInvalidFirmware() { return m_bIsInvalidFirmware; }
//    bool isFirmwareDownload() { return m_bFirmwareDownload; }

    void setFocusPanelName();

    void setActiveTab( int nActiveTab ) { m_nFirstActiveTab = nActiveTab; }
    void doExecute( QString str );

    bool checkModified();

protected:
    QT3kDevice*            m_pT3kHandle;
    int                     m_nFirstActiveTab;
    QString                 m_strDataFileFromCmdLine;

    //HDEVNOTIFY	m_hDeviceNotify;
    int                     m_nT3kDeviceCount;

    bool                    m_bFirmwareDownload;
    bool                    m_bIsInvalidFirmware;
    bool                    m_bIsConnected;

    QString                 m_strLoadedModelPathName;

    int                     m_nTimerReconnect;
#ifdef Q_OS_WIN
    int                     m_nTimerCheckRunning;
#endif

    QSelectDeviceWidget*          m_SelectDeviceDlg;

    TabPanelWidget*         m_pTabPanelWidget;
    TabKeyDesignWidget*     m_pTabKeyDesignWidget;
    TabLogicDesignWidget*	m_pTabLogicDesignWidget;
    TabCalibrationWidget*   m_pTabCalibrationWidget;

    QStackedLayout          m_StackedPreviewLayout;

protected:
    void init();

    void checkT3kDeviceStatus();
    bool openT3kHandle();

    bool loadModel( QString lpszPathName );
    bool saveModel( QString lpszPathName );

    bool isModified();

protected:
    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO devInfo, bool bIsDownload);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    // QWidget
    virtual void timerEvent(QTimerEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    //virtual bool installEventFilter(QObject *);

private:
    Ui::T3kSoftlogicDlg *ui;

signals:
    void connectedT3kDevice();

public slots:
    void onHandleMessage(const QString& msg);
    void onResizedScreen(int nScreen);

private slots:
    void on_BtnNew_clicked();
    void on_BtnLoad_clicked();
    void on_BtnSave_clicked();
    void on_BtnExit_clicked();
    void on_TabMainMenu_currentChanged(int index);
    void on_BtnLicense_clicked();
    void onUpdatePrewview();
    QT3kDevice* onGetT3kHandle() { return m_pT3kHandle; }
    void onUpdateCalibrationStep(GroupKey* pGroup, CSoftkey* key, int nCalPos, bool bSet);
    void onDisplayPreviewTouchCount(int nTouchCount);
    bool onIsValidT3kSensorState() { return m_bIsConnected && !m_bIsInvalidFirmware && !m_bFirmwareDownload; }
    bool onIsT3kConnected() { return m_bIsConnected; }
    bool onIsT3kInvalidFirmware() { return m_bIsInvalidFirmware; }
    void onEnableControls(bool bEnable);

    bool onIsAssociateFileExt();
    void onDoAssociateFileExt();
    void onDoRemoveFileExtAssociation();
    void onInvertDrawing(bool bInvert);

    void onConnectedT3kDevice();
};

#endif // T3KSOFTLOGICDLG_H
