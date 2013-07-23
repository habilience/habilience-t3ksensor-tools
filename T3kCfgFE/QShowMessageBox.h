#ifndef QSHOWMESSAGEBOX_H
#define QSHOWMESSAGEBOX_H

#include <QMessageBox>

int showMessageBox( QWidget* parent, const QString& strMessage, const QString& strTitle,
                    QMessageBox::Icon icon, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defButton );

bool isShownMessageBox();
QMessageBox* getShownMessageBox();

#endif // QSHOWMESSAGEBOX_H
