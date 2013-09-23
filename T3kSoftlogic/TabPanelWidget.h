#ifndef CTABPANELDESIGNWN_Q
#define CTABPANELDESIGNWN_Q

//#include "underlinetitlestatic.h"
//#include "borderstyleedit.h"

#include "TPDPEventMultiCaster.h"

#include <QWidget>

namespace Ui {
class TabPanelWidget;
}

class TabPanelWidget : public QWidget, public TPDPEventMultiCaster::ITPDPEventListener
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

    // T3kHIDNotify::IT3kEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE);
    virtual void OnCloseT3kDevice(T3K_HANDLE);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);

protected:
    bool				m_bLoadFromSensor;

private:
    Ui::TabPanelWidget* ui;

signals:
    T3kHandle* getT3kHandle();
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
};

#endif // CTABPANELDESIGNWN_Q
