#include "SoftKeyDesignToolWidget.h"
#include "ui_SoftKeyDesignToolWidget.h"

#include "T3kSoftlogicDlg.h"
#include "GraphicsKeyItem.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QtEvents>
#include <QMessageBox>
#include <QGraphicsItemGroup>
#include <QDebug>

#if 1
#define DEFAULT_SOFTKEY		"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEFAULT_SOFTLOGIC	"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#else
#define DEFAULT_SOFTKEY		"@>Co:X:1_oM@[<o:XA?ooMA84o:XHMooMATmo:XO\\OoMB1fo:XVj_oMBN`o:X^8ooMBkXo:XeFooMCHRo:XlUOoM0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEFAULT_SOFTLOGIC	"10P02T@0Q010H0R00PH0S00;@0T0<GU0U02\\I0V03XE`W0<CW@W00C`0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000P8LCW"
#endif

//#include <afxpriv.h>
//#include <float.h>

//void QSoftKeyDesignToolWidget::MYDDX_Text(CDataExchange* pDX, int nIDC, double& dValue, BOOL bIsScaleX)
//{
//	double	dValueDouble = dValue;
//	int		nValueInt;
//	if (pDX->m_bSaveAndValidate)
//	{
//		switch ( m_eUnit )
//		{
//		case UnitRes:
//			MyAfxTextIntFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &nValueInt);
//			dValue = (double)nValueInt;
//			break;
//		case UnitMM:
//			MyAfxTextFloatFormat(pDX, nIDC, &dValueDouble, dValueDouble, DBL_DIG);
//			if ( bIsScaleX )
//			{
//				dValue = dValueDouble / m_dD2PScaleWidth;
//			}
//			else
//			{
//				dValue = dValueDouble / m_dD2PScaleHeight;
//			}
//			break;
//		}
//	}
//	else
//	{
//		switch ( m_eUnit )
//		{
//		case UnitRes:
//			nValueInt = int(dValue + .5f);
//			MyAfxTextIntFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, nValueInt);
//			break;
//		case UnitMM:
//			if ( bIsScaleX )
//			{
//				dValueDouble = dValue * m_dD2PScaleWidth;
//			}
//			else
//			{
//				dValueDouble = dValue * m_dD2PScaleHeight;
//			}
//			MyAfxTextFloatFormat(pDX, nIDC, &dValueDouble, dValueDouble, DBL_DIG);
//			break;
//		}
//	}
//}

QSoftKeyDesignToolWidget::QSoftKeyDesignToolWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftKeyDesignToolWidget)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Tool|Qt::WindowStaysOnTopHint );
    setWindowModality(Qt::NonModal);

    m_dD2PScaleWidth = 0.0;
    m_dD2PScaleHeight = 0.0;

	m_eUnit = UnitRes;

    m_pSelectedKeys = NULL;

    ui->BtnRemove->setEnabled( false );
    ui->BtnGroup->setEnabled( false );
    ui->BtnUngroup->setEnabled( false );
    ui->BtnReorder->setEnabled( false );
    ui->EditName->setEnabled( false );
    ui->CBVisible->setEnabled( false );
    ui->EditPosX->setEnabled( false );
    ui->EditPosY->setEnabled( false );
    ui->EditWidth->setEnabled( false );
    ui->EditHeight->setEnabled( false );

//	if( !m_wndLayoutToolBar.Create( m_pWndDesignCanvas, this ) )
//		return -1;

    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "SoftkeyDesignTool_Pos" ).toString();

    QDesktopWidget desktop;
    QRect rcScreen( desktop.screenGeometry( desktop.primaryScreen() ) );

    if( !strPos.isEmpty() )
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

            QRect rcCaption = rcWin;
            int nTitleH = window()->style()->pixelMetric(QStyle::PM_TitleBarHeight);
            int nFrameW = window()->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
            rcCaption.adjust( nFrameW, nFrameW, -nFrameW, -nFrameW );
            rcCaption.setBottom( rcCaption.top() + nTitleH );
            rcCaption.setLeft( rcCaption.left() + rcCaption.height() );
            rcCaption.setRight( rcCaption.right() - rcCaption.height() * 4 );

            if ( rcScreen.intersects( rcCaption ) )
                move( rcWin.left(), rcWin.top() );
            else
                move( rcScreen.center() - QRect(0,0,width()-1,height()-1).center() );
        }
        while ( false );
    }
    else
    {
        move( rcScreen.center() - QRect(0,0,width()-1,height()-1).center() );
    }
//

    ui->CBUnit->clear();

    ui->CBUnit->addItem( "res", UnitRes );
    ui->CBUnit->addItem( "mm", UnitMM );

    if ( (m_dD2PScaleWidth != 0.0) && (m_dD2PScaleHeight != 0.0) )
    {
        ui->CBUnit->setVisible( true );
        ui->CBUnit->setCurrentIndex( 1 );
        m_eUnit = UnitMM;
    }
    else
    {
        ui->CBUnit->setVisible( false );
        m_eUnit = UnitRes;
    }

//	m_wndLayoutToolBar.SetUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );

    ui->CBVisible->addItem( "True" );
    ui->CBVisible->addItem( "False" );
    ui->CBVisible->setEnabled( false );
    ui->EditName->setEnabled( false );

    settings.beginGroup( "Settings" );
    bool bShow = settings.value( "ShowLayoutToolBar" ).toBool();
    settings.endGroup();

//	if ( m_wndLayoutToolBar )
//	{
//		m_wndLayoutToolBar.ShowWindow( bShow ? SW_SHOW : SW_HIDE );
//		SetDlgItemText( IDC_BTN_SHOW_LAYOUT_TOOLBAR, m_wndLayoutToolBar.IsWindowVisible() ? _T("Hide Layout ToolBar") : _T("Show Layout ToolBar") );
//	}

    ui->EditPosX->setInputMethodHints( Qt::ImhDialableCharactersOnly );
    ui->EditPosY->setInputMethodHints( Qt::ImhDialableCharactersOnly );
    ui->EditWidth->setInputMethodHints( Qt::ImhDialableCharactersOnly );
    ui->EditHeight->setInputMethodHints( Qt::ImhDialableCharactersOnly );

    updateUnit();
}

QSoftKeyDesignToolWidget::~QSoftKeyDesignToolWidget()
{
}

void QSoftKeyDesignToolWidget::updateUnit()
{
	switch ( m_eUnit )
	{
	case UnitRes:

//		m_edtPosX.SetFloatStyle( FALSE );
//		m_edtPosY.SetFloatStyle( FALSE );
//		m_edtWidth.SetFloatStyle( FALSE, FALSE );
//		m_edtHeight.SetFloatStyle( FALSE, FALSE );
		break;
	case UnitMM:
//		m_edtPosX.SetFloatStyle( TRUE );
//		m_edtPosY.SetFloatStyle( TRUE );
//		m_edtWidth.SetFloatStyle( TRUE, FALSE );
//		m_edtHeight.SetFloatStyle( TRUE, FALSE );
		break;
	}
}

void QSoftKeyDesignToolWidget::setScaleFactor(double dScaleWidth, double dScaleHeight )
{
    m_dD2PScaleWidth = dScaleWidth;
    m_dD2PScaleHeight = dScaleHeight;

    if ( (m_dD2PScaleWidth != 0.0) && (m_dD2PScaleHeight != 0.0) )
    {
        ui->CBUnit->setVisible( true );
        ui->CBUnit->setCurrentIndex( 1 );
        m_eUnit = UnitMM;
    }
    else
    {
        ui->CBUnit->setVisible( false );
        m_eUnit = UnitRes;
    }

    //m_wndLayoutToolBar.SetUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );

    updateUnit();
}

static bool s_bEnableRemove = true;
void QSoftKeyDesignToolWidget::updateUIButtonState( int nSelectKeyCount, GroupStatus eGroupStatus, QVector<CSoftkey*>& SelectKeys )
{
    bool bEnableRemove;
    if ( nSelectKeyCount >= 1 )
        bEnableRemove = true;
    else
        bEnableRemove = false;

    if ( s_bEnableRemove != bEnableRemove )
    {
        ui->BtnRemove->setEnabled( bEnableRemove );
        s_bEnableRemove = bEnableRemove;
    }

//	if ( m_wndLayoutToolBar )
//	{
//		m_wndLayoutToolBar.UpdateUIButtonState( nSelectKeyCount );
//	}

    switch ( eGroupStatus )
    {
    case EnableGroup:
        ui->BtnGroup->setEnabled( true );
        ui->BtnUngroup->setEnabled( false );
        break;
    case EnableUngroup:
        ui->BtnGroup->setEnabled( false );
        ui->BtnUngroup->setEnabled( true );
        break;
    case DisableGroup:
        ui->BtnGroup->setEnabled( false );
        ui->BtnUngroup->setEnabled( false );
        break;
    }

    if ( SelectKeys.count()  == 0 )
    {
        m_rcOld.setRect( 0,0,0,0 );

        ui->EditName->clear();
        ui->CBVisible->setCurrentIndex( -1 );
        ui->CBVisible->setEnabled( false );
        ui->EditName->setEnabled( false );
        ui->EditPosX->setEnabled( false );
        ui->EditPosY->setEnabled( false );
        ui->EditWidth->setEnabled( false );
        ui->EditHeight->setEnabled( false );
    }
    else if ( SelectKeys.count() == 1 )
    {
        CSoftkey* key = SelectKeys.at(0);
        ui->EditName->setText( key->getName() );
        ui->CBVisible->setCurrentIndex( key->getShow() ? 1 : 0 );
        QRect rc( key->getPosition() );
        ui->EditPosX->setText( QString::number( rc.left() ) );
        ui->EditPosY->setText( QString::number( rc.top() ) );
        ui->EditWidth->setText( QString::number( rc.width()-1 ) );
        ui->EditHeight->setText( QString::number( rc.height()-1 ) );

        m_rcOld.setRect( ui->EditPosX->text().toDouble()+.5, ui->EditPosY->text().toDouble()+.5,
                         ui->EditWidth->text().toDouble()+.5, ui->EditHeight->text().toDouble()+.5 );

        ui->CBVisible->setEnabled( true );
        ui->EditName->setEnabled( true );
        ui->EditPosX->setEnabled( true );
        ui->EditPosY->setEnabled( true );
        ui->EditWidth->setEnabled( true );
        ui->EditHeight->setEnabled( true );
    }
    else if( SelectKeys.count() > 1 )
    {
        int nVisible = -1;
        QRect rcMerge;
        for ( int nI=0 ; nI<SelectKeys.count() ; nI++ )
        {
            CSoftkey* key = SelectKeys.at(nI);
            rcMerge = rcMerge.united( key->getPosition() );
            if ( nVisible < 0 )
                nVisible = key->getShow();
            else
            {
                if ( nVisible != key->getShow() )
                {
                    nVisible = 2;
                }
            }
        }

        ui->EditPosX->setText( QString::number( rcMerge.left() ) );
        ui->EditPosY->setText( QString::number( rcMerge.top() ) );
        ui->EditWidth->setText( QString::number( rcMerge.width()-1 ) );
        ui->EditHeight->setText( QString::number( rcMerge.height()-1 ) );

        m_rcOld.setRect( ui->EditPosX->text().toDouble()+.5, ui->EditPosY->text().toDouble()+.5,
                         ui->EditWidth->text().toDouble()+.5, ui->EditHeight->text().toDouble()+.5 );

        switch ( nVisible )
        {
        case 0:
            ui->CBVisible->setCurrentIndex( 0 );
            break;
        case 1:
            ui->CBVisible->setCurrentIndex( 1 );
            break;
        case -1:
        case 2:
            ui->CBVisible->setCurrentIndex( -1 );
            break;
        }

        ui->EditName->clear();

        ui->CBVisible->setEnabled( true );
        ui->EditName->setEnabled( true );
        ui->EditPosX->setEnabled( true );
        ui->EditPosY->setEnabled( true );
        ui->EditWidth->setEnabled( true );
        ui->EditHeight->setEnabled( true );
    }
}

void QSoftKeyDesignToolWidget::updateLayoutButton()
{
    //ui->BtnShowToolbar->setText( m_wndLayoutToolBar.isVisible() ? "Hide Layout ToolBar" : "Show Layout ToolBar" );
}

void QSoftKeyDesignToolWidget::EditModified()
{
    QRect rcNew;
    rcNew.setLeft( (int)(ui->EditPosX->text().toDouble()+0.5) );
    rcNew.setTop( (int)(ui->EditPosY->text().toDouble()+0.5) );
    rcNew.setWidth( (int)(ui->EditWidth->text().toDouble()+0.5) );
    rcNew.setHeight( (int)(ui->EditHeight->text().toDouble()+0.5) );

    emit recalcSelectionKeys( m_rcOld, rcNew );
}

void QSoftKeyDesignToolWidget::closeEvent(QCloseEvent *)
{
    QRect rc( geometry() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "SoftkeyDesignTool_Pos", str );
    settings.endGroup();

    QString strShow( ui->BtnShowToolbar->text() );
    bool bShow = false;
    if( strShow.contains( "Hide" ) >= 0 )
        bShow = true;

    settings.beginGroup( "Settings" );
    settings.setValue( "ShowLayoutToolBar", bShow );
    settings.endGroup();

    emit closeWidget();
}
void QSoftKeyDesignToolWidget::on_BtnFitScreen_clicked()
{
    emit screenSize( QKeyDesignWidget::ScreenSizeFit );
}

void QSoftKeyDesignToolWidget::on_BtnShowToolbar_clicked()
{
//	if ( m_wndLayoutToolBar )
//	{
//		m_wndLayoutToolBar.ShowWindow( m_wndLayoutToolBar.IsWindowVisible() ? SW_HIDE : SW_SHOW );
//        updateLayoutButton();
//	}
}

void QSoftKeyDesignToolWidget::on_BtnAdd_clicked()
{
    emit addNewKey();
}

void QSoftKeyDesignToolWidget::on_BtnRemove_clicked()
{
    emit removeSelectedKeys();
}

void QSoftKeyDesignToolWidget::on_BtnGroup_clicked()
{
    emit groupSelectedKeys();
}

void QSoftKeyDesignToolWidget::on_BtnUngroup_clicked()
{
    emit ungroupSelectedKeys( true );
}

void QSoftKeyDesignToolWidget::on_BtnReorder_clicked()
{
    emit reorderKeys();
}

void QSoftKeyDesignToolWidget::on_CBUnit_currentIndexChanged(int index)
{
    if ( index < 0 ) return;

    ScreenUnit eNewUnit = (ScreenUnit)ui->CBUnit->itemData(index).toInt();
    if ( eNewUnit != m_eUnit )
        m_eUnit = eNewUnit;

    updateUnit();

    emit updateScreen();

    //m_wndLayoutToolBar.SetUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );
}

void QSoftKeyDesignToolWidget::on_EditName_editingFinished()
{
    if( !m_pSelectedKeys && m_pSelectedKeys->childItems().size() != 1 ) return;

    QGraphicsKeyItem* pKey = (QGraphicsKeyItem*) m_pSelectedKeys->childItems().at(0);
    pKey->setName( ui->EditName->text() );
    pKey->update();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys[pKey->getID()]->setName( ui->EditName->text() );
}

void QSoftKeyDesignToolWidget::on_CBVisible_currentIndexChanged(int index)
{
    if( index < 0 ) return;

    bool bVisible = (index == 0) ? true : false;

    if( !m_pSelectedKeys ) return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    for ( int nI=0 ; nI<m_pSelectedKeys->childItems().size() ; nI++ )
    {
        QGraphicsKeyItem* pKey = (QGraphicsKeyItem*) m_pSelectedKeys->childItems().at(nI);
//        pKey->onUpdateEnable( bVisible );

        Keys[pKey->getID()]->setShow( bVisible );
    }
}

void QSoftKeyDesignToolWidget::on_EditPosX_textChanged(const QString &arg1)
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditPosX->setPalette( plt );
}

void QSoftKeyDesignToolWidget::on_EditPosX_editingFinished()
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditPosX->setPalette( plt );

    EditModified();
}

void QSoftKeyDesignToolWidget::on_EditPosY_textChanged(const QString &arg1)
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditPosX->setPalette( plt );
}

void QSoftKeyDesignToolWidget::on_EditPosY_editingFinished()
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditPosX->setPalette( plt );

    EditModified();
}

void QSoftKeyDesignToolWidget::on_EditWidth_textChanged(const QString &arg1)
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditPosX->setPalette( plt );
}

void QSoftKeyDesignToolWidget::on_EditWidth_editingFinished()
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditPosX->setPalette( plt );

    EditModified();
}

void QSoftKeyDesignToolWidget::on_EditHeight_textChanged(const QString &arg1)
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::red );
    ui->EditPosX->setPalette( plt );
}

void QSoftKeyDesignToolWidget::on_EditHeight_editingFinished()
{
    QPalette plt( ui->EditPosX->palette() );
    plt.setColor( QPalette::Text, Qt::black );
    ui->EditPosX->setPalette( plt );

    EditModified();
}

void QSoftKeyDesignToolWidget::on_BtnReset_clicked()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    if ( QMessageBox::warning( this, "Warning", "Would you like to erase all softkey?", QMessageBox::Yes|QMessageBox::No ) == QMessageBox::No )
        return;

    Keys.load( DEFAULT_SOFTKEY, NULL, NULL );

    emit resetKeys();
}

void QSoftKeyDesignToolWidget::on_BtnMatch_clicked()
{
    emit screenSize( QKeyDesignWidget::ScreenSizeFull );
}

void QSoftKeyDesignToolWidget::on_BtnClose_clicked()
{
    emit closeWidget();
    close();
}

void QSoftKeyDesignToolWidget::onKeyStateCount(bool bAdd)
{
    ui->BtnAdd->setEnabled( bAdd );
}

void QSoftKeyDesignToolWidget::onSelectedKeys(bool bGroup, int nSelectedCount)
{
    if( !nSelectedCount )
    {
        ui->BtnRemove->setEnabled( false );
        ui->BtnGroup->setEnabled( false );
        ui->BtnUngroup->setEnabled( false );
        return;
    }

    ui->BtnRemove->setEnabled( true );

    ui->BtnGroup->setEnabled( true );

    ui->BtnUngroup->setEnabled( bGroup );
}
