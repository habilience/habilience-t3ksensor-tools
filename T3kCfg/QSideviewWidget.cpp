#include "QSideviewWidget.h"
#include "ui_QSideviewWidget.h"

#include "../common/T3kConstStr.h"
#include "Common/nv.h"

#include <QtEvents>
#include <QPainter>

static const char cstrNoCam[] = "no cam";

QSideviewWidget::QSideviewWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSideviewWidget)
{
    ui->setupUi(this);
    setFont( parent->font() );

    setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint );

    m_pT3kHandle = NULL;
    m_nCurrentCamNo = 0;
    m_nAutoLine = -1;

    ui->TitleSideview->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_CAMERA.png" );

    connect( this, SIGNAL(ChangeCamera(int)), this, SLOT(onChangeCamera(int)), Qt::QueuedConnection );

    OnChangeLanguage();
}

QSideviewWidget::~QSideviewWidget()
{
    delete ui;
}

void QSideviewWidget::showEvent(QShowEvent *evt)
{
    m_pT3kHandle->SetBothSensorData( true );

    RequestSensorData( false );

    ui->BtnCam1->setChecked( true );

    emit ChangeCamera(0);

    QDialog::showEvent(evt);
}

void QSideviewWidget::hideEvent(QHideEvent *evt)
{
    if( m_pT3kHandle )
    {
        ui->BtnCam1->setChecked( false );
        ui->BtnCam2->setChecked( false );
        ui->BtnCam3->setChecked( false );
        ui->BtnCam4->setChecked( false );

        m_pT3kHandle->SetBothSensorData( false );

        if( m_pT3kHandle->GetReportCommand() )
            m_pT3kHandle->SetReportCommand( false );
        if( m_pT3kHandle->GetReportView() )
            m_pT3kHandle->SetReportView( false );
    }

    m_nCurrentCamNo = 0;

    QDialog::hideEvent(evt);
}

void QSideviewWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Escape || evt->key() == Qt::Key_Control || evt->key() == Qt::Key_Meta || evt->key() == Qt::Key_Alt )
    {
        QWidget::hide();
        return;
    }

    QDialog::keyPressEvent(evt);
}

void QSideviewWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    //QLangRes& Res = QLangManager::GetPtr()->GetResource();


}

void QSideviewWidget::RequestSensorData( bool /*bDefault*/ )
{
    if( !m_pT3kHandle )
        return;

    m_pT3kHandle->SendCommand( (const char*)QString( "%1?" ).arg(cstrFirmwareVersion).toUtf8().data() );
    m_pT3kHandle->SendCommand( (const char*)QString( "%1?" ).arg(cstrSimpleDetection).toUtf8().data() );

    int nSub = (int)(m_nCurrentCamNo/2);
    int nDir = (int)(m_nCurrentCamNo%2);
    QString strCmd;
    strCmd += QString("cam%1/").arg(nDir+1);
    for( int i=0; i<nSub; i++ )
        strCmd += "sub/";

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(strCmd).arg(cstrDetectionLine).toUtf8().data() );
}

void QSideviewWidget::OnRSP(ResponsePart Part, ushort, const char *, long, bool, const char *sCmd)
{
    if( !winId() ) return;

    if ( strstr(sCmd, cstrFirmwareVersion) == sCmd )
    {
        if( Part == MM )
        {
            char szVer[255];
            strncpy( szVer, (sCmd + sizeof(cstrFirmwareVersion) - 1), 255 );

            QString str( szVer );
            int nS = str.indexOf( ' ' );
            if( nS >= 0 )
            {
                m_strModelName = str.right( str.length() - 1 - nS );
                ui->SensorImage->SetModel( m_strModelName );
            }
        }
    }
    else if( strstr(sCmd, cstrSimpleDetection) == sCmd )
    {
        ui->SensorImage->SetSimpleDetection( atoi( sCmd+sizeof(cstrSimpleDetection)-1 ) == 1 ? true : false );
    }
    else if( strstr(sCmd, cstrDetectionLine) == sCmd )
    {
        char* pCur = (char*)sCmd + sizeof(cstrDetectionLine) - 1;
        int nDetectLine = atoi(pCur);

        pCur = strchr(pCur, ',') + 1;

        int nAutoLine = -1;
        if( pCur != (char*)1 )
            nAutoLine = atoi(pCur);

        m_nAutoLine = nAutoLine;
        ui->SensorImage->SetAutoDetection( m_nAutoLine );

        if( nDetectLine < NV_DEF_CAM_DETECTION_LINE_RANGE_START )
            nDetectLine = NV_DEF_CAM_DETECTION_LINE_RANGE_START;
        if( nDetectLine > NV_DEF_CAM_DETECTION_LINE_RANGE_END )
            nDetectLine = NV_DEF_CAM_DETECTION_LINE_RANGE_END;

        QString strV;
        strV.sprintf( ("%d"), nDetectLine );

        ui->EditOffset->setText( strV );
    }
}

void QSideviewWidget::OnRSE(ResponsePart /*Part*/, ushort, const char *sPartId, long, bool, const char *sStatus)
{
    if( !isVisible() ) return;

    QString strPartID( sPartId );
    if( !strPartID.contains( "RSE" ) ) return;

    if( strstr( sStatus, cstrNoCam ) == sStatus )
    {
        ui->SensorImage->SetNoCam( true );
        // fill empty image
        m_bmpSnapSensor = QImage( ui->SensorImage->width(), ui->SensorImage->height(), QImage::Format_RGB32 );

        QRect rcClient( 0, 0, ui->SensorImage->width(), ui->SensorImage->height() );
        QPainter dc;
        dc.begin( &m_bmpSnapSensor );

        dc.fillRect( rcClient, Qt::black );
        dc.setPen( Qt::white );
        QString str( QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS SIDEVIEW"), QString::fromUtf8("TEXT_NO_IMAGE")) );
        dc.drawText( rcClient, str, QTextOption( Qt::AlignVCenter | Qt::AlignHCenter ) );

        dc.end();

        ui->SensorImage->SetImage( &m_bmpSnapSensor );
        ui->SensorImage->SetProgress( 100 );

        ui->EditOffset->setText( "" );

        if( m_pT3kHandle->GetReportCommand() )
            m_pT3kHandle->SetReportCommand( false );
        if( m_pT3kHandle->GetReportView() )
            m_pT3kHandle->SetReportView( false );
    }
}

void QSideviewWidget::OnPRV(ResponsePart /*Part*/, ushort, const char *, int nWidth, int nHeight, int, unsigned char *pBitmapBuffer)
{
    if( !isVisible() ) return;

    ui->SensorImage->SetNoCam( false );

    m_bmpSnapSensor = QImage( pBitmapBuffer, nWidth, nHeight, QImage::Format_RGB32 ).mirrored();

    ui->SensorImage->SetImage( &m_bmpSnapSensor );
    ui->SensorImage->SetProgress( 100 );
}

void QSideviewWidget::on_BtnCam1_clicked(bool clicked)
{
    if( clicked )
        emit ChangeCamera(0);
    else
        ui->BtnCam1->setChecked( true );
}

void QSideviewWidget::on_BtnCam2_clicked(bool clicked)
{
    if( clicked )
        emit ChangeCamera(1);
    else
        ui->BtnCam2->setChecked( true );
}

void QSideviewWidget::on_BtnCam3_clicked(bool clicked)
{
    if( clicked )
        emit ChangeCamera(2);
    else
        ui->BtnCam3->setChecked( true );
}

void QSideviewWidget::on_BtnCam4_clicked(bool clicked)
{
    if( clicked )
        emit ChangeCamera(3);
    else
        ui->BtnCam4->setChecked( true );
}

void QSideviewWidget::onChangeCamera(int nCamNo)
{
    QToolButton* pPrevButton = NULL;
    if( m_nCurrentCamNo != nCamNo )
    {
        switch( m_nCurrentCamNo )
        {
        case 0:
            pPrevButton = ui->BtnCam1;
            break;
        case 1:
            pPrevButton = ui->BtnCam2;
            break;
        case 2:
            pPrevButton = ui->BtnCam3;
            break;
        case 3:
            pPrevButton = ui->BtnCam4;
            break;
        default:
            Q_ASSERT( false );
            break;
        }

        if( pPrevButton->isChecked() )
            pPrevButton->setChecked( false );
        m_nCurrentCamNo = nCamNo;
    }

    if( !m_pT3kHandle )
        return;

    int nSub = (int)(nCamNo/2);
    int nDir = (int)(nCamNo%2);
    QString strSendCmd = QString("cam%1/").arg(nDir+1);
    for( int i=0; i<nSub; i++ )
        strSendCmd += "sub/";

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(strSendCmd).arg(cstrDetectionLine).toUtf8().data(), true );

    if( !m_pT3kHandle->GetReportCommand() )
        m_pT3kHandle->SetReportCommand( true );
    if( !m_pT3kHandle->GetReportView() )
        m_pT3kHandle->SetReportView( true );

    m_pT3kHandle->SendCommand( (const char*)QString("%1mode=sideview").arg(strSendCmd).toUtf8().data(), true );
}

void QSideviewWidget::on_BtnPlus_clicked()
{
    int nOffset = ui->EditOffset->text().toInt() + 1;

    if( nOffset < NV_DEF_CAM_DETECTION_LINE_RANGE_START )
        nOffset = NV_DEF_CAM_DETECTION_LINE_RANGE_START;
    else if( nOffset > NV_DEF_CAM_DETECTION_LINE_RANGE_END )
        nOffset = NV_DEF_CAM_DETECTION_LINE_RANGE_END;

    int nSub = (int)(m_nCurrentCamNo/2);
    int nDir = (int)(m_nCurrentCamNo%2);
    QString strCmd = QString("cam%1/").arg(nDir+1);
    for( int i=0; i<nSub; i++ )
        strCmd += "sub/";

    ui->EditOffset->setText( QString("%1").arg(nOffset) );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2%3").arg(strCmd).arg(cstrDetectionLine).arg(nOffset).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2!").arg(strCmd).arg(cstrDetectionLine).toUtf8().data(), true );
}

void QSideviewWidget::on_BtnMinus_clicked()
{
    int nOffset = ui->EditOffset->text().toInt() - 1;

    if( nOffset < NV_DEF_CAM_DETECTION_LINE_RANGE_START )
        nOffset = NV_DEF_CAM_DETECTION_LINE_RANGE_START;
    else if( nOffset > NV_DEF_CAM_DETECTION_LINE_RANGE_END )
        nOffset = NV_DEF_CAM_DETECTION_LINE_RANGE_END;

    int nDir = (int)(m_nCurrentCamNo%2);
    int nSub = (int)(m_nCurrentCamNo/2);
    QString strCmd = QString("cam%1/").arg(nDir+1);
    for( int i=0; i<nSub; i++ )
        strCmd += "sub/";

    ui->EditOffset->setText( QString("%1").arg(nOffset) );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2%3").arg(strCmd).arg(cstrDetectionLine).arg(nOffset).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2!").arg(strCmd).arg(cstrDetectionLine).toUtf8().data(), true );
}
