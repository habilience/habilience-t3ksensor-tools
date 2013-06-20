#include "QCompressToZip.h"

#include <QFile>
#include <QDir>

static bool copyData(QIODevice &inFile, QIODevice &outFile)
{
    while (!inFile.atEnd()) {
        char buf[4096];
        qint64 readLen = inFile.read(buf, 4096);
        if (readLen <= 0)
            return false;
        if (outFile.write(buf, readLen) != readLen)
            return false;
    }
    return true;
}

QCompressToZip::QCompressToZip()
{
}

bool QCompressToZip::CompressFolderAll(QString strOutputPath, QString fileCompressed, QString dir, bool recursive, QString strPassword)
{
    QuaZip* zip  = new QuaZip(strOutputPath + '/' + fileCompressed);
    QDir().mkpath(strOutputPath);
    if(!zip->open(QuaZip::mdCreate)) {
        delete zip;
        QFile::remove(strOutputPath + '/' + fileCompressed);
        return false;
    }

    m_strPasswrd = strPassword;

    if (!CompressSubDir(zip,dir,dir,recursive)<0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }

    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        QFile::remove(fileCompressed);
        return false;
    }
    delete zip;

/*    QDir tdir( dir );
    if( tdir.cdUp() )
    {
        QFile::setPermissions( tdir.path() + ZIP_STATEREPORT_NAME + ".zip", (QFile::Permission)0x7777 );
    }*/

    return true;
}

bool QCompressToZip::CompressFile(QuaZip* zip, QString fileName, QString fileDest)
{
    if (!zip)
        return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd)
        return false;

    QFile inFile;
    inFile.setFileName(fileName);
    if(!inFile.open(QIODevice::ReadOnly))
        return false;

    QuaZipFile outFile(zip);
    QuaZipNewInfo info(fileDest, inFile.fileName());
    info.externalAttr = 0x3B40<<12;
    if( m_strPasswrd.isEmpty() )
    {
        if(!outFile.open(QIODevice::WriteOnly, info))
            return false;
    }
    else
    {
        if(!outFile.open(QIODevice::WriteOnly, info, m_strPasswrd.toUtf8().data()))
            return false;
    }

    if (!copyData(inFile, outFile) || outFile.getZipError()!=UNZ_OK) {
        return false;
    }

    outFile.close();
    if (outFile.getZipError()!=UNZ_OK)
        return false;
    inFile.close();

    return true;
}

bool QCompressToZip::CompressSubDir(QuaZip* zip, QString dir, QString origDir, bool recursive)
{
    if (!zip) return false;

    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    QDir directory(dir);
    if (!directory.exists()) return false;

    if (recursive)
    {
        QFileInfoList files = directory.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot);
        Q_FOREACH (QFileInfo file, files)
        {
            if(!CompressSubDir(zip,file.absoluteFilePath(),origDir,recursive))
                return false;
        }
    }

    QFileInfoList files = directory.entryInfoList(QDir::Files|QDir::Hidden);
    QDir origDirectory(origDir);
    Q_FOREACH (QFileInfo file, files)
    {
        if(!file.isFile()||file.absoluteFilePath()==zip->getZipName())
            continue;

        QString filename = origDirectory.relativeFilePath(file.absoluteFilePath());

        if (!CompressFile(zip,file.absoluteFilePath(),filename))
            return false;
    }

    return true;
}
