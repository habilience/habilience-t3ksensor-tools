#ifndef USERWINDOWDEF_H
#define USERWINDOWDEF_H

#ifdef Q_OS_WIN
#define SPDRP_INSTALL_STATE               (0x00000022)  // Device Install State (R)

typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;
typedef USHORT USAGE, *PUSAGE;
typedef struct _HIDP_CAPS
{
    USAGE    Usage;
    USAGE    UsagePage;
    USHORT   InputReportByteLength;
    USHORT   OutputReportByteLength;
    USHORT   FeatureReportByteLength;
    USHORT   Reserved[17];

    USHORT   NumberLinkCollectionNodes;

    USHORT   NumberInputButtonCaps;
    USHORT   NumberInputValueCaps;
    USHORT   NumberInputDataIndices;

    USHORT   NumberOutputButtonCaps;
    USHORT   NumberOutputValueCaps;
    USHORT   NumberOutputDataIndices;

    USHORT   NumberFeatureButtonCaps;
    USHORT   NumberFeatureValueCaps;
    USHORT   NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;
typedef struct _HIDD_ATTRIBUTES {
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)

    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;
typedef LONG NTSTATUS;
typedef NTSTATUS (__stdcall * FnHidP_GetCaps) ( IN PHIDP_PREPARSED_DATA PreparsedData, OUT PHIDP_CAPS Capabilities );
typedef BOOLEAN (__stdcall * FnHidD_GetAttributes) ( IN  HANDLE HidDeviceObject, OUT PHIDD_ATTRIBUTES Attributes );
typedef void (__stdcall * FnHidD_GetHidGuid) ( OUT LPGUID HidGuid );
typedef BOOLEAN (__stdcall * FnHidD_GetPreparsedData) ( IN HANDLE HidDeviceObject, OUT PHIDP_PREPARSED_DATA* PreparsedData );
typedef BOOLEAN (__stdcall * FnHidD_FreePreparsedData) ( IN PHIDP_PREPARSED_DATA PreparsedData );

typedef enum _DEVICE_INSTALL_STATE {
  InstallStateInstalled        = 0,
  InstallStateNeedsReinstall   = 1,
  InstallStateFailedInstall    = 2,
  InstallStateFinishInstall    = 3
} DEVICE_INSTALL_STATE, *PDEVICE_INSTALL_STATE;
#endif

#endif // USERWINDOWDEF_H
