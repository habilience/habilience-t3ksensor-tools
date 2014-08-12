#ifndef QOMOUSEPROFILEWIDGET_H
#define QOMOUSEPROFILEWIDGET_H

#include <QWidget>

#include "QProfileLabel.h"
#include "QT3kDeviceEventHandler.h"
#include "QMouseMappingTable.h"
#include "QTouchSettingWidget.h"

#include "QCheckableButton.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

#include <QKeyEvent>

namespace Ui {
    class QOMouseProfileWidget;
}

class QOMouseProfileWidget : public QWidget, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QOMouseProfileWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    ~QOMouseProfileWidget();

    void SetDefault();
    void Refresh();

    void ReplaceLabelName(QCheckableButton* pBtn);

protected:
    void RequestSensorData( bool bDefault );

    void ChangeProfile( QCheckableButton* pBtn, int nIndex );

    virtual void onChangeLanguage();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

protected:
    QProfileLabel           m_ProfileLabel;

    bool                    m_bSetDefault;
    int                     m_nMouseProfileIndex;
    int                     m_nSelectedProfileIndex;

    int                     m_nProfileIndexData;

    int                     m_nCurInputMode;
    int                     m_nChkUsbCfgMode;

    QRequestHIDManager      m_RequestSensorData;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void SendInputModeState();

private:
    Ui::QOMouseProfileWidget *ui;
    QT3kDevice*&            m_pT3kHandle;

private slots:
    void on_Profile5_clicked();
    void on_Profile4_clicked();
    void on_Profile3_clicked();
    void on_Profile2_clicked();
    void on_Profile1_clicked();

public slots:
    void onEnableMacOSXGesture(bool bEnable);
};

#endif // QOMOUSEPROFILEWIDGET_H
