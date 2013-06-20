#ifndef LOGDATADEF_H
#define LOGDATADEF_H

#include <QString>
#include <QVector>
#include <QMap>

typedef struct _PairRSP
{
    QString                     strKey;
    QString                     strData;
} PairRSP;

typedef struct _FWVersionInfo
{
    QString                     strNV;
    QString                     strVer;
    QString                     strModel;
    QString                     strDateTime;
} FWVersionInfo;

typedef struct _MMLogDataGroup
{
    FWVersionInfo               VerInfo;
    QVector<PairRSP>            UserMM;
    QVector<PairRSP>            FactoryMM;
} MMLogDataGroup;

typedef struct _CMLogDataGroup
{
    FWVersionInfo               VerInfo;
    QVector<PairRSP>            UserCM;
    QVector<PairRSP>            FactoryCM;
    QString                     strIRD;
    QString                     strITD;
    bool                        bNoCam;
} CMLogDataGroup;

typedef struct _SensorLogData
{
    MMLogDataGroup              MM;
    QMap<int, CMLogDataGroup*>  CM;
    QVector<PairRSP>            Env;
    QVector<QString>            RSE;
} SensorLogData;

#endif // LOGDATADEF_H
