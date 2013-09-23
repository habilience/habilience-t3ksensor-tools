#ifndef QLOADSENSORDATAWIDGET_H
#define QLOADSENSORDATAWIDGET_H

#include <QDialog>
#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"
#include "QPrintLogData.h"
#include "LogDataDef.h"

typedef struct _CameraData
{
    QImage*                 pSideViewImage;
    uchar*                  pIRD;
    uchar*                  pITD;
    int                     nIRD;
    bool                    bNoCam;
} CameraData;

class QProgressBar;

class QT3kHIDObject;
class QT3kLoadSideviewObject;
class QT3kLoadDetectionObject;
class QT3kLoadSensorDataObject;
class QT3kLoadEnvironmentObject;

namespace Ui {
    class QLoadSensorDataWidget;
}

class QLoadSensorDataWidget : public QDialog, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    explicit QLoadSensorDataWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QLoadSensorDataWidget();
    enum eDataPart { DP_CM1 = 1, DP_CM2, DP_CM1_1, DP_CM2_1, DP_MM, DP_END };
    enum eLoadStep { LS_SIDEVIEW, LS_DETECTION, LS_DATA, LS_SAVE };

    void Start();

protected:
    void LoadSideView();
    void LoadDetection();
    void LoadData();
    void LoadEnvironment();
    bool SaveLogToFile();

    void CloseWidget();

    QString GetLastFELogFileNamePath();
    bool RemoveDirectoryR( QString strPath );

    void ResetStorage();

    bool ExportReportFiles( QString strOutPutPath );

    void ClearProgressLog();

    void MoveWidgetToCenter();
    void CreateSavePath();

protected:
    virtual void timerEvent(QTimerEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);

protected:
    // LangChangeNotify
    virtual void onChangeLanguage();

private:
    Ui::QLoadSensorDataWidget   *ui;
    T3kHandle*&            m_pT3kHandle;

    int                     m_nTimerLoadStep;
    int                     m_nLoadProgStep;

private:
    QString                 m_strSavePath;

    SensorLogData*          m_pSensorLogData;

    bool                    m_bSaveError;

    QT3kHIDObject*          m_pHIDObject;

signals:

private slots:
    void on_BTOK_clicked();

public slots:
    void on_InsertProgressLog( QString strMessage, bool bError = false );
    void onLoadComplete( int nStep );
};

#endif // QLOADSENSORDATAWIDGET_H
