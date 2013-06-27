#include "QTouchSettingWidget.h"
#include "ui_QTouchSettingWidget.h"

#include "stdInclude.h"

#include "Common/nv.h"

#include <QShowEvent>
#include <QTimer>
#include <QMessageBox>

QTouchSettingWidget::QTouchSettingWidget(T30xHandle*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTouchSettingWidget), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

    ui->setupUi(this);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    setFixedSize( width(), height() );

    connect( ui->BtnClose, SIGNAL(clicked()), this, SLOT(accept()) );

    ui->SldTap->setRange( (NV_DEF_TIME_A_RANGE_START)/100, NV_DEF_TIME_A_RANGE_END/100 );
    ui->SldTap->setValue( NV_DEF_TIME_A/100 );
    ui->SldTap->setTickInterval( 1 );
    ui->SldLongTap->setRange( (NV_DEF_TIME_L_RANGE_START)/100, NV_DEF_TIME_L_RANGE_END/100 );
    ui->SldLongTap->setValue( NV_DEF_TIME_L/100 );
    ui->SldLongTap->setTickInterval( 1 );
    ui->SldWheel->setRange( NV_DEF_WHEEL_SENSITIVITY_RANGE_START, NV_DEF_WHEEL_SENSITIVITY_RANGE_END );
    ui->SldWheel->setValue( NV_DEF_WHEEL_SENSITIVITY );
    ui->SldWheel->setTickInterval( NV_DEF_WHEEL_SENSITIVITY_RANGE_END );
    ui->SldZoom->setRange( NV_DEF_ZOOM_SENSITIVITY_RANGE_START, NV_DEF_ZOOM_SENSITIVITY_RANGE_END );
    ui->SldZoom->setValue( NV_DEF_ZOOM_SENSITIVITY );
    ui->SldZoom->setTickInterval( NV_DEF_ZOOM_SENSITIVITY_RANGE_END );

    m_pTimerSendData = new QTimer( this );
    connect( m_pTimerSendData, SIGNAL(timeout()), this, SLOT(OnTimer()) );
    m_pTimerSendData->setSingleShot( true );

    OnChangeLanguage();

    ui->BtnClose->setFocus();
}

QTouchSettingWidget::~QTouchSettingWidget()
{
    if( m_pTimerSendData )
    {
        m_pTimerSendData->stop();
        delete m_pTimerSendData;
        m_pTimerSendData = NULL;
    }

    m_RequestSensorData.Stop();

    delete ui;
}

void QTouchSettingWidget::OnChangeLanguage()
{
    if( !isWindow() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("WINDOW_CAPTION")) );
    ui->LBSlow1->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_SLOW")) );
    ui->LBSlow2->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_SLOW")) );
    ui->LBFast1->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_FAST")) );
    ui->LBFast2->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_FAST")) );
    ui->BtnDefault->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("BTN_CAPTION_DEFAULT")) );
    ui->BtnClose->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("BTN_CAPTION_CLOSE")) );

    ui->TitleTimeSetting->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TITLE_TIME_SETTING")) );
    ui->TitleWheelZoomSetting->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TITLE_WHEEL_SENSITIVITY_SETTING")) );

    ui->LBTap->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_TAP")) );
    ui->LBLongTap->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_LONG_TAP")) );
    ui->LBWheel->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_WHEEL")) );
    ui->LBZoom->setText( Res.GetResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_ZOOM")) );

    Qt::AlignmentFlag eFlag = Res.IsR2L() ? Qt::AlignLeft : Qt::AlignRight;
    ui->EditTap->setAlignment( eFlag );
    ui->EditLongTap->setAlignment( eFlag );
    ui->EditWheel->setAlignment( eFlag );
    ui->EditZoom->setAlignment( eFlag );
}

void QTouchSettingWidget::OnRSP(ResponsePart /*Part*/, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if ( strstr(sCmd, cstrTimeA) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrTimeA );

        int nTime = atoi(sCmd + sizeof(cstrTimeA) - 1);
        if( nTime < NV_DEF_TIME_A_RANGE_START )
            nTime = NV_DEF_TIME_A_RANGE_START;
        if( nTime > NV_DEF_TIME_A_RANGE_END )
            nTime = NV_DEF_TIME_A_RANGE_END;

        QString strV( QString("%1").arg(nTime) );
        ui->EditTap->setText( strV );
        ui->SldTap->setValue( NV_DEF_TIME_A_RANGE_END/100 - nTime/100 + (NV_DEF_TIME_A_RANGE_START)/100 );
    }
    else if ( strstr(sCmd, cstrTimeL) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrTimeL );

        int nTime = atoi(sCmd + sizeof(cstrTimeL) - 1);
        if( nTime < NV_DEF_TIME_L_RANGE_START )
            nTime = NV_DEF_TIME_L_RANGE_START;
        if( nTime > NV_DEF_TIME_L_RANGE_END )
            nTime = NV_DEF_TIME_L_RANGE_END;

        QString strV( QString("%1").arg(nTime) );
        ui->EditLongTap->setText( strV );
        ui->SldLongTap->setValue( NV_DEF_TIME_L_RANGE_END/100 - nTime/100 + (NV_DEF_TIME_L_RANGE_START)/100 );
    }
    else if ( strstr(sCmd, cstrWheelSensitivity) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrWheelSensitivity );

        int nSens = atoi(sCmd + sizeof(cstrWheelSensitivity) - 1);
        if( !(nSens >= NV_DEF_WHEEL_SENSITIVITY_RANGE_START && nSens <= NV_DEF_WHEEL_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_WHEEL_SENSITIVITY;
        }
        QString strV( QString("%1").arg(nSens) );
        ui->EditWheel->setText( strV );
        ui->SldWheel->setValue( nSens );
    }
    else if ( strstr(sCmd, cstrZoomSensitivity) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrZoomSensitivity );

        int nSens = atoi(sCmd + sizeof(cstrZoomSensitivity) - 1);
        if( !(nSens >= NV_DEF_ZOOM_SENSITIVITY_RANGE_START && nSens <= NV_DEF_ZOOM_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_ZOOM_SENSITIVITY;
        }
        QString strV( QString("%1").arg(nSens) );
        ui->EditZoom->setText( strV );
        ui->SldZoom->setValue( nSens );
    }
}

void QTouchSettingWidget::RequestSensorData( bool bDefault )
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    char cQ = bDefault ? '*' : '?';

    QString str( cQ );
    m_RequestSensorData.AddItem( cstrTimeA, str );
    m_RequestSensorData.AddItem( cstrTimeL, str );
    m_RequestSensorData.AddItem( cstrWheelSensitivity, str );
    m_RequestSensorData.AddItem( cstrZoomSensitivity, str );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTimeA).arg(cQ).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTimeL).arg(cQ).toUtf8().data(), true );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrWheelSensitivity).arg(cQ).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrZoomSensitivity).arg(cQ).toUtf8().data(), true );

    m_RequestSensorData.Start( m_pT3kHandle );
}

void QTouchSettingWidget::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        ui->BtnDefault->setEnabled( false );
        RequestSensorData( false );
        ui->BtnDefault->setEnabled( true );
    }
}

//void QTouchSettingWidget::on_BtnRefresh_clicked()
//{
//    ui->BtnDefault->setEnabled( false );
//    RequestSensorData( false );
//    ui->BtnDefault->setEnabled( true );
//}

void QTouchSettingWidget::on_BtnDefault_clicked()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    QString strMessage = Res.GetResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TEXT_DEFAULT") );
    QString strMsgTitle = Res.GetResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TITLE_DEFAULT") );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setText( strMessage );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setButtonText( QMessageBox::Yes, Res.GetResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
    msgBox.setButtonText( QMessageBox::No, Res.GetResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
    msgBox.setFont( font() );

    if( msgBox.exec() != QMessageBox::Yes ) return;

    ui->BtnDefault->setEnabled( false );
    RequestSensorData( true );
    ui->BtnDefault->setEnabled( true );
}

void QTouchSettingWidget::SendTimeTap( int nTime )
{
    if( !m_pT3kHandle ) return;

    if( nTime < NV_DEF_TIME_A_RANGE_START )
        nTime = NV_DEF_TIME_A_RANGE_START;
    if( nTime > NV_DEF_TIME_A_RANGE_END )
        nTime = NV_DEF_TIME_A_RANGE_END;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTimeA).arg(nTime).toUtf8().data(), false );
}

void QTouchSettingWidget::SendTimeLongTap( int nTime )
{
    if( !m_pT3kHandle ) return;

    if( nTime < NV_DEF_TIME_L_RANGE_START )
        nTime = NV_DEF_TIME_L_RANGE_START;
    if( nTime > NV_DEF_TIME_L_RANGE_END )
        nTime = NV_DEF_TIME_L_RANGE_END;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTimeL).arg(nTime).toUtf8().data(), false );
}

void QTouchSettingWidget::SendSensWheel( int nSensitivity )
{
    if( !m_pT3kHandle ) return;

    if( !(nSensitivity >= NV_DEF_WHEEL_SENSITIVITY_RANGE_START && nSensitivity <= NV_DEF_WHEEL_SENSITIVITY_RANGE_END) )
    {
        nSensitivity = NV_DEF_WHEEL_SENSITIVITY;
    }

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrWheelSensitivity).arg(nSensitivity).toUtf8().data(), false );
}

void QTouchSettingWidget::SendSensZoom( int nSensitivity )
{
    if( !m_pT3kHandle ) return;

    if( !(nSensitivity >= NV_DEF_ZOOM_SENSITIVITY_RANGE_START && nSensitivity <= NV_DEF_ZOOM_SENSITIVITY_RANGE_END) )
    {
        nSensitivity = NV_DEF_ZOOM_SENSITIVITY;
    }

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrZoomSensitivity).arg(nSensitivity).toUtf8().data(), false );
}

void QTouchSettingWidget::on_SldTap_valueChanged(int /*value*/)
{
    if( !ui->SldTap->isVisible() ) return;

    int nMinPos = ui->SldTap->minimum();
    int nMaxPos = ui->SldTap->maximum();
    int nCurPos = ui->SldTap->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg((nMaxPos-(nCurPos-nMinPos))*100) );
    ui->EditTap->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldLongTap_valueChanged(int /*value*/)
{
    if( !ui->SldLongTap->isVisible() ) return;

    int nMinPos = ui->SldLongTap->minimum();
    int nMaxPos = ui->SldLongTap->maximum();
    int nCurPos = ui->SldLongTap->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg((nMaxPos-(nCurPos-nMinPos))*100) );
    ui->EditLongTap->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldWheel_valueChanged(int /*value*/)
{
    if( !ui->SldWheel->isVisible() ) return;

//    int nMinPos = ui->SldWheel->minimum();
//    int nMaxPos = ui->SldWheel->maximum();
    int nCurPos = ui->SldWheel->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg(nCurPos) );
    ui->EditWheel->setText( strV );


    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldZoom_valueChanged(int /*value*/)
{
    if( !ui->SldZoom->isVisible() ) return;

//    int nMinPos = ui->SldZoom->minimum();
//    int nMaxPos = ui->SldZoom->maximum();
    int nCurPos = ui->SldZoom->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg(nCurPos) );
    ui->EditZoom->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::OnTimer()
{
    int nSens, nTime;
    QString strV( ui->EditWheel->text() );
    nSens = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendSensWheel( nSens );
    strV = QString(ui->EditZoom->text() );
    nSens = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendSensZoom( nSens );
    strV = QString(ui->EditTap->text() );
    nTime = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendTimeTap( nTime );
    strV = QString(ui->EditLongTap->text() );
    nTime = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendTimeLongTap( nTime );
}
