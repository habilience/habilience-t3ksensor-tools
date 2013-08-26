#include "QPrintLogData.h"

#include <QApplication>
#include "../common/T3kConstStr.h"

QPrintLogData::QPrintLogData(QObject* parent) :
        QFile(parent)
{
}

bool QPrintLogData::Open(QString strFilePathName, OpenMode eMode)
{
    if( strFilePathName.isEmpty() ) return false;

    if( strFilePathName.lastIndexOf('.') <= 0 )
    {
        strFilePathName += ".dat";
    }

    QFile::setFileName( strFilePathName );
    if( !QFile::open( eMode | QIODevice::Truncate ) )
    {
        QFile::setFileName( "" );
        return false;
    }

    m_TextStream.setDevice( this );

    return true;
}

bool QPrintLogData::Print(SensorLogData* vLogData)
{
    if( !isOpen() || !m_TextStream.device() ) return false;

    PrintTitle();

    m_TextStream << "var t3kdata = {\r\n";

    int nCM = 0;
    foreach( CMLogDataGroup* pCM, vLogData->CM )
    {
        PrintCM( pCM, nCM++ );
    }

    PrintMM( vLogData->MM );
    PrintEnvironment( vLogData->Env );
    PrintRSE( vLogData->RSE );

    m_TextStream << "};\r\n";
    m_TextStream << "t3k_start();\r\n";

    return true;
}

void QPrintLogData::PrintTitle()
{
    m_TextStream << "/*\r\n";
    m_TextStream << "\tCreated by Habilience <http://www.habilience.com>\r\n";
    m_TextStream << "\tHabilience(c).\r\n";
    m_TextStream << "*/\r\n\r\n";
}

void QPrintLogData::PrintMM(MMLogDataGroup &MM)
{
    m_TextStream << MakeBegin( "version_info", 0 );

    m_TextStream << MakeString( "nv", MM.VerInfo.strNV, 1 );
    m_TextStream << MakeString( "version", MM.VerInfo.strVer, 1 );
    m_TextStream << MakeString( "model", MM.VerInfo.strModel, 1 );
    m_TextStream << MakeString( "datetime", MM.VerInfo.strDateTime, 1, true );

    m_TextStream << MakeEnd( 0 );

    m_TextStream << MakeBegin( "factory", 0 );
    for( int i=0; i<MM.FactoryMM.size(); i++ )
    {
        PairRSP stRSP = MM.FactoryMM.at(i);
        m_TextStream << MakeString( stRSP.strKey, stRSP.strData, 1, i<MM.FactoryMM.size()-1 ? false : true );
    }
    m_TextStream << MakeEnd( 0 );

    foreach( PairRSP stRSP, MM.UserMM )
        m_TextStream << MakeString( stRSP.strKey, stRSP.strData );
}

void QPrintLogData::PrintCM(CMLogDataGroup *CM, int nCamNo)
{
    QString str;
    int nPart = nCamNo%2;
    int nSub = nCamNo/2;
    str = ( QString("cm%1").arg(nPart+1) );
    for( int i=0; i<nSub; i++ )
        str += QString("-%1").arg(nSub);
    //str = QString("cam%1").arg(nCamNo+1);

    m_TextStream << MakeBegin( str, 0 ); // S

    m_TextStream << MakeBegin( "version_info", 1 ); // S

    m_TextStream << MakeString( "nv", CM->VerInfo.strNV, 2 );
    m_TextStream << MakeString( "version", CM->VerInfo.strVer, 2 );
    m_TextStream << MakeString( "model", CM->VerInfo.strModel, 2 );
    m_TextStream << MakeString( "datetime", CM->VerInfo.strDateTime, 2, true );

    m_TextStream << MakeEnd( 1 ); // E

    if( CM->bNoCam )
    {
        m_TextStream << MakeBegin( "factory" , 1 ); // S

        m_TextStream << MakeString( cstrSensorGain, "", 2 );
        m_TextStream << MakeString( cstrAmbientLight, "", 2 );
        m_TextStream << MakeString( cstrFactorialCamPos, "", 2 );
        m_TextStream << MakeString( cstrAdminDetectionLine, "", 2 );
        m_TextStream << MakeString( cstrAdminDetectionCenter, "", 2 );
        m_TextStream << MakeString( cstrAdminDetectionBarrel, "", 2 );
        m_TextStream << MakeString( cstrAdminSettingTime, "", 2 );
        m_TextStream << MakeString( cstrAdminSerial, "", 2 );
        m_TextStream << MakeString( cstrDetectionLine, "", 2 );
        m_TextStream << MakeString( cstrDetectionThreshold, "", 2 );
        m_TextStream << MakeString( cstrDetectionRange, "", 2 );
        m_TextStream << MakeString( cstrDetectionCoef1, "", 2 );
        m_TextStream << MakeString( cstrDetectionCoef2, "", 2 );
        m_TextStream << MakeString( cstrDetectionCoef3, "", 2, true );

        m_TextStream << MakeEnd( 1 );        // E


        m_TextStream << MakeString( cstrSensorGain, "", 1 );
        m_TextStream << MakeString( cstrAmbientLight, "", 1 );
        m_TextStream << MakeString( cstrDetectionLine, "", 1 );
        m_TextStream << MakeString( cstrDetectionThreshold, "", 1 );
        m_TextStream << MakeString( cstrDetectionRange, "", 1 );
        m_TextStream << MakeString( cstrDetectionCoef1, "", 1 );
        m_TextStream << MakeString( cstrDetectionCoef2, "", 1 );
        m_TextStream << MakeString( cstrDetectionCoef3, "", 1 );

        m_TextStream << MakeString( "detection", CM->strIRD, 1 );
        m_TextStream << MakeString( "threshold", CM->strITD, 1, true );
    }
    else
    {
        m_TextStream << MakeBegin( "factory" , 1 ); // S

        for( int i=0; i<CM->FactoryCM.size(); i++ )
        {
            PairRSP stRSP = CM->FactoryCM.at(i);
            m_TextStream << MakeString( stRSP.strKey, stRSP.strData, 2, i<CM->FactoryCM.size()-1 ? false : true );
        }
        m_TextStream << MakeEnd( 1 );        // E

        foreach( PairRSP stRSP, CM->UserCM )
            m_TextStream << MakeString( stRSP.strKey, stRSP.strData, 1 );

        m_TextStream << MakeString( "detection", CM->strIRD, 1 );
        m_TextStream << MakeString( "threshold", CM->strITD, 1, true );
    }

    m_TextStream << MakeEnd( 0 );                      // E
}

void QPrintLogData::PrintEnvironment(QVector<PairRSP> &vEnv)
{
    m_TextStream << MakeBegin( "host", 0 );

    for( int i=0; i<vEnv.size(); i++ )
    {
        PairRSP stRSP = vEnv.at(i);
        m_TextStream << MakeString( stRSP.strKey, stRSP.strData, 1, i<vEnv.size()-1 ? false : true );
    }

    m_TextStream << MakeEnd( 0 );
}

void QPrintLogData::PrintRSE(QVector<QString> &RSE)
{
    m_TextStream << MakeBegin( "rse", 0 );

    for( int i=0; i<RSE.size(); i++ )
        m_TextStream << MakeString( QString("%1").arg(i), RSE.at(i), 1, i<RSE.size()-1 ? false : true );

    m_TextStream << MakeEnd( 0, true );
}

QString QPrintLogData::MakeBegin(QString strSection, int nSectionStep)
{
    QString str;
    for(int i=0;i<nSectionStep;i++)
        str += '\t';

    str += QString("\"%1\":{\r\n").arg(strSection);
    return str;
}

QString QPrintLogData::MakeEnd(int nSectionStep, bool bEndSection)
{
    QString str;
    for(int i=0;i<nSectionStep;i++)
        str += '\t';
    return str + QString("}%1\r\n").arg(bEndSection ? "" : ",");
}

QString QPrintLogData::MakeString(QString strSection, QString strData, int nSectionStep /*=0*/, bool bEndItem)
{
    QString str;
    for(int i=0;i<nSectionStep;i++)
        str += '\t';

    strSection.remove( '=' );
    str += QString("\"%1\":\"%2\"%3\r\n").arg(strSection).arg(strData).arg(bEndItem ? "" : ",");
    return str;
}
