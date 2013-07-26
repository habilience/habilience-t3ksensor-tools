#include "QTouchSettingDialog.h"
#include "ui_QTouchSettingDialog.h"
#include "dialog.h"
#include <QPainter>
#include <QCloseEvent>
#include "QT3kDevice.h"

QTouchSettingDialog::QTouchSettingDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QTouchSettingDialog)
{
    ui->setupUi(this);
    QT3kDevice* pDevice = QT3kDevice::instance();

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
    if (!pDevice->isVirtualDevice())
    {
#if defined(Q_OS_WIN)
        flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
        flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose);
}

QTouchSettingDialog::~QTouchSettingDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

bool QTouchSettingDialog::canClose()
{
    return false;
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

void QTouchSettingDialog::closeEvent(QCloseEvent *evt)
{
    if (!canClose())
        evt->ignore();
}

void QTouchSettingDialog::reject()
{
    close();
}

void QTouchSettingDialog::accept()
{
    close();
}
