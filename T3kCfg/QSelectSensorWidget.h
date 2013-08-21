#ifndef QSELECTSENSORWIDGET_H
#define QSELECTSENSORWIDGET_H

#include <QDialog>
#include <QToolButton>
#include <QTimer>
#include "../common/TPDPEventMultiCaster.h"
#include "QLangManager.h"

namespace Ui {
    class QSelectSensorWidget;
}

class QSelectSensorWidget : public QDialog, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QSelectSensorWidget(QWidget *parent = 0);
    ~QSelectSensorWidget();

    typedef struct _ModelID
    {
        unsigned short VID;
        unsigned short PID;
        unsigned short Idx;
    } ModelID;

protected:
    void AddSensorItem( QString strModel, QString strDevPath );

    void UpdateDeviceList();

    virtual void showEvent(QShowEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);

    virtual void OnChangeLanguage();

private:
    Ui::QSelectSensorWidget *ui;
    T3kHandle*             m_pT3kHandle;

    QTimer                  m_TimerBuzzer;

    QMap<int, ModelID>      m_mapDevModelID;
    int                     m_nSelectIndex;

    int                     m_nDeviceCount;
    int                     m_nTimerCheckDevice;

private slots:
    void on_BtnOpen_clicked();
    void on_TableSensor_Selection(int nRow, int nColumn);
    void on_TableSensor_DClick(int nRow, int nColumn);
    void on_TableButton_PlaySound(bool bToggled, int nIndex);
    void on_Timer_Buzzer();
};

class QIndexToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit QIndexToolButton(QWidget* parent = 0, int nIndex = -1);
    virtual ~QIndexToolButton();

    int GetIndex();

protected:
    int             m_nIndex;

signals:
    void Clicked_Signal(bool bToggled, int nIndex);
private slots:
    void on_Clicked_Signal(bool bToggled);

};

#endif // QSELECTSENSORWIDGET_H
