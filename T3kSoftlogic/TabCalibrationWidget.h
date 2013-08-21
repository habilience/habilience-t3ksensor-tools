#ifndef TABCALIBRATIONWIDGET_H
#define TABCALIBRATIONWIDGET_H

//#include "DesignCanvasWnd.h"
#include "../common/TPDPEventMultiCaster.h"
#include "SoftKey.h"

#include <QWidget>
#include <QTime>


namespace Ui {
class TabCalibrationWidget;
}

class TabCalibrationWidget : public QWidget, public TPDPEventMultiCaster::ITPDPEventListener
{
    Q_OBJECT

public:
    TabCalibrationWidget(QWidget* parent = NULL);
    virtual ~TabCalibrationWidget();

    bool isCalibrationMode() { return m_bCalibrationMode; }

    void onCloseTestWnd();

    //CDesignCanvasWnd	m_wndTestCanvas;

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

    void forceMouseEvent( uchar cButtons, char cWheel, ushort wX, ushort wY );

    // TPDPEventMulticaster::ITPDPEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE);
    virtual void OnCloseT3kDevice(T3K_HANDLE);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);
    virtual void OnMSG(ResponsePart, ushort, const char *, const char *);
    virtual void OnFirmwareDownload(bool);
    virtual void OnDVC(ResponsePart, ushort, T3kDVC &);

    //
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *);

    void onCalibrationPoint( QPoint ptCal );
    void onCalibrationFinish();

    void updateDesignCanvas( bool bCalibrate, int nCalPos=0, bool bSet=false );

    bool writeToSensor( bool bLogicOnly );

    void changeFileAssociateBtnTitle();

    QObject* findWantToParent(QObject *target, const char* strObjectName);

protected:
    bool			m_bCalibrationMode;

    int				m_nPointCount;
    QPoint			m_ptPrevTouch;
    long			m_lAveDiffX;
    long			m_lAveDiffY;

    long			m_lAreaC;
    bool			m_bPointOK;
    //ulong			m_dwTickPoint;
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

private:
    Ui::TabCalibrationWidget* ui;

signals:

private slots:

    void on_BtnCalibration_clicked();
    void on_BtnCancel_clicked();
    void on_BtnWriteNoCali_clicked();
    void on_BtnTest_clicked();
    void on_BtnWriteLogic_clicked();
    void on_BtnEraseAll_clicked();
    void on_BtnHSK_clicked();
    void on_ChkBackground_toggled(bool checked);
};

#endif // TABCALIBRATIONWIDGET_H
