#ifndef WINDOWINFOUSERDEF_H
#define WINDOWINFOUSERDEF_H

#ifdef Q_OS_WIN
//#define PRODUCT_BUSINESS                            0x00000006
//#define PRODUCT_BUSINESS_N                          0x00000010
//#define PRODUCT_CLUSTER_SERVER                      0x00000012
//#define PRODUCT_DATACENTER_SERVER                   0x00000008
//#define PRODUCT_DATACENTER_SERVER_CORE              0x0000000C
//#define PRODUCT_DATACENTER_SERVER_CORE_V            0x00000027
//#define PRODUCT_DATACENTER_SERVER_V                 0x00000025
//#define PRODUCT_ENTERPRISE                          0x00000004
//#define PRODUCT_ENTERPRISE_E                        0x00000046
//#define PRODUCT_ENTERPRISE_N                        0x0000001B
//#define PRODUCT_ENTERPRISE_SERVER                   0x0000000A
//#define PRODUCT_ENTERPRISE_SERVER_CORE              0x0000000E
//#define PRODUCT_ENTERPRISE_SERVER_CORE_V            0x00000029
//#define PRODUCT_ENTERPRISE_SERVER_IA64              0x0000000F
//#define PRODUCT_ENTERPRISE_SERVER_V                 0x00000026
//#define PRODUCT_HOME_BASIC                          0x00000002
//#define PRODUCT_HOME_BASIC_E                        0x00000043
//#define PRODUCT_HOME_BASIC_N                        0x00000005
//#define PRODUCT_HOME_PREMIUM                        0x00000003
//#define PRODUCT_HOME_PREMIUM_E                      0x00000044
//#define PRODUCT_HOME_PREMIUM_N                      0x0000001A
//#define PRODUCT_HYPERV                              0x0000002A
//#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    0x0000001E
//#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     0x00000020
//#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      0x0000001F
//#define PRODUCT_PROFESSIONAL                        0x00000030
//#define PRODUCT_PROFESSIONAL_E                      0x00000045
//#define PRODUCT_PROFESSIONAL_N                      0x00000031
//#define PRODUCT_SERVER_FOR_SMALLBUSINESS            0x00000018
//#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V          0x00000023
//#define PRODUCT_SERVER_FOUNDATION                   0x00000021
//#define PRODUCT_HOME_PREMIUM_SERVER                 0x00000022
//#define PRODUCT_SB_SOLUTION_SERVER                  0x00000032
//#define PRODUCT_HOME_SERVER                         0x00000013
//#define PRODUCT_SMALLBUSINESS_SERVER                0x00000009
//#define PRODUCT_SOLUTION_EMBEDDEDSERVER             0x00000038
//#define PRODUCT_STANDARD_SERVER                     0x00000007
//#define PRODUCT_STANDARD_SERVER_CORE                0x0000000D
//#define PRODUCT_STANDARD_SERVER_CORE_V              0x00000028
//#define PRODUCT_STANDARD_SERVER_V                   0x00000024
//#define PRODUCT_STARTER                             0x0000000B
//#define PRODUCT_STARTER_E                           0x00000042
//#define PRODUCT_STARTER_N                           0x0000002F
//#define PRODUCT_STORAGE_ENTERPRISE_SERVER           0x00000017
//#define PRODUCT_STORAGE_EXPRESS_SERVER              0x00000014
//#define PRODUCT_STORAGE_STANDARD_SERVER             0x00000015
//#define PRODUCT_STORAGE_WORKGROUP_SERVER            0x00000016
//#define PRODUCT_UNDEFINED                           0x00000000
//#define PRODUCT_ULTIMATE                            0x00000001
//#define PRODUCT_ULTIMATE_E                          0x00000047
//#define PRODUCT_ULTIMATE_N                          0x0000001C
//#define PRODUCT_WEB_SERVER                          0x00000011
//#define PRODUCT_WEB_SERVER_CORE                     0x0000001D


//#define PRODUCT_UNDEFINED                       0x00000000

//#define PRODUCT_ULTIMATE                        0x00000001
//#define PRODUCT_HOME_BASIC                      0x00000002
//#define PRODUCT_HOME_PREMIUM                    0x00000003
//#define PRODUCT_ENTERPRISE                      0x00000004
//#define PRODUCT_HOME_BASIC_N                    0x00000005
//#define PRODUCT_BUSINESS                        0x00000006
//#define PRODUCT_STANDARD_SERVER                 0x00000007
//#define PRODUCT_DATACENTER_SERVER               0x00000008
//#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
//#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
//#define PRODUCT_STARTER                         0x0000000B
//#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
//#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
//#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
//#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
//#define PRODUCT_BUSINESS_N                      0x00000010
//#define PRODUCT_WEB_SERVER                      0x00000011
//#define PRODUCT_CLUSTER_SERVER                  0x00000012
//#define PRODUCT_HOME_SERVER                     0x00000013
//#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
//#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
//#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
//#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
//#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
//#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019

//#define PRODUCT_UNLICENSED                      0xABCDABCD

#define VER_SERVER_NT                       0x80000000
#define VER_WORKSTATION_NT                  0x40000000
//#define VER_SUITE_SMALLBUSINESS             0x00000001
//#define VER_SUITE_ENTERPRISE                0x00000002
//#define VER_SUITE_BACKOFFICE                0x00000004
#define VER_SUITE_COMMUNICATIONS            0x00000008
//#define VER_SUITE_TERMINAL                  0x00000010
//#define VER_SUITE_SMALLBUSINESS_RESTRICTED  0x00000020
//#define VER_SUITE_EMBEDDEDNT                0x00000040
//#define VER_SUITE_DATACENTER                0x00000080
//#define VER_SUITE_SINGLEUSERTS              0x00000100
//#define VER_SUITE_PERSONAL                  0x00000200
//#define VER_SUITE_BLADE                     0x00000400
#define VER_SUITE_EMBEDDED_RESTRICTED       0x00000800
#define VER_SUITE_SECURITY_APPLIANCE        0x00001000
//#define VER_SUITE_STORAGE_SERVER            0x00002000
//#define VER_SUITE_COMPUTE_SERVER            0x00004000
#define VER_SUITE_WH_SERVER                 0x00008000

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
#endif

#endif // WINDOWINFOUSERDEF_H
