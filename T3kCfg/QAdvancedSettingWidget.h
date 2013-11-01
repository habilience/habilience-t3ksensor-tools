#ifndef QADVANCEDSETTINGWIDGET_H
#define QADVANCEDSETTINGWIDGET_H

#include "QRequestHIDManager.h"
#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"

#include <QDialog>


namespace Ui {
class QAdvancedSettingWidget;
}

class QAdvancedSettingWidget : public QDialog, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QAdvancedSettingWidget(QWidget *parent = 0);
    ~QAdvancedSettingWidget();

protected:
    virtual void closeEvent(QCloseEvent *);

    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);

    virtual void onChangeLanguage();

protected:
    QString m_strCam1PosTrc;
    QString m_strCam2PosTrc;
    QString m_strCamS1PosTrc;
    QString m_strCamS2PosTrc;
    QString m_strFactoryCalibration;

private:
    Ui::QAdvancedSettingWidget *ui;

    QRequestHIDManager      m_RequestManager;

private slots:
    void on_BtnStart_clicked();

    void on_ChkDetection_clicked();

    void on_BtnDefault_clicked();
};

#endif // QADVANCEDSETTINGWIDGET_H
