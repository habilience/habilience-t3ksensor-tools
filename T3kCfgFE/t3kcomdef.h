#ifndef T3K_COMDEF_H
#define T3K_COMDEF_H

struct T3K_SENSOR_DEVICE
{
    unsigned short nVID;
    unsigned short nPID;
    unsigned short nMI;
    const char* szModelName;
};

const T3K_SENSOR_DEVICE DEVICE_LIST[] = {
    {0xFFFF, 0x0000, 1, "T3000"},
    {0x2200, 0x3000, 1, "T3000"},
    {0x2200, 0x3100, 1, "T3100"},
    {0x2200, 0x3200, 1, "T3200"},
    {0x2200, 0x3500, 1, "T35xx"},
    {0x2200, 0x3900, 1, "T3900"},
    {0x2200, 0xFF02, 0, "T3kVHID"}
};

#define COUNT_OF_DEVICE_LIST ((int)(sizeof(DEVICE_LIST) / sizeof(T3K_SENSOR_DEVICE)))

#endif // T3K_COMDEF_H
