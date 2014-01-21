#ifndef T3KBUZZERDEF_H
#define T3KBUZZERDEF_H

#include "QT3kDeviceR.h"

enum BuzzerType { BuzzerEnterCalibration, BuzzerCancelCalibration, BuzzerCalibrationSucces, BuzzerClick, BuzzerNextPoint };
void playBuzzer( QT3kDeviceR* pHandle, BuzzerType type );

#endif // T3KBUZZERDEF_H
