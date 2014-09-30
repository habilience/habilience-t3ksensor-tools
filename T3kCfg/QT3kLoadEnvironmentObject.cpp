#include "QT3kLoadEnvironmentObject.h"

#include <QCoreApplication>
#include <QDesktopWidget>

#include "T3kConstStr.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <winsvc.h>
#include <shlobj.h>
#define SVC_TABLETINPUT         "TabletInputService"

#include "WindowInfoUserDef.h"
#include "HIDStateUserDef.h"
#include <setupapi.h>

#include "../common/t3kcomdef.h"
#endif

#ifndef Q_OS_WIN
#include <sys/utsname.h>
#include <stdio.h>
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

#include "ReportConstStr.h"

QT3kLoadEnvironmentObject::QT3kLoadEnvironmentObject(QObject *parent) :
    QObject(parent)
{
}

bool QT3kLoadEnvironmentObject::Start(SensorLogData *pStorage)
{
    if( !pStorage ) return false;

    PairRSP stRSP;

    stRSP.strKey = cstrProgramVersion;
    stRSP.strData = QCoreApplication::applicationVersion();
    pStorage->Env.push_back( stRSP );

    stRSP.strKey = cstrOperationSystem;
    stRSP.strData = QT3kLoadEnvironmentObject::GetOSDisplayString();

    pStorage->Env.push_back( stRSP );

    QDesktopWidget widget;
    stRSP.strKey = cstrPrimaryDisplay;
    stRSP.strData = QString("%1").arg( widget.primaryScreen() );
    pStorage->Env.push_back( stRSP );

    int nScreenCount = widget.screenCount();
    for( int i=0; i<nScreenCount; i++ )
    {
        QRect rcScreen( widget.screenGeometry(i) );
        stRSP.strKey = QString("%1%2").arg(cstrDisplay).arg( i );
        stRSP.strData = QString("%1,%2,%3,%4").
                        arg(rcScreen.left()).arg(rcScreen.top()).arg(rcScreen.width()).arg(rcScreen.height());
        pStorage->Env.push_back( stRSP );
    }

#ifdef Q_OS_WIN
    stRSP.strKey = cstrTabletInputService;
    stRSP.strData = QString("%1").arg( QT3kLoadEnvironmentObject::IsServiceRunning( SVC_TABLETINPUT ) );
    pStorage->Env.push_back( stRSP );

    QVector<PairRSP> vHIDState = QT3kLoadEnvironmentObject::GetHIDState();
    foreach( PairRSP stEnv, vHIDState )
        pStorage->Env.push_back( stEnv );
#endif

    return true;
}

QString QT3kLoadEnvironmentObject::GetOSDisplayString()
{
#if defined(Q_OS_WIN)
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   PGNSI pGNSI;
   PGPI pGPI;
   bool bOsVersionInfoEx;
   DWORD dwType;

   QString strOS;

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

   if( !bOsVersionInfoEx ) return "";

   pGNSI = (PGNSI) GetProcAddress(
      GetModuleHandle(TEXT("kernel32.dll")),
      "GetNativeSystemInfo");
   if(NULL != pGNSI)
      pGNSI(&si);
   else GetSystemInfo(&si);

   if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId &&
        osvi.dwMajorVersion > 4 )
   {
      strOS = "Microsoft ";

      if ( osvi.dwMajorVersion == 6 )
      {
         if( osvi.dwMinorVersion == 0 )
         {
            if( osvi.wProductType == VER_NT_WORKSTATION )
                strOS += "Windows Vista ";
            else
                strOS += "Windows Server 2008 ";
         }

         if ( osvi.dwMinorVersion == 1 )
         {
            if( osvi.wProductType == VER_NT_WORKSTATION )
                strOS += "Windows 7 ";
            else
                strOS += "Windows Server 2008 R2 ";
         }

         if( osvi.dwMinorVersion == 2 )
         {
             if( osvi.wProductType == VER_NT_WORKSTATION )
                 strOS += "Winwos 8 ";
             else
                 strOS += "Windows 8 Server ";
         }
         // build number
         // 8102 - windows 8 developer preview
         // 8250 = windows 8 consumer preview
         // 8400 - windows 8 release preview

         pGPI = (PGPI) GetProcAddress(
            GetModuleHandle(TEXT("kernel32.dll")),
            "GetProductInfo");

         pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

         switch( dwType )
         {
            case PRODUCT_ULTIMATE:
               strOS += "Ultimate Edition";
               break;
            case PRODUCT_PROFESSIONAL:
               strOS += "Professional";
               break;
            case PRODUCT_HOME_PREMIUM:
               strOS += "Home Premium Edition";
               break;
            case PRODUCT_HOME_BASIC:
               strOS += "Home Basic Edition";
               break;
            case PRODUCT_ENTERPRISE:
               strOS += "Enterprise Edition";
               break;
            case PRODUCT_BUSINESS:
               strOS += "Business Edition";
               break;
            case PRODUCT_STARTER:
               strOS += "Starter Edition";
               break;
            case PRODUCT_CLUSTER_SERVER:
               strOS += "Cluster Server Edition";
               break;
            case PRODUCT_DATACENTER_SERVER:
               strOS += "Datacenter Edition";
               break;
            case PRODUCT_DATACENTER_SERVER_CORE:
               strOS += "Datacenter Edition (core installation)";
               break;
            case PRODUCT_ENTERPRISE_SERVER:
               strOS += "Enterprise Edition";
               break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
               strOS += "Enterprise Edition (core installation)";
               break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
               strOS += "Enterprise Edition for Itanium-based Systems";
               break;
            case PRODUCT_SMALLBUSINESS_SERVER:
               strOS += "Small Business Server";
               break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
               strOS += "Small Business Server Premium Edition";
               break;
            case PRODUCT_STANDARD_SERVER:
               strOS += "Standard Edition";
               break;
            case PRODUCT_STANDARD_SERVER_CORE:
               strOS += "Standard Edition (core installation)";
               break;
            case PRODUCT_WEB_SERVER:
               strOS += "Web Server Edition";
               break;
            default:
               strOS += QString("%1").arg(dwType);
               break;
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         if( GetSystemMetrics(SM_SERVERR2) )
            strOS +=  "Windows Server 2003 R2, ";
         else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
            strOS +=  "Windows Storage Server 2003";
         else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
            strOS +=  "Windows Home Server";
         else if( osvi.wProductType == VER_NT_WORKSTATION &&
                  si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
         {
            strOS +=  "Windows XP Professional x64 Edition";
         }
         else strOS += "Windows Server 2003, ";

         if ( osvi.wProductType != VER_NT_WORKSTATION )
         {
            if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   strOS +=  "Datacenter Edition for Itanium-based Systems";
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   strOS +=  "Enterprise Edition for Itanium-based Systems";
            }

            else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            {
                if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   strOS +=  "Datacenter x64 Edition";
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   strOS +=  "Enterprise x64 Edition";
                else strOS +=  "Standard x64 Edition";
            }

            else
            {
                if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                   strOS +=  "Compute Cluster Edition";
                else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                   strOS +=  "Datacenter Edition";
                else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                   strOS +=  "Enterprise Edition";
                else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                   strOS +=  "Web Edition";
                else strOS +=  "Standard Edition";
            }
         }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
      {
         strOS += "Windows XP ";
         if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
            strOS +=  "Home Edition";
         else strOS +=  "Professional";
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
      {
         strOS += "Windows 2000 ";

         if ( osvi.wProductType == VER_NT_WORKSTATION )
         {
            strOS +=  "Professional";
         }
         else
         {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
               strOS +=  "Datacenter Server";
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
               strOS +=  "Advanced Server";
            else strOS +=  "Server";
         }
      }

      QString strCSDVersion = QString::fromWCharArray( osvi.szCSDVersion );
      if( strCSDVersion.size() > 0 )
      {
          strOS += " ";
          strOS += strCSDVersion;
      }

      strOS += QString(" (build %1)").arg(osvi.dwBuildNumber);

      if ( osvi.dwMajorVersion >= 6 )
      {
         if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            strOS +=  ", 64-bit";
         else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
            strOS += ", 32-bit";
      }

      return strOS;
   }
   else
   {
      return "Not support this version of Windows.";
   }
#elif defined(Q_OS_LINUX)
   QString strOSVer;
   char szBuff[512];
   FILE *fp = ::popen( "cat /etc/issue", "r" );
   while( !::feof( fp ) )
   {
       ::memset( szBuff, 0, sizeof(char)*512 );
       if( ::fgets( szBuff, sizeof(szBuff), fp ) != NULL )
       {
           strOSVer = szBuff;
           break;
       }
   }
   ::pclose(fp);
   strOSVer = strOSVer.trimmed();
   int nPos = strOSVer.indexOf( ' ' );
   if( nPos > 0 )
   {
       nPos = strOSVer.indexOf( ' ', nPos+1 );
       if( nPos > 0 )
       {
           strOSVer = strOSVer.left( nPos );
       }
   }

   utsname stUname;
   ::memset( &stUname, 0, sizeof(utsname) );
   ::uname( &stUname );

   return QString("%1 %2 (Kernal : %3)").arg(strOSVer.trimmed()).arg(stUname.machine).arg(stUname.release);
#elif defined(Q_OS_MAC)
   SInt32 versionMajor = 0;
   SInt32 versionMinor = 0;
   SInt32 versionBugFix = 0;
   ::Gestalt( gestaltSystemVersionMajor, &versionMajor );
   ::Gestalt( gestaltSystemVersionMinor, &versionMinor );
   ::Gestalt( gestaltSystemVersionBugFix, &versionBugFix );

   utsname stUname;
   memset( &stUname, 0, sizeof(utsname) );
   ::uname( &stUname );

   return QString("Mac OS X : %1.%2.%3 %4 (Darwin Kernal : %5)").arg(versionMajor).arg(versionMinor).arg(versionBugFix).arg(stUname.machine).arg(stUname.release);
#endif
}

#ifdef Q_OS_WIN
bool QT3kLoadEnvironmentObject::IsServiceRunning( QString strServiceName )
{
    HKEY hKey;

    if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"system\\currentcontrolset\\services", 0, KEY_READ, &hKey) != ERROR_SUCCESS )
    {
       return false;
    }

    SC_HANDLE hScm = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
    if ( hScm == NULL )
    {
       return false;
    }

    LONG lResult = ERROR_SUCCESS;
    for ( int n=0; lResult==ERROR_SUCCESS; n++ )
    {
       wchar_t chSubKey[MAX_PATH];
       DWORD dwSize = MAX_PATH;
       FILETIME ftFileTime;
       lResult = ::RegEnumKeyEx(hKey, n, chSubKey, &dwSize, NULL, NULL, NULL, &ftFileTime);
       if ( lResult == ERROR_SUCCESS)
       {
           QString strSubKey = QString::fromWCharArray( chSubKey );

           QString strV = strServiceName;
           strSubKey = strSubKey.toLower();
           strV = strV.toLower();

           if ( strV != strSubKey ) continue;

           SC_HANDLE hSrv = ::OpenService(hScm, chSubKey, SERVICE_QUERY_STATUS);
           if ( hSrv == NULL )
           {
                continue;
           }

           SERVICE_STATUS_PROCESS srvStatusProgress;
           DWORD dwNeeded;
           bool bRet = ::QueryServiceStatusEx( hSrv, SC_STATUS_PROCESS_INFO, (LPBYTE)&srvStatusProgress, sizeof(srvStatusProgress), &dwNeeded );
           if ( bRet && (srvStatusProgress.dwCurrentState == SERVICE_RUNNING) )
           {
              ::CloseServiceHandle(hSrv);
              ::RegCloseKey(hKey);
              ::CloseServiceHandle(hScm);
              return true;
           }

           ::CloseServiceHandle(hSrv);
       }
    }

    ::RegCloseKey(hKey);
    ::CloseServiceHandle(hScm);

    return false;
}

QVector<PairRSP> QT3kLoadEnvironmentObject::GetHIDState()
{
    QVector<PairRSP>            Env;

    do
    {
        HDEVINFO hDevInfo;
        SP_DEVINFO_DATA DeviceInfoData;
        DWORD i;

        FnHidP_GetCaps          	HidP_GetCaps;
        FnHidD_GetAttributes        HidD_GetAttributes;
        FnHidD_GetHidGuid           HidD_GetHidGuid;
        FnHidD_GetPreparsedData     HidD_GetPreparsedData;
        FnHidD_FreePreparsedData	HidD_FreePreparsedData;

        HMODULE hHIDLibrary = ::LoadLibrary( L"hid.dll" );
        if( !hHIDLibrary ) break;

        HidP_GetCaps                = (FnHidP_GetCaps) ::GetProcAddress( hHIDLibrary, "HidP_GetCaps" );
        HidD_GetAttributes		= (FnHidD_GetAttributes) ::GetProcAddress( hHIDLibrary, "HidD_GetAttributes" );
        HidD_GetHidGuid             = (FnHidD_GetHidGuid) ::GetProcAddress( hHIDLibrary, "HidD_GetHidGuid" );
        HidD_GetPreparsedData	= (FnHidD_GetPreparsedData) ::GetProcAddress( hHIDLibrary, "HidD_GetPreparsedData" );
        HidD_FreePreparsedData	= (FnHidD_FreePreparsedData) ::GetProcAddress( hHIDLibrary, "HidD_FreePreparsedData" );

        if ( !HidP_GetCaps || !HidD_GetAttributes || !HidD_GetHidGuid || !HidD_GetPreparsedData || !HidD_FreePreparsedData )
        {
            ::FreeLibrary( hHIDLibrary );
            break;
        }

        hDevInfo = ::SetupDiGetClassDevs( NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE );

        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        typedef struct _HIDState
        {
            int                 nMouse;
            int                 nVendorDefined;
            int                 nKeyboard;
            int                 nConsumerControl;
            int                 nDigitizer;
        } HIDState;

        HIDState stHIDState;
        ::memset( &stHIDState, -1, sizeof(HIDState) );

        GUID hidGuid;
        HidD_GetHidGuid( &hidGuid );
        for( i=0 ; SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData) ; i++ )
        {
            SP_DEVICE_INTERFACE_DATA            deviceInferfaceData;
            deviceInferfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if( ::SetupDiEnumDeviceInterfaces( hDevInfo,
                                               0, // No care about specific PDOs
                                               &hidGuid,
                                               i,
                                               &deviceInferfaceData) )
            {
                ULONG requiredLength;
                bool bRet = ::SetupDiGetDeviceInterfaceDetail( hDevInfo, &deviceInferfaceData, NULL, 0, &requiredLength, NULL );

                if( !bRet && ::GetLastError() != ERROR_INSUFFICIENT_BUFFER )
                {
                    continue;
                }

                ULONG predictedLength = requiredLength;

                PSP_DEVICE_INTERFACE_DETAIL_DATA functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::malloc( predictedLength );

                if( functionClassDeviceData )
                {
                    functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                    ::ZeroMemory(functionClassDeviceData->DevicePath, sizeof(functionClassDeviceData->DevicePath));
                }
                else
                {
                    ::free(functionClassDeviceData);
                    continue;
                }

                if( !::SetupDiGetDeviceInterfaceDetail( hDevInfo, &deviceInferfaceData, functionClassDeviceData, predictedLength, &requiredLength, NULL) )
                {
                    ::free(functionClassDeviceData);
                    continue;
                }

                HANDLE hHID = ::CreateFile( functionClassDeviceData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
                if( hHID != INVALID_HANDLE_VALUE )
                {
                    PHIDP_PREPARSED_DATA Ppd;
                    HIDP_CAPS            Caps;
                    HIDD_ATTRIBUTES      Attributes;

                    if( HidD_GetPreparsedData( hHID, &Ppd ) )
                    {
                        if( HidD_GetAttributes( hHID, &Attributes ) )
                        {
                            if( HidP_GetCaps( Ppd, &Caps ) )
                            {
                                bool bChecked = false;
                                for ( int d = 0 ; d<COUNT_OF_DEVICE_LIST(BASE_DEVICE_LIST) ; d++)
                                {
                                    if( (Attributes.ProductID == BASE_DEVICE_LIST[d].nPID) &&
                                            (Attributes.VendorID == BASE_DEVICE_LIST[d].nVID) )
                                    {
                                        bChecked = true;
                                        break;
                                    }
                                }

                                if( bChecked )
                                {
                                    bool bFind = false;
                                    DWORD dwDeviceUsage = ((DWORD)Caps.UsagePage<<16) + Caps.Usage;
                                    switch ( dwDeviceUsage )
                                    {
                                    case 0x00010002:// HID Mouse
                                    case 0xFF000001:// HID Vendor-defined
                                    case 0x00FF0001:
                                    case 0x008C0001:
                                    case 0x00010006:// HID Keyboard
                                    case 0x000c0001:// HID Consumer Control
                                    case 0x000d0004:// HID Digitizer
                                    case 0x000d000e:// HID Touch Configuration
                                        bFind = true;
                                        break;
                                    }

                                    DWORD DataT;
                                    DWORD buffersize = 0;
                                    DWORD dwStatus;

                                    bool bState = false;
                                    if( ::SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_INSTALL_STATE, &DataT,  (PBYTE)&dwStatus, sizeof(DWORD), &buffersize) )
                                    {
                                        switch( dwStatus )
                                        {
                                        case InstallStateInstalled:
                                        case InstallStateFinishInstall:
                                            bState = true;
                                            break;
                                        case InstallStateNeedsReinstall:
                                        case InstallStateFailedInstall:
                                            bState = false;
                                            break;
                                        }
                                    }

                                    if( bFind )
                                    {
                                        switch( dwDeviceUsage )
                                        {
                                        case 0x00010002:// HID Mouse
                                            stHIDState.nMouse = bState ? 1 : 0;
                                            break;
                                        case 0x00FF0001:// HID Vendor-defined
                                        case 0xFF000001:
                                        case 0x008C0001:
                                            stHIDState.nVendorDefined = bState ? 1 : 0;
                                            break;
                                        case 0x00010006:// HID Keyboard
                                            stHIDState.nKeyboard = bState ? 1 : 0;
                                            break;
                                        case 0x000c0001:// HID Consumer Control
                                            stHIDState.nConsumerControl = bState ? 1 : 0;
                                            break;
                                        case 0x000d0004:// HID Digitizer
                                            stHIDState.nDigitizer = bState ? 1 : 0;
                                            break;
                                        default:
                                            break;
                                        }
                                    }
                                }
                            }

                            HidD_FreePreparsedData(Ppd);
                        }

                        ::CloseHandle( hHID );
                    }
                    else
                    {
                        qDebug( "GetHIDState : %ld", ::GetLastError() );
                    }

                    ::free(functionClassDeviceData);
                }
                else
                {
                    DWORD dwError = ::GetLastError();
                    if ( dwError == ERROR_NO_MORE_ITEMS )
                        break;
                }
            }
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);

        ::FreeLibrary( hHIDLibrary );

        PairRSP stData;
        stData.strKey = "HID Mouse";
        stData.strData = stHIDState.nMouse == -1 ? "Not Found" : stHIDState.nMouse == 1 ? "OK" : "NG";
        Env.push_back( stData );

        stData.strKey = "HID VendorDefined";
        stData.strData = stHIDState.nVendorDefined == -1 ? "Not Found" : stHIDState.nVendorDefined == 1 ? "OK" : "NG";
        Env.push_back( stData );

        stData.strKey = "HID Keyboard";
        stData.strData = stHIDState.nKeyboard == -1 ? "Not Found" : stHIDState.nKeyboard == 1 ? "OK" : "NG";
        Env.push_back( stData );

        stData.strKey = "HID ConsumerControl";
        stData.strData = stHIDState.nConsumerControl == -1 ? "Not Found" : stHIDState.nConsumerControl == 1 ? "OK" : "NG";
        Env.push_back( stData );

        stData.strKey = "HID Digitizer";
        stData.strData = stHIDState.nDigitizer == -1 ? "Not Found" : stHIDState.nDigitizer == 1 ? "OK" : "NG";
        Env.push_back( stData );
    }
    while( false );

    return Env;
}

typedef struct _CustomdevicemodeW {
    WCHAR  dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    union {
      /* printer only fields */
      struct {
        short dmOrientation;
        short dmPaperSize;
        short dmPaperLength;
        short dmPaperWidth;
        short dmScale;
        short dmCopies;
        short dmDefaultSource;
        short dmPrintQuality;
      };
      /* display only fields */
      struct {
        POINTL dmPosition;
        DWORD  dmDisplayOrientation;
        DWORD  dmDisplayFixedOutput;
      };
    };
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    WCHAR  dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    union {
        DWORD  dmDisplayFlags;
        DWORD  dmNup;
    };
    DWORD  dmDisplayFrequency;
#if(WINVER >= 0x0400)
    DWORD  dmICMMethod;
    DWORD  dmICMIntent;
    DWORD  dmMediaType;
    DWORD  dmDitherType;
    DWORD  dmReserved1;
    DWORD  dmReserved2;
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= _WIN32_WINNT_NT4)
    DWORD  dmPanningWidth;
    DWORD  dmPanningHeight;
#endif
#endif /* WINVER >= 0x0400 */
} CT_DEVMODEW;

unsigned long QT3kLoadEnvironmentObject::GetScreenOrientation()
{
    CT_DEVMODEW dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    if( 0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, (PDEVMODEW)&dm) )
    {
        return (unsigned long)dm.dmDisplayOrientation;
    }

    return 0xFFFFFFFF;
}
#endif
