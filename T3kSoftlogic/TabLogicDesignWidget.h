#pragma once

//#include "LogicDesignerWnd.h"
#include "../common/TPDPEventMultiCaster.h"

#include <QWidget>


namespace Ui {
class TabLogicDesignWidget;
}

class TabLogicDesignWidget : public QWidget, public TPDPEventMultiCaster::ITPDPEventListener
{
    Q_OBJECT

public:
    TabLogicDesignWidget(QWidget* parent = NULL);
    virtual ~TabLogicDesignWidget();

//	CLogicDesignerWnd m_LogicDesigner;

	int		m_nSensorGPIOCount;

protected:
	// CTPDPEventMulticaster::ITPDPEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE);
    virtual void OnCloseT3kDevice(T3K_HANDLE);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);
    virtual void OnFirmwareDownload(bool);

    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

    bool writeToSensor( bool bLogicOnly );
    bool verifyGPIO( int &nSensorGPIOCount );

    QObject* findWantToParent(QObject *target, const char* strObjectName);

protected:

private:
    Ui::TabLogicDesignWidget* ui;

signals:

private slots:

    void on_BtnLogicdesign_clicked();
    void on_BtnApply_clicked();
};
