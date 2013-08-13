#ifndef QTOUCHSETTINGDIALOG_H
#define QTOUCHSETTINGDIALOG_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "t3kcomdef.h"
#include "QEventRedirection.h"

namespace Ui {
class QTouchSettingDialog;
}

class Dialog;
class QBorderStyleEdit;
class QTouchSettingDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
        , public QLangManager::ILangChangeNotify
        , public QEventRedirection::IEventListener
{
    Q_OBJECT
private:
    bool    m_bIsModified;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent *);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    enum RequestCmd { cmdRefresh, cmdWriteToFactoryDefault, cmdLoadFactoryDefault, cmdInitialize };
    bool requestSensorData( RequestCmd cmd, bool bWait );
    void sensorReset();
    void sensorLoadFactoryDefault();
    void sensorRefresh();
    void sensorWriteToFactoryDefault();

    void resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault=true);

    void enableAllControls( bool bEnable );

    void resetEditColors();
    void setModifyEditColor(QBorderStyleEdit* pEdit);
    void setModifyAllEditColors();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    // override QEventRedirection::IEventListener
    virtual bool onKeyPress(QKeyEvent *evt);
    virtual bool onKeyRelease(QKeyEvent *evt);
    virtual bool onRButtonDblClicked();

    void sendEditValue( QBorderStyleEdit* txtEdit, float step, float fMin, float fMax, const QString& strCmd );

    void drawTouchMargin( QPainter& p );

    void drawHorzArrow( QPainter& p, QPoint pt1, QPoint pt2, QPoint pt3, QPoint pt4 );
    void drawVertArrow( QPainter& p, QPoint pt1, QPoint pt2, QPoint pt3, QPoint pt4 );

public:
    explicit QTouchSettingDialog(Dialog *parent = 0);
    ~QTouchSettingDialog();
    bool canClose();
    
private slots:
    virtual void reject();
    virtual void accept();

    void onEditModified(QBorderStyleEdit* pEdit, int nValue, double dValue);

    void on_btnClose_clicked();
    void on_btnReset_clicked();
    void on_btnRefresh_clicked();
    void on_btnSave_clicked();
    void on_btnGestureProfile_clicked();
    void on_btnTMLeftDec_clicked();
    void on_btnTMLeftInc_clicked();
    void on_btnTMTopDec_clicked();
    void on_btnTMTopInc_clicked();
    void on_btnTMRightDec_clicked();
    void on_btnTMRightInc_clicked();
    void on_btnTMBottomDec_clicked();
    void on_btnTMBottomInc_clicked();
    void on_gbAutoSelect_clicked();
    void on_btnMouse_clicked();
    void on_btnMultitouch_clicked();
    void on_btnFull_clicked();
    void on_btnDigitizerOnly_clicked();

    void on_horzSliderTap_valueChanged(int value);

    void on_horzSliderLongTap_valueChanged(int value);

    void on_horzSliderWheel_valueChanged(int value);

    void on_horzSliderZoom_valueChanged(int value);

private:
    Dialog* m_pMainDlg;
    Ui::QTouchSettingDialog *ui;
    QEventRedirection m_EventRedirect;
};

#endif // QTOUCHSETTINGDIALOG_H
