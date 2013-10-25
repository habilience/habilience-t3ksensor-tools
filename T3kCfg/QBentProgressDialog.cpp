#include "QBentProgressDialog.h"
#include "ui_QBentProgressDialog.h"

#include <QPainter>
#include <math.h>

#include "T3kCamNameDef.h"

#include "QBentAdjustment.h"

QBentProgressDialog::QBentProgressDialog(BentItem& item, QWidget *parent) :
    QDialog(parent),
    m_item(item),
    ui(new Ui::QBentProgressDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog;

#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif

    setWindowFlags(flags);
}

QBentProgressDialog::~QBentProgressDialog()
{
    delete ui;
}

void QBentProgressDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width(), height());

    p.save();

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255,210,210));

    if ( m_item.nCalcPosProg < 1000 )
    {
        p.drawPie( rcBody, 0, m_item.nCalcPosProg * 360 / 1000 * 16 );      // Qt 1/16th of a degree
    }

    p.setPen( QColor(120,120,120) );

    QString str;
    QRect rcInfo = rcBody;

    QFont fntCam(font());
    fntCam.setPixelSize(24);
    fntCam.setBold(QFont::Black);
    p.setFont(fntCam);
    rcInfo.setTop( rcInfo.top() + rcBody.height() / 6 );
    str = getCameraText( m_item.nCameraIndex );
    int flags = Qt::AlignCenter;
    p.drawText( rcInfo, flags, str );

    QFont fntInf(font());
    fntInf.setPixelSize(15);
    fntInf.setBold(QFont::Black);
    p.setFont(fntInf);
    rcInfo.setTop( rcInfo.top() + rcBody.height() / 3 );
    str = QString("using %1 points").arg(m_item.nCalcPosDataCnt);
    p.drawText( rcInfo, flags, str );

    p.restore();
}

void QBentProgressDialog::accept()
{

}

void QBentProgressDialog::reject()
{

}
