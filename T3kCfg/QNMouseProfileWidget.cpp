#include "QNMouseProfileWidget.h"
#include "ui_QNMouseProfileWidget.h"

#include "QT3kUserData.h"
#include "T3kConstStr.h"
#include "QCustomDefaultSensor.h"

#include <QKeyEvent>


QNMouseProfileWidget::QNMouseProfileWidget(QT3kDeviceR*& pT3kHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QNMouseProfileWidget), m_pT3kHandle(pT3kHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    m_nInputMode = -1;
    m_nChkUsbCfgMode = -1;

    ui->TabMouseSettingTable->setTabDirection( QColorTabWidget::TabDirectionHorzLeftTop, 28, 10 );
    QWidget* pWidget = &m_MouseProfileTableWidget;
    m_MouseProfileTableWidget.setParent(ui->TabMouseSettingTable);
    QLangRes& res = QLangManager::instance()->getResource();
    ui->TabMouseSettingTable->addTab( res.getResString("MOUSE SETTING", "TEXT_PROFILE_LEGEND_MODE1"), pWidget, QColor(237, 28, 36, 200), NULL );
    ui->TabMouseSettingTable->addTab( res.getResString("MOUSE SETTING", "TEXT_PROFILE_LEGEND_MODE2"), pWidget, QColor(0, 162, 232, 200), NULL );

    connect( ui->TabMouseSettingTable, SIGNAL(tabSelectChanged(QColorTabWidget*,int)), SLOT(onTabSelChanged(QColorTabWidget*,int)) );
    connect( &m_MouseProfileTableWidget, &QGestureMappingTable::UpdateProfile, this, &QNMouseProfileWidget::onUpdateProfile, Qt::DirectConnection );

    connect( &m_MouseProfileTableWidget, SIGNAL(sendCommand(QString,bool,unsigned short)), this, SLOT(onSendCommand(QString,bool,unsigned short)), Qt::QueuedConnection );
    connect( this, SIGNAL(SendInputModeState()), &m_MouseProfileTableWidget, SLOT(onUpdateInputMode()), Qt::QueuedConnection );

    m_pEditActionWnd = new QEditActionWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditActionEWnd = new QEditActionEDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction2WDWnd = new QEditAction2WDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction4WDWnd = new QEditAction4WDWnd( QT3kUserData::GetInstance()->getTopParent() );

    connect( m_pEditActionWnd, &QEditActionWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditActionEWnd, &QEditActionEDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction2WDWnd, &QEditAction2WDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction4WDWnd, &QEditAction4WDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );

    connect( &m_MouseProfileTableWidget, &QGestureMappingTable::UpdateProfile, this, &QNMouseProfileWidget::onUpdateProfile, Qt::QueuedConnection );

    ui->TabMouseSettingTable->selectTab( 0 );
}

QNMouseProfileWidget::~QNMouseProfileWidget()
{
    delete ui;

    m_RequestCmdManager.Stop();
}

void QNMouseProfileWidget::setDefault()
{
    requestSensorData( true );
}

void QNMouseProfileWidget::refresh()
{
    requestSensorData( false );
}

void QNMouseProfileWidget::requestSensorData( bool bDefault )
{
    QT3kDeviceR* pT3kHandle = QT3kUserData::GetInstance()->getT3kHandle();
    if( !pT3kHandle ) return;

    m_RequestCmdManager.Stop();

    char cQ = bDefault ? '*' : '?';

    QCustomDefaultSensor* pInstance = QCustomDefaultSensor::Instance();
    if( bDefault )
    {
        if( pInstance->IsLoaded() )
        {
            QString strQ = QString( cQ );
            QString strP1 = pInstance->GetDefaultData( cstrMouseProfile1, strQ );
            QString strP2 = pInstance->GetDefaultData( cstrMouseProfile2, strQ );

            m_RequestCmdManager.AddItem( cstrMouseProfile1, strP1 );
            m_RequestCmdManager.AddItem( cstrMouseProfile2, strP2 );

            pT3kHandle->sendCommand( QString("%1%2").arg(cstrMouseProfile1).arg(strP1), true );
            pT3kHandle->sendCommand( QString("%1%2").arg(cstrMouseProfile2).arg(strP2), true );
        }
        else
        {
            QString str( cQ );
            m_RequestCmdManager.AddItem( cstrMouseProfile1, str );
            m_RequestCmdManager.AddItem( cstrMouseProfile2, str );

            pT3kHandle->sendCommand( QString("%1%2").arg(cstrMouseProfile1).arg(cQ), true );
            pT3kHandle->sendCommand( QString("%1%2").arg(cstrMouseProfile2).arg(cQ), true );
        }
        m_nSelectedProfileIndex = -1;
    }

    m_RequestCmdManager.AddItem( cstrUsbConfigMode, "?" );
    m_nChkUsbCfgMode = pT3kHandle->sendCommand( QString("%1?").arg(cstrUsbConfigMode), true );

    int nRet = 3;
    do
    {
    if( !pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), false ) )
        nRet--;
    else
        break;
    } while( nRet );

    if( !nRet ) m_RequestCmdManager.AddItem( cstrInputMode, "?" );

    m_RequestCmdManager.AddItem( cstrMouseProfile, "?" );
    pT3kHandle->sendCommand( QString("%1?").arg(cstrMouseProfile), true );

    m_RequestCmdManager.Start( pT3kHandle );
}

void QNMouseProfileWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    if( strstr(cmd, cstrUsbConfigMode) == cmd )
    {
        m_RequestCmdManager.RemoveItem( cstrUsbConfigMode );
        m_nChkUsbCfgMode = -1;

        int nMode = strtol(cmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( nMode )
        {
        case 0x04: // digitizer
            ui->TabMouseSettingTable->setEnabled( false );
            break;
        case 0x07: // full
            ui->TabMouseSettingTable->setEnabled( true );
            break;
        default:
            break;
        }
    }
    else if( strstr(cmd, cstrInputMode) == cmd )
    {
        m_RequestCmdManager.RemoveItem( cstrInputMode );

        char* pInputMode = NULL;
        pInputMode = (char*)strchr( cmd, ',' );

        int nMode1 = -1;

        if( !pInputMode )
            m_nInputMode = strtol(cmd + sizeof(cstrInputMode) - 1, NULL, 16);
        else
        {
            nMode1 = strtol(cmd + sizeof(cstrInputMode) - 1, NULL, 16);
            if( nMode1 == 0xFF )
                m_nInputMode = strtol( pInputMode+1, NULL, 16 );
            else
                m_nInputMode = nMode1;
        }

        emit SendInputModeState();
    }
    else if ( strstr(cmd, cstrMouseProfile) == cmd )
    {
        m_RequestCmdManager.RemoveItem( cstrMouseProfile );

        int nProfile = atoi(cmd + sizeof(cstrMouseProfile) - 1 );
        char* pProfile = (char*)strchr( cmd, ',' );
        if( pProfile )
        {
            int nMouseIdx = strtol( cmd + sizeof(cstrMouseProfile) - 1, NULL, 10 );
            int nMultiIdx = strtol( pProfile+1, NULL, 10 );

            switch( m_nInputMode )
            {
            case 0xFF:
            case -1:
                Q_ASSERT(false);
                break;
            case 0x02:
                nProfile = nMultiIdx;
                break;
            case 0x00:
            default:
                nProfile = nMouseIdx;
                break;
            }

            ui->TabMouseSettingTable->selectTab( nProfile-1 );
        }
    }

    if ( strstr(cmd, cstrMouseProfile1) == cmd )
        m_RequestCmdManager.RemoveItem( cstrMouseProfile1 );

    if ( strstr(cmd, cstrMouseProfile2) == cmd )
        m_RequestCmdManager.RemoveItem( cstrMouseProfile2 );
}

void QNMouseProfileWidget::TPDP_OnRSE(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int id, bool /*bFinal*/, const char */*cmd*/)
{
    if( m_nChkUsbCfgMode == id )
    {
        m_RequestCmdManager.RemoveItem( cstrUsbConfigMode );
        m_nChkUsbCfgMode = -1;
    }
}

void QNMouseProfileWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->TitleMouseMapping->setText( Res.getResString(QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TITLE_CAPTION_MOUSE_BUTTON_MAPPING")) );
}

void QNMouseProfileWidget::showEvent(QShowEvent *)
{
    requestSensorData( false );
    setFocusPolicy( Qt::StrongFocus );
}

void QNMouseProfileWidget::hideEvent(QHideEvent *)
{
    setFocusPolicy( Qt::NoFocus );

    m_RequestCmdManager.Stop();

    m_nSelectedProfileIndex = -1;
    //m_nMouseProfileIndex = -1;
}

void QNMouseProfileWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        ByPassKeyPressEvent( evt );
        return;
    }

    QWidget::keyPressEvent(evt);
}

void QNMouseProfileWidget::onTabSelChanged(QColorTabWidget* /*pTabWidget*/, int tabIndex)
{
    m_MouseProfileTableWidget.setProfileIndex(tabIndex);
    if( isVisible() )
        sensorRefresh(true);
}

void QNMouseProfileWidget::onSendCommand(QString strCmd, bool bAsync, unsigned short nTimeout)
{
    QT3kUserData::GetInstance()->getT3kHandle()->sendCommand( strCmd, bAsync, nTimeout );
}

void QNMouseProfileWidget::onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo &ci, ushort nProfileFlags)
{
    switch (ci.keyType)
    {
    default:
        break;
    case QGestureMappingTable::KeyTypeEnable:
        m_pEditActionEWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], nProfileFlags );
        m_pEditActionEWnd->exec();
        break;
    case QGestureMappingTable::KeyType1Key:
        m_pEditActionWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], nProfileFlags );
        m_pEditActionWnd->exec();
        break;
    case QGestureMappingTable::KeyType2Way:
        m_pEditAction2WDWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1] );
        m_pEditAction2WDWnd->exec();
        break;
    case QGestureMappingTable::KeyType4Way:
        m_pEditAction4WDWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1], ci.wKeyValue[2], ci.wKeyValue[3] );
        m_pEditAction4WDWnd->exec();
        break;
    }
}

void QNMouseProfileWidget::sensorRefresh( bool bTabOnly/*=false*/ )
{
    m_RequestCmdManager.Stop();

    int nActiveIndex = ui->TabMouseSettingTable->getActiveTab();
    switch ( nActiveIndex )
    {
    case 0:
        m_RequestCmdManager.AddItem( cstrMouseProfile1, "?" );
        break;
    case 1:
        m_RequestCmdManager.AddItem( cstrMouseProfile2, "?" );
        break;
    }

    if ( !bTabOnly )
    {
//        strSensorCmd = QString(cstrMouseProfile) + "?";
//        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    }

    m_RequestCmdManager.Start( QT3kUserData::GetInstance()->getT3kHandle() );
}
