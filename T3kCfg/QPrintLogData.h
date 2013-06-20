#ifndef QPRINTLOGDATA_H
#define QPRINTLOGDATA_H

#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QVector>
#include "LogDataDef.h"


class QPrintLogData : public QFile
{
public:
    QPrintLogData(QObject* parent = 0);

    bool Open( QString strFilePathName, OpenMode eMode );
    bool Print( SensorLogData* vLogData );

protected:
    void PrintTitle();
    void PrintMM( MMLogDataGroup& MM );
    void PrintCM( CMLogDataGroup* CM, int nCamNo );
    void PrintRSE( QVector<QString>& RSE );

    void PrintEnvironment( QVector<PairRSP>& vEnv );

private:
    QString MakeT30xConstStrTitle( QString strConstStr );
    QString MakeString( QString strSection, QString strData, int nSectionStep = 0, bool bEndItem = false );

    QString MakeBegin( QString strSection, int nSectionStep );
    QString MakeEnd( int nSectionStep, bool bEndSection = false );

private:
    QTextStream         m_TextStream;
};

#endif // QPRINTLOGDATA_H
