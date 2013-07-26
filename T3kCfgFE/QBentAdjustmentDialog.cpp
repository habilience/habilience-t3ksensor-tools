#include "QBentAdjustmentDialog.h"
#include "ui_QBentAdjustmentDialog.h"
#include "dialog.h"
#include <QPainter>
#include <QCloseEvent>

#include "../common/QUtils.h"
#include "QT3kDevice.h"

QBentAdjustmentDialog::QBentAdjustmentDialog(Dialog *parent) :
    QDialog(parent),
    m_pMainDlg(parent),
    ui(new Ui::QBentAdjustmentDialog)
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

    DISABLE_MSWINDOWS_TOUCH_PROPERTY
}

QBentAdjustmentDialog::~QBentAdjustmentDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

bool QBentAdjustmentDialog::canClose()
{
    return false;
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

void QBentAdjustmentDialog::closeEvent(QCloseEvent *evt)
{
    if (!canClose())
        evt->ignore();
}

void QBentAdjustmentDialog::reject()
{
    close();
}

void QBentAdjustmentDialog::accept()
{
    close();
}

