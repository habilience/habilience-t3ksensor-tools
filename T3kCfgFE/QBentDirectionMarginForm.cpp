#include "QBentDirectionMarginForm.h"
#include "ui_QBentDirectionMarginForm.h"

QBentDirectionMarginForm::QBentDirectionMarginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QBentDirectionMarginForm)
{
    ui->setupUi(this);
}

QBentDirectionMarginForm::~QBentDirectionMarginForm()
{
    delete ui;
}
