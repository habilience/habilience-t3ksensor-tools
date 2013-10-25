#include "T3kBuzzerDef.h"

#include "T3kConstStr.h"

void playBuzzer( T3kHandle* pHandle, BuzzerType type )
{
    int nCat = 0, nType = 0;

    switch( type )
    {
    case BuzzerEnterCalibration:
        nCat = 3, nType = 1;
        break;
    case BuzzerCancelCalibration:
        nCat = 3, nType = 3;
        break;
    case BuzzerCalibrationSucces:
        nCat = 3, nType = 2;
        break;
    case BuzzerClick:
        nCat = 1, nType = 1;
        break;
    case BuzzerNextPoint:
        nCat = 5, nType = 1;
        break;
    }

    QString strCmd;
    strCmd = QString(cstrBuzzerPlay) + QString::number(nCat) + "," + QString::number(nType);
    pHandle->SendCommand(strCmd.toUtf8().data(), true);
}
