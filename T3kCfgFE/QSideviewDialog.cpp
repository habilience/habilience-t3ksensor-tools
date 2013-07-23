#include "QSideviewDialog.h"
#include "ui_QSideviewDialog.h"
#include <QPainter>
#include "dialog.h"

QSideviewDialog::QSideviewDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QSideviewDialog)
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

QSideviewDialog::~QSideviewDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

void QSideviewDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);
    p.fillRect( rcBody, Qt::white );
}

void QSideviewDialog::on_btnClose_clicked()
{
    close();
}
