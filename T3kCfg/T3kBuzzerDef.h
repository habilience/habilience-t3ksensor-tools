#ifndef T3KBUZZERDEF_H
#define T3KBUZZERDEF_H

#include "T3kHandle.h"

enum BuzzerType { BuzzerEnterCalibration, BuzzerCancelCalibration, BuzzerCalibrationSucces, BuzzerClick, BuzzerNextPoint };
void playBuzzer( T3kHandle* pHandle, BuzzerType type );

#endif // T3KBUZZERDEF_H
