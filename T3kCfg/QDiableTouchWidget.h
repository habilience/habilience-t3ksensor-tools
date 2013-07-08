#ifndef QDIABLETOUCHWIDGET_H
#define QDIABLETOUCHWIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QTimer>

#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"

namespace Ui{
    class QDiableTouchWidget;
}

class QDiableTouchWidget : public QDialog, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QDiableTouchWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QDiableTouchWidget();

protected:
    virtual void timerEvent(QTimerEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    virtual void OnChangeLanguage();
    virtual void OnRSP(ResponsePart Part, ushort nTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

protected:
    T3kHandle*&            m_pT3kHandle;

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
