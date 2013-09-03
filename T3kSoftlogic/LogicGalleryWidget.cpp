#include "LogicGalleryWidget.h"
#include "ui_LogicGalleryWidget.h"

#include "T3kCommonData.h"

#include <QPainter>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopWidget>


QLogicGalleryWidget::QLogicGalleryWidget(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::LogicGalleryWidget)
{
    ui->setupUi(this);

    m_nGalleryItemType = 0;
	m_portKey = CSoftlogic::epNoGate;

    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "LogicGallery_Pos" ).toString();
    settings.endGroup();

    QDesktopWidget desktop;
    QRect rcScreen( desktop.screenGeometry( desktop.primaryScreen() ) );

    if ( !strPos.isEmpty() )
    {
        QRect rcWin;
        QString str;
        int nD;
        do
        {
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setLeft( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setTop( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setRight( str.toLong() );
            rcWin.setBottom( strPos.toLong() );
            rcWin = rcWin.normalized();

            if ( rcScreen.intersects( rcWin ) )
                move( rcWin.left(), rcWin.top() );
            else
                move( rcScreen.center() - QRect(0,0,width(),height()).center() );
        }
        while ( false );
    }
    else
    {
        move( rcScreen.center() - QRect(0,0,width(),height()).center() );
    }

    m_pGalleryImage[0] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_TOUCH_ONOFF.png" );
    m_pGalleryImage[1] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_CALIBRATION.png" );
    m_pGalleryImage[2] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_TASK_SWITCH.png" );
    m_pGalleryImage[3] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_TOUCHONOFF_GPIO.png" );
    m_pGalleryImage[4] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_CALIBRATION_GPIO.png" );
    m_pGalleryImage[5] = new QPixmap( ":/T3kSoftlogic/resources/PNG_GALLERY_IRPEN_MODE_GPIO.png" );

    m_pRadioButtons[0] = ui->RBTouch;
    m_pRadioButtons[1] = ui->RBCalibration;
    m_pRadioButtons[2] = ui->RBTaskSwitch;
    m_pRadioButtons[3] = ui->RBTouchGPIO;
    m_pRadioButtons[4] = ui->RBCalibrationGPIO;
    m_pRadioButtons[5] = ui->RBTaskSwitchGPIO;

    connect( ui->BtnCancel, &QPushButton::clicked, this, &QWidget::close );
}

QLogicGalleryWidget::~QLogicGalleryWidget()
{
    delete ui;

    for( int i=0; i<MAX_IMAGE; i++ )
    {
        if( m_pGalleryImage[i] )
            delete m_pGalleryImage[i];
    }
}

void QLogicGalleryWidget::setInfo( CSoftlogic::Port portKey, QPoint ptMouse )
{
	m_portKey = portKey;
	m_ptMouse = ptMouse;

    if ( winId() )
	{
        ui->RBTouch->setEnabled( m_portKey == CSoftlogic::epNoGate ? false : true );
        ui->RBCalibration->setEnabled( m_portKey == CSoftlogic::epNoGate ? false : true );
        ui->RBTaskSwitch->setEnabled( m_portKey == CSoftlogic::epNoGate ? false : true );
        ui->RBTouchGPIO->setEnabled( m_portKey != CSoftlogic::epNoGate ? false : true );
        ui->RBCalibrationGPIO->setEnabled( m_portKey != CSoftlogic::epNoGate ? false : true );
        ui->RBTaskSwitchGPIO->setEnabled( m_portKey != CSoftlogic::epNoGate ? false : true );

        ui->CBGPIO->setEnabled( m_portKey != CSoftlogic::epNoGate ? false : true );
        ui->TextGPIO->setEnabled( m_portKey != CSoftlogic::epNoGate ? false : true );
	}
}

void QLogicGalleryWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;

    QRect rcPreview( ui->Gallery->x()+10, ui->Gallery->y()+10, ui->Gallery->width(), ui->Gallery->height() );

    painter.begin( this );

    painter.fillRect( rcPreview, Qt::black );

	if ( m_nGalleryItemType < 0 )
	{
        painter.setPen( Qt::white );
        painter.drawText( rcPreview, "<<  Select type", Qt::AlignVCenter|Qt::AlignHCenter );

        painter.end();
		return;
	}


    QPixmap* pImage = m_pGalleryImage[m_nGalleryItemType];

    QRect rect;
    int nDstW = rcPreview.width();
    int nDstH = rcPreview.height();
    int nIW, nIH;

    if ( nDstW > pImage->width() &&
        nDstH > pImage->height() )
	{
        nIW = pImage->width();
        nIH = pImage->height();
	}
	else
	{
        if ( pImage->width() > pImage->height() )
		{
            if ( nDstW < pImage->width() )
			{
				nIW = nDstW;
                nIH = pImage->height() * nDstW / pImage->width();
			}
		}
		else
		{
            if ( nDstH < pImage->height() )
			{
				nIH = nDstH;
                nIW = pImage->width() * nDstH / pImage->height();
			}
		}
	}

    rect.setX( rcPreview.left() + (nDstW-nIW)/2 );
    rect.setY( rcPreview.top() + (nDstH-nIH)/2 );
    rect.setWidth( nIW );
    rect.setHeight( nIH );

    painter.drawPixmap( rect, *pImage, QRect(0, 0, pImage->width(), pImage->height()) );

    painter.end();
}

void QLogicGalleryWidget::showEvent(QShowEvent *)
{
    setInfo(m_portKey, m_ptMouse);

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    ui->CBGPIO->clear();
    for ( int i=0 ; i<Keys.getGPIOCount() ; i++ )
    {
        GPIOInfo* pInfo = Keys.getGPIOInfo(i);
        if ( pInfo )
        {
            if ( pInfo->bEnable )
            {
                ui->CBGPIO->addItem( QString("GPIO %1").arg(i+1) );
                ui->CBGPIO->setItemData( ui->CBGPIO->count()-1, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
            }
        }
    }

    ui->RBTouch->setChecked( false );
    ui->RBCalibration->setChecked( false );
    ui->RBTaskSwitch->setChecked( false );
    ui->RBTouchGPIO->setChecked( false );
    ui->RBCalibrationGPIO->setChecked( false );
    ui->RBTaskSwitchGPIO->setChecked( false );

    m_nGalleryItemType = -1;
    ui->CBGPIO->setCurrentIndex( -1 );
}

void QLogicGalleryWidget::closeEvent(QCloseEvent *)
{
    QRect rc( x(), y(), width(), height() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "LogicGallery_Pos", str );
    settings.endGroup();
}

void QLogicGalleryWidget::on_BtnInsert_clicked()
{
    CSoftlogic::Port port = m_portKey;

    if ( m_portKey == CSoftlogic::epNoGate )
    {
        int nCurSel = ui->CBGPIO->currentIndex();
        if ( nCurSel < 0 )
        {
            QMessageBox::critical( this, "Error", "Please select GPIO.", QMessageBox::Ok );
            ui->CBGPIO->setFocus();
            return;
        }

        port = (CSoftlogic::Port)ui->CBGPIO->itemData( nCurSel ).toInt();
    }

    emit addLogicFromGallery( m_nGalleryItemType, port, m_ptMouse );

    close();
}

void QLogicGalleryWidget::on_RBTouch_clicked()
{
    if( m_nGalleryItemType == 0 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 0;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}

void QLogicGalleryWidget::on_RBCalibration_clicked()
{
    if( m_nGalleryItemType == 1 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 1;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}

void QLogicGalleryWidget::on_RBTaskSwitch_clicked()
{
    if( m_nGalleryItemType == 2 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 2;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}

void QLogicGalleryWidget::on_RBTouchGPIO_clicked()
{
    if( m_nGalleryItemType == 3 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 3;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}

void QLogicGalleryWidget::on_RBCalibrationGPIO_clicked()
{
    if( m_nGalleryItemType == 4 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 4;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}

void QLogicGalleryWidget::on_RBTaskSwitchGPIO_clicked()
{
    if( m_nGalleryItemType == 5 ) return;

    if( m_nGalleryItemType >= 0 )
        m_pRadioButtons[m_nGalleryItemType]->setChecked( false );

    m_nGalleryItemType = 5;

    QRect rc( ui->Gallery->x(), ui->Gallery->y(), ui->Gallery->width(), ui->Gallery->height() );
    update( rc );
}
