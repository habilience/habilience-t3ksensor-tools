#ifndef T3KBUZZERDEF_H
#define T3KBUZZERDEF_H

#include "QT3kDevice.h"

enum BuzzerType { BuzzerEnterCalibration, BuzzerCancelCalibration, BuzzerCalibrationSucces, BuzzerClick, BuzzerNextPoint };
void playBuzzer( QT3kDevice* pHandle, BuzzerType type );

#endif // T3KBUZZERDEF_H
