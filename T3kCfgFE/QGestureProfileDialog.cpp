#include "QGestureProfileDialog.h"
#include "ui_QGestureProfileDialog.h"
#include "QLangManager.h"
#include <QResizeEvent>
#include "../common/QUtils.h"
#include "../common/T3kConstStr.h"
#include "QSensorInitDataCfg.h"
#include "QT3kDevice.h"

#include "QLogSystem.h"

#define RES_TAG     ("GESTURE PROFILE DIALOG")
#define MAIN_TAG    ("MAIN")

QGestureProfileDialog::QGestureProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QGestureProfileDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;

    setWindowFlags(flags);

    QLangRes& res = QLangManager::getResource();
    ui->widgetProfileTab->setTabDirection( QColorTabWidget::TabDirectionHorzLeftTop, 28, 10 );
    QWidget* pWidget = &m_widgetGestureMappingTable;
    m_widgetGestureMappingTable.setParent(ui->widgetProfileTab);
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_GENERAL"), pWidget, QColor(237, 28, 36, 200), NULL );
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_WINDOWS7"), pWidget, QColor(255, 127, 39, 200), NULL );
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_MAC_OS_X"), pWidget, QColor(34, 177, 36, 200), NULL );
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_LINUX"), pWidget, QColor(0, 162, 232, 200), NULL );
    ui->widgetProfileTab->addTab( res.getResString(RES_TAG, "TEXT_ITEM_USER_DEFINED"), pWidget, QColor(63, 72, 204, 200), NULL );

    onChangeLanguage();

    ui->cmdAsyncMngr->setT3kDevice(QT3kDevice::instance());

    connect( ui->widgetProfileTab, SIGNAL(tabSelectChanged(QColorTabWidget*,int)), SLOT(onTabSelChanged(QColorTabWidget*,int)) );

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

    ui->lblSelectProfile->setText( res.getResString(RES_TAG, "TEXT_SELECT_PROFILE") );
    //ui->lblInputMode->setText( res.getResString(RES_TAG, "TEXT_INPUT_MODE") );
    ui->lblInputModeMouse->setText( res.getResString(RES_TAG, "TEXT_INPUT_MODE_MOUSE") );
    ui->lblInputModeMT->setText( res.getResString(RES_TAG, "TEXT_INPUT_MODE_MULTITOUCH") );

    ui->btnReset->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_RESET") );
    ui->btnRefresh->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_REFRESH") );
    ui->btnClose->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CLOSE") );

    ui->btnInputModeMouseGeneral->setText( res.getResString(RES_TAG, "TEXT_ITEM_GENERAL") );
    ui->btnInputModeMouseWindows7->setText( res.getResString(RES_TAG, "TEXT_ITEM_WINDOWS7") );
    ui->btnInputModeMouseMac->setText( res.getResString(RES_TAG, "TEXT_ITEM_MAC_OS_X") );
    ui->btnInputModeMouseLinux->setText( res.getResString(RES_TAG, "TEXT_ITEM_LINUX") );
    ui->btnInputModeMouseUserDefined->setText( res.getResString(RES_TAG, "TEXT_ITEM_USER_DEFINED") );

    ui->btnInputModeMTGeneral->setText( res.getResString(RES_TAG, "TEXT_ITEM_GENERAL") );
    ui->btnInputModeMTWindows7->setText( res.getResString(RES_TAG, "TEXT_ITEM_WINDOWS7") );
    ui->btnInputModeMTMac->setText( res.getResString(RES_TAG, "TEXT_ITEM_MAC_OS_X") );
    ui->btnInputModeMTLinux->setText( res.getResString(RES_TAG, "TEXT_ITEM_LINUX") );
    ui->btnInputModeMTUserDefined->setText( res.getResString(RES_TAG, "TEXT_ITEM_USER_DEFINED") );

    if ( bIsR2L != s_bIsR2L )
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QGestureProfileDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    const char* buf = NULL;
    if ( strstr(szCmd, cstrMouseProfile) == szCmd )
    {
        buf = szCmd + sizeof(cstrMouseProfile) - 1;
        int nMouseProfileIndex = atoi(buf) - 1;
        int nMultitouchProfileIndex = nMouseProfileIndex;
        if ( (buf = strchr(buf, ',') + 1) != (char *)1 )
        {
            nMultitouchProfileIndex = atoi(buf) - 1;
        }

        switch (nMouseProfileIndex)
        {
        case 0:
            ui->btnInputModeMouseGeneral->setChecked(true);
            break;
        case 1:
            ui->btnInputModeMouseWindows7->setChecked(true);
            break;
        case 2:
            ui->btnInputModeMouseMac->setChecked(true);
            break;
        case 3:
            ui->btnInputModeMouseLinux->setChecked(true);
            break;
        case 4:
            ui->btnInputModeMouseUserDefined->setChecked(true);
            break;
        }

        switch (nMultitouchProfileIndex)
        {
        case 0:
            ui->btnInputModeMTGeneral->setChecked(true);
            break;
        case 1:
            ui->btnInputModeMTWindows7->setChecked(true);
            break;
        case 2:
            ui->btnInputModeMTMac->setChecked(true);
            break;
        case 3:
            ui->btnInputModeMTLinux->setChecked(true);
            break;
        case 4:
            ui->btnInputModeMTUserDefined->setChecked(true);
            break;
        }
    }
}

void QGestureProfileDialog::sensorReset()
{
    if ( ui->cmdAsyncMngr->isStarted() )
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    resetDataWithInitData(cstrMouseProfile1, false );
    resetDataWithInitData(cstrMouseProfile2, false );
    resetDataWithInitData(cstrMouseProfile3, false );
    resetDataWithInitData(cstrMouseProfile4, false );
    resetDataWithInitData(cstrMouseProfile5, false );
    resetDataWithInitData(cstrMouseProfile, false );

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
    case 2:
        strSensorCmd = QString(cstrMouseProfile3) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    case 3:
        strSensorCmd = QString(cstrMouseProfile4) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    case 4:
        strSensorCmd = QString(cstrMouseProfile5) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    }

    if ( !bTabOnly )
    {
        strSensorCmd = QString(cstrMouseProfile) + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    }

    ui->cmdAsyncMngr->start( (unsigned int)-1 );
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

void QGestureProfileDialog::getRadioProfileIndex(int& nMouseIndex, int& nMTIndex)
{
    QRadioButton* inputModeMouseRadioBtns[] =
    {
        ui->btnInputModeMouseGeneral,
        ui->btnInputModeMouseWindows7,
        ui->btnInputModeMouseMac,
        ui->btnInputModeMouseLinux,
        ui->btnInputModeMouseUserDefined
    };
    QRadioButton* inputModeMTRadioBtns[] =
    {
        ui->btnInputModeMTGeneral,
        ui->btnInputModeMTWindows7,
        ui->btnInputModeMTMac,
        ui->btnInputModeMTLinux,
        ui->btnInputModeMTUserDefined
    };

    for (unsigned int i=0 ; i<sizeof(inputModeMouseRadioBtns)/sizeof(QRadioButton*) ; i++)
    {
        if (inputModeMouseRadioBtns[i]->isChecked())
        {
            nMouseIndex = i+1;
            break;
        }
    }

    for (unsigned int i=0 ; i<sizeof(inputModeMTRadioBtns)/sizeof(QRadioButton*) ; i++)
    {
        if (inputModeMTRadioBtns[i]->isChecked())
        {
            nMTIndex = i+1;
            break;
        }
    }
}

void QGestureProfileDialog::sendInputMode()
{
    char szCmd[256];
    int nMouseIndex, nMTIndex;
    getRadioProfileIndex(nMouseIndex, nMTIndex);
    snprintf( szCmd, 256, "%s%d,%d", cstrMouseProfile, nMouseIndex, nMTIndex );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QGestureProfileDialog::on_btnInputModeMouseGeneral_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(Mouse) - General" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMouseWindows7_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(Mouse) - Windows 7" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMouseMac_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(Mouse) - Mac OS X" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMouseLinux_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(Mouse) - Linux(GNOME)" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMouseUserDefined_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(Mouse) - User Defined" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMTGeneral_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(MT) - General" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMTWindows7_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(MT) - Windows 7" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMTMac_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(MT) - Mac OS X" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMTLinux_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(MT) - Linux(GNOME)" );
    sendInputMode();
}

void QGestureProfileDialog::on_btnInputModeMTUserDefined_clicked()
{
    LOG_B( "Gesture Profile: Input Mode(MT) - User Defined" );
    sendInputMode();
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

void QGestureProfileDialog::on_btnClose_clicked()
{
    LOG_B( "Gesture Profile: Close" );
    close();
}
