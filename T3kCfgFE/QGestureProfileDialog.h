#ifndef QGESTUREPROFILEDIALOG_H
#define QGESTUREPROFILEDIALOG_H

#include <QDialog>
#include "QGestureMappingTable.h"
#include "QT3kDeviceEventHandler.h"

namespace Ui {
class QGestureProfileDialog;
}

class QColorTabWidget;
class QGestureProfileDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

protected:
    void onChangeLanguage();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    virtual void paintEvent(QPaintEvent *);

    void sensorReset();
    void sensorRefresh( bool bTabOnly=false );

    void resetDataWithInitData(const QString& strCmd, bool bWithFactoryDefault=true );

    void getRadioProfileIndex(int& nMouseIndex, int& nMTIndex);
    void sendInputMode();
public:
    explicit QGestureProfileDialog(QWidget *parent = 0);
    ~QGestureProfileDialog();

private slots:
    void onTabSelChanged(QColorTabWidget* pTabWidget, int tabIndex);
    
    void on_btnInputModeMouseGeneral_clicked();
    void on_btnInputModeMouseWindows7_clicked();
    void on_btnInputModeMouseMac_clicked();
    void on_btnInputModeMouseLinux_clicked();
    void on_btnInputModeMouseUserDefined_clicked();
    void on_btnInputModeMTGeneral_clicked();
    void on_btnInputModeMTWindows7_clicked();
    void on_btnInputModeMTMac_clicked();
    void on_btnInputModeMTLinux_clicked();
    void on_btnInputModeMTUserDefined_clicked();
    void on_btnReset_clicked();
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();

private:
    Ui::QGestureProfileDialog *ui;
    QGestureMappingTable    m_widgetGestureMappingTable;
};

#endif // QGESTUREPROFILEDIALOG_H
