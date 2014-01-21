#ifndef QMAINMENUWIDGET_H
#define QMAINMENUWIDGET_H

#include <QWidget>
#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

#include <QMap>

namespace Ui {
    class QMainMenuWidget;
}

class QMainMenuWidget : public QWidget, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QMainMenuWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QMainMenuWidget();

    void RequestInformation();

protected:
    void UpdateInformation();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    virtual void onChangeLanguage();

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

private:
    Ui::QMainMenuWidget *ui;

    QT3kDeviceR*&            m_pT3kHandle;

    QString                 m_strMMVersion;
    QString                 m_strMMSN;
    float                   m_fMMVersion;
    int                     m_bMMNoCam;

    typedef struct _CMVerInfo
    {
        QString             strCMVersion;
        QString             strCMSN;
        QString             strSetDate;
        float               fCMVersion;
        short               nModel;
        int                 bNoCam;
    } CMVerInfo;

    QMap<int, CMVerInfo>    m_mapCMVerInfo;

    QRequestHIDManager      m_RequestSensorData;

    bool                    m_bSoftkey;
    bool                    m_bDigitizerMode;

signals:
    void ShowErrorMsgBox(QString str);
    void ShowMenuEvent(int nMenu);

private slots:
    void on_BtnMainMouse_clicked();
    void on_BtnMainCali_clicked();
    void on_BtnMainSensor_clicked();
    void on_BtnMainGeneral_clicked();
    void on_BtnMainSoftkey_clicked();
    void onRequestFinish();
};

#endif // QMAINMENUWIDGET_H
