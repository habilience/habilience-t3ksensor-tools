#ifndef QASSISTANCEWIDGET_H
#define QASSISTANCEWIDGET_H

#include <QDialog>
#include "../common/TPDPEventMultiCaster.h"

#include "QLangManager.h"

class QTabSensorStatus;
class QTabRemoteAssistance;
class QTabChat;

namespace Ui{
    class QAssistanceWidget;
}

class QAssistanceWidget : public QDialog, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QAssistanceWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    virtual ~QAssistanceWidget();
    virtual void OnChangeLanguage();

    void ForcedClose() { m_bForcedClose = true; close(); }

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);

    virtual void OnRSP(ResponsePart Part, ushort nTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);
    virtual void OnSTT(ResponsePart, ushort, const char *, const char *pStatus);

private:
    Ui::QAssistanceWidget* ui;

    QTabSensorStatus*           m_pTabStatus;
    QTabRemoteAssistance*       m_pTabRemote;
    QTabChat*                   m_pTabChat;

    T3kHandle*&                m_pT3kHandle;

    int                         m_nCurrentTab;
    bool                        m_bForcedClose;

private slots:
    void on_TabCamWidget_currentChanged(int index);
    void onStartRemoteMode();
    void onEndRemoteMode();
    void onBlinkChat(QString);
};

#endif // QASSISTANCEWIDGET_H
