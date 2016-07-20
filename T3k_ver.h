#ifndef T3KS_VERSION
#define T3KS_VERSION

#define T3K_FILEVERSION                 2,9,0,0
#define T3K_VERSION                     "2.9.0"


#define MODEL_T3000                     0x01
#define MODEL_T3100                     0x02
#define MODEL_T3500                     0x35
#define MODEL_T3200                     0x36
#define MODEL_UNKNOWN                   0xFF

#define MM_MIN_TOOLS_FIRMWARE_VERSION   2.8
#define MM_MIN_FIRMWARE_VERSION			2.9		// 최소 펌웨어 버전
#define MM_NEXT_FIRMWARE_VERSION		3.0		// 다음 펌웨어 버전
#define MM_MIN_FIRMWARE_VERSION_STR     "2.9"  // 최소 펌웨어 버전 == resource binary file version
#define MM_NEXT_FIRMWARE_VERSION_STR    "2.9"  // 다음 펌웨어 버전

// 2.8d < 2.86 < 2.8e < 2.8f

// 지원 펌웨어 버전: (MM_MIN_FIRMWARE_VERSION <= f/w < MM_NEXT_FIRMWARE_VERSION)

//#define FEATURE_CUSTOMER // customer 용 definition

#endif // T3KS_VERSION

