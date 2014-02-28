#ifndef TABCALIBRATIONWIDGET_H
#define TABCALIBRATIONWIDGET_H

#include "KeyDesignWidget.h"
#include "QT3kDeviceEventHandler.h"
#include "SoftKey.h"

#include <QWidget>
#include <QTime>


namespace Ui {
class TabCalibrationWidget;
}

class TabCalibrationWidget : public QWidget, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

public:
    TabCalibrationWidget(QWidget* parent = NULL);
    virtual ~TabCalibrationWidget();

    bool isCalibrationMode() { return m_bCalibrationMode; }
    void setInvertDrawing(bool bInvert) { m_wndTestCanvas.setInvertDrawing( bInvert ); if( isVisible() ) update(); }
    void closeChildWidget() { if( m_wndTestCanvas.isVisible() ) m_wndTestCanvas.close(); }

    virtual void keyPressEvent(QKeyEvent *);

protected:

    bool prepareKeyCalibration();
    void keyCalibration();
    void cancelKeyCalibration();

    bool verifyCalibrationPoint( bool bIsGroup );

	enum BuzzerType { BuzzerEnterCalibration, BuzzerCancelCalibration, BuzzerCalibrationSucces, BuzzerClick, BuzzerNextPoint };
    void playBuzzer( BuzzerType eType );

    bool doCalibration( GroupKey* pGroup );
    bool doCalibration( CSoftkey* key );

    bool verifyGPIO( int &nSensorGPIOCount );

    void forceMouseEvent( uchar cButtons, char cWheel, int wX, int wY );

    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO devInfo, bool bIsDownload);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnMSG(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *txt);
    virtual void TPDP_OnDVC(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, t3kpacket::_body::_dvc *device);

    //
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *);

    void onCalibrationPoint( QPoint ptCal );
    bool onCalibrationFinish();

    void updateDesignCanvas( bool bCalibrate, int nCalPos=0, bool bSet=false );

    bool writeToSensor( bool bLogicOnly );

    void changeFileAssociateBtnTitle();

protected:
    bool			m_bCalibrationMode;

    int				m_nPointCount;
    QPoint			m_ptPrevTouch;
    long			m_lAveDiffX;
    long			m_lAveDiffY;

    long			m_lAreaC;
    bool			m_bPointOK;
    ulong			m_dwTickPoint;
    QTime           m_TickPoint;
    QPoint			m_ptTouch;
    QPoint			m_ptTickPos;

    bool			m_bTouchCheck;

    int				m_nTouchCount;

    QPoint			m_ptCalibration[4];

    int				m_nCalibrationMaxStep;
    int				m_nCalibrationStep;
    int				m_nCalibrationCheckPoint;

    bool			m_bLoadFromSensor;

    uchar			m_cMouseButtons;

    bool			m_bFirmwareDownload;

    int				m_nSensorGPIOCount;

    QString			m_strBackupSoftlogic;

    QVector<CSoftkey*>	m_NoGroupKeys;

    bool m_bCheckInvertDrawing;

    int             m_nTimerCheckPoint;
    int             m_nTimerRecheckPoint;
    int             m_nTimerCheckAssociated;

    QKeyDesignWidget	m_wndTestCanvas;

private:
    Ui::TabCalibrationWidget* ui;

signals:
    QT3kDevice* getT3kHandle();
    bool isT3kConnected();
    bool isT3kInvalidFirmware();
    void updatePreview();
    void updateCalibrationStep(GroupKey* pGroup, CSoftkey* key, int nCalPos, bool bSet);
    void displayPreviewTouchCount(int nTouchCount);
    void enableControls(bool bEnable);
    bool isAssociateFileExt();
    void doAssociateFileExt();
    void doRemoveFileExtAssociation();
    void invertDrawing(bool bInvert);

private slots:
    void on_BtnCalibration_clicked();
    void on_BtnCancel_clicked();
    void on_BtnWriteNoCali_clicked();
    void on_BtnTest_clicked();
    void on_BtnWriteLogic_clicked();
    void on_BtnEraseAll_clicked();
    void on_BtnHSK_clicked();
    void on_ChkBackground_toggled(bool checked);
    void onCloseTestWidget();

public slots:
    void onConnectedT3kDevice();
};

#endif // TABCALIBRATIONWIDGET_H
