#include "QBentAdjustmentDialog.h"
#include "ui_QBentAdjustmentDialog.h"
#include "dialog.h"
#include <QPainter>

QBentAdjustmentDialog::QBentAdjustmentDialog(Dialog *parent) :
    QDialog(parent),
    m_pMainDlg(parent),
    ui(new Ui::QBentAdjustmentDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    setFixedSize(this->size());
    setAttribute(Qt::WA_DeleteOnClose);
}

QBentAdjustmentDialog::~QBentAdjustmentDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

void QBentAdjustmentDialog::on_btnClose_clicked()
{
    close();
}

void QBentAdjustmentDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);
    p.fillRect( rcBody, Qt::white );
}
