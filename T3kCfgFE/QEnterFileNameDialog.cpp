#include "QEnterFileNameDialog.h"
#include "ui_QEnterFileNameDialog.h"

QEnterFileNameDialog::QEnterFileNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEnterFileNameDialog)
{
    ui->setupUi(this);

    m_strFileName = "";
    ui->txtEditFileName->setFocus();
}

QEnterFileNameDialog::~QEnterFileNameDialog()
{
    delete ui;
}

void QEnterFileNameDialog::showEvent(QShowEvent *)
{
    onChangeLanguage();
}

#define RES_TAG     "ENTER FILENAME DIALOG"
#define MAIN_TAG    "MAIN"
void QEnterFileNameDialog::onChangeLanguage()
{
    if (!isVisible())
        return;

    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString( RES_TAG, "TITLE") );

    ui->lblFileName->setText( res.getResString( RES_TAG, "TEXT_FILENAME") );
    ui->txtEditFileName->setText( "" );
    ui->btnOK->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_OK") );
    ui->btnCancel->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_CANCEL") );

    if (s_bIsR2L != bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QEnterFileNameDialog::on_btnOK_clicked()
{
    m_strFileName = ui->txtEditFileName->toPlainText();
    accept();
}

void QEnterFileNameDialog::on_btnCancel_clicked()
{
    reject();
}
