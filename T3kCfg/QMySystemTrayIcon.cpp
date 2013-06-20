#include "QMySystemTrayIcon.h"

QMySystemTrayIcon::QMySystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
{
}

QMySystemTrayIcon::QMySystemTrayIcon(const QIcon &icon, QObject *parent)
    : QSystemTrayIcon(icon, parent)
{
    setIcon(icon);
}

void QMySystemTrayIcon::UserActivated(QSystemTrayIcon::ActivationReason reason)
{
    emit activated(reason);
}
