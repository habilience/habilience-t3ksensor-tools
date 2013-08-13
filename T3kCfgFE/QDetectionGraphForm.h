#ifndef QDETECTIONGRAPHFORM_H
#define QDETECTIONGRAPHFORM_H

#include <QWidget>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "t3kcomdef.h"

namespace Ui {
class QDetectionGraphForm;
}

class QBorderStyleEdit;
class QDetectionGraphForm : public QWidget, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
private:
    unsigned char*  m_pIRD;
    unsigned char*  m_pITD;
    int             m_nIRD;

    bool            m_bIsModified;

    bool            m_bBackupIsModified;
    QVector<QBorderStyleEdit*> m_modifiedEdits;
    QVector<QBorderStyleEdit*> m_backupModifiedEdits;

    int             m_nCameraIndex;
    int             m_nThreshold;

    int             m_nDTCCount;
    int             m_TimerDisplayTouchCount;

    RangeI*         m_pDTCArray;
    int             m_nMaxDTCCount;
public:
    void setDisplayCrackInfo( bool bDisplay );
    void clear();
    void enableAllControls( bool bEnable );
    void setTitle( const QString& strTitle );
    void setCameraIndex( int nIndex );
    void setModifiedRange();
    bool isModified();
    void setModified( bool bModified );
    void updateGraph();
    bool isSetUpdateGraph();
    void setUpdateGraph(bool bSet);
    void installEventFilterForEventRedirect(QObject* obj);

protected:
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent *);

    void resetEditColors();
    void setModifyEditColor( QBorderStyleEdit* pEdit );
    void setModifyAllEditColors();

    void initDetectionData();
    void updateGraph( int nStart, int nEnd, bool bForce=false );
    void updateSensitivity();

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnIRD(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);
    virtual void TPDP_OnITD(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);
    virtual void TPDP_OnDTC(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, unsigned char *layerid, unsigned long *start_pos, unsigned long *end_pos, int cnt);

    void sendEditValue( QBorderStyleEdit* txtEdit, int step, const QString& strCmd );
    void sendEditValue( QBorderStyleEdit* txtEditLeft, QBorderStyleEdit* txtEditRight, bool bModifyLeft, int step, const QString& strCmd );
public:
    explicit QDetectionGraphForm(QWidget *parent = 0);
    ~QDetectionGraphForm();
    
private slots:
    void onEditModified(QBorderStyleEdit* pEdit, int nValue, double dValue);

    void on_btnThresholdDec_clicked();
    void on_btnThresholdInc_clicked();
    void on_btnGainDec_clicked();
    void on_btnGainInc_clicked();
    void on_btnOffsetDec_clicked();
    void on_btnOffsetInc_clicked();
    void on_btnLeftRangeDec500_clicked();
    void on_btnLeftRangeDec100_clicked();
    void on_btnLeftRangeInc100_clicked();
    void on_btnLeftRangeInc500_clicked();
    void on_btnRightRangeDec500_clicked();
    void on_btnRightRangeDec100_clicked();
    void on_btnRightRangeInc100_clicked();
    void on_btnRightRangeInc500_clicked();
    void on_btnZoomIn_clicked();
    void on_btnZoomOut_clicked();
    void on_btnZoomFit_clicked();

private:
    Ui::QDetectionGraphForm *ui;
};

#endif // QDETECTIONGRAPHFORM_H
