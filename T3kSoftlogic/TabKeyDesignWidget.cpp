#include "TabKeyDesignWidget.h"
#include "ui_TabKeyDesignWidget.h"

#include "T3kCommonData.h"
#include "ui/QHoverComboBox.h"

#include <QComboBox>
#include <QDesktopWidget>


TabKeyDesignWidget::TabKeyDesignWidget(QWidget* parent /*=NULL*/) :
    QWidget(parent),
    ui(new Ui::TabKeyDesignWidget)
{
    ui->setupUi(this);

    m_DesignCanvasWidget.hide();

    ui->EditGPIOCount->setText( "0" );

    ui->TableGPIO->verticalHeader()->setVisible( true );
    ui->TableGPIO->horizontalHeader()->setVisible( true );

    ui->TableGPIO->setColumnCount( 2 );
    ui->TableGPIO->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Enable" ) );
    ui->TableGPIO->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Input/Output" ) );

    ui->TableGPIO->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->TableGPIO->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );

    ui->TableGPIO->setColumnWidth( 0, 100 );
    ui->TableGPIO->setColumnWidth( 1, ui->TableGPIO->width()-110 );

    connect( &m_DesignCanvasWidget, &QKeyDesignWidget::closeWidget, this, &TabKeyDesignWidget::updatePreview );
}

TabKeyDesignWidget::~TabKeyDesignWidget()
{
}

void TabKeyDesignWidget::verifySoftlogicGPIO(void)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

	int nGPIOIdx;
    const GPIOInfo* pInfo;
	CSoftlogic::PortType type;
	CSoftlogic::Port port;
	CSoftlogic* pLogic;

    for ( int i=0 ; i<Logics.getSize() ; i++ )
	{
		pLogic = Logics[i];

		if ( pLogic == NULL ) continue;

		port = pLogic->getIn1Port();

		if ( port >= CSoftlogic::epGpio0 && port < CSoftlogic::epSoftkey0 )
		{
			nGPIOIdx = int(port-CSoftlogic::epGpio0);
            pInfo = Keys.getGPIOInfo( nGPIOIdx );
			if ( !pInfo || !pInfo->bEnable )
			{
				pLogic->setIn1Port( CSoftlogic::epNoGate );
			}
		}

		port = pLogic->getIn2Port();

		if ( port >= CSoftlogic::epGpio0 && port < CSoftlogic::epSoftkey0 )
		{
			nGPIOIdx = int(port-CSoftlogic::epGpio0);
            pInfo = Keys.getGPIOInfo( nGPIOIdx );
			if ( !pInfo || !pInfo->bEnable )
			{
				pLogic->setIn2Port( CSoftlogic::epNoGate );
			}
		}

		type = pLogic->getOutPortType();
		if ( type == CSoftlogic::eptGPIO )
		{
			port = pLogic->getOutPort();
			nGPIOIdx = int(port - CSoftlogic::epGpio0);
            pInfo = Keys.getGPIOInfo( nGPIOIdx );
			if ( !pInfo || !pInfo->bOutput || !pInfo->bEnable )
			{
				pLogic->setOutPort( CSoftlogic::epNoGate );
			}
		}
	}
}

void TabKeyDesignWidget::refresh()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    int nGPIOCount = Keys.getGPIOCount();
	if ( nGPIOCount > 0 )
	{
        ui->EditGPIOCount->setText( QString("%1").arg(nGPIOCount) );
        if ( nGPIOCount != ui->TableGPIO->rowCount() )
		{
            ui->TableGPIO->clear();
            ui->TableGPIO->setRowCount( nGPIOCount );
            for ( int i=0 ; i<nGPIOCount ; i++ )
			{
                GPIOInfo* pInfo = Keys.getGPIOInfo(i);
				if ( pInfo )
				{
                    QHoverComboBox* cbEnable = new QHoverComboBox( ui->TableGPIO, true, i, 0 );
                    connect( cbEnable, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
                    cbEnable->addItem( "True" ); cbEnable->addItem( "False" );
                    cbEnable->setCurrentIndex( pInfo->bEnable ? 0 : 1 );
                    QHoverComboBox* cbIO = new QHoverComboBox( ui->TableGPIO, true, i, 1 );
                    connect( cbIO, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
                    cbIO->addItem( "Input" ); cbIO->addItem( "Output" );
                    cbIO->setCurrentIndex( pInfo->bOutput ? 1 : 0 );

                    ui->TableGPIO->setCellWidget( i, 0, cbEnable );
                    ui->TableGPIO->setCellWidget( i, 1, cbIO );
                }
			}
		}
		else
		{
            for ( int i=0 ; i<nGPIOCount ; i++ )
			{
                GPIOInfo* pInfo = Keys.getGPIOInfo(i);
				if ( pInfo )
				{
                    Q_ASSERT( ui->TableGPIO->cellWidget( i, 0 )->inherits( "QHoverComboBox" ) );
                    QHoverComboBox* cbEnable = (QHoverComboBox*)ui->TableGPIO->cellWidget( i, 0 );
                    connect( cbEnable, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
                    cbEnable->setCurrentIndex( pInfo->bEnable ? 0 : 1 );
                    Q_ASSERT( ui->TableGPIO->cellWidget( i, 1 )->inherits( "QHoverComboBox" ) );
                    QHoverComboBox* cbIO = (QHoverComboBox*)ui->TableGPIO->cellWidget( i, 1 );
                    connect( cbIO, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
                    cbIO->setCurrentIndex( pInfo->bOutput ? 1 : 0 );
				}
			}
		}
	}
	else
	{
        ui->EditGPIOCount->setText( "0" );
        ui->TableGPIO->clear();
        ui->TableGPIO->setRowCount( 0 );
	}
}

void TabKeyDesignWidget::showEvent(QShowEvent *)
{
    refresh();
}

void TabKeyDesignWidget::closeEvent(QCloseEvent *)
{
    m_DesignCanvasWidget.close();

    verifySoftlogicGPIO();
}

void TabKeyDesignWidget::on_BtnKeydesign_clicked()
{
    QDesktopWidget desktop;
    Q_ASSERT( desktop.screenCount() );

    QRect rcPrimary( desktop.screenGeometry( desktop.primaryScreen() ) );

    m_DesignCanvasWidget.setFont( font() );
    m_DesignCanvasWidget.setGeometry( rcPrimary );

    m_DesignCanvasWidget.show();
    m_DesignCanvasWidget.raise();
    m_DesignCanvasWidget.activateWindow();
}

void TabKeyDesignWidget::on_BtnSet_clicked()
{
    int nGPIOCount = ui->EditGPIOCount->text().toInt();
    if ( nGPIOCount == ui->TableGPIO->rowCount() )
        return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    ui->TableGPIO->clear();

    Keys.createGPIO( nGPIOCount );
    ui->TableGPIO->setRowCount( nGPIOCount );
    for ( int i=0 ; i<nGPIOCount ; i++ )
    {
        GPIOInfo* pInfo = Keys.getGPIOInfo(i);
        if ( pInfo )
        {
            QHoverComboBox* cbEnable = new QHoverComboBox( ui->TableGPIO, true, i, 0 );
            connect( cbEnable, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
            cbEnable->addItem( "True" ); cbEnable->addItem( "False" );
            cbEnable->setCurrentIndex( pInfo->bEnable ? 0 : 1 );
            QHoverComboBox* cbIO = new QHoverComboBox( ui->TableGPIO, true, i, 1 );
            connect( cbIO, &QHoverComboBox::ItemChanged, this, &TabKeyDesignWidget::onTableGPIOcellChanged );
            cbIO->addItem( "Input" ); cbIO->addItem( "Output" );
            cbIO->setCurrentIndex( pInfo->bOutput ? 1 : 0 );

            ui->TableGPIO->setCellWidget( i, 0, cbEnable );
            ui->TableGPIO->setCellWidget( i, 1, cbIO );
        }
    }
}

void TabKeyDesignWidget::onTableGPIOcellChanged(QObject* obj, int index)
{
    if( !obj->inherits( "QHoverComboBox" ) ) return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QHoverComboBox* cb = (QHoverComboBox*)obj;
    int row = cb->GetRowIndex();
    int column = cb->GetColumnIndex();

    GPIOInfo* pInfo = Keys.getGPIOInfo( row );
    if ( pInfo )
    {
        if( column == 0 )
            pInfo->bEnable = (index == 0 ? true : false);
        else if( column == 1 )
            pInfo->bOutput = (index == 1 ? true : false);
    }
}
