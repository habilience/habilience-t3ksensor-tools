#ifndef T3KSOFTLOGICDLG_H
#define T3KSOFTLOGICDLG_H

#include "../common/TPDPEventMultiCaster.h"
#include "T3kCommonData.h"

#include "TabPanelWidget.h"
#include "TabKeyDesignWidget.h"
#include "TabLogicDesignWidget.h"
#include "TabCalibrationWidget.h"
#include "SelectDeviceWidget.h"

#include "KeyDesignWidget.h"

#include <QMainWindow>
#include <QStackedLayout>

namespace Ui {
class T3kSoftlogicDlg;
}

class T3kSoftlogicDlg : public QMainWindow, public TPDPEventMultiCaster::ITPDPEventListener
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
    T3kHandle*              m_pT3kHandle;
    int                     m_nFirstActiveTab;
    QString                 m_strDataFileFromCmdLine;

    //HDEVNOTIFY	m_hDeviceNotify;
    int                     m_nT3kDeviceCount;

    bool                    m_bFirmwareDownload;
    bool                    m_bIsInvalidFirmware;
    bool                    m_bIsConnected;

    QString                 m_strLoadedModelPathName;

    int                     m_nTimerReconnect;

    QSelectDeviceWidget*          m_SelectDeviceDlg;

    TabPanelWidget*         m_pTabPanelWidget;
    TabKeyDesignWidget*     m_pTabKeyDesignWidget;
    TabLogicDesignWidget*	m_pTabLogicDesignWidget;
    TabCalibrationWidget*   m_pTabCalibrationWidget;

    QStackedLayout          m_StackedPreviewLayout;
//    CLogicDesignerWnd	m_wndPrevLogicDesign;

protected:
    void init();

    void checkT3kDeviceStatus();
    bool openT3kHandle();

    bool loadModel( QString lpszPathName );
    bool saveModel( QString lpszPathName );

    //void copyDataFile( QString lpszDataPath, QString lpszFileName );

    bool isModified();

protected:
    // T3kHIDNotify::IT3kEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE);
    virtual void OnCloseT3kDevice(T3K_HANDLE);
    virtual void OnFirmwareDownload(bool);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);

    // QWidget
    virtual void timerEvent(QTimerEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    //virtual bool installEventFilter(QObject *);

//    afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );

//    afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD_PTR dwData );

private:
    Ui::T3kSoftlogicDlg *ui;

signals:

public slots:
    void onHandleMessage(const QString& msg);
    void onResizedScreen(int nScreen);

private slots:
    void on_BtnNew_clicked();
    void on_BtnLoad_clicked();
    void on_BtnSave_clicked();
    void on_BtnExit_clicked();
    void on_TabMainMenu_currentChanged(int index);
    void onUpdatePrewview();
    T3kHandle* onGetT3kHandle() { return m_pT3kHandle; }
    void onUpdateCalibrationStep(GroupKey* pGroup, CSoftkey* key, int nCalPos, bool bSet);
    void onDisplayPreviewTouchCount(int nTouchCount);
    bool onIsValidT3kSensorState() { return m_bIsConnected && !m_bIsInvalidFirmware && !m_bFirmwareDownload; }
    bool onIsT3kConnected() { return m_bIsConnected; }
    bool onIsT3kInvalidFirmware() { return m_bIsInvalidFirmware; }
    void onEnableControls(bool bEnable);
    void onNotifyTab(int index);

    bool onIsAssociateFileExt();
    void onDoAssociateFileExt();
    void onDoRemoveFileExtAssociation();
    void onInvertDrawing(bool bInvert);
};

#endif // T3KSOFTLOGICDLG_H
