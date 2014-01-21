#ifndef QMENUSTRIPWIDGET_H
#define QMENUSTRIPWIDGET_H

#include <QWidget>
#include "QRaisePushButton.h"

#include "QT3kDeviceREventHandler.h"
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

class QMenuStripWidget : public QWidget, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    explicit QMenuStripWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QMenuStripWidget();

    void SetMenuButton( int nIndex );

protected:
    void ClickMenu( int nMenu );

    virtual void showEvent(QShowEvent *evt);

    virtual void onChangeLanguage();
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

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
    QT3kDeviceR*&            m_pT3kHandle;

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
