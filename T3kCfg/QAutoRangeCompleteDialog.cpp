#include "QAutoRangeCompleteDialog.h"
#include "ui_QAutoRangeCompleteDialog.h"

#include <QPainter>
#include <QTimerEvent>
#include <QLinearGradient>
#include "QLangManager.h"

#define	TIMER_TICK			(20)
#define	TIMER_TIMEOUT		(1500)
#define	MAX_TIMER_COUNT		(TIMER_TIMEOUT/TIMER_TICK)

QAutoRangeCompleteDialog::QAutoRangeCompleteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAutoRangeCompleteDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog;

#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif

    setWindowFlags(flags);

    m_TimerDummyProgress = 0;
    m_nTimerCount = 0;

    m_TimerDummyProgress = startTimer( TIMER_TICK );
}

QAutoRangeCompleteDialog::~QAutoRangeCompleteDialog()
{
    delete ui;
}

#define RES_TAG "AUTORANGE COMPLETE DIALOG"
void QAutoRangeCompleteDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.save();
    QRect rcBody(0, 0, width()-1, height()-1);

    QRect rcBorder = rcBody;
    p.fillRect(rcBorder, QColor(120, 120, 120));
    rcBorder.adjust( 2, 2, -2, -2 );
    p.fillRect(rcBorder, QColor(255, 255, 255));

//    QLangRes& res = QLangManager::getResource();

    p.setRenderHint(QPainter::Antialiasing);

    QRectF rectText(rcBody.left(), rcBody.top(), rcBody.width(), rcBody.height()*2/3);

    m_rcProgress = rcBody;
    m_rcProgress.setTop( rcBody.bottom() - rcBody.height()/5 );

    QRect rectProgressBody = m_rcProgress;
    rectProgressBody.adjust( 5, 5, -5, -5 );

    float fFontHeight = rectText.height() / 4.f;
    QFont fntCaption = font();
    fntCaption.setPixelSize((int)(fFontHeight+0.5f));
    fntCaption.setBold(true);
    int flags = Qt::AlignCenter|Qt::AlignVCenter;
    p.setFont(fntCaption);

    QRectF rectText1 = rectText;
    rectText1.setHeight( rectText.height()/2.f );
    rectText1.setTop( rectText1.top() + rectText1.height()/4.f );

    QString strProgressText = "Processing...";//res.getResString( RES_TAG, "TEXT_PROCESSING" );
    p.setPen( QColor(90, 90, 90) );
    p.drawText( rectText1, flags, strProgressText );

    QRectF rectText2 = rectText;
    rectText2.setTop( rectText.top() + rectText.height()/2.f );
    rectText2.setHeight( rectText.height()/2.f );

    QString strDontTouchText = "DO NOT TOUCH!";//res.getResString( RES_TAG, "TEXT_DO_NOT_TOUCH" );
    p.setPen( QColor(237, 28, 36) );
    p.drawText( rectText2, flags, strDontTouchText );

    p.setPen( QColor(206, 206, 206) );
    p.setBrush( QColor(216, 216, 216) );

    p.drawRoundRect( rectProgressBody, 5, 5 );

    QRect rectProgress = rectProgressBody;
    rectProgress.setWidth( rectProgressBody.width() * m_nTimerCount / MAX_TIMER_COUNT );
    rectProgress.adjust( 1, 1, -1, -1 );

    QColor clrChunk(43, 140, 238);
    QLinearGradient grad(QPointF(rectProgress.topLeft()), QPointF(rectProgress.bottomRight()));
    clrChunk.setAlpha(100);
    grad.setColorAt( 0.0, clrChunk );
    clrChunk.setAlpha(255);
    grad.setColorAt( 1.0, clrChunk );

    p.setPen(QColor(28, 116, 149));
    p.setBrush(grad);
    p.drawRoundRect( rectProgress, 5, 5 );

    p.restore();
}

void QAutoRangeCompleteDialog::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerDummyProgress)
    {
        if (m_nTimerCount >= MAX_TIMER_COUNT)
        {
            killTimer(m_TimerDummyProgress);
            m_TimerDummyProgress = 0;
            QDialog::accept();
        }
        else
        {
            m_nTimerCount ++;
            update( m_rcProgress );
        }
    }
}

void QAutoRangeCompleteDialog::accept()
{

}

void QAutoRangeCompleteDialog::reject()
{

}
