#ifndef QMENUSTRIPWIDGET_H
#define QMENUSTRIPWIDGET_H

#include <QWidget>
#include "QRaisePushButton.h"

#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"


#define QMENU_COUNT                 7

#define QMENU_HOME                  0
#define QMENU_MOUSE                 1
#define QMENU_CALIBRATION           2
#define QMENU_SENSOR                3
#define QMENU_SOFTKEY               4
#define QMENU_GENERAL               5
#define QMENU_REMOTE                6

namespace Ui {
    class QMenuStripWidget;
}

class QMenuStripWidget : public QWidget, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT
public:
    explicit QMenuStripWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QMenuStripWidget();

    void SetMenuButton( int nIndex );

protected:
    void ClickMenu( int nMenu );

    virtual void showEvent(QShowEvent *evt);

    virtual void OnChangeLanguage();
    virtual void OnRSP(ResponsePart Part, ushort nTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

protected:
    QRaisePushButton**      m_arybtnMenu;

    bool                    m_bAnimation;
    QObject*                m_pCurObj;
    QObject*                m_pNextObj;

    QList<QIcon*>           m_listIcon;

    bool                    m_bSoftkey;
    bool                    m_bDigitizerMode;

private:
    Ui::QMenuStripWidget *ui;
    T3kHandle*&            m_pT3kHandle;

signals:
    void ShowMenuEvent(int nMenu);

public slots:
    void OnBtnClickHome();
    void OnBtnClickMouse();
    void OnBtnClickCali();
    void OnBtnClickSensor();
    void OnBtnClickGeneral();
    void OnBtnClickSoftkey();
};

#endif // QMENUSTRIPWIDGET_H
