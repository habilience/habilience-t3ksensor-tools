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

    m_pImageArrangePicture = new QPixmap( ":/T3kSoftlogicRes/resources/PNG_PICTURE_AUTO_ARRANGE.png" );

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

    m_strWidth = ui->EditWidth->text();
    m_strHeight = ui->EditHeight->text();
    m_strInterval = ui->EditInterval->text();

    if( m_eUnit )
    {
        double dW = ui->EditWidth->text().toDouble() * m_dD2PScaleWidth;
        double dH = ui->EditHeight->text().toDouble() * m_dD2PScaleHeight;
        double dI = ui->EditInterval->text().toDouble() * (ui->RBHorizontal->isChecked() ? m_dD2PScaleWidth : m_dD2PScaleHeight);

        ui->EditWidth->setText( QString("%1").arg(dW, 0, 'f', 1) );
        ui->EditHeight->setText( QString("%1").arg(dH, 0, 'f', 1) );
        ui->EditInterval->setText( QString("%1").arg(dI, 0, 'f', 1) );
    }
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
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditKeyCount->setPalette( plt );

    if( ui->EditKeyCount->text().toInt() > 30 )
        ui->EditKeyCount->setText( "30" );
}

void QArrangeHelperWidget::on_EditWidth_editingFinished()
{
    QPalette plt( ui->EditWidth->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditWidth->setPalette( plt );

    if( m_eUnit == UnitMM )
    {
        int nNewW = int(ui->EditWidth->text().toDouble() / m_dD2PScaleWidth);
        if( m_strWidth.toInt() != nNewW )
            m_strWidth = QString("%1").arg(nNewW);
    }
    else
        m_strWidth = ui->EditWidth->text();
}

void QArrangeHelperWidget::on_EditHeight_editingFinished()
{
    QPalette plt( ui->EditHeight->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditHeight->setPalette( plt );

    if( m_eUnit == UnitMM )
    {
        int nNewH = int(ui->EditHeight->text().toDouble() / m_dD2PScaleHeight);
        if( m_strHeight.toInt() != nNewH )
            m_strHeight = QString("%1").arg(nNewH);
    }
    else
        m_strHeight = ui->EditHeight->text();
}

void QArrangeHelperWidget::on_EditInterval_editingFinished()
{
    QPalette plt( ui->EditInterval->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditInterval->setPalette( plt );

    if( m_eUnit == UnitMM )
    {
        int nNewI = int(ui->EditInterval->text().toDouble() / (ui->RBHorizontal->isChecked() ? m_dD2PScaleWidth : m_dD2PScaleHeight));
        if( m_strInterval.toInt() != nNewI )
            m_strInterval = QString("%1").arg(nNewI);
    }
    else
        m_strInterval = ui->EditInterval->text();
}

void QArrangeHelperWidget::on_BtnApply_clicked()
{
    if( ui->RBHorizontal->isChecked() )
    {
        emit generateKeys( KeyArrangeHorizontal, ui->EditKeyCount->text().toInt(), m_strWidth.toInt(),
                           m_strHeight.toInt(), m_strInterval.toInt() );
    }
    else if( ui->RBVertical->isChecked() )
    {
        emit generateKeys( KeyArrangeVertical, ui->EditKeyCount->text().toInt(),  m_strWidth.toInt(),
                           m_strHeight.toInt(), m_strInterval.toInt() );
    }

    close();
}

void QArrangeHelperWidget::on_BtnCancel_clicked()
{
    close();
}
void QArrangeHelperWidget::on_EditKeyCount_textEdited(const QString &)
{
    QPalette plt( ui->EditKeyCount->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditKeyCount->setPalette( plt );
}

void QArrangeHelperWidget::on_EditWidth_textEdited(const QString &)
{
    QPalette plt( ui->EditWidth->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditWidth->setPalette( plt );
}

void QArrangeHelperWidget::on_EditHeight_textEdited(const QString &)
{
    QPalette plt( ui->EditHeight->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditHeight->setPalette( plt );
}

void QArrangeHelperWidget::on_EditInterval_textEdited(const QString &)
{
    QPalette plt( ui->EditInterval->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditInterval->setPalette( plt );
}
