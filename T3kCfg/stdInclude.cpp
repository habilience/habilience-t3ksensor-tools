#include "stdInclude.h"
#include <QObject>
#include <QPushButton>

void genAdjustButtonWidgetForWinAndX11(QWidget *rootWidget)
{
    if (rootWidget == NULL)
        return;

    QObject *child = NULL;
    QObjectList Containers;
    QObject *container  = NULL;
    QStringList DoNotAffect;

    const int ButtonHeight = 30;        // OSX: 34

    // Make an exception list (Objects not to be affected)
    DoNotAffect.append("aboutTitleLabel");     // about Dialog
    DoNotAffect.append("aboutVersionLabel");   // about Dialog
    DoNotAffect.append("aboutCopyrightLabel"); // about Dialog
    DoNotAffect.append("aboutUrlLabel");       // about Dialog
    DoNotAffect.append("aboutLicenseLabel");   // about Dialog

    // Append root to Containers
    Containers.append(rootWidget);
    while (!Containers.isEmpty())
    {
        container = Containers.takeFirst();
        if (container != NULL)
        {
            for (int ChIdx=0; ChIdx < container->children().size(); ChIdx++)
            {
                child = container->children()[ChIdx];
                if (!child->isWidgetType() || DoNotAffect.contains(child->objectName()))
                    continue;
                // Append containers to Stack for recursion
                if (child->children().size() > 0)
                    Containers.append(child);
                else
                {
                    QPushButton *button = qobject_cast<QPushButton *>(child);
                    if (button != NULL)
                    {
                        button->setMinimumHeight(ButtonHeight); // Win
                        button->setMaximumHeight(ButtonHeight); // Win
                        button->setSizePolicy(QSizePolicy::Fixed,
                                              button->sizePolicy().horizontalPolicy());
                    }
                }
            }
        }
    }
}
