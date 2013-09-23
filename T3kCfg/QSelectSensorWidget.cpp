#include "QSelectSensorWidget.h"
#include "ui_QSelectSensorWidget.h"

#include "QWidgetCloseEventManager.h"
#include "T3kConstStr.h"
#include "T3kCfgWnd.h"
#include "QT3kUserData.h"

#include <QScrollBar>

QSelectSensorWidget::QSelectSensorWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSelectSensorWidget)
{
    ui->setupUi(this);
    setFont( parent->font() );

    setWindowFlags( Qt::Window | Qt::WindowTitleHint );

    QString strTitle( QT3kUserData::GetInstance()->GetProgramTitle() );
    if( !strTitle.isEmpty() )
        setWindowTitle( strTitle );

    m_nSelectIndex = -1;

    m_nDeviceCount = 0;
    m_nTimerCheckDevice = 0;

    ui->TitleSelectSensor->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_HABILIENCE.png" );

    m_pT3kHandle = new T3kHandle();

    int nW = ui->TableSensor->width() - ui->TableSensor->verticalHeader()->size().width();
    ui->TableSensor->verticalHeader()->setVisible( false );
    ui->TableSensor->verticalScrollBar()->setVisible( false );
    ui->TableSensor->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->TableSensor->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->TableSensor->setColumnWidth( 0, 40 );
    ui->TableSensor->setColumnWidth( 1, 100 );
    ui->TableSensor->setColumnWidth( 2, nW - 225 );
    ui->TableSensor->setColumnWidth( 3, 80 );
    ui->TableSensor->setFocusPolicy( Qt::ClickFocus );
    ui->TableSensor->setAlternatingRowColors( true );

    onChangeLanguage();
}

QSelectSensorWidget::~QSelectSensorWidget()
{
    if( m_nTimerCheckDevice )
    {
        killTimer( m_nTimerCheckDevice );
        m_nTimerCheckDevice = 0;
    }

    for( int i=0; i<ui->TableSensor->rowCount(); i++ )
    {
        if( ui->TableSensor->itemAt( 0, i ) )
            delete ui->TableSensor->itemAt( 0, i );
        if( ui->TableSensor->itemAt( 1, i ) )
            delete ui->TableSensor->itemAt( 1, i );
        if( ui->TableSensor->itemAt( 2, i ) )
            delete ui->TableSensor->itemAt( 2, i );
    }

    if( m_pT3kHandle )
    {
        delete m_pT3kHandle;
        m_pT3kHandle = NULL;
    }

    delete ui;
}

void QSelectSensorWidget::on_BtnOpen_clicked()
{
    if( m_nSelectIndex < 0 || m_nSelectIndex > ui->TableSensor->rowCount() ) return;

    if( m_TimerBuzzer.isActive() )
        m_TimerBuzzer.stop();

    if( m_pT3kHandle->IsOpen() )
        m_pT3kHandle->Close();

    ModelID stID = m_mapDevModelID.value( m_nSelectIndex );
    ui->BtnOpen->setEnabled( false );
    if( !m_pT3kHandle->OpenWithVIDPID( stID.VID, stID.PID, 1, stID.Idx ) )
    {
        ui->BtnOpen->setEnabled( true );
        return;
    }

    m_pT3kHandle->Close();

    QT3kUserData* pUD = QT3kUserData::GetInstance();
    pUD->SetSelectedVID( stID.VID );
    pUD->SetSelectedPID( stID.PID );
    pUD->SetSelectedIdx( stID.Idx );

    QDialog::accept();
}

void QSelectSensorWidget::on_TableSensor_Selection(int nRow, int /*nColumn*/)
{
    m_nSelectIndex = nRow;
}

void QSelectSensorWidget::on_TableSensor_DClick(int nRow, int /*nColumn*/)
{
    m_nSelectIndex = nRow;
    on_BtnOpen_clicked();
}

void QSelectSensorWidget::on_TableButton_PlaySound(bool bToggled, int nIndex)
{
    for( int i=0; i<m_mapDevModelID.size(); i++ )
    {
        if( i != nIndex && ui->TableSensor->cellWidget( i, 3 )->inherits( "QIndexToolButton" ) )
        {
            QIndexToolButton* pWidget = (QIndexToolButton*)ui->TableSensor->cellWidget( i, 3 );
            if( pWidget )
                pWidget->setChecked( false );
        }
    }

    if( bToggled )
    {
        if( nIndex < 0 ) return;
        ModelID stID = m_mapDevModelID.value( nIndex );
        if( m_pT3kHandle->IsOpen() )
            m_pT3kHandle->Close();
        if( m_pT3kHandle->OpenWithVIDPID( stID.VID, stID.PID, 1, stID.Idx ) )
        {
            m_TimerBuzzer.setParent( this );
            connect( &m_TimerBuzzer, SIGNAL(timeout()), this, SLOT(on_Timer_Buzzer()) );
            m_TimerBuzzer.start( 1000 );
        }
    }
    else
    {
        if( m_TimerBuzzer.isActive() )
        {
            m_TimerBuzzer.stop();
            m_pT3kHandle->Close();
        }
    }
}

void QSelectSensorWidget::on_Timer_Buzzer()
{
    if( m_TimerBuzzer.timerId() >= 0 )
    {
        m_pT3kHandle->SendCommand( (const char*)QString("%1%2,3").arg(cstrBuzzerPlay).arg(1).toUtf8().data(), true );
    }
}

void QSelectSensorWidget::showEvent(QShowEvent *evt)
{
    setFixedSize( width(), height() );

    UpdateDeviceList();

    m_nDeviceCount = m_mapDevModelID.count();

    if( m_nDeviceCount <= 0 )
    {
        QWidgetCloseEventManager::instance()->AddClosedWidget( this, 1000 );
        accept();
        QDialog::showEvent(evt);
        return;
    }
    else if( m_nDeviceCount == 1 )
    {
        if( m_mapDevModelID.count() == 1 )
        {
            ModelID stID = m_mapDevModelID.value( 0 );
            if( m_pT3kHandle->OpenWithVIDPID( stID.VID, stID.PID, 1, stID.Idx ) )
                QT3kUserData::GetInstance()->SetModel( stID.PID );
            QWidgetCloseEventManager::instance()->AddClosedWidget( this, 1000 );
            accept();
            QDialog::showEvent(evt);
            return;
        }
    }

    connect( ui->TableSensor, SIGNAL(cellClicked(int,int)), this, SLOT(on_TableSensor_Selection(int,int)) );
    connect( ui->TableSensor, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(on_TableSensor_DClick(int,int)) );

    if( !m_nTimerCheckDevice )
    {
        m_nTimerCheckDevice = startTimer( 5000 );
    }

    QDialog::showEvent(evt);
}

void QSelectSensorWidget::closeEvent(QCloseEvent *evt)
{
    reject();

    QDialog::closeEvent(evt);
}

void QSelectSensorWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerCheckDevice )
    {
        int nTotalDetectCnt = m_pT3kHandle->GetDeviceCount( 0xFFFF, 0x0000 );
        nTotalDetectCnt += m_pT3kHandle->GetDeviceCount( 0x2200, 0x3000 );
        nTotalDetectCnt += m_pT3kHandle->GetDeviceCount( 0x2200, 0x3100 );
        nTotalDetectCnt += m_pT3kHandle->GetDeviceCount( 0x2200, 0x3200 );
        nTotalDetectCnt += m_pT3kHandle->GetDeviceCount( 0x2200, 0x3500 );
        nTotalDetectCnt += m_pT3kHandle->GetDeviceCount( 0x2200, 0x3900 );

        if( m_nDeviceCount != nTotalDetectCnt )
        {
            m_nDeviceCount = nTotalDetectCnt;
            UpdateDeviceList();
        }
    }

    QDialog::timerEvent(evt);
}

void QSelectSensorWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("WINDOW_CAPTION")) );

    ui->BtnOpen->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("BTN_CAPTION_SELECT")) );
    ui->TitleSelectSensor->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("TITLE_CAPTION_DEVICE_LIST")) );

    ui->TableSensor->horizontalHeaderItem( 0 )->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("TEXT_NUMBER")) );
    ui->TableSensor->horizontalHeaderItem( 1 )->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("TEXT_MODEL")) );
    ui->TableSensor->horizontalHeaderItem( 2 )->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("TEXT_DEV_PATH")) );
    ui->TableSensor->horizontalHeaderItem( 3 )->setText( Res.getResString(QString::fromUtf8("SELECT DEVICE"), QString::fromUtf8("BTN_CAPTION_PLAY_BUZZER")) );
}

void QSelectSensorWidget::AddSensorItem( QString strModel, QString strDevPath )
{
    int nRowIndex = ui->TableSensor->rowCount();
    ui->TableSensor->setRowCount( nRowIndex+1 );
    QTableWidgetItem* pOneItem = new QTableWidgetItem( QString("%1").arg(nRowIndex+1) );
    pOneItem->setTextAlignment( Qt::AlignCenter );
    ui->TableSensor->setItem( nRowIndex, 0, pOneItem );
    QTableWidgetItem* pTwoItem = new QTableWidgetItem( strModel );
    pTwoItem->setTextAlignment( Qt::AlignVCenter );
    ui->TableSensor->setItem( nRowIndex, 1, pTwoItem );
    QTableWidgetItem* pThreeItem = new QTableWidgetItem( strDevPath );
    pThreeItem->setTextAlignment( Qt::AlignVCenter );
    ui->TableSensor->setItem( nRowIndex, 2, pThreeItem );
    QIndexToolButton* btnSound = new QIndexToolButton( ui->TableSensor, nRowIndex );
    btnSound->setAttribute( Qt::WA_DeleteOnClose );
    btnSound->setCursor( Qt::PointingHandCursor );
    btnSound->setFocusPolicy( Qt::NoFocus );
    btnSound->setAutoRaise( true );
    btnSound->setCheckable( true );
    btnSound->setAttribute( Qt::WA_DeleteOnClose );
    btnSound->setIcon( QIcon(":/T3kCfgRes/resources/PNG_ICON_PLAY_BUZZER.png") );
    connect( btnSound, SIGNAL(Clicked_Signal(bool,int)), this, SLOT(on_TableButton_PlaySound(bool,int)) );
    ui->TableSensor->setCellWidget( nRowIndex, 3, btnSound );
}

void QSelectSensorWidget::UpdateDeviceList()
{
    int i = ui->TableSensor->rowCount();
    while( i )
    {
        ui->TableSensor->removeRow( i-1 );
        i = ui->TableSensor->rowCount();
    }
    m_mapDevModelID.clear();

    ModelID stID;
    stID.VID = 0xFFFF;
    stID.PID = 0x0000;
    int nOldT3000DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nOldT3000DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3000", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
    stID.VID = 0x2200;
    stID.PID = 0x3000;
    int nT3000DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nT3000DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3000", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
    stID.VID = 0x2200;
    stID.PID = 0x3100;
    int nT3100DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nT3100DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3100", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
    stID.VID = 0x2200;
    stID.PID = 0x3200;
    int nT3200DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nT3200DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3200", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
    stID.VID = 0x2200;
    stID.PID = 0x3500;
    int nT3500DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nT3500DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3500", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
    stID.VID = 0x2200;
    stID.PID = 0x3900;
    int nT3900DetectCnt = m_pT3kHandle->GetDeviceCount( stID.VID, stID.PID );
    for( int i=0; i<nT3900DetectCnt; i++ )
    {
        stID.Idx = i;
        m_mapDevModelID.insert( m_mapDevModelID.count(), stID );
        AddSensorItem( "T3900", m_pT3kHandle->GetDevPath( stID.VID, stID.PID, 1, stID.Idx ) );
    }
}


// QIndexToolButton class

QIndexToolButton::QIndexToolButton(QWidget *parent, int nIndex) :
        QToolButton(parent),
        m_nIndex(nIndex)
{
    setCursor( Qt::PointingHandCursor );
    connect( this, SIGNAL(clicked(bool)), this, SLOT(on_Clicked_Signal(bool)) );
}

QIndexToolButton::~QIndexToolButton()
{
}

int QIndexToolButton::GetIndex()
{
    return m_nIndex;
}

void QIndexToolButton::on_Clicked_Signal(bool bToggled)
{
    Clicked_Signal( bToggled, m_nIndex );
}
