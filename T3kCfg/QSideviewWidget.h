#ifndef QSIDEVIEWWIDGET_H
#define QSIDEVIEWWIDGET_H

#include <QDialog>
#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"

#include <QImage>

namespace Ui {
    class QSideviewWidget;
}

class QSideviewWidget : public QDialog, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QSideviewWidget(QWidget *parent = 0);
    ~QSideviewWidget();

    void setT3kHandle(QT3kDeviceR* pHandle) { m_pT3kHandle = pHandle; }

protected:
    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    // QLangManager::LangChangeNotify
protected:
    virtual void onChangeLanguage();

protected:
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnPRV(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);

protected:
    void RequestSensorData( bool bDefault );

protected:
    QT3kDeviceR*         m_pT3kHandle;

    QString             m_strModelName;

    QImage              m_bmpSnapSensor;
    uchar*              m_pImgTempBuffer;
    int                 m_nCurrentCamNo;

    int                 m_nAutoLine;

private:
    Ui::QSideviewWidget *ui;

signals:
    void ChangeCamera( int nCamNo );

private slots:
    void on_BtnMinus_clicked();
    void on_BtnPlus_clicked();
    void on_BtnCam4_clicked(bool clicked);
    void on_BtnCam3_clicked(bool clicked);
    void on_BtnCam2_clicked(bool clicked);
    void on_BtnCam1_clicked(bool clicked);

    void onChangeCamera( int nCamNo );
};

#endif // QSIDEVIEWWIDGET_H
