#include "LayoutToolWidget.h"
#include "ui_LayoutToolWidget.h"

#include <QSettings>
#include <QDesktopWidget>


QLayoutToolWidget::QLayoutToolWidget(QVector<CSoftkey*>* pSelectedKeys, QWidget* parent /*=NULL*/) :
    QDialog(parent),
    ui(new Ui::LayoutToolWidget)
{
    ui->setupUi(this);

    ui->BtnReorder->setVisible( false );

    setWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint );

    m_eUnit = UnitRes;

    m_pSelectedKeys = pSelectedKeys;

    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "LayoutToolBar_Pos" ).toString();
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
}

QLayoutToolWidget::~QLayoutToolWidget()
{
    delete ui;
}

static bool s_bEnableLayout = true;
void QLayoutToolWidget::updateUIButtonState( int nSelectKeyCount )
{
    QWidget* pWnd;

    bool bEnableLayout;
	if( nSelectKeyCount > 1 )
        bEnableLayout = true;
	else
        bEnableLayout = false;

	//if( s_bEnableLayout != bEnableLayout )
	{
        pWnd = ui->BtnLeft;

        pWnd->setEnabled( bEnableLayout );

        pWnd = ui->BtnCenter;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnRight;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnTop;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnMiddle;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnBottom;
        pWnd->setEnabled( bEnableLayout );

        pWnd = ui->BtnWidth;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnHeight;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnBoth;
        pWnd->setEnabled( bEnableLayout );

        pWnd = ui->BtnHorizon;
        pWnd->setEnabled( bEnableLayout );
        pWnd = ui->BtnVertical;
        pWnd->setEnabled( bEnableLayout );

		s_bEnableLayout = bEnableLayout;
	}
}

void QLayoutToolWidget::setUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight )
{
    m_eUnit = eUnit;
    m_dD2PScaleWidth = dScaleWidth;
    m_dD2PScaleHeight = dScaleHeight;
}

void QLayoutToolWidget::closeEvent(QCloseEvent *)
{
    emit updateLayoutButton( false );

    QRect rc( x(), y(), width(), height() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "LayoutToolBar_Pos", str );
    settings.endGroup();
}

void QLayoutToolWidget::on_BtnLeft_clicked()
{
    emit alignSelectedKeys( KeyAlignLeft );
}

void QLayoutToolWidget::on_BtnCenter_clicked()
{
    emit alignSelectedKeys( KeyAlignCenter );
}

void QLayoutToolWidget::on_BtnRight_clicked()
{
    emit alignSelectedKeys( KeyAlignRight );
}

void QLayoutToolWidget::on_BtnTop_clicked()
{
    emit alignSelectedKeys( KeyAlignTop );
}

void QLayoutToolWidget::on_BtnMiddle_clicked()
{
    emit alignSelectedKeys( KeyAlignMiddle );
}

void QLayoutToolWidget::on_BtnBottom_clicked()
{
    emit alignSelectedKeys( KeyAlignBottom );
}

void QLayoutToolWidget::on_BtnWidth_clicked()
{
    emit adjustSizeSelectedKeys( AdjustSizeSameWidth );
}

void QLayoutToolWidget::on_BtnHeight_clicked()
{
    emit adjustSizeSelectedKeys( AdjustSizeSameHeight );
}

void QLayoutToolWidget::on_BtnBoth_clicked()
{
    emit adjustSizeSelectedKeys( AdjustSizeSameBoth );
}

void QLayoutToolWidget::on_BtnHorizon_clicked()
{
    emit distribSelectKeys( DistribHorzEqualGap );
}

void QLayoutToolWidget::on_BtnVertical_clicked()
{
    emit distribSelectKeys( DistribVertEqualGap );
}

void QLayoutToolWidget::on_BtnAraange_clicked()
{   
    Q_ASSERT( m_pSelectedKeys );
    QArrangeHelperWidget widget( !m_pSelectedKeys->count(), this );
    connect( &widget, &QArrangeHelperWidget::generateKeys, this, &QLayoutToolWidget::generateKeys );
    widget.setUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );

    widget.exec();
}

void QLayoutToolWidget::on_BtnReorder_clicked()
{
    emit reorderKeys();
}
