#include "QTabSensorStatus.h"
#include "ui_QTabSensorStatus.h"

#include "QSaveLogWidget.h"

#include <QCloseEvent>


QTabSensorStatus::QTabSensorStatus(QT3kDevice *&pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTabSensorStatus), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );
    setFixedSize( width(), height() );

    onChangeLanguage();
}

QTabSensorStatus::~QTabSensorStatus()
{
    delete ui;
}

void QTabSensorStatus::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->TitleSensorStatus->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TITLE_SENSOR_STATUS")) );

    ui->LBInfoMessage->setText( Res.getResString(QString::fromUtf8("INFORMATION GATHERING"), QString::fromUtf8("TEXT_MESSAGE")) );
    ui->BtnInfoSave->setText( Res.getResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_CAPTION_SAVELOG")) );
}

void QTabSensorStatus::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Escape )
    {
        evt->ignore();
        return;
    }

    QDialog::keyPressEvent(evt);
}

void QTabSensorStatus::on_BtnInfoSave_clicked()
{
    QSaveLogWidget widget( m_pT3kHandle );
    widget.exec();
}
