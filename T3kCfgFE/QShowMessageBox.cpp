#include "QShowMessageBox.h"
#include "QLogSystem.h"
#include <QString>
#include <QApplication>
#include "QLangManager.h"
#include <QFont>

static bool s_bShowMessageBox = false;
static QMessageBox* s_pShowedMessageBox = NULL;

int showMessageBox( QWidget* parent, const QString& strMessage, const QString& strTitle,
                    QMessageBox::Icon icon, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defButton )
{
    QString strLog = strMessage;
    strLog.replace( "\r", " " );
    strLog.replace( "\n", " " );
    LOG_I( strLog );

    QString strStyle =
        "QWidget {"
            "background-color: rgb(225, 225, 225);"
        "}"
        "QPushButton {"
            "border: 1px solid #8f8f91;"
            "border-radius: 6px;"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
            "min-width: 80px;"
        "}"

        "QPushButton:pressed {"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);"
        "}"

        "QPushButton:flat {"
            "border: none;"
        "}"

        "QPushButton:focus {"
            "outline: none;"
        "}"

        "QPushButton:default {"
            "border-color: rgb(90, 129, 129);"
        "}";

    s_bShowMessageBox = true;
    QMessageBox msgBox(icon, strTitle, strMessage, buttons, parent);
    msgBox.setDefaultButton(defButton);
    //msgBox.setStyleSheet(strStyle);
#define MAIN "MAIN"
    QLangRes& res = QLangManager::getResource();
    msgBox.setButtonText(QMessageBox::Ok, res.getResString(MAIN, "BTN_CAPTION_OK"));
    msgBox.setButtonText(QMessageBox::Cancel, res.getResString(MAIN, "BTN_CAPTION_CANCEL"));
    msgBox.setButtonText(QMessageBox::Yes, res.getResString(MAIN, "BTN_CAPTION_YES"));
    msgBox.setButtonText(QMessageBox::No, res.getResString(MAIN, "BTN_CAPTION_NO"));

    QFont fnt(msgBox.font());
    fnt.setPointSize(fnt.pointSize()+2);
    msgBox.setFont(fnt);
    s_pShowedMessageBox = &msgBox;
    int nRet = msgBox.exec();

    s_pShowedMessageBox = NULL;
    s_bShowMessageBox = false;

    switch (nRet)
    {
    case QMessageBox::Yes:
        LOG_B( "Yes" );
        break;
    case QMessageBox::No:
        LOG_B( "No" );
        break;
    case QMessageBox::Ok:
        LOG_B( "OK" );
        break;
    case QMessageBox::Cancel:
        LOG_B( "Cancel" );
        break;
    }
    return nRet;
}

bool isShownMessageBox()
{
    return s_bShowMessageBox;
}

QMessageBox* getShownMessageBox()
{
    return s_pShowedMessageBox;
}
