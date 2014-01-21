#include "QGestureProfileDialog.h"
#include "ui_QGestureProfileDialog.h"
#include "QLangManager.h"
#include <QResizeEvent>
#include "QUtils.h"
#include "T3kConstStr.h"
#include "QSensorInitDataCfg.h"
#include "QT3kDevice.h"
#include "QShowMessageBox.h"
#include <QPainter>

#include "QLogSystem.h"

#define RES_TAG     ("GESTURE PROFILE DIALOG")
#define MAIN_TAG    ("MAIN")

QGestureProfileDialog::QGestureProfileDialog(QWidget *parent) :
    QDialog(parent),
    m_editActionEnableDialog(this), m_editActionKey1Dialog(this), m_editActionKey2WayDialog(this), m_editActionKey4WayDialog(this),
    ui(new Ui::QGestureProfileDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Tool;

    setWindowFlags(flags);
    setWindowModality( Qt::WindowModal );
    setModal( true );

    QLangRes& res = QLangManager::getResource();
    ui->widgetProfileTab->setTabDirection( QColorTabWidget::TabDirectionHorzLeftTop, 28, 10 );
    QWidget* pWidget = &m_widgetGestureMappingTable;
    m_widgetGestureMappingTable.setParent(ui->widgetProfileTab);
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_GENERAL"), pWidget, QColor(237, 28, 36, 200), NULL );
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_WINDOWS7"), pWidget, QColor(0, 162, 232, 200), NULL );

    onChangeLanguage();

    ui->cmdAsyncMngr->setT3kDevice(QT3kDevice::instance());

    connect( ui->widgetProfileTab, SIGNAL(tabSelectChanged(QColorTabWidget*,int)), SLOT(onTabSelChanged(QColorTabWidget*,int)) );
    connect( &m_widgetGestureMappingTable, &QGestureMappingTable::updateProfile, this, &QGestureProfileDialog::onUpdateProfile, Qt::DirectConnection );

    ui->widgetProfileTab->selectTab(0);

    sensorRefresh();

    ui->btnClose->setFocus();
}

QGestureProfileDialog::~QGestureProfileDialog()
{
    delete ui;
}

void QGestureProfileDialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString( RES_TAG, "TITLE") );

    ui->btnReset->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_RESET") );
    ui->btnRefresh->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_REFRESH") );
    ui->btnClose->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CLOSE") );

    if ( bIsR2L != s_bIsR2L )
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QGestureProfileDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width(), height());
    p.fillRect( rcBody, Qt::white );
}

void QGestureProfileDialog::sensorReset()
{
    if ( ui->cmdAsyncMngr->isStarted() )
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    resetDataWithInitData( cstrMouseProfile1 );
    resetDataWithInitData( cstrMouseProfile2 );

    ui->cmdAsyncMngr->start( (unsigned int)-1 );
}

void QGestureProfileDialog::sensorRefresh( bool bTabOnly/*=false*/ )
{
    int nActiveIndex = ui->widgetProfileTab->getActiveTab();
    if ( ui->cmdAsyncMngr->isStarted() )
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    QString strSensorCmd;

    switch ( nActiveIndex )
    {
    case 0:
        strSensorCmd = QString(cstrMouseProfile1) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    case 1:
        strSensorCmd = QString(cstrMouseProfile2) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    }

    if ( !bTabOnly )
    {
//        strSensorCmd = QString(cstrMouseProfile) + "?";
//        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    }

    ui->cmdAsyncMngr->start( (unsigned int)-1 );
}

bool QGestureProfileDialog::sensorWrite()
{
    if ( ui->cmdAsyncMngr->isStarted() )
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    QString strSensorCmd;

    strSensorCmd = QString(cstrMouseProfile1) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = QString(cstrMouseProfile2) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    ui->cmdAsyncMngr->start( (unsigned int)-1 );

    QEventLoop loop;
    loop.connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), SLOT(quit()));

    ui->cmdAsyncMngr->start( 6000 );

    loop.exec();

    return ui->cmdAsyncMngr->getLastResult();
}

void QGestureProfileDialog::resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault/*=true*/ )
{
    QString strSensorCmd;
    QString strValue;
    QString strC = strCmd;
    strC = trim( strC, "=" );
    if ( QSensorInitDataCfg::instance()->isLoaded() )
    {
        if ( QSensorInitDataCfg::instance()->getValue( strC, strValue ) )
        {
            strSensorCmd = strC + "=" + strValue;
            ui->cmdAsyncMngr->insertCommand(strSensorCmd);
            if ( bWithFactoryDefault )
            {
                strSensorCmd = strC + "=!";
                ui->cmdAsyncMngr->insertCommand(strSensorCmd);
            }
            return;
        }
    }

    if ( bWithFactoryDefault )
    {
        strSensorCmd = strC + "=**";
    }
    else
    {
        strSensorCmd = strC + "=*";
    }
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
}

void QGestureProfileDialog::onTabSelChanged(QColorTabWidget* /*pTabWidget*/, int tabIndex)
{
    m_widgetGestureMappingTable.setProfileIndex(tabIndex);
    sensorRefresh(true);
}

void QGestureProfileDialog::sendInputMode()
{
//    char szCmd[256];
//    int nMouseIndex, nMTIndex;
//    getRadioProfileIndex(nMouseIndex, nMTIndex);
//    snprintf( szCmd, 256, "%s%d,%d", cstrMouseProfile, nMouseIndex, nMTIndex );
//    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QGestureProfileDialog::on_btnReset_clicked()
{
    LOG_B( "Gesture Profile: Reset" );
    sensorReset();
}

void QGestureProfileDialog::on_btnRefresh_clicked()
{
    LOG_B( "Gesture Profile: Refresh" );
    sensorRefresh();
}

void QGestureProfileDialog::on_btnSave_clicked()
{
    setEnabled( false );
    if( !sensorWrite() )
    {
        QLangRes& res = QLangManager::getResource();
        QString strPrompt = res.getResString( MAIN_TAG, "TEXT_ERROR_WRITE_FACTORY_DEFAULT_FAILURE" );
        QString strTitle = res.getResString( MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE" );
        showMessageBox( this,
            strPrompt,
            strTitle,
            QMessageBox::Critical, QMessageBox::Ok, QMessageBox::Ok );

        setEnabled( true );
        ui->btnSave->setFocus();
        return;
    }

    setEnabled( true );
    close();
}

void QGestureProfileDialog::on_btnClose_clicked()
{
    LOG_B( "Gesture Profile: Close" );
    close();
}

void QGestureProfileDialog::onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo &ci, ushort nProfileFlags)
{
    switch (ci.keyType)
    {
    default:
        break;
    case QGestureMappingTable::KeyTypeEnable:
        m_editActionEnableDialog.setProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], nProfileFlags );
        m_editActionEnableDialog.exec();
        break;
    case QGestureMappingTable::KeyType1Key:
        m_editActionKey1Dialog.setProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0] );
        m_editActionKey1Dialog.exec();
        break;
    case QGestureMappingTable::KeyType2Way:
        m_editActionKey2WayDialog.setProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1] );
        m_editActionKey2WayDialog.exec();
        break;
    case QGestureMappingTable::KeyType4Way:
        m_editActionKey4WayDialog.setProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1], ci.wKeyValue[2], ci.wKeyValue[3] );
        m_editActionKey4WayDialog.exec();
        break;
    }
}
