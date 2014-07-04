#include "QBriefingDialog.h"
#include "ui_briefing_dialog.h"

#include "QPartCheckBox.h"
#include "../common/T3kCamNameDef.h"


QBriefingDialog::QBriefingDialog(bool bPartialUpgrade, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QBriefingDialog)
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

    m_bPartialUpgrade = bPartialUpgrade;
    m_nSelected = 0;

    ui->tableDetailInformation->horizontalHeader()->setHighlightSections( false );
    ui->tableDetailInformation->verticalHeader()->setVisible( false );
    ui->tableDetailInformation->horizontalHeader()->setVisible( true );
    ui->tableDetailInformation->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->tableDetailInformation->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );

    if( m_bPartialUpgrade )
    {
        ui->tableDetailInformation->setColumnCount( 6 );
        ui->tableDetailInformation->setHorizontalHeaderLabels(QString("Part;Model;From;;To;;").split(";"));

        ui->tableDetailInformation->setColumnWidth( 0, 50 );
        ui->tableDetailInformation->setColumnWidth( 1, 80 );
#ifdef Q_OS_MAC
        ui->tableDetailInformation->setColumnWidth( 2, 70 );
        ui->tableDetailInformation->setColumnWidth( 3, 40 );
        ui->tableDetailInformation->setColumnWidth( 4, 70 );
#else
        ui->tableDetailInformation->setColumnWidth( 2, 80 );
        ui->tableDetailInformation->setColumnWidth( 3, 30 );
        ui->tableDetailInformation->setColumnWidth( 4, 80 );
#endif
        ui->tableDetailInformation->setColumnWidth( 5, 20 );

        ui->pushButtonOK->setEnabled( false );
    }
    else
    {
        ui->tableDetailInformation->setColumnCount( 5 );
        ui->tableDetailInformation->setHorizontalHeaderLabels(QString("Part;Model;From;;To").split(";"));

        ui->tableDetailInformation->setColumnWidth( 0, 50 );
        ui->tableDetailInformation->setColumnWidth( 1, 80 );
        ui->tableDetailInformation->setColumnWidth( 2, 80 );
        ui->tableDetailInformation->setColumnWidth( 3, 50 );
        ui->tableDetailInformation->setColumnWidth( 4, 80 );
    }
}

QBriefingDialog::~QBriefingDialog()
{
    delete ui;
}

void QBriefingDialog::addPart(QString strPart, QString strModel, QString strCurrentVer, QString strTargetVer)
{
    int nIdx = ui->tableDetailInformation->rowCount();
    ui->tableDetailInformation->setRowCount( nIdx+1 );
    ui->tableDetailInformation->setItem( nIdx, 0, new QTableWidgetItem(strPart) );
    ui->tableDetailInformation->setItem( nIdx, 1, new QTableWidgetItem(strModel) );
    ui->tableDetailInformation->setItem( nIdx, 2, new QTableWidgetItem(strCurrentVer) );
    ui->tableDetailInformation->setItem( nIdx, 4, new QTableWidgetItem(strTargetVer) );

    ui->tableDetailInformation->setItem( (nIdx-1)/2, 3, new QTableWidgetItem("") );
    ui->tableDetailInformation->setItem( nIdx/2, 3, new QTableWidgetItem("=>") );

    if( m_bPartialUpgrade )
    {
        QPartCheckBox* pChk = new QPartCheckBox( ui->tableDetailInformation );
        pChk->setObjectName( strPart );
        connect( pChk, &QPartCheckBox::togglePart, this, &QBriefingDialog::onTogglePart );
        ui->tableDetailInformation->setCellWidget( nIdx, 5, pChk );
    }
}

void QBriefingDialog::on_pushButtonOK_clicked()
{
    accept();
}

void QBriefingDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void QBriefingDialog::onTogglePart(QString strPart, bool bChecked)
{
    if( bChecked )
        m_nSelected++;
    else
        m_nSelected--;

    ui->pushButtonOK->setEnabled( m_nSelected > 0 ? true : false );

    emit togglePart( strPart, bChecked );
}
