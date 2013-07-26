#include "dialog.h"
#include "QMyApplication.h"
#include "AppData.h"

AppData g_AppData;
QMyApplication* g_pApp = NULL;

int main(int argc, char *argv[])
{
    g_AppData.bMaximizeToVirtualScreen = false;
    g_AppData.bScreenShotMode = false;

    if (argc > 1)
    {
        for (int c = 1; c < argc; c++)
        {
            if (strcmpi( argv[c], "/screenshot" ) == 0)
                g_AppData.bScreenShotMode = true;
            if (strcmpi( argv[c], "/virtualscreen" ) == 0)
                g_AppData.bMaximizeToVirtualScreen = true;
        }
    }

    QMyApplication a(argc, argv);
    g_pApp = &a;
    Dialog w;
    w.show();
    
    return a.exec();
}
