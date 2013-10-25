#ifndef QADVANCEDSETTINGWIDGET_H
#define QADVANCEDSETTINGWIDGET_H

#include "QRequestHIDManager.h"
#include "TPDPEventMultiCaster.h"

#include <QDialog>


namespace Ui {
class QAdvancedSettingWidget;
}

class QAdvancedSettingWidget : public QDialog, public TPDPEventMultiCaster::ITPDPEventListener
{
    Q_OBJECT

public:
    explicit QAdvancedSettingWidget(QWidget *parent = 0);
    ~QAdvancedSettingWidget();

protected:
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);

protected:
    QString m_strCam1PosDefault;
    QString m_strCam2PosDefault;
    QString m_strCamS1PosDefault;
    QString m_strCamS2PosDefault;

private:
    Ui::QAdvancedSettingWidget *ui;

    QRequestHIDManager      m_RequestManager;

private slots:
    void on_BtnStart_clicked();

    void on_ChkDetection_clicked();

    void on_BtnDefault_clicked();
};

#endif // QADVANCEDSETTINGWIDGET_H
