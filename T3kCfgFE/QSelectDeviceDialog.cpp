#include "QSelectDeviceDialog.h"
#include "ui_QSelectDeviceDialog.h"
#include "QT3kDevice.h"
#include "t3kcomdef.h"
#include "QLogSystem.h"

#define RES_TAG "SELECT SENSOR DIALOG"
#define MAIN_TAG "MAIN"

QSelectDeviceDialog::QSelectDeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSelectDeviceDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    ui->tblDeviceList->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    ui->tblDeviceList->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->tblDeviceList->setColumnWidth( 0, 40 );
    ui->tblDeviceList->setColumnWidth( 1, 100 );
    ui->tblDeviceList->setColumnWidth( 2, 200 );
}

QSelectDeviceDialog::~QSelectDeviceDialog()
{
    delete ui;
}

void QSelectDeviceDialog::showEvent(QShowEvent *)
{
    memset( &m_deviceId, 0, sizeof(DEVICE_ID) );

    onChangeLanguage();

    refreshDeviceList();

    ui->btnPlayBuzzer->setEnabled(false);
    ui->btnSelect->setEnabled(false);
}

void QSelectDeviceDialog::closeEvent(QCloseEvent *)
{
    qDebug( "row: %d", ui->tblDeviceList->rowCount() );
    for ( int i=0; i<ui->tblDeviceList->rowCount(); i++ )
    {
        QVariant var = ui->tblDeviceList->item(i, 0)->data(Qt::UserRole);
        DEVICE_ID* pDevId = (DEVICE_ID*)var.value<void *>();
        qDebug( "delete pDevId: %p", pDevId );
        delete pDevId;
    }

    while (ui->tblDeviceList->rowCount() > 0)
    {
        ui->tblDeviceList->removeRow(ui->tblDeviceList->rowCount()-1);
    }

    ui->tblDeviceList->clearContents();
    qDebug( "row: %d", ui->tblDeviceList->rowCount() );
}

void QSelectDeviceDialog::onChangeLanguage()
{
    if (!isVisible())
        return;

    QLangRes& res = QLangManager::instance()->getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString(RES_TAG, "TITLE") );

    ui->lblDeviceList->setText( res.getResString(RES_TAG, "TEXT_DEVICE_LIST") );
    ui->btnPlayBuzzer->setText( res.getResString(RES_TAG, "BTN_CAPTION_PLAY_BUZZER") );
    ui->btnSelect->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_SELECT") );

    ui->tblDeviceList->horizontalHeaderItem(0)->setText( res.getResString(RES_TAG, "TEXT_NUMBER") );
    ui->tblDeviceList->horizontalHeaderItem(1)->setText( res.getResString(RES_TAG, "TEXT_MODEL") );
    ui->tblDeviceList->horizontalHeaderItem(2)->setText( res.getResString(RES_TAG, "TEXT_DEVICE_PATH") );

    if (s_bIsR2L != bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QSelectDeviceDialog::refreshDeviceList()
{
    for ( int i=0; i<ui->tblDeviceList->rowCount(); i++ )
    {
        QVariant var = ui->tblDeviceList->item(i, 0)->data(Qt::UserRole);
        DEVICE_ID* pDevId = (DEVICE_ID*)var.value<void *>();
        delete pDevId;
    }

    while (ui->tblDeviceList->rowCount() > 0)
    {
        ui->tblDeviceList->removeRow(ui->tblDeviceList->rowCount()-1);
    }

    ui->tblDeviceList->clearContents();

    for (int d=0 ; d<COUNT_OF_DEVICE_LIST ; d++)
    {
        int nDevCnt = QT3kDevice::getDeviceCount( DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI );
        for (int i=0 ; i<nDevCnt ; i++)
        {
            insertListItem( DEVICE_LIST[d].szModelName, DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI, i );
        }
    }
}

void QSelectDeviceDialog::insertListItem( const QString& strModelName, unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDeviceIndex )
{
    int nRowIndex = ui->tblDeviceList->rowCount();
    QString strNum;
    strNum.setNum(nRowIndex);

    DEVICE_ID* pDeviceId = new DEVICE_ID;
    pDeviceId->nVID = nVID;
    pDeviceId->nPID = nPID;
    pDeviceId->nMI = nMI;
    pDeviceId->nDeviceIndex = nDeviceIndex;

    QString strDevPath = QT3kDevice::getDevicePath(nVID, nPID, nMI, nDeviceIndex);

    ui->tblDeviceList->setRowCount(nRowIndex+1);
    QTableWidgetItem* pItem;
    pItem = new QTableWidgetItem( strNum );
    pItem->setTextAlignment( Qt::AlignCenter|Qt::AlignVCenter );
    pItem->setData(Qt::UserRole, qVariantFromValue((void*)pDeviceId));
    ui->tblDeviceList->setItem( nRowIndex, 0, pItem );
    pItem = new QTableWidgetItem( strModelName );
    pItem->setTextAlignment( Qt::AlignCenter|Qt::AlignVCenter );
    ui->tblDeviceList->setItem( nRowIndex, 1, pItem );
    pItem = new QTableWidgetItem( strDevPath );
    pItem->setTextAlignment( Qt::AlignCenter|Qt::AlignVCenter );
    ui->tblDeviceList->setItem( nRowIndex, 2, pItem );
}

void QSelectDeviceDialog::on_btnSelect_clicked()
{
    LOG_B( "Select" );

    if (ui->tblDeviceList->currentIndex().isValid())
    {
        int nRowIndex = ui->tblDeviceList->currentRow();
        QTableWidgetItem* pItem = ui->tblDeviceList->item(nRowIndex, 0);
        DEVICE_ID* pDeviceId = (DEVICE_ID*)(pItem->data(Qt::UserRole).value<void *>());
        memcpy( &m_deviceId, pDeviceId, sizeof(DEVICE_ID) );
        accept();
    }
}

void QSelectDeviceDialog::on_btnPlayBuzzer_clicked()
{
    LOG_B( "Play Buzzer" );

    if (ui->tblDeviceList->currentIndex().isValid())
    {
        int nRowIndex = ui->tblDeviceList->currentRow();
        QTableWidgetItem* pItem = ui->tblDeviceList->item(nRowIndex, 0);
        DEVICE_ID* pDeviceId = (DEVICE_ID*)(pItem->data(Qt::UserRole).value<void *>());
        //memcpy( &m_deviceId, pDeviceId, sizeof(DEVICE_ID) );
        QT3kDevice* pDevice = QT3kDevice::instance();
        if (pDevice->isOpen())
        {
            pDevice->close();
        }
        if (pDevice->open(pDeviceId->nVID, pDeviceId->nPID, pDeviceId->nMI, pDeviceId->nDeviceIndex))
        {
            LOG_C( "-> %s", "buzzer_play=3,1" );
            pDevice->sendCommand( "buzzer_play=3,1", true );
            pDevice->close();
        }
    }
}


void QSelectDeviceDialog::on_tblDeviceList_doubleClicked(const QModelIndex &/*index*/)
{
    on_btnSelect_clicked();
}

void QSelectDeviceDialog::on_tblDeviceList_itemSelectionChanged()
{
    if (ui->tblDeviceList->currentIndex().isValid())
    {
        ui->btnPlayBuzzer->setEnabled(true);
        ui->btnSelect->setEnabled(true);
    }
    else
    {
        ui->btnPlayBuzzer->setEnabled(false);
        ui->btnSelect->setEnabled(false);
    }
}
