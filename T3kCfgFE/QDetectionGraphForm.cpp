#include "QDetectionGraphForm.h"
#include "ui_QDetectionGraphForm.h"

#include "QBorderStyleEdit.h"
#include "QLangManager.h"
#include "QInitDataIni.h"
#include "QLogSystem.h"
#include "QT3kDevice.h"

#include <QResizeEvent>
#include <QScrollBar>

#include <math.h>

#include "../common/T3kConstStr.h"

#define RES_TAG "DETECTION"

inline void getPrefixLogNCmd( int nIndex, QString& strPrefixLog, QString& strPrefixCmd )
{
    switch (nIndex)
    {
    case IDX_CM1:
        strPrefixLog = "[CM1]";
        strPrefixCmd = sCam1;
        break;
    case IDX_CM2:
        strPrefixLog = "[CM2]";
        strPrefixCmd = sCam2;
        break;
    case IDX_CM1_1:
        strPrefixLog = "[CM1-1]";
        strPrefixCmd = sCam1_1;
        break;
    case IDX_CM2_1:
        strPrefixLog = "[CM2-1]";
        strPrefixCmd = sCam2_1;
        break;
    }
}

QDetectionGraphForm::QDetectionGraphForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDetectionGraphForm)
{
    ui->setupUi(this);

    m_pIRD = NULL;
    m_pITD = NULL;
    m_nIRD = 0;
    m_bIsModified = false;
    m_nThreshold = 50;
    m_nDTCCount = 0;
    m_TimerDisplayTouchCount = 0;
    m_pDTCArray = NULL;
    m_nMaxDTCCount = 0;

    m_nCameraIndex = -1;

    m_bBackupIsModified = false;

    ui->sensitivityCtrl->setRange( 20 );
    ui->sensitivityCtrl->setStep( 5 );
    ui->lblTouchCount->setText("");

    ui->lblOffset->setVisible(false);
    ui->btnOffsetDec->setVisible(false);
    ui->btnOffsetInc->setVisible(false);
    ui->txtEdtOffset->setVisible(false);

    //connect( ui->txtEdtAutoOffset, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtGain, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtLeftRange, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtOffset, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtRightRange, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtThreshold, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    ui->txtEdtGain->setAlignment(Qt::AlignRight);
    ui->txtEdtLeftRange->setAlignment(Qt::AlignRight);
    ui->txtEdtOffset->setAlignment(Qt::AlignRight);
    ui->txtEdtRightRange->setAlignment(Qt::AlignRight);
    ui->txtEdtThreshold->setAlignment(Qt::AlignRight);

    resetEditColors();

    onChangeLanguage();
}

QDetectionGraphForm::~QDetectionGraphForm()
{
    if (m_pIRD)
        delete[] m_pIRD;
    if (m_pITD)
        delete[] m_pITD;
    m_pIRD = m_pITD = NULL;
    if (m_pDTCArray)
        delete[] m_pDTCArray;
    m_pDTCArray = NULL;

    delete ui;
}

void QDetectionGraphForm::installEventFilterForEventRedirect(QObject* obj)
{
    ui->txtEdtAutoOffset->installEventFilter(obj);
    ui->txtEdtGain->installEventFilter(obj);
    ui->txtEdtLeftRange->installEventFilter(obj);
    ui->txtEdtOffset->installEventFilter(obj);
    ui->txtEdtRightRange->installEventFilter(obj);
    ui->txtEdtThreshold->installEventFilter(obj);
    ui->widgetDetectionGraph->installEventFilter(obj);
}

void QDetectionGraphForm::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    ui->lblThreshold->setText( res.getResString(RES_TAG, "TEXT_THRESHOLD") );
    ui->lblGain->setText( res.getResString(RES_TAG, "TEXT_GAIN") );
    ui->lblAutoOffset->setText( res.getResString(RES_TAG, "TEXT_AUTO_OFFSET") );
    ui->lblOffset->setText( res.getResString(RES_TAG, "TEXT_OFFSET") );
    ui->lblLeftRange->setText( res.getResString(RES_TAG, "TEXT_LEFT_RANGE") );
    ui->lblRightRange->setText( res.getResString(RES_TAG, "TEXT_RIGHT_RANGE") );

    ui->btnZoomIn->setToolTip( res.getResString(RES_TAG, "TEXT_ZOOM_IN") );
    ui->btnZoomOut->setToolTip( res.getResString(RES_TAG, "TEXT_ZOOM_OUT") );
    ui->btnZoomFit->setToolTip( res.getResString(RES_TAG, "TEXT_FIT_SCREEN") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QDetectionGraphForm::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerDisplayTouchCount)
    {
        killTimer(m_TimerDisplayTouchCount);
        m_TimerDisplayTouchCount = 0;
        ui->lblTouchCount->setText("");
    }
}

void QDetectionGraphForm::onEditModified(QBorderStyleEdit* pEdit, int nValue, double /*dValue*/)
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    if (pEdit == ui->txtEdtThreshold)
    {
        LOG_I( strPrefixLog + " Manual Edit Threshold" );

        QString strCmd = strPrefixCmd + cstrDetectionThreshold + QString::number(nValue);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( pEdit );
        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtGain)
    {
        LOG_I( strPrefixLog + " Manual Edit Gain" );

        QString strCmd = strPrefixCmd + cstrSensorGain + QString::number(nValue);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( pEdit );
        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtOffset)
    {
        LOG_I( strPrefixLog + " Manual Edit Offset" );

        QString strCmd = strPrefixCmd + cstrDetectionLine + QString::number(nValue);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( pEdit );
        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtLeftRange)
    {
        LOG_I( strPrefixLog + " Manual Edit Left Range" );
        int nRight = ui->txtEdtRightRange->toPlainText().toInt();

        QString strCmd = strPrefixCmd + cstrDetectionRange + QString::number(nValue) + "," + QString::number(nRight);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( pEdit );
        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtRightRange)
    {
        LOG_I( strPrefixLog + " Manual Edit Right Range" );
        int nLeft = ui->txtEdtLeftRange->toPlainText().toInt();

        QString strCmd = strPrefixCmd + cstrDetectionRange + QString::number(nLeft) + "," + QString::number(nValue);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( pEdit );
        m_bIsModified = true;
    }
}

void QDetectionGraphForm::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    int nCameraIndex = getIndexFromPart(Part);

    if (m_nCameraIndex != nCameraIndex) return;

    if ( strstr(szCmd, cstrDetectionLine) == szCmd )
    {
        char* pCur = (char*)szCmd + sizeof(cstrDetectionLine) - 1;
        int nDetectLine = atoi(pCur);

        pCur = strchr(pCur, ',') + 1;

        int nAutoLine = 0;
        if( pCur != (char*)1 )
        {
            nAutoLine = atoi(pCur);
            //qDebug( "AUTO_LINE: %d", nAutoLine );
        }

        if (!ui->txtEdtOffset->isEditing())
            ui->txtEdtOffset->setText( QString::number(nDetectLine) );
        ui->txtEdtAutoOffset->setText( QString::number(nAutoLine) );
    }
    else if ( strstr(szCmd, cstrDetectionThreshold) == szCmd )
    {
        int nThreshold = atoi(szCmd + sizeof(cstrDetectionThreshold) - 1);
        m_nThreshold = nThreshold;
        if (!ui->txtEdtThreshold->isEditing())
            ui->txtEdtThreshold->setText( QString::number(nThreshold) );
    }
    else if ( strstr(szCmd, cstrDetectionRange) == szCmd )
    {
        char* pCur = (char*)szCmd + sizeof(cstrDetectionRange) - 1;
        int nLeft = atoi(pCur);

        pCur = strchr(pCur, ',') + 1;
        int nRight = atoi(pCur);

        ui->widgetDetectionGraph->setDetectionRange( nLeft, nRight );

        ui->txtEdtLeftRange->setText( QString::number(nLeft) );
        ui->txtEdtRightRange->setText( QString::number(nRight) );
    }
    else if ( strstr(szCmd, cstrSensorGain) == szCmd )
    {
        char* pCur = (char*)szCmd + sizeof(cstrSensorGain) - 1;
        int nGain = atoi(pCur);

        ui->txtEdtGain->setText( QString::number(nGain) );
    }
}

void QDetectionGraphForm::TPDP_OnIRD(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int total, int offset, const unsigned char *data, int cnt)
{
    int nCameraIndex = getIndexFromPart(Part);

    if (m_nCameraIndex != nCameraIndex) return;

    if ( cnt <= 0 || total <= 0 || total > 2048 )
        return;

    if (m_nIRD == 0)
        m_nIRD = total;
    else if (m_nIRD != total)
    {
        initDetectionData();
        return;
    }

    if (offset + cnt > total)
        return;

    if (m_pIRD == NULL)
    {
        Q_ASSERT( m_nIRD == total );
        m_pIRD = new unsigned char[total];
        memset( m_pIRD, 0, sizeof(unsigned char) * total );
    }

    memcpy( m_pIRD + offset, data, cnt );

    updateGraph( offset, offset+cnt );
}

void QDetectionGraphForm::TPDP_OnITD(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int total, int offset, const unsigned char *data, int cnt)
{
    int nCameraIndex = getIndexFromPart(Part);

    if (m_nCameraIndex != nCameraIndex) return;

    if ( cnt <= 0 || total <= 0 || total > 2048 )
        return;

    if (m_nIRD == 0)
        m_nIRD = total;
    else if (m_nIRD != total)
    {
        initDetectionData();
        return;
    }

    if (offset + cnt > total)
        return;

    if( m_pITD == NULL )
    {
        Q_ASSERT( m_nIRD == total );
        m_pITD = new unsigned char[total];
        memset( m_pITD, 0, sizeof(unsigned char) * total );
    }

    memcpy( m_pITD + offset, data, cnt );

    updateSensitivity();
}

void QDetectionGraphForm::TPDP_OnDTC(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, unsigned char */*layerid*/, unsigned long *start_pos, unsigned long *end_pos, int cnt)
{
    int nCameraIndex = getIndexFromPart(Part);

    if (m_nCameraIndex != nCameraIndex) return;

    if (cnt ==0)
    {
        if (!m_pDTCArray)
        {
            m_nMaxDTCCount = 3;
            m_pDTCArray = new RangeI[m_nMaxDTCCount];
        }
        if (m_pDTCArray[0].start != -1)
        {
            for (int i=0 ; i<m_nMaxDTCCount ; i++)
                m_pDTCArray[i].start = m_pDTCArray[i].end = -1;
            ui->widgetDetectionGraph->updateDetectionData( m_pDTCArray, cnt );
        }
    }
    else
    {
        if (m_nMaxDTCCount < cnt)
        {
            if (m_pDTCArray)
                delete[] m_pDTCArray;
            m_nMaxDTCCount = cnt;
            m_pDTCArray = new RangeI[m_nMaxDTCCount];
        }

        for (int i=0 ; i<m_nMaxDTCCount ; i++)
        {
            if (i<cnt)
            {
                m_pDTCArray[i].start = start_pos[i] * m_nIRD / 0xffff;
                m_pDTCArray[i].end = end_pos[i] * m_nIRD / 0xffff;
            }
            else
            {
                m_pDTCArray[i].start = m_pDTCArray[i].end = -1;
            }
        }

        ui->widgetDetectionGraph->updateDetectionData( m_pDTCArray, cnt );
    }

    if (m_nDTCCount != cnt)
    {
        QLangRes& res = QLangManager::getResource();
        QString strTouchCount = res.getResString(RES_TAG, "TEXT_TOUCH_COUNT");
        strTouchCount += " : " + QString::number(cnt);
        ui->lblTouchCount->setText( strTouchCount );

        m_nDTCCount = cnt;
        if (m_TimerDisplayTouchCount)
            killTimer(m_TimerDisplayTouchCount);
        m_TimerDisplayTouchCount = startTimer(200);
    }
    else
    {
        if (m_nDTCCount > 0)
        {
            if (m_TimerDisplayTouchCount)
                killTimer(m_TimerDisplayTouchCount);
            m_TimerDisplayTouchCount = startTimer(200);
        }
    }
}

void QDetectionGraphForm::setDisplayCrackInfo( bool bDisplay )
{
    ui->widgetDetectionGraph->setDisplayCrackInfo( bDisplay );
}

void QDetectionGraphForm::clear()
{
    ui->txtEdtThreshold->setText("");
    ui->txtEdtGain->setText("");
    ui->txtEdtOffset->setText("");
    ui->txtEdtLeftRange->setText("");
    ui->txtEdtRightRange->setText("");
    ui->txtEdtAutoOffset->setText("");

    if (m_nIRD > 0)
    {
        if (m_pIRD)
            memset( m_pIRD, 0, sizeof(unsigned char)*m_nIRD );
        if (m_pITD)
            memset( m_pITD, 0, sizeof(unsigned char)*m_nIRD );

        ui->widgetDetectionGraph->setDetectionRange( 0, 0xffff );
        m_nThreshold = 0;
        ui->widgetDetectionGraph->setThreshold( 0 );
        ui->sensitivityCtrl->setColorLevel( 0 );
        ui->sensitivityCtrl->setStep( 0 );

        updateGraph( 0, m_nIRD, true );
    }
}

void QDetectionGraphForm::enableAllControls( bool bEnable )
{
    setEnabled(bEnable);
}

void QDetectionGraphForm::setTitle( const QString& strTitle )
{
    ui->lblSensorTitle->setText(strTitle);
}

void QDetectionGraphForm::setCameraIndex( int nIndex )
{
    m_nCameraIndex = nIndex;
    bool bTempModified = m_bBackupIsModified;
    m_bBackupIsModified = m_bIsModified;
    m_bIsModified = bTempModified;
    QVector<QBorderStyleEdit*> tempEdits;
    for (int i=0 ; i<m_backupModifiedEdits.size() ; i++ )
        tempEdits.push_back(m_backupModifiedEdits.at(i));
    m_backupModifiedEdits.clear();
    for (int i=0 ; i<m_modifiedEdits.size() ; i++ )
        m_backupModifiedEdits.push_back(m_modifiedEdits.at(i));
    m_modifiedEdits.clear();
    for (int i=0 ; i<tempEdits.size() ; i++ )
    {
        m_modifiedEdits.push_back(tempEdits.at(i));
        qDebug( "edit push: %p", tempEdits.at(i) );
    }

    qDebug( "modified edit count: %d", m_backupModifiedEdits.size() );

    resetEditColors();
    for (int i=0 ; i<m_modifiedEdits.size() ; i++)
    {
        setModifyEditColor( m_modifiedEdits.at(i) );
    }
}

void QDetectionGraphForm::setModifiedRange()
{
    m_bIsModified = true;
    setModifyEditColor( ui->txtEdtLeftRange );
    setModifyEditColor( ui->txtEdtRightRange );
}

bool QDetectionGraphForm::isModified()
{
    return m_bIsModified || m_bBackupIsModified;
}

void QDetectionGraphForm::setModified( bool bModified )
{
    m_bIsModified = bModified;
    if (m_bIsModified)
    {
        setModifyAllEditColors();
    }
    else
    {
        resetEditColors();
        m_modifiedEdits.clear();
        m_backupModifiedEdits.clear();
        m_bBackupIsModified = false;
    }
}

void QDetectionGraphForm::resetEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtGain, ui->txtEdtLeftRange, ui->txtEdtOffset,
        ui->txtEdtRightRange, ui->txtEdtThreshold
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrNormalBorderColor, s_clrNormalBgColor);
        edits[i]->setFloatStyle(false);
        edits[i]->update();
    }

    m_bIsModified = false;
}

void QDetectionGraphForm::setModifyEditColor( QBorderStyleEdit* pEdit )
{
    pEdit->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
    pEdit->update();
    if (m_modifiedEdits.indexOf(pEdit) < 0)
    {
        m_modifiedEdits.push_back(pEdit);
        qDebug( "edit push: %p", pEdit );
    }
    m_bIsModified = true;
}

void QDetectionGraphForm::setModifyAllEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtGain, ui->txtEdtLeftRange, ui->txtEdtOffset,
        ui->txtEdtRightRange, ui->txtEdtThreshold
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        if (m_modifiedEdits.indexOf(edits[i]) < 0)
        {
            m_modifiedEdits.push_back(edits[i]);
            qDebug( "edit push: %p", edits[i] );
        }
        edits[i]->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
        edits[i]->update();
    }
}

void QDetectionGraphForm::initDetectionData()
{
    if (m_pIRD)
        delete[] m_pIRD;
    if (m_pITD)
        delete[] m_pITD;
    m_pIRD = m_pITD = NULL;
    m_nIRD = 0;

    if (!m_pDTCArray)
    {
        m_nMaxDTCCount = 3;
        m_pDTCArray = new RangeI[m_nMaxDTCCount];
    }
    for (int i=0 ; i<m_nMaxDTCCount ; i++)
        m_pDTCArray[i].start = m_pDTCArray[i].end = -1;
}

void QDetectionGraphForm::updateGraph( int nStart, int nEnd, bool bForce/*=false*/ )
{
    if (m_nIRD > 0)
    {
        ui->widgetDetectionGraph->setThreshold( m_nThreshold );
        ui->widgetDetectionGraph->setGraphData( m_nIRD, m_pIRD, m_pITD );
        ui->widgetDetectionGraph->redrawGraph( nStart, nEnd, bForce );
    }
}

void QDetectionGraphForm::updateGraph()
{
    ui->widgetDetectionGraph->update();
}

bool QDetectionGraphForm::isSetUpdateGraph()
{
    return ui->widgetDetectionGraph->isSetUpdateGraph();
}

void QDetectionGraphForm::setUpdateGraph(bool bSet)
{
    ui->widgetDetectionGraph->setUpdateGraph(bSet);
}

void QDetectionGraphForm::updateSensitivity()
{
    if (m_pIRD && m_pITD)
    {
        int nMaxStep = ui->sensitivityCtrl->getRange();

        float fSum = 0.f;
        float fAve;
        float fStdDev = 0.f;
        int nMin = INT_MAX;
        int nMax = 0;
        int nCount = 0;
        for (int i=0 ; i<m_nIRD ; i++)
        {
            if (m_pITD[i] == 0) continue;
            if (m_pITD[i] < nMin)
            {
                nMin =  m_pITD[i];
            }
            if (m_pITD[i] > nMax)
            {
                nMax = m_pITD[i];
            }
            fSum += (float)m_pITD[i];
            nCount++;
        }
        fAve = fSum / nCount;
        for (int i=0 ; i<m_nIRD ; i++)
        {
            if (m_pITD[i] == 0) continue;
            fStdDev += (float)(m_pITD[i] - fAve) * (m_pITD[i] - fAve);
        }
        fStdDev /= nCount;
        fStdDev = sqrt( fStdDev );

        //qDebug( "Ave: %.1f, StdDev; %.1f", fAve, fStdDev );

        // 200
        int nStep = (int)(fAve * nMaxStep / 0xff + .5f );
        if( nStep > nMaxStep ) nStep = nMaxStep;

        int nLevel;
        int nLightThError =  QInitDataIni::instance()->getDTCGraphLightThresholdError();
        int nLightThWarning =  QInitDataIni::instance()->getDTCGraphLightThresholdWarning();
        if (nMin <= nLightThError*0xff/100)
            nLevel = 0;
        else if (nMin <= (nLightThError+nLightThWarning)*0xff/100/2)
            nLevel = 1;
        else if (nMin <= nLightThWarning*0xff/100)
            nLevel = 2;
        else
            nLevel = 3;

        ui->sensitivityCtrl->setColorLevel( nLevel );
        ui->sensitivityCtrl->setStep( nStep );
    }
}

void QDetectionGraphForm::sendEditValue( QBorderStyleEdit* txtEdit, int step, const QString& strCmd )
{
    int nValue = txtEdit->toPlainText().toInt();
    nValue += step;

    setModifyEditColor(txtEdit);

    QString strFullCmd = strCmd + QString::number(nValue);
    QT3kDevice::instance()->sendCommand( strFullCmd, true );
}

void QDetectionGraphForm::sendEditValue( QBorderStyleEdit* txtEditLeft, QBorderStyleEdit* txtEditRight, bool bModifyLeft, int step, const QString& strCmd )
{
    int nValueLeft = txtEditLeft->toPlainText().toInt();
    int nValueRight = txtEditRight->toPlainText().toInt();
    if (bModifyLeft)
    {
        nValueLeft += step;
        setModifyEditColor(txtEditLeft);
    }
    else
    {
        nValueRight += step;
        setModifyEditColor(txtEditRight);
    }

    QString strFullCmd = strCmd + QString::number(nValueLeft) + "," + QString::number(nValueRight);
    QT3kDevice::instance()->sendCommand( strFullCmd, true );
}

void QDetectionGraphForm::on_btnThresholdDec_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Threshold \"-\"" );

    sendEditValue( ui->txtEdtThreshold, -5, strPrefixCmd + cstrDetectionThreshold );
}

void QDetectionGraphForm::on_btnThresholdInc_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Threshold \"+\"" );

    sendEditValue( ui->txtEdtThreshold, 5, strPrefixCmd + cstrDetectionThreshold );
}

void QDetectionGraphForm::on_btnGainDec_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Gain \"-\"" );

    sendEditValue( ui->txtEdtGain, -1, strPrefixCmd + cstrSensorGain );
}

void QDetectionGraphForm::on_btnGainInc_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Gain \"+\"" );

    sendEditValue( ui->txtEdtGain, 1, strPrefixCmd + cstrSensorGain );
}

void QDetectionGraphForm::on_btnOffsetDec_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Offset \"-\"" );

    sendEditValue( ui->txtEdtOffset, -1, strPrefixCmd + cstrDetectionLine );
}

void QDetectionGraphForm::on_btnOffsetInc_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Offset \"+\"" );

    sendEditValue( ui->txtEdtOffset, 1, strPrefixCmd + cstrDetectionLine );
}

void QDetectionGraphForm::on_btnLeftRangeDec500_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Left Range \"-500\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, true, -500, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnLeftRangeDec100_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Left Range \"-100\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, true, -100, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnLeftRangeInc100_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Left Range \"+100\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, true, 100, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnLeftRangeInc500_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Left Range \"+500\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, true, 500, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnRightRangeDec500_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Right Range \"-500\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, false, -500, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnRightRangeDec100_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Right Range \"-100\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, false, -100, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnRightRangeInc100_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Right Range \"+100\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, false, 100, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::on_btnRightRangeInc500_clicked()
{
    QString strPrefixLog, strPrefixCmd;
    getPrefixLogNCmd( m_nCameraIndex, strPrefixLog, strPrefixCmd );

    LOG_B( strPrefixLog + " Right Range \"+500\"" );

    sendEditValue( ui->txtEdtLeftRange, ui->txtEdtRightRange, false, 500, strPrefixCmd + cstrDetectionRange );
}

void QDetectionGraphForm::resizeEvent(QResizeEvent *evt)
{
    ui->widgetDetectionGraph->resetZoom();
    QWidget::resizeEvent(evt);
}

void QDetectionGraphForm::on_btnZoomIn_clicked()
{
    ui->widgetDetectionGraph->zoomIn();
}

void QDetectionGraphForm::on_btnZoomOut_clicked()
{
    ui->widgetDetectionGraph->zoomOut();
}

void QDetectionGraphForm::on_btnZoomFit_clicked()
{
    ui->widgetDetectionGraph->resetZoom();
}
