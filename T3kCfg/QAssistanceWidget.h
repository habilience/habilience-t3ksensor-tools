#ifndef QASSISTANCEWIDGET_H
#define QASSISTANCEWIDGET_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"

#include "QLangManager.h"

class QTabSensorStatus;
class QTabRemoteAssistance;
class QTabChat;

namespace Ui{
    class QAssistanceWidget;
}

class QAssistanceWidget : public QDialog, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QAssistanceWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    virtual ~QAssistanceWidget();

    // QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    void ForcedClose() { m_bForcedClose = true; close(); }

protected:
    // QDialog
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);

    // public QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnSTT(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *status);

private:
    Ui::QAssistanceWidget* ui;

    QTabSensorStatus*           m_pTabStatus;
    QTabRemoteAssistance*       m_pTabRemote;
    QTabChat*                   m_pTabChat;

    QT3kDevice*&                m_pT3kHandle;

    int                         m_nCurrentTab;
    bool                        m_bForcedClose;

signals:
    void showLoadStatusWidget();

private slots:
    void on_TabCamWidget_currentChanged(int index);
    void onStartRemoteMode();
    void onEndRemoteMode();
    void onBlinkChat(QString);
    void onShowLoadSensorStatusWidget();
};

#endif // QASSISTANCEWIDGET_H
