#ifndef QLOGSYSTEM_H
#define QLOGSYSTEM_H

#include <QDateTime>
#include <QString>
#include <QFile>
#include <QMutex>

#define LOG_I		(*QLogSystem::instance("INFO"))
#define LOG_C		(*QLogSystem::instance("CMD"))
#define LOG_R		(*QLogSystem::instance("RSP"))
#define LOG_B		(*QLogSystem::instance("BTN"))

class QLogSystem
{
public:
    class _gc
    {
    public:
        _gc();
        ~_gc();
    };
    friend class _gc;

public:
    static QLogSystem* instance();
    static QLogSystem* instance(const QString &strTag );

    void write( const QString& szTag, const char* szFormat, ... );
    void writeV( const QString& strTag, const QString& strFormatV );

    void operator()( const char* szFormat, ... );

protected:
    void getLogPathName( QDateTime& time, QString& strLogPathName );
    bool openLogFile( QString& strLogPathName );
    void deleteExpiredLogFiles();

private:
    QFile       m_File;
    QDateTime   m_Time;
    int         m_nExpire;
    QString     m_strRootPath;
    QMutex      m_csFile;

    QString     m_strTag;
    static QLogSystem* m_pThis;

private:
    QLogSystem();
    ~QLogSystem();
};

#endif // QLOGSYSTEM_H
