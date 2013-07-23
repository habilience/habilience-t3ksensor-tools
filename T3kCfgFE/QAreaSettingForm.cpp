#include "QAreaSettingForm.h"
#include "ui_QAreaSettingForm.h"

QAreaSettingForm::QAreaSettingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QAreaSettingForm)
{
    ui->setupUi(this);
}

QAreaSettingForm::~QAreaSettingForm()
{
    delete ui;
}
