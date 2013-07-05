#include "QBriefingDialog.h"
#include "ui_briefing_dialog.h"

QBriefingDialog::QBriefingDialog(QString& strDetailHTML, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QBriefingDialog),
    m_strDetailHTML(strDetailHTML)
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

    ui->textEditDetailInformation->setHtml( m_strDetailHTML );
}

QBriefingDialog::~QBriefingDialog()
{
    delete ui;
}

void QBriefingDialog::on_pushButtonOK_clicked()
{
    accept();
}

void QBriefingDialog::on_pushButtonCancel_clicked()
{
    reject();
}
