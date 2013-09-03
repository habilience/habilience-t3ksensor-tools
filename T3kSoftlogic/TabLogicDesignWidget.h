#ifndef TABLOGICDESIGNWIDGET_H
#define TABLOGICDESIGNWIDGET_H

#include "LogicDesignWidget.h"
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

    void setInvertDrawing(bool bInvert) { m_LogicDesigner.setInvertDrawing( bInvert ); if( isVisible() ) update(); }

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

protected:

private:
    Ui::TabLogicDesignWidget* ui;
    QLogicDesignWidget          m_LogicDesigner;

    int                         m_nSensorGPIOCount;

signals:
    T3kHandle* getT3kHandle();
    bool isValidT3kSensorState();
    void updatePreview();

private slots:

    void on_BtnLogicdesign_clicked();
    void on_BtnApply_clicked();
};

#endif // TABLOGICDESIGNWIDGET_H
