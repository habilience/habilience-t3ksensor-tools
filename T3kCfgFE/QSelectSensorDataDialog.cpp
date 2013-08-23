#include "QSelectSensorDataDialog.h"
#include "ui_QSelectSensorDataDialog.h"

#include <QEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include "QSensorInitDataCfg.h"
#include "QShowMessageBox.h"
#include "QEnterFileNameDialog.h"
#include <QSettings>

QSelectSensorDataDialog::QSelectSensorDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSelectSensorDataDialog)
{
    ui->setupUi(this);
    m_bIsNewData = false;
    m_bSaveMode = false;

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    ui->tblDataList->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    ui->tblDataList->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->tblDataList->setColumnWidth( 0, 200 );
    ui->tblDataList->setColumnWidth( 1, 200 );

    ui->btnCancel->setFocus();
}

QSelectSensorDataDialog::~QSelectSensorDataDialog()
{
    delete ui;
}

#define RES_TAG "SELECT SENSOR DATA DIALOG"
#define MAIN_TAG "MAIN"
void QSelectSensorDataDialog::onChangeLanguage()
{
    if (!isVisible())
        return;

    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection(Qt::RightToLeft);
    else
        setLayoutDirection(Qt::LeftToRight);

    setWindowTitle( res.getResString(RES_TAG, "TITLE") );

    ui->btnNewData->setText( res.getResString(RES_TAG, "BTN_CAPTION_NEW_DATA") );
    ui->btnOK->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_SELECT") );
    ui->btnCancel->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CANCEL") );
    ui->lblDataList->setText( res.getResString(RES_TAG, "TEXT_DATA_LIST") );

    ui->tblDataList->horizontalHeaderItem(0)->setText( res.getResString(RES_TAG, "TEXT_DATE") );
    ui->tblDataList->horizontalHeaderItem(1)->setText( res.getResString(RES_TAG, "TEXT_FILENAME") );

    if ( m_bSaveMode )
    {
        ui->btnOK->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_SAVE") );
    }
    else
    {
        ui->btnOK->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_LOAD") );
    }

    if ( bIsR2L != s_bIsR2L )
    {
        // update layout direction
    }

    s_bIsR2L = bIsR2L;
}

void QSelectSensorDataDialog::insertListItem( const QString& strDateTime, const QString& strFileName )
{
    int nRowIndex = ui->tblDataList->rowCount();

    ui->tblDataList->setRowCount(nRowIndex+1);
    QTableWidgetItem* pItem;
    pItem = new QTableWidgetItem( strDateTime );
    pItem->setTextAlignment( Qt::AlignLeft|Qt::AlignVCenter );
    ui->tblDataList->setItem( nRowIndex, 0, pItem );
    pItem = new QTableWidgetItem( strFileName );
    pItem->setTextAlignment( Qt::AlignLeft|Qt::AlignVCenter );
    ui->tblDataList->setItem( nRowIndex, 1, pItem );
}

void QSelectSensorDataDialog::onInitDialog()
{
    onChangeLanguage();

    ui->btnOK->setEnabled(false);

    QSensorInitDataCfg::Enumerator* pEnumerator = QSensorInitDataCfg::instance()->getEnumerator();

    pEnumerator->enumCfgData();
    QString     strFileName;
    QDateTime	tmDate;

    for ( int i=0 ; i<pEnumerator->getCfgDataCount() ; i++ )
    {
        if ( pEnumerator->getCfgDataAt( i, strFileName, tmDate ) )
        {
            insertListItem( tmDate.toString(Qt::SystemLocaleDate), strFileName );
        }
    }

    if ( m_bSaveMode )
    {
        ui->btnNewData->setVisible( true );
    }
    else
    {
        ui->btnNewData->setVisible( false );
    }

    QString strRegFileName;
    QSettings settings( "Habilience", qApp->applicationName() );
    settings.beginGroup("SENSOR_INIT_DATA");
    strRegFileName = settings.value("FILENAME").toString();
    settings.endGroup();

    if ( !strRegFileName.isEmpty() )
    {
        QTableWidgetItem* pSelectedItem = NULL;
        QTableWidgetItem* pItem;
        int nSelectedRow = -1;
        for ( int i=0 ; i<ui->tblDataList->rowCount() ; i++ )
        {
            pItem = ui->tblDataList->item(i, 1);
            if (pItem->text().compare(strRegFileName, Qt::CaseInsensitive) == 0)
            {
                pSelectedItem = pItem;
                nSelectedRow = i;
                break;
            }
        }

        if (pSelectedItem != NULL)
        {
            ui->tblDataList->selectRow(nSelectedRow);
            ui->tblDataList->setFocus();
            ui->tblDataList->scrollToItem(pSelectedItem);
        }
    }
}

void QSelectSensorDataDialog::showEvent(QShowEvent *evt)
{
    if (evt->type() == QEvent::Show)
    {
        onInitDialog();
    }
}

void QSelectSensorDataDialog::closeEvent(QCloseEvent *evt)
{
    if (evt->type() == QEvent::Close)
    {
        //onDestroy();
    }
}

void QSelectSensorDataDialog::on_btnNewData_clicked()
{
    QEnterFileNameDialog enterFileNameDlg(this);
    if ( enterFileNameDlg.exec() != QDialog::Accepted )
        return;

    QString strFileName = enterFileNameDlg.getFileName();
    m_strFileName = strFileName;
    m_bIsNewData = true;
    on_btnOK_clicked();
}

void QSelectSensorDataDialog::on_btnOK_clicked()
{
    if ( m_bSaveMode )
    {
        QSensorInitDataCfg::Enumerator* pEnumerator = QSensorInitDataCfg::instance()->getEnumerator();
        pEnumerator->enumCfgData();
        if ( pEnumerator->findCfgData(m_strFileName) )
        {
            QLangRes& res = QLangManager::getResource();
            QString strMessage;
            if ( m_bIsNewData )
            {
                QString strFormat = res.getResString(RES_TAG, "TEXT_WARNING_FILE_EXIST_OVERWRITE_PROMPT1");
                strFormat.replace( "%s", "%1" );
                strMessage = strFormat.arg(m_strFileName);
            }
            else
            {
                QString strFormat = res.getResString(RES_TAG, "TEXT_WARNING_FILE_EXIST_OVERWRITE_PROMPT2");
                strFormat.replace( "%s", "%1" );
                strMessage = strFormat.arg(m_strFileName);
            }
            if ( showMessageBox(this, strMessage, res.getResString(MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE"),
                                QMessageBox::Question, QMessageBox::Yes|QMessageBox::No, QMessageBox::No ) != QMessageBox::Yes )
            {
                return;
            }
        }
    }

    accept();
}

void QSelectSensorDataDialog::on_btnCancel_clicked()
{
    reject();
}

void QSelectSensorDataDialog::on_tblDataList_doubleClicked(const QModelIndex &/*index*/)
{
    if (!ui->tblDataList->currentIndex().isValid())
    {
        return;
    }
    int nRowIndex = ui->tblDataList->currentRow();
    QTableWidgetItem* pItem = ui->tblDataList->item(nRowIndex, 1);
    m_strFileName = pItem->text();

    on_btnOK_clicked();
}

void QSelectSensorDataDialog::on_tblDataList_itemSelectionChanged()
{
    if (ui->tblDataList->currentIndex().isValid())
    {
        int nRowIndex = ui->tblDataList->currentRow();
        QTableWidgetItem* pItem = ui->tblDataList->item(nRowIndex, 1);
        m_strFileName = pItem->text();
        ui->btnOK->setEnabled(true);
    }
    else
    {
        ui->btnOK->setEnabled(false);
    }
}
