#include "QInitDataIni.h"

#include "QIni.h"
#include "QUtils.h"

#include "QBentCfgParam.h"
#include "conf.h"
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

#define DTC_GRAPH_INIT_SHARP_WIDTH					(50)
#define DTC_GRAPH_INIT_CRACK_THRESHOLD_ERROR		(0.9f)
#define DTC_GRAPH_INIT_CRACK_THRESHOLD_WARNING		(0.8f)
#define DTC_GRAPH_INIT_LIGHT_THRESHOLD_ERROR		(15)
#define DTC_GRAPH_INIT_LIGHT_THRESHOLD_WARNING		(30)

QInitDataIni::QInitDataIni()
{
    m_nBentMargin[0] = BENT_MARGIN_INIT_LEFT;
    m_nBentMargin[1] = BENT_MARGIN_INIT_TOP;
    m_nBentMargin[2] = BENT_MARGIN_INIT_RIGHT;
    m_nBentMargin[3] = BENT_MARGIN_INIT_BOTTOM;

    m_nBentDirection = BENT_MARGIN_INIT_DIR;

    m_nBentAlgorithm2 = BENT_INIT_ALGORITHM2;
    m_nBentAlgorithm4 = BENT_INIT_ALGORITHM4;
    m_nCalibrationCamera = BENT_INIT_CALIBRATION;

    m_bBentWithDummyLoad = false;
    m_bBentWithDummy = BENT_WITH_DUMMY;
    m_bCalibrationCameraLoad = false;
    m_bCalibrationCamera = BENT_WITH_CALIBRATION;
    m_bIgnoreCameraPairForBentLoad = false;
    m_bIgnoreCameraPairForBent = BENT_IGNORE_CAMERAPAIR;
    m_nMiniSideView = 0;
    m_bMirrorSideView = false;

    m_nActiveLanguageIndex = 0;

    m_nDetectionGraphSharpWidth				= DTC_GRAPH_INIT_SHARP_WIDTH;
    m_fDetectionGraphCrackThresholdError	= DTC_GRAPH_INIT_CRACK_THRESHOLD_ERROR;
    m_fDetectionGraphCrackThresholdWarning	= DTC_GRAPH_INIT_CRACK_THRESHOLD_WARNING;
    m_nDetectionGraphLightThresholdError	= DTC_GRAPH_INIT_LIGHT_THRESHOLD_ERROR;
    m_nDetectionGraphLightThresholdWarning	= DTC_GRAPH_INIT_LIGHT_THRESHOLD_WARNING;

    m_nDetectionGainDefault = -1;
}

void QInitDataIni::setBentMargin( int nLeft, int nTop, int nRight, int nBottom, int nDir )
{
    m_nBentMargin[0] = nLeft;
    m_nBentMargin[1] = nTop;
    m_nBentMargin[2] = nRight;
    m_nBentMargin[3] = nBottom;
    m_nBentDirection = nDir;
}

void QInitDataIni::getBentMargin( int& nLeft, int& nTop, int& nRight, int& nBottom, int& nDir )
{
    nLeft	= m_nBentMargin[0];
    nTop	= m_nBentMargin[1];
    nRight	= m_nBentMargin[2];
    nBottom = m_nBentMargin[3];
    nDir	= m_nBentDirection;
}

void QInitDataIni::setBentAlgorithm2( int nType )
{
    m_nBentAlgorithm2 = nType;
}

int QInitDataIni::getBentAlgorithm2() const
{
    return m_nBentAlgorithm2;
}

void QInitDataIni::setBentAlgorithm4( int nType )
{
    m_nBentAlgorithm4 = nType;
}

int QInitDataIni::getBentAlgorithm4() const
{
    return m_nBentAlgorithm4;
}

int QInitDataIni::getActiveLanguageIndex() const
{
    return m_nActiveLanguageIndex;
}

bool QInitDataIni::getBentWithDummy() const
{
    return m_bBentWithDummy;
}

bool QInitDataIni::getCalibrationWarning() const
{
    return m_bCalibrationCamera;
}

bool QInitDataIni::getIgnoreCameraPair() const
{
    return m_bIgnoreCameraPairForBent;
}

int QInitDataIni::getMiniSideView() const
{
    return m_nMiniSideView;
}

bool QInitDataIni::getMirrorSideView() const
{
    return m_bMirrorSideView;
}

int QInitDataIni::getDTCGraphSharpWidth() const
{
    return m_nDetectionGraphSharpWidth;
}

float QInitDataIni::getDTCGraphCrackThresholdError() const
{
    return m_fDetectionGraphCrackThresholdError;
}

float QInitDataIni::getDTCGraphCrackThresholdWarning() const
{
    return m_fDetectionGraphCrackThresholdWarning;
}

int QInitDataIni::getDTCGraphLightThresholdError() const
{
    return m_nDetectionGraphLightThresholdError;
}

int QInitDataIni::getDTCGraphLightThresholdWarning() const
{
    return m_nDetectionGraphLightThresholdWarning;
}

int QInitDataIni::getDTGraphDefaultGain() const
{
    return m_nDetectionGainDefault;
}

bool QInitDataIni::load()
{
//#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
    QString strFile = QCoreApplication::applicationFilePath();
#ifdef Q_OS_WIN
    int nPos = strFile.lastIndexOf('.');
    if ( nPos >= 0 )
    {
        strFile = strFile.left( nPos+1 ) + "ini";
    }
    else
    {
        return false;
    }
#else
    strFile += ".ini";
#endif
//#else
//#ifdef Q_OS_MAC
//    QString strFile = QCoreApplication::applicationFilePath();
//    strFile += ".ini";
//#else
//    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//    strDocuments = rstrip( strDocuments, "/\\" );
//    QString strFile = strDocuments + QDir::separator() + "T3kCfgFE" + QDir::separator();
//    strFile += QCoreApplication::applicationName() + ".ini";
//#endif
//#endif

    QIni ini;

    if( !ini.load(strFile) )
    {
        return false;
    }

    QString strData;
    int nSP;
    int idx;

    QIni::QSection * pLanguageSection = ini.getSectionNoCase("LANGUAGE");
    if ( pLanguageSection )
    {
        m_nActiveLanguageIndex = 0;
        idx = pLanguageSection->getDataIndex("ActiveIndex");
        if ( idx >= 0 )
        {
            strData = pLanguageSection->getData(idx);
            strData = trim(strData);
            if ( !strData.isEmpty() )
                m_nActiveLanguageIndex = strData.toInt(0, 10);
        }
    }

    QIni::QSection * pSideViewSection = ini.getSectionNoCase("SIDEVIEW");
    if ( pSideViewSection )
    {
        m_nMiniSideView = 0;
        idx = pSideViewSection->getDataIndex("MiniSideView");
        if ( idx >= 0 )
        {
            strData = pSideViewSection->getData(idx);
            strData = trim(strData);
            if ( !strData.isEmpty() )
                m_nMiniSideView = strData.toInt(0, 10);
        }

        m_bMirrorSideView = false;
        idx = pSideViewSection->getDataIndex("MirrorSideView");
        if ( idx >= 0 )
        {
            strData = trim(pSideViewSection->getData(idx));
            if ( !strData.isEmpty() )
                m_bMirrorSideView = strData.toInt(0, 10);
        }
    }

    QIni::QSection * pBentAdjustmentSection = ini.getSectionNoCase("BENT ADJUSTMENT");

    if ( !pBentAdjustmentSection )
        return false;

    int nMargin[4] = {BENT_MARGIN_INIT_LEFT, BENT_MARGIN_INIT_TOP, BENT_MARGIN_INIT_RIGHT, BENT_MARGIN_INIT_BOTTOM};
    int nDir = BENT_MARGIN_INIT_DIR;
    idx = pBentAdjustmentSection->getDataIndex("BentMargin");
    if ( idx >= 0 )
    {
        strData = pBentAdjustmentSection->getData(idx);
        do
        {
            nSP = strData.indexOf( ',' );
            if ( nSP < 0 ) break;
            nMargin[0] = trim(strData.left(nSP)).toInt(0, 10);
            strData.remove( 0, nSP+1 );
            nSP = strData.indexOf( ',' );
            if ( nSP < 0 ) break;
            nMargin[1] = trim(strData.left(nSP)).toInt(0, 10);
            strData.remove( 0, nSP+1 );
            nSP = strData.indexOf( ',' );
            if ( nSP < 0 ) break;
            nMargin[2] = trim(strData.left(nSP)).toInt(0, 10);
            strData.remove( 0, nSP+1 );
            nSP = strData.indexOf( ',' );
            if ( nSP < 0 ) break;
            nMargin[3] = trim(strData.left(nSP)).toInt(0, 10);
            strData.remove( 0, nSP+1 );
            if ( strData.isEmpty() ) break;
            nDir = trim(strData).toInt(0, 10);
        }
        while( false );
    }
    memcpy( m_nBentMargin, nMargin, sizeof(int) * 4 );
    m_nBentDirection = nDir;

    m_nBentAlgorithm2 = BENT_INIT_ALGORITHM2;
    idx = pBentAdjustmentSection->getDataIndex("BentAlgorithm2");
    if ( idx >= 0 )
    {
        strData = pBentAdjustmentSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nBentAlgorithm2 = trim(strData).toInt(0, 10);

        if (m_nBentAlgorithm2 == 0)
            m_nBentAlgorithm2 = BENT_INIT_ALGORITHM2;
    }

    m_nBentAlgorithm4 = BENT_INIT_ALGORITHM4;
    idx = pBentAdjustmentSection->getDataIndex("BentAlgorithm4");
    if ( idx >= 0 )
    {
        strData = pBentAdjustmentSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nBentAlgorithm4 = trim(strData).toInt(0, 10);

        if (m_nBentAlgorithm4 == 0)
            m_nBentAlgorithm4 = BENT_INIT_ALGORITHM4;
    }

    m_bBentWithDummy = BENT_WITH_DUMMY;
    idx = pBentAdjustmentSection->getDataIndex("BentWithDummy");
    if ( idx >= 0 )
    {
        m_bBentWithDummyLoad = true;
        strData = pBentAdjustmentSection->getData(idx);
        if ( !strData.isEmpty() )
            m_bBentWithDummy = (strData.toInt( 0, 10 ) != 0) ? true : false;
    }

    m_bCalibrationCamera = BENT_WITH_CALIBRATION;
    idx = pBentAdjustmentSection->getDataIndex("CalibrationCamera");
    if ( idx >= 0 )
    {
        m_bCalibrationCameraLoad = true;
        strData = pBentAdjustmentSection->getData(idx);
        if ( !strData.isEmpty() )
        {
            m_bCalibrationCamera = (strData.toInt( 0, 10 ) != 0) ? true : false;
            if(m_bCalibrationCamera)
                m_nCalibrationCamera = 1;
        }
    }

    m_bIgnoreCameraPairForBent = BENT_IGNORE_CAMERAPAIR;
    idx = pBentAdjustmentSection->getDataIndex("IgnoreCameraPair");
    if ( idx >= 0 )
    {
        m_bIgnoreCameraPairForBentLoad = true;
        strData = pBentAdjustmentSection->getData(idx);
        if ( !strData.isEmpty() )
            m_bIgnoreCameraPairForBent = (strData.toInt( 0, 10 ) != 0) ? true : false;
    }

    QIni::QSection * pDTCGraphSection = ini.getSectionNoCase("DETECTION GRAPH");
    if ( !pDTCGraphSection )
        return false;

    m_nDetectionGraphSharpWidth = DTC_GRAPH_INIT_SHARP_WIDTH;
    idx = pDTCGraphSection->getDataIndex("CrackSharpWidth");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nDetectionGraphSharpWidth = trim(strData).toInt(0, 10);
    }

    m_fDetectionGraphCrackThresholdError = DTC_GRAPH_INIT_CRACK_THRESHOLD_ERROR;
    idx = pDTCGraphSection->getDataIndex("CrackThresholdError");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_fDetectionGraphCrackThresholdError = trim(strData).toFloat();
    }

    m_fDetectionGraphCrackThresholdWarning = DTC_GRAPH_INIT_CRACK_THRESHOLD_WARNING;
    idx = pDTCGraphSection->getDataIndex("CrackThresholdWarning");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_fDetectionGraphCrackThresholdWarning = trim(strData).toFloat();
    }

    m_nDetectionGraphLightThresholdError = DTC_GRAPH_INIT_LIGHT_THRESHOLD_ERROR;
    idx = pDTCGraphSection->getDataIndex("LightThresholdError");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nDetectionGraphLightThresholdError = trim(strData).toInt(0, 10);
    }

    m_nDetectionGraphLightThresholdWarning = DTC_GRAPH_INIT_LIGHT_THRESHOLD_WARNING;
    idx = pDTCGraphSection->getDataIndex("LightThresholdWarning");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nDetectionGraphLightThresholdWarning = trim(strData).toInt(0, 10);
    }

    m_nDetectionGainDefault = -1;
    idx = pDTCGraphSection->getDataIndex("DefaultGain");
    if ( idx >= 0 )
    {
        strData = pDTCGraphSection->getData(idx);
        if ( !strData.isEmpty() )
            m_nDetectionGainDefault = trim(strData).toInt(0, 10);
    }

    return true;
}

bool QInitDataIni::save()
{
//#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
    QString strFile = QCoreApplication::applicationFilePath();
#ifdef Q_OS_WIN
    int nPos = strFile.lastIndexOf('.');
    if ( nPos >= 0 )
    {
        strFile = strFile.left( nPos+1 ) + "ini";
    }
    else
    {
        return false;
    }
#else
    strFile += ".ini";
#endif
//#else
//#ifdef Q_OS_MAC
//    QString strFile = QCoreApplication::applicationFilePath();
//    strFile += ".ini";
//#else
//    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//    strDocuments = rstrip( strDocuments, "/\\" );
//    QString strFile = strDocuments + QDir::separator() + "T3kCfgFE" + QDir::separator();
//    makeDirectory(strFile);
//    strFile += QCoreApplication::applicationName() + ".ini";
//#endif
//#endif

    QIni ini;
    QString strData;

    QIni::QSection * pLanguageSection = ini.addSection("LANGUAGE");
    strData = QString("%1").arg(m_nActiveLanguageIndex);
    pLanguageSection->addData( "ActiveIndex", strData, "Language Index: English(0)" );

    QIni::QSection * pSideViewSection = ini.addSection("SIDEVIEW");
    strData = QString("%1").arg(m_nMiniSideView);
    pSideViewSection->addData( "MiniSideView", strData, "disable : 0, top : 1, left : 2, right : 3, bottom : 4" );

    strData = QString("%1").arg(m_bMirrorSideView);
    pSideViewSection->addData( "MirrorSideView", strData, "disable : 0, enable : 1" );

    QIni::QSection * pBentAdjustmentSection = ini.addSection("BENT ADJUSTMENT");

    strData = QString("%1,%2,%3,%4,%5").arg(m_nBentMargin[0]).arg(m_nBentMargin[1]).arg(m_nBentMargin[2]).arg(m_nBentMargin[3]).arg(m_nBentDirection);
    pBentAdjustmentSection->addData("BentMargin", strData, "Left,Top,Right,Bottom,Direction");

    strData = QString("%1").arg(m_nBentAlgorithm2);
    pBentAdjustmentSection->addData("BentAlgorithm2", strData, "2-camera algorithm (0: not used[old 13 points], 1: new 13 points v2, 2: new 9 points)");

    strData = QString("%1").arg(m_nBentAlgorithm4);
    pBentAdjustmentSection->addData("BentAlgorithm4", strData, "3,4-camera algorithm (0: not used[old 13 points], 1: new 13 points v2, 2: new 9 points)");

    if ( m_bBentWithDummyLoad )
    {
        strData = QString("%1").arg(m_bBentWithDummy);
        pBentAdjustmentSection->addData("BentWithDummy", strData, "bent-adjustment by 17 points that be included dummy points");
    }

    if ( m_bCalibrationCameraLoad )
    {
        strData = QString("%1").arg(m_nCalibrationCamera);
        pBentAdjustmentSection->addData("CalibrationCamera", strData);
    }

    if ( m_bIgnoreCameraPairForBentLoad )
    {
        strData = QString("%1").arg(m_bIgnoreCameraPairForBent);
        pBentAdjustmentSection->addData("IgnoreCameraPair", strData, "Only one camera can be even bent-adjustment");
    }

    QIni::QSection * pDTCGraphSection = ini.addSection("DETECTION GRAPH");

    strData = QString("%1").arg(m_nDetectionGraphSharpWidth);
    pDTCGraphSection->addData( "CrackSharpWidth", strData );

    strData = QString("%1").arg(m_fDetectionGraphCrackThresholdError);
    pDTCGraphSection->addData( "CrackThresholdError", strData );

    strData = QString("%1").arg(m_fDetectionGraphCrackThresholdWarning);
    pDTCGraphSection->addData( "CrackThresholdWarning", strData );

    strData = QString("%1").arg(m_nDetectionGraphLightThresholdError);
    pDTCGraphSection->addData( "LightThresholdError", strData );

    strData = QString("%1").arg(m_nDetectionGraphLightThresholdWarning);
    pDTCGraphSection->addData( "LightThresholdWarning", strData );

    strData = QString("%1").arg(m_nDetectionGainDefault);
    pDTCGraphSection->addData( "DefaultGain", strData );

    return ini.save(strFile);
}
