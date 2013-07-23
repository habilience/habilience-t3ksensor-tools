#ifndef QLANGZIPFILE_H
#define QLANGZIPFILE_H

#include <QVector>
#include <QString>
#include <quazipfile.h>

class QLangZipFile
{
protected:
    QVector<QString>        m_aryFileName;
    QVector<unsigned long>  m_aryFileSize;

    QuaZip                  m_zip;
public:
    QLangZipFile();
    ~QLangZipFile();

    bool open(const QString& strPathName);
    bool isOpen();
    void close();

    bool isExist( const QString& strFileName );
    unsigned long readFile( const QString& strFileName, char* pBuffer, unsigned long dwBufSize );
};

#endif // QLANGZIPFILE_H
