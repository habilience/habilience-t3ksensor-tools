#include "QTouchSettingDialog.h"
#include "ui_QTouchSettingDialog.h"
#include "dialog.h"
#include <QPainter>

QTouchSettingDialog::QTouchSettingDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QTouchSettingDialog)
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

QTouchSettingDialog::~QTouchSettingDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

void QTouchSettingDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);
    p.fillRect( rcBody, Qt::white );
}

void QTouchSettingDialog::on_btnClose_clicked()
{
    close();
}
