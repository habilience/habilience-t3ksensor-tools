#include "QLicenseWidget.h"
#include "ui_QLicenseWidget.h"
#include <QFile>
#include <QCoreApplication>

QLicenseWidget::QLicenseWidget(QString strHtml, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QLicenseWidget)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    setWindowModality( Qt::WindowModal );
    setModal( true );

    setFixedSize( width(), height() );

    ui->TBLicenceHTML->setAcceptRichText( true );

    QFile fLicenseHtml( strHtml );
    if( fLicenseHtml.open( QIODevice::ReadOnly ) )
    {
        QString str = fLicenseHtml.readAll();
        ui->TBLicenceHTML->setHtml( str );

        fLicenseHtml.close();
    }
    else
        close();
#ifndef NO_LANGUAGE
    onChangeLanguage();
#endif
}

QLicenseWidget::~QLicenseWidget()
{
    delete ui;
}
#ifndef NO_LANGUAGE
void QLicenseWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("MAIN"), QString::fromUtf8("TITLE_CAPTION_LICENSE")) );
    ui->BTNOK->setText( Res.getResString(QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_OK")) );
}
#endif
void QLicenseWidget::on_BTNOK_clicked()
{
    QDialog::accept();
}
