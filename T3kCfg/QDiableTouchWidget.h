#ifndef QDIABLETOUCHWIDGET_H
#define QDIABLETOUCHWIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QTimer>

#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"

namespace Ui{
    class QDiableTouchWidget;
}

class QDiableTouchWidget : public QDialog, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QDiableTouchWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    ~QDiableTouchWidget();

protected:
    virtual void timerEvent(QTimerEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    virtual void onChangeLanguage();
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

protected:
    QT3kDevice*&            m_pT3kHandle;

    int                     m_nTimerRemain;
    QString                 m_strTimeout;
    QTimer                  m_TimerCountDown;

private:
    Ui::QDiableTouchWidget* ui;

private slots:
    void on_BtnOK_clicked();
    void OnTimer();
    void On_Cancel();
};

#endif // QDIABLETOUCHWIDGET_H
