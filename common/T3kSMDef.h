#ifndef T3KSMDEF_H
#define T3KSMDEF_H

#define T3K_SM_NAME     "Habilience T3k Series Configure"

typedef struct _T3K_SHAREDMEMORY
{
    char szRunningCFG;
    char szRunningFE;
    char szRunningSoftlogic;
    char szRunningUpgrade;

    char szNotifyCFG;
    char szNotifyFE;
    char szNotifySoftlogic;
    char szNotifyUpgrade;
} T3K_SHAREDMEMORY;

#endif // T3KSMDEF_H
