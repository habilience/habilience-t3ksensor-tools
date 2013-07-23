#include "QDetectionGraphForm.h"
#include "ui_QDetectionGraphForm.h"

QDetectionGraphForm::QDetectionGraphForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDetectionGraphForm)
{
    ui->setupUi(this);
}

QDetectionGraphForm::~QDetectionGraphForm()
{
    delete ui;
}
