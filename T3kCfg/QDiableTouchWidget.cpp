#include "QDiableTouchWidget.h"
#include "ui_QDiableTouchWidget.h"

#include "stdInclude.h"
#include <QCloseEvent>

#include "Common/nv.h"

#include "QWidgetCloseEventManager.h"

QDiableTouchWidget::QDiableTouchWidget(QT3kDeviceR*& pHandle, QWidget *parent) :
    QDialog(parent),
    m_pT3kHandle(pHandle), ui(new Ui::QDiableTouchWidget)
{   
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    setWindowModality( Qt::NonModal );
    setModal( true );

    ui->setupUi(this);
    setFont( parent->font() );

    m_nTimerRemain = 10;
    m_strTimeout = "%1 secs";

    onChangeLanguage();

    QString strText = m_strTimeout.arg( m_nTimerRemain );
    ui->BtnOK->setText( strText );
    ui->BtnOK->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_TIME.png" );
    ui->BtnOK->SetBorder( true );
    ui->BtnOK->SetAlignmentText( QFlatTextButton::FBA_CENTER );

    m_pT3kHandle->enableMouse( false );

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

void QDiableTouchWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();
    setWindowTitle( Res.getResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("TITLE_CAPTION")) );
    ui->LBMessage->setText( Res.getResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("TEXT_MESSAGE")) );
    m_strTimeout = Res.getResString(QString::fromUtf8("CHECK TOUCH DISABLE"), QString::fromUtf8("BTN_TIMER_TEXT"));
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

void QDiableTouchWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    if ( strstr(cmd, cstrCalibrationMode) == cmd )
    {
        int nMode;
        nMode = atoi(cmd + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                QWidgetCloseEventManager::instance()->AddClosedWidget( this, 1000 );
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
    m_pT3kHandle->enableMouse( true );
}
