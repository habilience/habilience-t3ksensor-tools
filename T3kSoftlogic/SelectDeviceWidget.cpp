#include "SelectDeviceWidget.h"
#include "ui_SelectDeviceWidget.h"

#include "QT3kDevice.h"
#include "t3kcomdef.h"

#include <QScrollBar>

Q_DECLARE_METATYPE(QSelectDeviceWidget::DEVICE_ID)


QSelectDeviceWidget::QSelectDeviceWidget(QWidget* parent /*=NULL*/) :
    QDialog(parent),
    ui(new Ui::SelectDeviceWidget)
{
    ui->setupUi(this);
    setFont( parent->font() );

    setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

    ui->TableDeviceList->verticalHeader()->setVisible( true );
    ui->TableDeviceList->horizontalHeader()->setVisible( true );

    ui->TableDeviceList->setColumnCount( 2 );
    ui->TableDeviceList->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Model" ) );
    ui->TableDeviceList->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Device Path" ) );

    ui->TableDeviceList->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->TableDeviceList->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );

    ui->TableDeviceList->setColumnWidth( 0, 50 );
    ui->TableDeviceList->setColumnWidth( 1, 310 );

    ui->BtnBuzzer->setEnabled( false );
    ui->BtnSelect->setEnabled( false );
}

QSelectDeviceWidget::~QSelectDeviceWidget()
{
    delete ui;
}

void QSelectDeviceWidget::showEvent(QShowEvent *)
{
    refreshDeviceList();
}

void QSelectDeviceWidget::refreshDeviceList()
{
    ui->TableDeviceList->clear();

    int nDevCnt = 0;
    for ( int d = 0 ; d<COUNT_OF_DEVICE_LIST(APP_DEVICE_LIST) ; d++)
    {
        int nCnt = QT3kDevice::getDeviceCount( APP_DEVICE_LIST[d].nVID, APP_DEVICE_LIST[d].nPID, APP_DEVICE_LIST[d].nMI );
        for( int i=0; i<nCnt; i++ )
        {
            insertListItem( APP_DEVICE_LIST[d].szModelName, nDevCnt, APP_DEVICE_LIST[d].nVID, APP_DEVICE_LIST[d].nPID, APP_DEVICE_LIST[d].nMI, i);

            nDevCnt++;
        }
    }

    int nW = 310;
    if( ui->TableDeviceList->verticalHeader()->isVisible() )
        nW -= 16;
    if( ui->TableDeviceList->rowCount() > 4 )
        nW -= 16;

    ui->TableDeviceList->setColumnWidth( 1, nW );
    // ... ?
    ui->TableDeviceList->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Model" ) );
    ui->TableDeviceList->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Device Path" ) );
}

void QSelectDeviceWidget::insertListItem(QString strModelName, int nIdx, ushort nVID, ushort nPID, ushort nMI, int nDeviceIndex)
{
    DEVICE_ID deviceId;
    deviceId.nVID = nVID;
    deviceId.nPID = nPID;
    deviceId.nMI = nMI;
    deviceId.nDeviceIndex = nDeviceIndex;
    QVariant vData;
    vData.setValue(deviceId);

    ui->TableDeviceList->setRowCount( nIdx+1 );
    QTableWidgetItem* pItem1 = new QTableWidgetItem(strModelName);
    pItem1->setTextAlignment( Qt::AlignCenter );
    pItem1->setData( Qt::UserRole, vData );
    ui->TableDeviceList->setItem( nIdx, 0, pItem1 );
    ui->TableDeviceList->setItem( nIdx, 1, new QTableWidgetItem(QString(QT3kDevice::getDevicePath(nVID, nPID, nMI, nDeviceIndex))) );
}

void QSelectDeviceWidget::on_TableDeviceList_itemSelectionChanged()
{
    if( ui->TableDeviceList->currentRow() < 0 )
    {
        ui->BtnBuzzer->setEnabled( false );
        ui->BtnSelect->setEnabled( false );
        return;
    }

    if( !ui->BtnBuzzer->isEnabled() )
        ui->BtnBuzzer->setEnabled( true );
    if( !ui->BtnSelect->isEnabled() )
        ui->BtnSelect->setEnabled( true );
}

void QSelectDeviceWidget::on_TableDeviceList_itemDoubleClicked(QTableWidgetItem */*item*/)
{
    on_BtnSelect_clicked();
}

void QSelectDeviceWidget::on_BtnBuzzer_clicked()
{
    int nIdx = ui->TableDeviceList->currentRow();
    if( nIdx < 0 ) return;

    DEVICE_ID deviceId = ui->TableDeviceList->item( nIdx, 0 )->data( Qt::UserRole ).value<DEVICE_ID>();

    QT3kDevice handle;
    if( handle.open( deviceId.nVID, deviceId.nPID, deviceId.nMI, deviceId.nDeviceIndex ) )
        handle.sendCommand( "buzzer_play=3,1", true );

    handle.close();
}

void QSelectDeviceWidget::on_BtnSelect_clicked()
{
    int nIdx = ui->TableDeviceList->currentRow();
    if( nIdx < 0 ) return;

    m_DeviceId = ui->TableDeviceList->item( nIdx, 0 )->data( Qt::UserRole ).value<DEVICE_ID>();
    accept();
}
