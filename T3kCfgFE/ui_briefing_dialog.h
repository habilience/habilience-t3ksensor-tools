/********************************************************************************
** Form generated from reading UI file 'briefing_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BRIEFING_DIALOG_H
#define UI_BRIEFING_DIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QBriefingDialog
{
public:
    QVBoxLayout *layoutBody;
    QVBoxLayout *layoutFwUpgInfo;
    QLabel *labelFirmwareUpgradeInformation;
    QTableWidget *tableDetailInformation;
    QLabel *label;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;

    void setupUi(QDialog *QBriefingDialog)
    {
        if (QBriefingDialog->objectName().isEmpty())
            QBriefingDialog->setObjectName(QStringLiteral("QBriefingDialog"));
        QBriefingDialog->setWindowModality(Qt::ApplicationModal);
        QBriefingDialog->resize(370, 340);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QBriefingDialog->sizePolicy().hasHeightForWidth());
        QBriefingDialog->setSizePolicy(sizePolicy);
        QBriefingDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        QBriefingDialog->setModal(true);
        layoutBody = new QVBoxLayout(QBriefingDialog);
        layoutBody->setObjectName(QStringLiteral("layoutBody"));
        layoutFwUpgInfo = new QVBoxLayout();
        layoutFwUpgInfo->setSpacing(0);
        layoutFwUpgInfo->setObjectName(QStringLiteral("layoutFwUpgInfo"));
        labelFirmwareUpgradeInformation = new QLabel(QBriefingDialog);
        labelFirmwareUpgradeInformation->setObjectName(QStringLiteral("labelFirmwareUpgradeInformation"));
        labelFirmwareUpgradeInformation->setMinimumSize(QSize(0, 31));
        labelFirmwareUpgradeInformation->setStyleSheet(QLatin1String("QLabel {\n"
"	border-color: #82A6A6;\n"
"	background-color : #DDE7E7;\n"
"	color : #395151;\n"
"	font-weight: bold;\n"
"    border-style: solid;\n"
"    border-width: 1px;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"	padding-bottom: 2px;\n"
"}"));
        labelFirmwareUpgradeInformation->setTextFormat(Qt::AutoText);
        labelFirmwareUpgradeInformation->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        layoutFwUpgInfo->addWidget(labelFirmwareUpgradeInformation);

        tableDetailInformation = new QTableWidget(QBriefingDialog);
        if (tableDetailInformation->columnCount() < 2)
            tableDetailInformation->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableDetailInformation->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableDetailInformation->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableDetailInformation->setObjectName(QStringLiteral("tableDetailInformation"));
        tableDetailInformation->setFocusPolicy(Qt::NoFocus);
        tableDetailInformation->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
        tableDetailInformation->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableDetailInformation->setTabKeyNavigation(false);
        tableDetailInformation->setProperty("showDropIndicator", QVariant(false));
        tableDetailInformation->setDragDropOverwriteMode(false);
        tableDetailInformation->setSelectionMode(QAbstractItemView::NoSelection);
        tableDetailInformation->setShowGrid(false);
        tableDetailInformation->setCornerButtonEnabled(false);
        tableDetailInformation->horizontalHeader()->setVisible(false);
        tableDetailInformation->horizontalHeader()->setHighlightSections(false);
        tableDetailInformation->verticalHeader()->setVisible(false);
        tableDetailInformation->verticalHeader()->setDefaultSectionSize(20);

        layoutFwUpgInfo->addWidget(tableDetailInformation);


        layoutBody->addLayout(layoutFwUpgInfo);

        label = new QLabel(QBriefingDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 26));
        label->setStyleSheet(QStringLiteral("color: rgb(203, 45, 5); font-weight: bold;"));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        layoutBody->addWidget(label);

        line = new QFrame(QBriefingDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        layoutBody->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButtonOK = new QPushButton(QBriefingDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setMinimumSize(QSize(84, 31));
        pushButtonOK->setAutoDefault(false);

        horizontalLayout->addWidget(pushButtonOK);

        pushButtonCancel = new QPushButton(QBriefingDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setMinimumSize(QSize(84, 31));
        pushButtonCancel->setAutoDefault(false);

        horizontalLayout->addWidget(pushButtonCancel);


        layoutBody->addLayout(horizontalLayout);


        retranslateUi(QBriefingDialog);

        QMetaObject::connectSlotsByName(QBriefingDialog);
    } // setupUi

    void retranslateUi(QDialog *QBriefingDialog)
    {
        QBriefingDialog->setWindowTitle(QApplication::translate("QBriefingDialog", "Confirm", 0));
        labelFirmwareUpgradeInformation->setText(QApplication::translate("QBriefingDialog", "<html><head/><body><p><img src=\":/T3kUpgradeRes/resources/brief.png\"/> Firmware Upgrade Information</p></body></html>", 0));
        QTableWidgetItem *___qtablewidgetitem = tableDetailInformation->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("QBriefingDialog", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableDetailInformation->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("QBriefingDialog", "New Column", 0));
        label->setText(QApplication::translate("QBriefingDialog", "Do you want to upgrade?", 0));
        pushButtonOK->setText(QApplication::translate("QBriefingDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("QBriefingDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class QBriefingDialog: public Ui_QBriefingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BRIEFING_DIALOG_H
