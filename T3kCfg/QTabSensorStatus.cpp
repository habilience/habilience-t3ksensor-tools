#include "QTabSensorStatus.h"
#include "ui_QTabSensorStatus.h"

#include "QSaveLogWidget.h"

QTabSensorStatus::QTabSensorStatus(T30xHandle *&pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTabSensorStatus), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );
    setFixedSize( width(), height() );

    OnChangeLanguage();
}

QTabSensorStatus::~QTabSensorStatus()
{
    delete ui;
}

void QTabSensorStatus::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->TitleSensorStatus->setText( Res.GetResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TITLE_SENSOR_STATUS")) );

    ui->LBInfoMessage->setText( Res.GetResString(QString::fromUtf8("INFORMATION GATHERING"), QString::fromUtf8("TEXT_MESSAGE")) );
    ui->BtnInfoSave->setText( Res.GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_CAPTION_SAVELOG")) );
}

void QTabSensorStatus::on_BtnInfoSave_clicked()
{
    QSaveLogWidget widget( m_pT3kHandle, this );
    widget.exec();
}
