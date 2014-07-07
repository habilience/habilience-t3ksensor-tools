/********************************************************************************
** Form generated from reading UI file 'QFirmwareUpgradeDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QFIRMWAREUPGRADEDIALOG_H
#define UI_QFIRMWAREUPGRADEDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QSlidingStackedWidget.h"

QT_BEGIN_NAMESPACE

class Ui_QFirmwareUpgradeDialog
{
public:
    QVBoxLayout *layoutBody;
    QSlidingStackedWidget *stackedWidget;
    QWidget *pageProgress;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *layoutProgress;
    QLabel *labelPart;
    QProgressBar *progressBar;
    QLabel *labelMessage;

    void setupUi(QDialog *QFirmwareUpgradeDialog)
    {
        if (QFirmwareUpgradeDialog->objectName().isEmpty())
            QFirmwareUpgradeDialog->setObjectName(QStringLiteral("QFirmwareUpgradeDialog"));
        QFirmwareUpgradeDialog->resize(400, 110);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QFirmwareUpgradeDialog->sizePolicy().hasHeightForWidth());
        QFirmwareUpgradeDialog->setSizePolicy(sizePolicy);
        QFirmwareUpgradeDialog->setMinimumSize(QSize(400, 110));
        QFirmwareUpgradeDialog->setMaximumSize(QSize(400, 110));
        QFirmwareUpgradeDialog->setAutoFillBackground(false);
        QFirmwareUpgradeDialog->setStyleSheet(QLatin1String("background-color: rgb(100, 100, 100);\n"
"border-color: rgb(100, 100, 100);\n"
"selection-background-color: rgba(100, 100, 100, 100);"));
        QFirmwareUpgradeDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        QFirmwareUpgradeDialog->setSizeGripEnabled(false);
        layoutBody = new QVBoxLayout(QFirmwareUpgradeDialog);
        layoutBody->setSpacing(6);
        layoutBody->setContentsMargins(11, 11, 11, 11);
        layoutBody->setObjectName(QStringLiteral("layoutBody"));
        layoutBody->setSizeConstraint(QLayout::SetNoConstraint);
        layoutBody->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QSlidingStackedWidget(QFirmwareUpgradeDialog);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setMaximumSize(QSize(400, 110));
        stackedWidget->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 240);"));
        stackedWidget->setFrameShape(QFrame::NoFrame);
        stackedWidget->setFrameShadow(QFrame::Plain);
        pageProgress = new QWidget();
        pageProgress->setObjectName(QStringLiteral("pageProgress"));
        verticalLayout_3 = new QVBoxLayout(pageProgress);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        layoutProgress = new QHBoxLayout();
        layoutProgress->setSpacing(6);
        layoutProgress->setObjectName(QStringLiteral("layoutProgress"));
        labelPart = new QLabel(pageProgress);
        labelPart->setObjectName(QStringLiteral("labelPart"));
        labelPart->setMinimumSize(QSize(50, 0));
        labelPart->setStyleSheet(QLatin1String("QLabel {\n"
"	color : #395151;\n"
"	font-weight: bold;\n"
"}"));

        layoutProgress->addWidget(labelPart);

        progressBar = new QProgressBar(pageProgress);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setMinimumSize(QSize(0, 30));
        progressBar->setStyleSheet(QLatin1String("QProgressBar {\n"
"     border: 2px solid #8f8f91;\n"
"     border-radius: 5px;\n"
" }\n"
"\n"
" QProgressBar::chunk {\n"
"     background-color: rgb(150,182,182);\n"
"     width: 1px;\n"
" }"));
        progressBar->setValue(50);
        progressBar->setAlignment(Qt::AlignCenter);
        progressBar->setTextVisible(true);
        progressBar->setInvertedAppearance(false);

        layoutProgress->addWidget(progressBar);


        verticalLayout_3->addLayout(layoutProgress);

        labelMessage = new QLabel(pageProgress);
        labelMessage->setObjectName(QStringLiteral("labelMessage"));
        labelMessage->setMinimumSize(QSize(0, 31));
        labelMessage->setStyleSheet(QStringLiteral("color: rgb(203, 45, 5); font-weight: bold;"));
        labelMessage->setWordWrap(true);

        verticalLayout_3->addWidget(labelMessage);

        stackedWidget->addWidget(pageProgress);

        layoutBody->addWidget(stackedWidget);


        retranslateUi(QFirmwareUpgradeDialog);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QFirmwareUpgradeDialog);
    } // setupUi

    void retranslateUi(QDialog *QFirmwareUpgradeDialog)
    {
        QFirmwareUpgradeDialog->setWindowTitle(QApplication::translate("QFirmwareUpgradeDialog", "T3k Firmware Upgrader", 0));
        labelPart->setText(QApplication::translate("QFirmwareUpgradeDialog", "CM2-1", 0));
        labelMessage->setText(QApplication::translate("QFirmwareUpgradeDialog", "CAUTION: Do not unplug the device until the process is completed.", 0));
    } // retranslateUi

};

namespace Ui {
    class QFirmwareUpgradeDialog: public Ui_QFirmwareUpgradeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QFIRMWAREUPGRADEDIALOG_H
