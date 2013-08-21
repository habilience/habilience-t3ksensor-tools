#include "TabKeyDesignWidget.h"
#include "ui_TabKeyDesignWidget.h"

#include "T3kCommonData.h"
#include "../common/ui/QHoverComboBox.h"

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
    ui->TableGPIO->setColumnWidth( 1, 120 );
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
                    QHoverComboBox* cbEnable = new QHoverComboBox( ui->TableGPIO );
                    cbEnable->addItem( "True" ); cbEnable->addItem( "False" );
                    cbEnable->setCurrentIndex( pInfo->bEnable ? 1 : 0 );
                    QHoverComboBox* cbIO = new QHoverComboBox( ui->TableGPIO );
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
                    cbEnable->setCurrentIndex( pInfo->bEnable ? 1 : 0 );
                    Q_ASSERT( ui->TableGPIO->cellWidget( i, 1 )->inherits( "QHoverComboBox" ) );
                    QHoverComboBox* cbIO = (QHoverComboBox*)ui->TableGPIO->cellWidget( i, 1 );
                    cbIO->setCurrentIndex( pInfo->bOutput ? 1 : 0 );
				}
			}
		}
	}
	else
	{
        ui->EditGPIOCount->setText( "0" );
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
            QHoverComboBox* cbEnable = new QHoverComboBox( ui->TableGPIO );
            cbEnable->addItem( "True" ); cbEnable->addItem( "False" );
            cbEnable->setCurrentIndex( pInfo->bEnable ? 1 : 0 );
            QHoverComboBox* cbIO = new QHoverComboBox( ui->TableGPIO );
            cbIO->addItem( "Input" ); cbIO->addItem( "Output" );
            cbIO->setCurrentIndex( pInfo->bOutput ? 1 : 0 );

            ui->TableGPIO->setCellWidget( i, 0, cbEnable );
            ui->TableGPIO->setCellWidget( i, 1, cbIO );
        }
    }
}

void TabKeyDesignWidget::on_TableGPIO_cellChanged(int row, int column)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    GPIOInfo* pInfo = Keys.getGPIOInfo( row );
    if ( pInfo )
    {
        Q_ASSERT( ui->TableGPIO->cellWidget( row, column )->inherits( "QHoverComboBox" ) );

        QHoverComboBox* cb = (QHoverComboBox*) ui->TableGPIO->cellWidget( row, column );
        if( cb->currentIndex() == 0 )
            pInfo->bEnable = (column == 0 ? true : false);
        else if( cb->currentIndex() == 1 )
            pInfo->bOutput = (column == 0 ? true : false);
    }
}
