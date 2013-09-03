#include "ArrangeHelperWidget.h"
#include "ui_ArrangeHelperWidget.h"

#include "T3kCommonData.h"

#include <QPainter>
#include <QKeyEvent>


QArrangeHelperWidget::QArrangeHelperWidget(bool bMake, QWidget* parent /*=NULL*/) :
    QDialog(parent),
    ui(new Ui::ArrangeHelperWidget)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Tool|Qt::WindowStaysOnTopHint );
    setWindowModality(Qt::NonModal);

    ui->RBHorizontal->setChecked( true );
    ui->RBVertical->setChecked( false );

    m_strKeycount = "8";
    m_strWidth = "2000";
    m_strHeight = "1900";
    m_strInterval = "300";

    ui->EditKeyCount->setText( m_strKeycount );
    ui->EditWidth->setText( m_strWidth );
    ui->EditHeight->setText( m_strHeight );
    ui->EditInterval->setText( m_strInterval );

	m_eUnit = UnitRes;
	m_dD2PScaleWidth = m_dD2PScaleHeight = 0.0;

    m_pImageArrangePicture = new QPixmap( ":/T3kSoftlogic/resources/PNG_PICTURE_AUTO_ARRANGE.png" );

    if( bMake )
    {
        ui->GBArrangment->setTitle( "Make Arranged Keys" );
        ui->LBKeyCount->setVisible( true );
        ui->EditKeyCount->setVisible( true );
    }
    else
    {
        ui->GBArrangment->setTitle( "Modify Arrangement" );
        ui->LBKeyCount->setVisible( false );
        ui->EditKeyCount->setVisible( false );
    }

//    m_edtKeyCount.SetFloatStyle( false );
//    m_edtKeyCount.SetRange( 2, 30 );

    updateUnit();

//    pIconArray->copy(QRect( i*pIconArray->width()/2, 0, pIconArray->width()/2, pIconArray->height() ))
}

QArrangeHelperWidget::~QArrangeHelperWidget()
{
    delete ui;

	if( m_pImageArrangePicture )
		delete m_pImageArrangePicture;
	m_pImageArrangePicture = NULL;
}

void QArrangeHelperWidget::setUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight )
{
	m_eUnit = eUnit;
	m_dD2PScaleWidth = dScaleWidth;
	m_dD2PScaleHeight = dScaleHeight;
}

void QArrangeHelperWidget::updateUnit()
{
	switch ( m_eUnit )
	{
	case UnitRes:
//		m_edtKeyWidth.SetFloatStyle( false, false );
//		m_edtKeyHeight.SetFloatStyle( false, false );
//		m_edtKeyInterval.SetFloatStyle( false, false );
		break;
	case UnitMM:
//		m_edtKeyWidth.SetFloatStyle( true, false );
//		m_edtKeyHeight.SetFloatStyle( true, false );
//		m_edtKeyInterval.SetFloatStyle( true, false );
		break;
	}
//	UpdateData( false );
}

void QArrangeHelperWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;

    painter.begin( this );

    QRect rcBody( ui->LBArrangePicture->x(), ui->LBArrangePicture->y(), ui->LBArrangePicture->width(), ui->LBArrangePicture->height() );

	if( m_pImageArrangePicture )
	{
        int nPicW = m_pImageArrangePicture->width() / 2;
        int nPicH = m_pImageArrangePicture->height();

        painter.drawPixmap( rcBody, *m_pImageArrangePicture, QRect( ui->RBHorizontal->isChecked() ? 0 : nPicW, 0, nPicW, nPicH ) );
	}

    painter.end();
}

void QArrangeHelperWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Return )
	{
        setFocus();
	}
}

void QArrangeHelperWidget::on_RBHorizontal_clicked()
{
    ui->RBVertical->setChecked( false );

    update();
}

void QArrangeHelperWidget::on_RBVertical_clicked()
{
    ui->RBHorizontal->setChecked( false );

    update();
}

void QArrangeHelperWidget::on_EditKeyCount_editingFinished()
{
    QPalette plt( ui->EditKeyCount->palette() );
    plt.setColor( QPalette::Text, m_strKeycount == ui->EditKeyCount->text() ? Qt::black : Qt::red );
    ui->EditKeyCount->setPalette( plt );
}

void QArrangeHelperWidget::on_EditWidth_editingFinished()
{
    QPalette plt( ui->EditWidth->palette() );
    plt.setColor( QPalette::Text, m_strWidth == ui->EditWidth->text() ? Qt::black : Qt::red );
    ui->EditWidth->setPalette( plt );
}

void QArrangeHelperWidget::on_EditHeight_editingFinished()
{
    QPalette plt( ui->EditHeight->palette() );
    plt.setColor( QPalette::Text, m_strHeight == ui->EditHeight->text() ? Qt::black : Qt::red );
    ui->EditHeight->setPalette( plt );
}

void QArrangeHelperWidget::on_EditInterval_editingFinished()
{
    QPalette plt( ui->EditInterval->palette() );
    plt.setColor( QPalette::Text, m_strInterval == ui->EditInterval->text() ? Qt::black : Qt::red );
    ui->EditInterval->setPalette( plt );
}

void QArrangeHelperWidget::on_BtnApply_clicked()
{
    if( ui->RBHorizontal->isChecked() )
    {
        emit generateKeys( KeyArrangeHorizontal, ui->EditKeyCount->text().toInt(), ui->EditWidth->text().toInt(),
                           ui->EditHeight->text().toInt(), ui->EditInterval->text().toInt() );
    }
    else if( ui->RBVertical->isChecked() )
    {
        emit generateKeys( KeyArrangeVertical, ui->EditKeyCount->text().toInt(), ui->EditWidth->text().toInt(),
                           ui->EditHeight->text().toInt(), ui->EditInterval->text().toInt() );
    }

    close();
}

void QArrangeHelperWidget::on_BtnCancel_clicked()
{
    close();
}
