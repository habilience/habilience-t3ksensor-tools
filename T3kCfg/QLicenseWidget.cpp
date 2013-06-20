#include "QLicenseWidget.h"
#include "ui_QLicenseWidget.h"
#include <QFile>
#include <QCoreApplication>

QLicenseWidget::QLicenseWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QLicenseWidget)
{
    ui->setupUi(this);

    ui->TBLicenceHTML->setAcceptRichText( true );

    QString strLicense( QCoreApplication::applicationDirPath() );
    strLicense += "/License/License.html";
    QFile fLicenseHtml( strLicense );
    if( fLicenseHtml.open( QIODevice::ReadOnly ) )
    {
        QString str = fLicenseHtml.readAll();
        ui->TBLicenceHTML->setHtml( str );

        fLicenseHtml.close();
    }
    else
    {
        fLicenseHtml.setFileName( ":/T3kCfgRes/License/License.html" );

        if( fLicenseHtml.open( QIODevice::ReadOnly ) )
        {
            QString str = fLicenseHtml.readAll();
            ui->TBLicenceHTML->setHtml( str );

            fLicenseHtml.close();
        }
    }

    OnChangeLanguage();
}

QLicenseWidget::~QLicenseWidget()
{
    delete ui;
}

void QLicenseWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString(QString::fromUtf8("MAIN"), QString::fromUtf8("TITLE_CAPTION_LICENSE")) );
    ui->BTNOK->setText( Res.GetResString(QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_OK")) );
}

void QLicenseWidget::on_BTNOK_clicked()
{
    QDialog::accept();
}
