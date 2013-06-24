#include "dialog.h"
#include "ui_dialog.h"

#include <QPropertyAnimation>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    setWindowIcon( QIcon(":/T3kUpgradeRes/resources/T3kUpgrade.png") );

    ui->stackedWidget->setCurrentIndex(0);

    QObject::connect(ui->pushButtonUpgrade,SIGNAL(clicked()),ui->stackedWidget,SLOT(slideInNext()));
    QObject::connect(ui->pushButtonCancel,SIGNAL(clicked()),ui->stackedWidget,SLOT(slideInPrev()));
}

Dialog::~Dialog()
{
    delete ui;
}

