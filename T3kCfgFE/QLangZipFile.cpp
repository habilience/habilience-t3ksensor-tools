#include "QLangZipFile.h"

#define LANGZIP_PASSWD      "langzip"

QLangZipFile::QLangZipFile()
{
}

QLangZipFile::~QLangZipFile()
{

}

bool QLangZipFile::open(const QString& strPathName)
{
    close();
    m_zip.setZipName( strPathName );
    if (!m_zip.open(QuaZip::mdUnzip))
        return false;

    QuaZipFile file(&m_zip);

    for ( bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile() )
    {
        if (!file.open(QIODevice::ReadOnly, LANGZIP_PASSWD))
            continue;

        if ( file.size() > 0 )
        {
            QString strFileName = file.getActualFileName();
            int nSP = strFileName.lastIndexOf('/');
            if ( nSP >= 0 )
            {
                strFileName = strFileName.right(strFileName.size() - nSP - 1);
            }
            m_aryFileName.push_back(strFileName);
            m_aryFileSize.push_back((unsigned long)file.size());
        }
        file.close();
    }

    return true;
}

bool QLangZipFile::isOpen()
{
    return m_zip.isOpen();
}

void QLangZipFile::close()
{
    if (isOpen())
        m_zip.close();
}

bool QLangZipFile::isExist( const QString& strFileName )
{
    if (!isOpen())
        return false;

    QString str;
    QString strF = strFileName.toLower();
    for (int i=0 ; i<m_aryFileName.size() ; i++ )
    {
        str = m_aryFileName.at(i).toLower();
        if ( str.indexOf(strF) >= 0 )
        {
            return true;
        }
    }

    return false;
}

unsigned long QLangZipFile::readFile( const QString& strFileName, char* pBuffer, unsigned long dwBufSize )
{
    int nFileIndex = -1;
    QString str;
    QString strF = strFileName.toLower();
    for (int i=0 ; i<m_aryFileName.size() ; i++ )
    {
        str = m_aryFileName.at(i).toLower();
        if ( str.indexOf(strF) >= 0 )
        {
            nFileIndex = i;
            break;
        }
    }

    if ( nFileIndex < 0 )
        return 0;

    if ( !pBuffer )
    {
        return m_aryFileSize.at(nFileIndex);
    }

    QuaZipFile file(&m_zip);

    int nIndex = 0;
    for ( bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile() )
    {
        if (!file.open(QIODevice::ReadOnly, LANGZIP_PASSWD))
            continue;

        if ( file.size() > 0 )
        {
            if ( nIndex == nFileIndex )
            {
                unsigned long dwReaded = file.read( pBuffer, dwBufSize );
                file.close();
                return dwReaded;
            }

            nIndex++;
        }
        file.close();
    }

    return 0;
}
