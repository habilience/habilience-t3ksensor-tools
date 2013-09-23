#include "QDataProgressDialog.h"
#include "ui_QDataProgressDialog.h"

#include "QLogSystem.h"

QDataProgressDialog::QDataProgressDialog(QT3kDevice *pDevice, QWidget *parent, bool bSetSensorCfg) :
    QDialog(parent),
    m_pDevice(pDevice),
    m_bSetSensorCfg(bSetSensorCfg),
    ui(new Ui::QDataProgressDialog)
{
    ui->setupUi(this);

    connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), this, SLOT(onFinish(bool,int)), Qt::QueuedConnection );
}

QDataProgressDialog::~QDataProgressDialog()
{
    delete ui;
}

void QDataProgressDialog::showEvent(QShowEvent *)
{
    qDebug( "QDataProgressDialog::showEvent" );
    onChangeLanguage();

    ui->cmdAsyncMngr->setSensorCfgMode(m_bSetSensorCfg);
    ui->cmdAsyncMngr->setT3kDevice(m_pDevice);
    ui->cmdAsyncMngr->start(-1, true);
}

void QDataProgressDialog::on_btnCancel_clicked()
{
    LOG_B( "QDataProgressDialog::Cancel" );
    if (ui->cmdAsyncMngr->isStarted())
        ui->cmdAsyncMngr->stop();
    reject();
}

void QDataProgressDialog::insertCommand(const QString &strCmd)
{
    ui->cmdAsyncMngr->insertCommand(strCmd);
}

void QDataProgressDialog::resetCommands()
{
    ui->cmdAsyncMngr->resetCommands();
}

#define RES_TAG "DATA PROGRESS DIALOG"
#define MAIN_TAG "MAIN"
void QDataProgressDialog::onChangeLanguage()
{
    if (!isVisible())
        return;

    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection(Qt::RightToLeft);
    else
        setLayoutDirection(Qt::LeftToRight);

    setWindowTitle( res.getResString(RES_TAG, "TITLE") );

    ui->btnCancel->setText( res.getResString("MESSAGEBOX", "BTN_CAPTION_CANCEL") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QDataProgressDialog::onFinish(bool bResult, int /*nReason*/)
{
    if (bResult)
    {
        accept();
    }
    else
    {
        reject();
    }
}
