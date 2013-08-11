#include "QLogSystem.h"

#include <QDir>
#include <QStandardPaths>
#include "../common/QUtils.h"
#include <QCoreApplication>

QLogSystem::_gc::_gc() {}
QLogSystem::_gc::~_gc()
{
    if (QLogSystem::m_pThis)
        delete QLogSystem::m_pThis;
    QLogSystem::m_pThis = NULL;
}
static QLogSystem::_gc s_gc;

QLogSystem* QLogSystem::m_pThis = NULL;
QLogSystem::QLogSystem() : m_nExpire(15)
{
    m_Time = QDateTime::currentDateTime();
    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    strDocuments = rstrip( strDocuments, "/\\" );

    m_strRootPath = strDocuments;
    m_strRootPath += "/T3kCfgFE/Logs";

    makeDirectory(m_strRootPath.toLatin1());

    QString strLogPathName;
    getLogPathName( m_Time, strLogPathName );
    openLogFile( strLogPathName );

    deleteExpiredLogFiles();

    write( "INFO", ("==== T3kCfgFE Start! ====") );
    write( "INFO", "Exec Path: %s", (const char*)qApp->applicationFilePath().toLatin1() );
}

QLogSystem::~QLogSystem()
{
    write( "INFO", "==== T3kCfgFE Exit! ====" );
}

QLogSystem* QLogSystem::instance()
{
    if (m_pThis) { m_pThis->m_strTag = ""; return m_pThis; }
    m_pThis = new QLogSystem();
    m_pThis->m_strTag = "";
    return m_pThis;
}

QLogSystem* QLogSystem::instance( const QString& strTag )
{
    if (m_pThis) { m_pThis->m_strTag = strTag; return m_pThis; }
    m_pThis = new QLogSystem();
    m_pThis->m_strTag = strTag;
    return m_pThis;
}

#define LOG_BUFFER_SIZE (1024)

void QLogSystem::operator()( const char* szFormat, ... )
{
    if (szFormat == NULL)
        return;

    va_list argList;
    va_start(argList, szFormat);

    char szBuffer[LOG_BUFFER_SIZE];
    vsnprintf( szBuffer, LOG_BUFFER_SIZE, szFormat, argList );

    va_end(argList);

    writeV( m_strTag, szBuffer );
}

void QLogSystem::operator()( const QString& strLog )
{
    writeV( m_strTag, strLog );
}

void QLogSystem::write( const QString& strTag, const char* szFormat, ... )
{
    if (szFormat == NULL)
        return;

    va_list argList;
    va_start(argList, szFormat);

    char szBuffer[LOG_BUFFER_SIZE];
    vsnprintf( szBuffer, LOG_BUFFER_SIZE, szFormat, argList );

    va_end(argList);

    writeV( strTag, szBuffer );
}

void QLogSystem::writeV(const QString &strTag, const QString &strFormatV )
{
    if (strFormatV.isEmpty())
        return;

    do
    {
        QMutexLocker Lock(&m_csFile);

        if (!m_File.isOpen()) break;

        QDateTime curTime = QDateTime::currentDateTime();
        QDate dateOld = m_Time.date();
        QDate dateCur = curTime.date();

        if ( (dateCur.year() != dateOld.year()) ||
             (dateCur.month() != dateOld.month()) ||
             (dateCur.day() != dateOld.day()) )
        {
            m_File.close();
            QString strLogPathName;
            getLogPathName( curTime, strLogPathName );

            if (!openLogFile(strLogPathName))
                break;
            deleteExpiredLogFiles();

            m_Time = curTime;
        }

        QTime timeCur = curTime.time();
        char szBuffer[LOG_BUFFER_SIZE+128];
        snprintf( szBuffer, LOG_BUFFER_SIZE+128, "%02d:%02d:%02d\t%s\t%s\r\n", timeCur.hour(), timeCur.minute(), timeCur.second(), (const char*)strTag.toLatin1(), (const char*)strFormatV.toLatin1() );
        QString strCurrentLog = szBuffer;
        QByteArray byteArray = strCurrentLog.toLatin1();
        m_File.write( byteArray );
        m_File.flush();
    } while (false);
}

void QLogSystem::getLogPathName( QDateTime& time, QString& strLogPathName )
{
    QDate date = time.date();
    int nYear = date.year();
    int nMonth = date.month();
    int nDay = date.day();

    char szBuffer[512];
    snprintf( szBuffer, 512, "%s\\%04d_%02d_%02d.txt", (const char*)m_strRootPath.toLatin1(), nYear, nMonth, nDay );
    strLogPathName = szBuffer;
}

bool QLogSystem::openLogFile( QString& strLogPathName )
{
    //char* UTF8Header = "\xEF\xBB\xBF";

    m_File.setFileName(strLogPathName);

    if (!m_File.exists())
    {
        if (!m_File.open(QIODevice::WriteOnly))
            return false;

        QDate curDate = QDate::currentDate();
        char szBuffer[512];
        snprintf( szBuffer, 512, "%04d/%02d/%02d", curDate.year(), curDate.month(), curDate.day() );
        QString strTime = szBuffer;
        QString strComment = "################ T3k Configurator FE ################\r\n"
                             "# - Created by T3kCfgFE\r\n"
                             "# - Date: " + strTime + "\r\n"
                             "#####################################################\r\n";
        m_File.write( strComment.toUtf8() );
        m_File.close();
    }

    if( !m_File.open(QIODevice::Append) )
        return false;

    // seek to end
    m_File.seek(m_File.size());

    return true;
}

void QLogSystem::deleteExpiredLogFiles()
{
    if( m_nExpire < 0 )
        return;

    QDir currentDir(m_strRootPath);
    QStringList files;
    QString fileName = "*.txt";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    QString strFileName;
    QString strDate;
    for ( int i=0 ; i<files.size() ; i++ )
    {
        strFileName = files.at(i);
        strDate = strFileName;

        QString strYear = extractLeft( strDate, '_' );
        if (strYear.isEmpty()) break;

        QString strMonth = extractLeft( strDate, '_' );
        if (strMonth.isEmpty()) break;

        QString strDay = extractLeft( strDate, '.' );
        if (strDay.isEmpty()) break;

        int nYear = strYear.toInt( 0, 10 );
        int nMonth = strMonth.toInt( 0, 10 );
        int nDay = strDay.toInt( 0, 10 );

        QDate findFileDate(nYear, nMonth, nDay);
        QDate checkDate = m_Time.date();
        if ((int)findFileDate.daysTo(checkDate) >= m_nExpire)
        {
            QString strFullPathName = m_strRootPath;
            strFullPathName += '/';
            strFullPathName += strFileName;
            QFile::remove(strFullPathName);
        }
    }
}
