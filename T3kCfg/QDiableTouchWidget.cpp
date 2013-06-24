#include "QDiableTouchWidget.h"
#include "ui_QDiableTouchWidget.h"

#include "stdInclude.h"
#include <QCloseEvent>

#include "Common/nv.h"

#include "QWidgetCloseEventManager.h"

QDiableTouchWidget::QDiableTouchWidget(T30xHandle*& pHandle, QWidget *parent) :
    QDialog(parent),
    m_pT3kHandle(pHandle), ui(new Ui::QDiableTouchWidget)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

    ui->setupUi(this);
    setFont( parent->font() );

    m_nTimerRemain = 10;
    m_strTimeout = "%1 secs";

    OnChangeLanguage();

    QString strText = m_strTimeout.arg( m_nTimerRemain );
    ui->BtnOK->setText( strText );
    ui->BtnOK->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_TIME.png" );
    ui->BtnOK->SetBorder( true );
    ui->BtnOK->SetAlignmentText( QFlatTextButton::FBA_CENTER );

    m_pT3kHandle->EnableMouse( false, true );

    m_TimerCountDown.setParent( this );
    connect( &m_TimerCountDown, SIGNAL(timeout()), this, SLOT(OnTimer()) );
    connect( this, SIGNAL(rejected()), this, SLOT(On_Cancel()) );
    m_TimerCountDown.start( 1000 );

    setFixedSize( width(), height() );
}

QDiableTouchWidget::~QDiableTouchWidget()
{
    delete ui;

    if( m_TimerCountDown.isActive() )
        m_TimerCountDown.stop();
}

void QDiableTouchWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    setWindowTitle( Res.GetResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("TITLE_CAPTION")) );
    ui->LBMessage->setText( Res.GetResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("TEXT_MESSAGE")) );
    m_strTimeout = Res.GetResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("BTN_TIMER_TEXT"));
}

void QDiableTouchWidget::OnTimer()
{
    QString strText( m_strTimeout );
    strText = strText.arg(--m_nTimerRemain);
    ui->BtnOK->setText( strText );

    if( m_nTimerRemain < 1 )
    {
        m_TimerCountDown.stop();
        QDialog::close();
    }
}

void QDiableTouchWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_TimerCountDown.timerId() )
    {
        QString strText;
        strText.sprintf( m_strTimeout.toStdString().c_str(), --m_nTimerRemain );
        ui->BtnOK->setText( strText );

        if( m_nTimerRemain < 1 )
        {
            m_TimerCountDown.stop();
            QDialog::close();
        }
    }

    QDialog::timerEvent(evt);
}

void QDiableTouchWidget::OnRSP(ResponsePart /*Part*/, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if ( strstr(sCmd, cstrCalibrationMode) == sCmd )
    {
        int nMode;
        nMode = atoi(sCmd + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                QWidgetCloseEventManager::GetPtr()->AddClosedWidget( this, 1000 );
            }
        }
    }
}

void QDiableTouchWidget::closeEvent(QCloseEvent *evt)
{
    if( evt->type() == QEvent::Close )
    {
        On_Cancel();
    }
}

void QDiableTouchWidget::on_BtnOK_clicked()
{
    accept();
    close();
}

void QDiableTouchWidget::On_Cancel()
{
    m_pT3kHandle->EnableMouse( true, true );
}
