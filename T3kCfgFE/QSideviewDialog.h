#ifndef QSIDEVIEWDIALOG_H
#define QSIDEVIEWDIALOG_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "t3kcomdef.h"
#include "QEventRedirection.h"

namespace Ui {
class QSideviewDialog;
}

class Dialog;
class QBorderStyleEdit;
class QSideviewDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
        , public QLangManager::ILangChangeNotify
        , public QEventRedirection::IEventListener
{
    Q_OBJECT

private:
    bool    m_bIsModified;
    int     m_TimerPreviewCountDown;
    int     m_TimerNoData;
    int     m_TimerRefreshAutoOffset;

    int     m_nPreviewCountDown;

    bool    m_bIsNoData;
    bool    m_bDrawNoData;
    int     m_nCurrentCameraIndex;

    int     m_nDetectionLine;
    int     m_nAutoOffset;

    QString m_strCMModelName;
    bool    m_bSimpleDetection;

    QImage* m_pImgSideview;
    uchar*  m_pImgTempBuffer;

    QRect   m_rcUpdateImage;
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *evt);
    virtual void closeEvent(QCloseEvent *);

    // override QEventRedirection::IEventListener
    virtual bool onKeyPress(QKeyEvent *evt);
    virtual bool onKeyRelease(QKeyEvent *evt);
    virtual bool onMouseWheel(QWheelEvent *evt);
    virtual void onRButtonClicked();
    virtual bool onRButtonDblClicked();

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };

    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorRefresh();
    void sensorWriteToFactoryDefault();

    void resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault=true);

    void resetEditColors();
    void setModifyEditColor(QBorderStyleEdit* pEdit);
    void setModifyAllEditColors();

    void enableAllControls( bool bEnable );

    void setSideview(int nCameraIndex, bool setMode);
    QString getCameraPrefix( int nCameraIndex );

    void initImageBuffer( int width, int height );

    void drawSideviewImage(QPainter &p, const QRect& rcImageDst, int nScaleY );
    void drawGraph(QPainter& p, const QRect& rcGraphDst);

    void adjustAmbientLight(int nLight1, int nLight2, int nLight3);
    void stopPreviewCountDown();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnPRV(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnMSG(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *txt);
public:
    explicit QSideviewDialog(Dialog *parent);
    ~QSideviewDialog();

    bool canClose();
    void onClose();
    
private slots:
    virtual void reject();
    virtual void accept();

    void onGlobalMouseMoved();
    void onEditModified(QBorderStyleEdit* pEdit, int nValue, double dValue);

    void on_btnClose_clicked();
    void on_btnReset_clicked();
    void on_btnRefresh_clicked();
    void on_btnSave_clicked();
    void on_btnCam1_clicked();
    void on_btnCam2_clicked();
    void on_btnCam1_1_clicked();
    void on_btnCam2_1_clicked();
    void on_btnDetectLineUp_clicked();
    void on_btnDetectLineDn_clicked();
    void on_btnLight1Dec_clicked();
    void on_btnLight1Inc_clicked();
    void on_btnLight2Dec_clicked();
    void on_btnLight2Inc_clicked();
    void on_btnLight3Dec_clicked();
    void on_btnLight3Inc_clicked();

private:
    Dialog*     m_pMainDlg;
    Ui::QSideviewDialog *ui;
    QEventRedirection m_EventRedirect;
};

#endif // QSIDEVIEWDIALOG_H
