#ifndef T3K_COMDEF_H
#define T3K_COMDEF_H

struct T3K_SENSOR_DEVICE
{
    unsigned short nVID;
    unsigned short nPID;
    unsigned short nMI;
    const char* szModelName;
};

const T3K_SENSOR_DEVICE BASE_DEVICE_LIST[] = {
    {0xFFFF, 0x0000, 1, "T3000"},
    {0x2200, 0x3000, 1, "T3000"},
    {0x2200, 0x3100, 1, "T3100"},
    {0x2200, 0x3200, 1, "T3200"},
    {0x2200, 0x3300, 1, "T3300"},
    {0x2200, 0x3500, 1, "T35xx"},
    {0x2200, 0x3600, 1, "T36xx"},
    {0x2200, 0x3900, 1, "T3900"}
};

const T3K_SENSOR_DEVICE APP_DEVICE_LIST[] = {
    {0xFFFF, 0x0000, 1, "T3000"},
    {0x2200, 0x3000, 1, "T3000"},
    {0x2200, 0x3100, 1, "T3100"},
    {0x2200, 0x3200, 1, "T3200"},
    {0x2200, 0x3300, 1, "T3300"},
    {0x2200, 0x3500, 1, "T35xx"},
    {0x2200, 0x3600, 1, "T36xx"},
    {0x2200, 0x3900, 1, "T3900"},

    {0x2200, 0xFF02, 0, "T3kVHID"}
};

const T3K_SENSOR_DEVICE UPGRADE_DEVICE_LIST[] = {
    {0xFFFF, 0x0000, 1, "T3000"},
    {0x2200, 0x3000, 1, "T3000"},
    {0x2200, 0x3100, 1, "T3100"},
    {0x2200, 0x3200, 1, "T3200"},
    {0x2200, 0x3300, 1, "T3300"},
    {0x2200, 0x3500, 1, "T35xx"},
    {0x2200, 0x3600, 1, "T36xx"},
    {0x2200, 0x3900, 1, "T3900"},

    {0x2200, 0x0001, 0, ""},        // T3100 iap
    {0x2200, 0x0002, 0, ""},        // T3100 iap upg
    {0x2200, 0x0003, 0, ""},        // T3000 iap
    {0x2200, 0x0004, 0, ""},        // T3000 iap upg
    {0x2200, 0x3201, 0, ""},        // T3200 iap
    {0x2200, 0x3202, 0, ""},        // T3200 iap upg
    {0x2200, 0x3301, 0, ""},
    {0x2200, 0x3302, 0, ""},
    {0x2200, 0x3501, 0, ""},        // T3500 iap
    {0x2200, 0x3502, 0, ""},        // T3500 iap upg
    {0x2200, 0x3601, 0, ""},        // T3600 iap
    {0x2200, 0x3602, 0, ""},        // T3600 iap upg
    {0x2200, 0x3901, 0, ""},        // T3900 iap
    {0x2200, 0x3902, 0, ""}         // T3900 iap upg
};

#define COUNT_OF_DEVICE_LIST(list) ((int)(sizeof(list) / sizeof(T3K_SENSOR_DEVICE)))


#define REPORTID_FEATURE_SENSOR_STATUS		8
#define REPORTID_FEATURE_SCREEN_MODE		9
#define REPORTID_FEATURE_CURSOR_POS			12

#pragma pack(1)
    typedef struct _FeatureSensorStatus
    {
        unsigned char   ReportID;
        unsigned char   Status;             // 0: Normal, 1: Firmware Downloading
    } FeatureSensorStatus;

    typedef struct _FeatureScreenMode
    {
        unsigned char   ReportID;
        unsigned char   ScreenMode;         // 0: Normal, 1: Sideview, 2: Detection
    } FeatureScreenMode;

    typedef struct _FeatureCursorPos
    {
        unsigned char   ReportID;
        unsigned char   Show;
        unsigned char   IsScreenCoordinate; // if 0 -> X, Y is 4 corner(LT:0,0 RT:1,0 RB:1,1, LB:0,1)
        unsigned char   TouchOK;
        short           X;
        short           Y;
        unsigned char   Progress;
    } FeatureCursorPos;
#pragma pack()

#endif // T3K_COMDEF_H
