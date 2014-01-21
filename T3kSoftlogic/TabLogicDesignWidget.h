#ifndef TABLOGICDESIGNWIDGET_H
#define TABLOGICDESIGNWIDGET_H

#include "LogicDesignWidget.h"
#include "QT3kDeviceEventHandler.h"

#include <QWidget>


namespace Ui {
class TabLogicDesignWidget;
}

class TabLogicDesignWidget : public QWidget, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

public:
    TabLogicDesignWidget(QWidget* parent = NULL);
    virtual ~TabLogicDesignWidget();

    void setInvertDrawing(bool bInvert) { m_LogicDesigner.setInvertDrawing( bInvert ); if( isVisible() ) update(); }

protected:
    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO devInfo, bool bIsDownload);

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
    QT3kDevice* getT3kHandle();
    bool isValidT3kSensorState();
    void updatePreview();

private slots:
    void on_BtnLogicdesign_clicked();
    void on_BtnApply_clicked();

public slots:
    void onConnectedT3kDevice();
};

#endif // TABLOGICDESIGNWIDGET_H
