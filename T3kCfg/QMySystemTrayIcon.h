#ifndef QMYSYSTEMTRAYICON_H
#define QMYSYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class QMySystemTrayIcon : public QSystemTrayIcon
{
public:
    QMySystemTrayIcon(QObject *parent = 0);
    QMySystemTrayIcon(const QIcon &icon, QObject *parent = 0);

    void UserActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // QMYSYSTEMTRAYICON_H
