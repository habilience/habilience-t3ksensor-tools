#ifndef T3KBENTDEF_H
#define T3KBENTDEF_H

//#define IDX_MM      (0)
//#define IDX_CM1     (1)
//#define IDX_CM2     (2)
//#define IDX_CM1_1   (3)
//#define IDX_CM2_1   (4)
//#define IDX_MAX     (5)
//#include <QString>

//const QString   sCam1("cam1/");
//const QString   sCam2("cam2/");
//const QString   sCam1_1("cam1/sub/");
//const QString   sCam2_1("cam2/sub/");
#include "T3KCamDef.h"

static const QString PartName[] = { "MM", "CM1", "CM2", "CM1-1", "CM2-1" };

inline int camIdxToIdx( int nCameraIndex )
{
    switch (nCameraIndex)
    {
    case IDX_CM1:
        return 0;
    case IDX_CM2:
        return 1;
    case IDX_CM1_1:
        return 2;
    case IDX_CM2_1:
        return 3;
    }
    return 0;
}

inline QString getCameraText( int nIndex )
{
    switch (nIndex)
    {
    case IDX_CM1:
        return "CM1";
    case IDX_CM2:
        return "CM2";
    case IDX_CM1_1:
        return "CM1-1";
    case IDX_CM2_1:
        return "CM2-1";
    }
    return "";
}

inline QString getCameraPrefix( int nIndex )
{
    switch (nIndex)
    {
    case IDX_CM1:
        return sCam1;
    case IDX_CM2:
        return sCam2;
    case IDX_CM1_1:
        return sCam1_1;
    case IDX_CM2_1:
        return sCam2_1;
    }
    return "";
}

inline int partNameToIdx( QString strPartName )
{
    for( int i=0; i<IDX_MAX; i++ )
    {
        if( PartName[i].compare( strPartName ) == 0 )
            return i;
    }

    return -1;
}

#endif // T3KBENTDEF_H
