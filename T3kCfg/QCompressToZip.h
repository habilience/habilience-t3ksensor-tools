#ifndef QCOMPRESSTOZIP_H
#define QCOMPRESSTOZIP_H

#include <QString>
#include <zlib.h>

#include <quazip.h>
#include <quazipfile.h>
#include <quazipfileinfo.h>

class QUrlInfo;

class QCompressToZip
{
public:
    QCompressToZip();

    bool CompressFolderAll(QString strOutputPath, QString fileCompressed, QString dir = QString(), bool recursive = true, QString strPassword="");

protected:
    QString m_strPasswrd;
private:
    bool CompressFile(QuaZip* zip, QString fileName, QString fileDest);
    bool CompressSubDir(QuaZip* parentZip, QString dir, QString parentDir, bool recursive = true);
};

#endif // QCOMPRESSTOZIP_H
