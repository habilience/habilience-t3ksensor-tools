#ifndef CTABPANELDESIGNWN_Q
#define CTABPANELDESIGNWN_Q

#include "QT3kDeviceEventHandler.h"

#include <QWidget>

namespace Ui {
class TabPanelWidget;
}

class TabPanelWidget : public QWidget, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

public:
    TabPanelWidget(QWidget* pParent = NULL);
    virtual ~TabPanelWidget();

    void updateUIFromData();
    void updateDataFromUI();

    void setFocusPaname();

protected:
    // QWidget
    virtual void showEvent(QShowEvent *);

    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

protected:
    bool				m_bLoadFromSensor;

private:
    Ui::TabPanelWidget* ui;

signals:
    QT3kDevice* getT3kHandle();
    void updatePreview();
    bool isValidT3kSensorState();
    void notifyTab(int index);

private slots:
    void on_EditInfoName_editingFinished();
    void on_EditScreenWidth_editingFinished();
    void on_EditScreenHeight_editingFinished();
    void on_EditInfoName_textChanged(const QString&);
    void on_EditScreenWidth_textChanged(const QString&);
    void on_EditScreenHeight_textChanged(const QString&);
    void on_BtnLoad_clicked();

public slots:
    void onConnectedT3kDevice();
};

#endif // CTABPANELDESIGNWN_Q
