#ifndef QSIDEVIEWWIDGET_H
#define QSIDEVIEWWIDGET_H

#include <QDialog>
#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"

#include <QImage>

namespace Ui {
    class QSideviewWidget;
}

class QSideviewWidget : public QDialog, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QSideviewWidget(QWidget *parent = 0);
    ~QSideviewWidget();

    void SetT3kHandle(T3kHandle* pHandle) { m_pT3kHandle = pHandle; }

protected:
    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    // QLangManager::LangChangeNotify
protected:
    virtual void OnChangeLanguage();

protected:
    virtual void OnRSP(ResponsePart Part, short, const char *, long, bool, const char *sCmd);
    virtual void OnRSE(ResponsePart Part, short, const char *sPartId, long, bool, const char *sStatus);
    virtual void OnPRV(ResponsePart Part, short, const char *, int nWidth, int nHeight, int, unsigned char *pBitmapBuffer);

protected:
    void RequestSensorData( bool bDefault );

protected:
    T3kHandle*         m_pT3kHandle;

    QString             m_strModelName;

    QImage              m_bmpSnapSensor;
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
