#include "dialog.h"
#include <QApplication>
#include <QSharedMemory>

const static QString UniqueID = "T3kUpgrade@Habilience";

int main(int argc, char *argv[])
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey(UniqueID);

    bool bIsRunning = false;
    if (sharedMemory.attach())
    {
        bIsRunning = true;
    }

    if (!bIsRunning &&!sharedMemory.create(1))
    {
        bIsRunning = true;
    }

    if (bIsRunning)
    {
        return 0; // Exit already a process running
    }
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    
    return a.exec();
}
