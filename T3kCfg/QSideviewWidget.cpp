#include "QSideviewWidget.h"
#include "ui_QSideviewWidget.h"

#include "T3kConstStr.h"
#include "Common/nv.h"

#include <QtEvents>
#include <QPainter>


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

    m_pImgTempBuffer = NULL;

    ui->TitleSideview->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_CAMERA.png" );

    connect( this, SIGNAL(ChangeCamera(int)), this, SLOT(onChangeCamera(int)), Qt::QueuedConnection );

    onChangeLanguage();
}

QSideviewWidget::~QSideviewWidget()
{
    if( m_pImgTempBuffer )
    {
        delete m_pImgTempBuffer;
        m_pImgTempBuffer = NULL;
    }

    delete ui;
}

void QSideviewWidget::showEvent(QShowEvent *evt)
{
    m_pT3kHandle->setBothSensorData( true );

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

        m_pT3kHandle->setBothSensorData( false );

        if( m_pT3kHandle->getReportCommand() )
            m_pT3kHandle->setReportCommand( false );
        if( m_pT3kHandle->getReportView() )
            m_pT3kHandle->setReportView( false );
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

void QSideviewWidget::onChangeLanguage()
{
    if( !winId() ) return;

    //QLangRes& Res = QLangManager::instance()->getResource();


}

void QSideviewWidget::RequestSensorData( bool /*bDefault*/ )
{
    if( !m_pT3kHandle )
        return;

    m_pT3kHandle->sendCommand( QString( "%1?" ).arg(cstrFirmwareVersion) );
    m_pT3kHandle->sendCommand( QString( "%1?" ).arg(cstrSimpleDetection) );

    int nSub = (int)(m_nCurrentCamNo/2);
    int nDir = (int)(m_nCurrentCamNo%2);
    QString strCmd;
    strCmd += QString("cam%1/").arg(nDir+1);
    for( int i=0; i<nSub; i++ )
        strCmd += "sub/";

    m_pT3kHandle->sendCommand( QString("%1%2?").arg(strCmd).arg(cstrDetectionLine) );
}

void QSideviewWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !winId() ) return;

    if ( strstr(cmd, cstrFirmwareVersion) == cmd )
    {
        if( Part == MM )
        {
            char szVer[255];
            strncpy( szVer, (cmd + sizeof(cstrFirmwareVersion) - 1), 255 );

            QString str( szVer );
            int nS = str.indexOf( ' ' );
            if( nS >= 0 )
            {
                m_strModelName = str.right( str.length() - 1 - nS );
                ui->SensorImage->SetModel( m_strModelName );
            }
        }
    }
    else if( strstr(cmd, cstrSimpleDetection) == cmd )
    {
        ui->SensorImage->SetSimpleDetection( atoi( cmd+sizeof(cstrSimpleDetection)-1 ) == 1 ? true : false );
    }
    else if( strstr(cmd, cstrDetectionLine) == cmd )
    {
        char* pCur = (char*)cmd + sizeof(cstrDetectionLine) - 1;
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

void QSideviewWidget::TPDP_OnRSE(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char *partid, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    QString strPartID( partid );
    if( !strPartID.contains( "RSE" ) ) return;

    if( strstr( cmd, cstrNoCam ) == cmd )
    {
        ui->SensorImage->SetNoCam( true );
        // fill empty image
        m_bmpSnapSensor = QImage( ui->SensorImage->width(), ui->SensorImage->height(), QImage::Format_RGB32 );

        QRect rcClient( 0, 0, ui->SensorImage->width(), ui->SensorImage->height() );
        QPainter dc;
        dc.begin( &m_bmpSnapSensor );

        dc.fillRect( rcClient, Qt::black );
        dc.setPen( Qt::white );
        QString str( QLangManager::instance()->getResource().getResString(QString::fromUtf8("SENSOR DIAGNOSIS SIDEVIEW"), QString::fromUtf8("TEXT_NO_IMAGE")) );
        dc.drawText( rcClient, str, QTextOption( Qt::AlignVCenter | Qt::AlignHCenter ) );

        dc.end();

        ui->SensorImage->SetImage( &m_bmpSnapSensor );
        ui->SensorImage->SetProgress( 100 );

        ui->EditOffset->setText( "" );

        if( m_pT3kHandle->getReportCommand() )
            m_pT3kHandle->setReportCommand( false );
        if( m_pT3kHandle->getReportView() )
            m_pT3kHandle->setReportView( false );
    }
}

void QSideviewWidget::TPDP_OnPRV(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int total, int offset, const unsigned char *data, int cnt)
{
    if( !isVisible() ) return;

    if ( (m_bmpSnapSensor.width() != cnt) || (m_bmpSnapSensor.height() != total) )
    {
        if (m_pImgTempBuffer)
            delete m_pImgTempBuffer;
        m_pImgTempBuffer = new uchar[cnt * total];

        memset( m_pImgTempBuffer, 0, sizeof(uchar)*cnt*total );

        m_bmpSnapSensor = QImage( cnt, total, QImage::Format_RGB32 );
        m_bmpSnapSensor.fill(QColor(0,0,0));
    }

    bool bUpdate = false;

#ifndef Q_OS_LINUX
    if (offset == 0)
#endif
        bUpdate = true;

    offset += total/2;
    if ( offset >= 0 && offset < total )
    {
        int nOffsetY = offset * cnt;
        memcpy( m_pImgTempBuffer + nOffsetY, data, cnt );
    }

    if (bUpdate)
    {
        int nW = cnt;
        int nH = total;
        int nOffsetY = 0;
        for ( int y=0 ; y<nH ; y++ )
        {
            QRgb* pRgbLine = (QRgb*)m_bmpSnapSensor.scanLine(y);
            nOffsetY = y * nW;
            int g;
            for (int i=0 ; i<m_bmpSnapSensor.width(); i++ )
            {
                g = m_pImgTempBuffer[nOffsetY+i];
                pRgbLine[i] = qRgb(g, g, g);
            }
        }
//        if (m_rcUpdateImage.isEmpty())
            update();
//        else
//            update(m_rcUpdateImage);
    }
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

    m_pT3kHandle->sendCommand( QString("%1%2?").arg(strSendCmd).arg(cstrDetectionLine), true );

    if( !m_pT3kHandle->getReportCommand() )
        m_pT3kHandle->setReportCommand( true );
    if( !m_pT3kHandle->getReportView() )
        m_pT3kHandle->setReportView( true );

    m_pT3kHandle->sendCommand( QString("%1mode=sideview").arg(strSendCmd), true );
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

    m_pT3kHandle->sendCommand( QString("%1%2%3").arg(strCmd).arg(cstrDetectionLine).arg(nOffset), true );
    m_pT3kHandle->sendCommand( QString("%1%2!").arg(strCmd).arg(cstrDetectionLine), true );
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

    m_pT3kHandle->sendCommand( QString("%1%2%3").arg(strCmd).arg(cstrDetectionLine).arg(nOffset), true );
    m_pT3kHandle->sendCommand( QString("%1%2!").arg(strCmd).arg(cstrDetectionLine), true );
}
