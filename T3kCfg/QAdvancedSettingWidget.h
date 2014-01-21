#ifndef QADVANCEDSETTINGWIDGET_H
#define QADVANCEDSETTINGWIDGET_H

#include "QRequestHIDManager.h"
#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"

#include <QDialog>


namespace Ui {
class QAdvancedSettingWidget;
}

class QAdvancedSettingWidget : public QDialog, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QAdvancedSettingWidget(QWidget *parent = 0);
    ~QAdvancedSettingWidget();

protected:
    // QDialog
    virtual void closeEvent(QCloseEvent *);

    // QT3kDeviceREventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    // QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

protected:
    QString m_strCam1PosTrc;
    QString m_strCam2PosTrc;
    QString m_strCamS1PosTrc;
    QString m_strCamS2PosTrc;
    QString m_strFactoryCalibration;

    bool    m_bRSE;

private:
    Ui::QAdvancedSettingWidget *ui;

    QRequestHIDManager      m_RequestManager;

private slots:
    void on_BtnStart_clicked();
    void on_ChkDetection_clicked();
    void on_BtnDefault_clicked();
};

#endif // QADVANCEDSETTINGWIDGET_H
