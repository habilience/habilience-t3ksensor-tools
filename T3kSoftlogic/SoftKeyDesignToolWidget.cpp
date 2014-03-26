#include "SoftKeyDesignToolWidget.h"
#include "ui_SoftKeyDesignToolWidget.h"

#include "T3kSoftlogicDlg.h"

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


QSoftKeyDesignToolWidget::QSoftKeyDesignToolWidget(QVector<CSoftkey*>* pSelectedKeys, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftKeyDesignToolWidget)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint );

    m_dD2PScaleWidth = 0.0;
    m_dD2PScaleHeight = 0.0;

	m_eUnit = UnitRes;

    m_pvSelectedKeys = pSelectedKeys;

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

    m_pLayoutToolWidget = new QLayoutToolWidget( pSelectedKeys, this );
    m_pLayoutToolWidget->hide();
    connect( m_pLayoutToolWidget, &QLayoutToolWidget::generateKeys, this, &QSoftKeyDesignToolWidget::generateKeys );

    connect( m_pLayoutToolWidget, &QLayoutToolWidget::alignSelectedKeys, this, &QSoftKeyDesignToolWidget::alignSelectedKeys );
    connect( m_pLayoutToolWidget, &QLayoutToolWidget::adjustSizeSelectedKeys, this, &QSoftKeyDesignToolWidget::adjustSizeSelectedKeys );
    connect( m_pLayoutToolWidget, &QLayoutToolWidget::distribSelectKeys, this, &QSoftKeyDesignToolWidget::distribSelectKeys );
    connect( m_pLayoutToolWidget, &QLayoutToolWidget::reorderKeys, this, &QSoftKeyDesignToolWidget::reorderKeys );

    connect( m_pLayoutToolWidget, &QLayoutToolWidget::updateLayoutButton, this, &QSoftKeyDesignToolWidget::onUpdateLayoutButton );
    connect( this, &QSoftKeyDesignToolWidget::closeWidget, m_pLayoutToolWidget, &QLayoutToolWidget::close );

    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "SoftkeyDesignTool_Pos" ).toString();
    settings.endGroup();

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

    m_pLayoutToolWidget->setUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );

    ui->CBVisible->addItem( "True" );
    ui->CBVisible->addItem( "False" );
    ui->CBVisible->setEnabled( false );
    ui->EditName->setEnabled( false );

    ui->BtnShowToolbar->setText( m_pLayoutToolWidget->isVisible() ? "Hide Layout ToolBar" : "Show Layout ToolBar" );

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
//		m_edtPosX.SetFloatStyle( false );
//		m_edtPosY.SetFloatStyle( false );
//		m_edtWidth.SetFloatStyle( false, false );
//		m_edtHeight.SetFloatStyle( false, false );
		break;
	case UnitMM:
//		m_edtPosX.SetFloatStyle( true );
//		m_edtPosY.SetFloatStyle( true );
//		m_edtWidth.SetFloatStyle( true, false );
//		m_edtHeight.SetFloatStyle( true, false );
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

    m_pLayoutToolWidget->setUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );

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

    m_pLayoutToolWidget->updateUIButtonState( nSelectKeyCount );

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

        ui->BtnReorder->setEnabled( false );

        ui->EditPosX->clear();
        ui->EditPosY->clear();
        ui->EditWidth->clear();
        ui->EditHeight->clear();
    }
    else if ( SelectKeys.count() == 1 )
    {
        CSoftkey* key = SelectKeys.at(0);
        ui->EditName->setText( key->getName() );
        ui->CBVisible->setCurrentIndex( key->getShow() ? 0 : 1 );
        QRect rc( key->getPosition() );

        if( m_eUnit == UnitMM )
        {
            double dPX = rc.left() * m_dD2PScaleWidth;
            double dPY = rc.top() * m_dD2PScaleHeight;
            double dW = rc.width() * m_dD2PScaleWidth;
            double dH = rc.height() * m_dD2PScaleHeight;

            ui->EditPosX->setText( QString("%1").arg(dPX, 0, 'f', 1) );
            ui->EditPosY->setText( QString("%1").arg(dPY, 0, 'f', 1) );
            ui->EditWidth->setText( QString("%1").arg(dW, 0, 'f', 1) );
            ui->EditHeight->setText( QString("%1").arg(dH, 0, 'f', 1) );

            m_rcOld.setRect( dPX+.5, dPY+.5, dW+.5, dH+.5 );
        }
        else
        {
            qDebug() << QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.width()).arg(rc.height());

            ui->EditPosX->setText( QString::number( rc.left() ) );
            ui->EditPosY->setText( QString::number( rc.top() ) );
            ui->EditWidth->setText( QString::number( rc.width() ) );
            ui->EditHeight->setText( QString::number( rc.height() ) );

            m_rcOld.setRect( ui->EditPosX->text().toDouble()+.5, ui->EditPosY->text().toDouble()+.5,
                             ui->EditWidth->text().toDouble()+.5, ui->EditHeight->text().toDouble()+.5 );
        }

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

        if( m_eUnit == UnitMM )
        {
            double dPX = rcMerge.left() * m_dD2PScaleWidth;
            double dPY = rcMerge.top() * m_dD2PScaleHeight;
            double dW = rcMerge.width() * m_dD2PScaleWidth;
            double dH = rcMerge.height() * m_dD2PScaleHeight;

            ui->EditPosX->setText( QString("%1").arg(dPX, 0, 'f', 1) );
            ui->EditPosY->setText( QString("%1").arg(dPY, 0, 'f', 1) );
            ui->EditWidth->setText( QString("%1").arg(dW, 0, 'f', 1) );
            ui->EditHeight->setText( QString("%1").arg(dH, 0, 'f', 1) );

            m_rcOld.setRect( dPX+.5, dPY+.5, dW+.5, dH+.5 );
        }
        else
        {
            ui->EditPosX->setText( QString::number( rcMerge.left() ) );
            ui->EditPosY->setText( QString::number( rcMerge.top() ) );
            ui->EditWidth->setText( QString::number( rcMerge.width() ) );
            ui->EditHeight->setText( QString::number( rcMerge.height() ) );

            m_rcOld.setRect( ui->EditPosX->text().toDouble()+.5, ui->EditPosY->text().toDouble()+.5,
                             ui->EditWidth->text().toDouble()+.5, ui->EditHeight->text().toDouble()+.5 );
        }

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

        ui->BtnReorder->setEnabled( true );
    }
}

void QSoftKeyDesignToolWidget::updateLayoutButton(bool bVisible)
{
    ui->BtnShowToolbar->setText( bVisible ? "Hide Layout ToolBar" : "Show Layout ToolBar" );
}

void QSoftKeyDesignToolWidget::EditModified()
{
    QRect rcNew;

    if( m_eUnit == UnitMM )
    {
        rcNew.setLeft( (int)(ui->EditPosX->text().toDouble() / m_dD2PScaleWidth + .5) );
        rcNew.setTop( (int)(ui->EditPosY->text().toDouble() / m_dD2PScaleWidth + .5) );
        rcNew.setWidth( (int)(ui->EditWidth->text().toDouble() / m_dD2PScaleWidth + .5) );
        rcNew.setHeight( (int)(ui->EditHeight->text().toDouble() / m_dD2PScaleWidth + .5) );
    }
    else
    {
        rcNew.setLeft( (int)(ui->EditPosX->text().toDouble()+.5) );
        rcNew.setTop( (int)(ui->EditPosY->text().toDouble()+.5) );
        rcNew.setWidth( (int)(ui->EditWidth->text().toDouble()+.5) );
        rcNew.setHeight( (int)(ui->EditHeight->text().toDouble()+.5) );
    }

    emit recalcSelectionKeys( m_rcOld, rcNew );
}

void QSoftKeyDesignToolWidget::showEvent(QShowEvent *)
{
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Settings" );
    bool bShow = settings.value( "ShowLayoutToolBar", false ).toBool();
    settings.endGroup();

    if( bShow )
        m_pLayoutToolWidget->show();
    else
        m_pLayoutToolWidget->hide();

    updateLayoutButton( bShow );
}

void QSoftKeyDesignToolWidget::closeEvent(QCloseEvent *)
{
    QRect rc( x(), y(), width(), height() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "SoftkeyDesignTool_Pos", str );
    settings.endGroup();

    QString strShow( ui->BtnShowToolbar->text() );
    bool bShow = false;
    if( strShow.contains( "Hide" ) )
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
    if( m_pLayoutToolWidget->isVisible() )
        m_pLayoutToolWidget->hide();
    else
        m_pLayoutToolWidget->show();

    updateLayoutButton( m_pLayoutToolWidget->isVisible() );
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

void QSoftKeyDesignToolWidget::on_CBUnit_activated(int index)
{
    if ( index < 0 ) return;

    ScreenUnit eNewUnit = (ScreenUnit)ui->CBUnit->itemData(index).toInt();
    if ( eNewUnit != m_eUnit )
        m_eUnit = eNewUnit;

    updateUnit();

    emit updateScreen();

    m_pLayoutToolWidget->setUnit( m_eUnit, m_dD2PScaleWidth, m_dD2PScaleHeight );
}

void QSoftKeyDesignToolWidget::on_EditName_editingFinished()
{
    if( !m_pvSelectedKeys || m_pvSelectedKeys->count() != 1 ) return;

    CSoftkey* pKey = m_pvSelectedKeys->at(0);
    pKey->setName( ui->EditName->text() );

    emit invalidateKey( pKey );
}

void QSoftKeyDesignToolWidget::on_CBVisible_activated(int index)
{
    if( index < 0 ) return;

    bool bVisible = (index == 0) ? true : false;

    if( !m_pvSelectedKeys ) return;

    for ( int nI=0 ; nI<m_pvSelectedKeys->count() ; nI++ )
    {
        CSoftkey* pKey = m_pvSelectedKeys->at(nI);
        pKey->setShow( bVisible );
        emit invalidateKey( pKey );
    }
}

void QSoftKeyDesignToolWidget::on_EditPosX_textEdited(const QString &/*arg1*/)
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

void QSoftKeyDesignToolWidget::on_EditPosY_textEdited(const QString &/*arg1*/)
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

void QSoftKeyDesignToolWidget::on_EditWidth_textEdited(const QString &/*arg1*/)
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

void QSoftKeyDesignToolWidget::on_EditHeight_textEdited(const QString &/*arg1*/)
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

        ui->EditPosX->clear();
        ui->EditPosY->clear();
        ui->EditWidth->clear();
        ui->EditHeight->clear();
        return;
    }

    ui->BtnRemove->setEnabled( true );

    ui->BtnGroup->setEnabled( true );

    ui->BtnUngroup->setEnabled( bGroup );
}

void QSoftKeyDesignToolWidget::onUpdateLayoutButton(bool bVisible)
{
    updateLayoutButton(bVisible);
}
