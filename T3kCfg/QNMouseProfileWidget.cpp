#include "QNMouseProfileWidget.h"
#include "ui_QNMouseProfileWidget.h"

#include "QT3kUserData.h"
#include "T3kConstStr.h"
#include "QCustomDefaultSensor.h"
#include "QPredefProfileEditDialog.h"
#include "../common/QIni.h"

#include <QKeyEvent>
#include <QSettings>

#define GROUPNAME_MOUSE         "MOUSE"
#define GROUPNAME_MULTITOUCH    "MULTI-TOUCH"


QNMouseProfileWidget::QNMouseProfileWidget(QT3kDevice*& pT3kHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QNMouseProfileWidget), m_pT3kHandle(pT3kHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    m_nInputMode = -1;
    m_nChkUsbCfgMode = -1;
    m_bDefault = false;
    m_bCheckMacOSXZoom = false;

    ui->TitleMouseMapping->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_MOUSE_MAP.png" );

    QWidget* pWidget = &m_MouseProfileTableWidget;
    m_MouseProfileTableWidget.setParent(ui->TabMouseSettingTable);
    QLangRes& res = QLangManager::instance()->getResource();
    ui->TabMouseSettingTable->addTab( res.getResString("MOUSE SETTING", "TEXT_PROFILE_LEGEND_MODE1"), pWidget, QColor(237, 28, 36, 200), NULL );
    ui->TabMouseSettingTable->addTab( res.getResString("MOUSE SETTING", "TEXT_PROFILE_LEGEND_MODE2"), pWidget, QColor(0, 162, 232, 200), NULL );

    m_cbPredefinedProfile.setParent( ui->TabMouseSettingTable );
    m_cbPredefinedProfile.setFixedSize( 150, 22 );

    loadPredefProfiles( 0 );

    ui->TabMouseSettingTable->setExtraWidget( &m_cbPredefinedProfile );
    connect( &m_cbPredefinedProfile, SIGNAL(activated(int)), this, SLOT(onCBPredefinedProfileActivated(int)) );

    connect( ui->TabMouseSettingTable, SIGNAL(tabSelectChanged(QColorTabWidget*,int)), SLOT(onTabSelChanged(QColorTabWidget*,int)) );
    connect( &m_MouseProfileTableWidget, &QGestureMappingTable::updateProfile, this, &QNMouseProfileWidget::onUpdateProfile, Qt::DirectConnection );

    connect( &m_MouseProfileTableWidget, SIGNAL(sendCommand(QString,bool,unsigned short)), this, SLOT(onSendCommand(QString,bool,unsigned short)), Qt::QueuedConnection );
    connect( &m_MouseProfileTableWidget, &QGestureMappingTable::modifiedProfile, this, &QNMouseProfileWidget::onModifiedProfile );
    connect( this, SIGNAL(SendInputModeState()), &m_MouseProfileTableWidget, SLOT(onUpdateInputMode()), Qt::QueuedConnection );

    m_pEditActionWnd = new QEditActionWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditActionEWnd = new QEditActionEDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction2WDWnd = new QEditAction2WDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction4WDWnd = new QEditAction4WDWnd( QT3kUserData::GetInstance()->getTopParent() );

    connect( m_pEditActionWnd, &QEditActionWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditActionEWnd, &QEditActionEDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction2WDWnd, &QEditAction2WDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction4WDWnd, &QEditAction4WDWnd::sendCommand, this, &QNMouseProfileWidget::onSendCommand, Qt::QueuedConnection );

    connect( &m_MouseProfileTableWidget, &QGestureMappingTable::updateProfile, this, &QNMouseProfileWidget::onUpdateProfile, Qt::QueuedConnection );

    ui->TabMouseSettingTable->selectTab( 0 );

    onChangeLanguage();
}

QNMouseProfileWidget::~QNMouseProfileWidget()
{
    delete ui;

    m_RequestCmdManager.Stop();

//    if( m_pEditActionWnd )
//    {
//        m_pEditActionWnd->close();
//        delete m_pEditActionWnd;
//        m_pEditActionWnd = NULL;
//    }
//    if( m_pEditActionEWnd )
//    {
//        m_pEditActionEWnd->close();
//        delete m_pEditActionEWnd;
//        m_pEditActionEWnd = NULL;
//    }
//    if( m_pEditAction2WDWnd )
//    {
//        m_pEditAction2WDWnd->close();
//        delete m_pEditAction2WDWnd;
//        m_pEditAction2WDWnd = NULL;
//    }
//    if( m_pEditAction4WDWnd )
//    {
//        m_pEditAction4WDWnd->close();
//        delete m_pEditAction4WDWnd;
//        m_pEditAction4WDWnd = NULL;
//    }
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
    m_RequestCmdManager.Stop();

    m_bDefault = bDefault;

    char cQ = bDefault ? '*' : '?';

    QCustomDefaultSensor* pInstance = QCustomDefaultSensor::Instance();
    if( pInstance->IsLoaded() )
    {
        QString strQ = QString( cQ );
        QString strP1 = pInstance->GetDefaultData( cstrMouseProfile1, strQ );
        QString strP2 = pInstance->GetDefaultData( cstrMouseProfile2, strQ );

        m_RequestCmdManager.AddItem( cstrMouseProfile1, strP1 );
        m_RequestCmdManager.AddItem( cstrMouseProfile2, strP2 );
    }
    else
    {
        QString str( cQ );
        m_RequestCmdManager.AddItem( cstrMouseProfile1, str );
        m_RequestCmdManager.AddItem( cstrMouseProfile2, str );
    }

    m_RequestCmdManager.AddItem( cstrUsbConfigMode, "?" );
    m_nChkUsbCfgMode = m_pT3kHandle->sendCommand( QString("%1?").arg(cstrUsbConfigMode), true );

    int nRet = 3;
    do
    {
    if( !m_pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), false ) )
        nRet--;
    else
        break;
    } while( nRet );

    if( !nRet ) m_RequestCmdManager.AddItem( cstrInputMode, "?" );

    m_RequestCmdManager.AddItem( cstrMouseProfile, "?" );

    m_RequestCmdManager.Start( m_pT3kHandle );
}

void QNMouseProfileWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !m_bCheckMacOSXZoom && !isVisible() ) return;

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
            if( m_bDefault )
            {
                loadPredefProfiles( nProfile-1 );
                onModifiedProfile();
                m_bDefault = false;
            }
        }
    }

    if ( strstr(cmd, cstrMouseProfile1) == cmd )
    {
        m_RequestCmdManager.RemoveItem( cstrMouseProfile1 );
        m_bCheckMacOSXZoom = false;
    }

    if ( strstr(cmd, cstrMouseProfile2) == cmd )
    {
        m_RequestCmdManager.RemoveItem( cstrMouseProfile2 );
        m_bCheckMacOSXZoom = false;
    }
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

    ui->TabMouseSettingTable->setTabDirection( Res.isR2L() ? QColorTabWidget::TabDirectionHorzRightTop : QColorTabWidget::TabDirectionHorzLeftTop, 28, 20 );

    if( m_cbPredefinedProfile.isVisible() && m_cbPredefinedProfile.itemText(0).at(0) == '(' )
        m_cbPredefinedProfile.setItemText( 0, Res.getResString(QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED")) );
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
    //m_nMouseProfileIndex = -1;

    if( m_pEditActionWnd && m_pEditActionWnd->isVisible() )
        m_pEditActionWnd->close();
    if( m_pEditActionEWnd && m_pEditActionEWnd->isVisible() )
        m_pEditActionEWnd->close();
    if( m_pEditAction2WDWnd && m_pEditAction2WDWnd->isVisible() )
        m_pEditAction2WDWnd->close();
    if( m_pEditAction4WDWnd && m_pEditAction4WDWnd->isVisible() )
        m_pEditAction4WDWnd->close();
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

    loadPredefProfiles( tabIndex );
}

void QNMouseProfileWidget::onSendCommand(QString strCmd, bool bAsync, unsigned short nTimeout)
{
    m_pT3kHandle->sendCommand( strCmd, bAsync, nTimeout );
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

void QNMouseProfileWidget::onCBPredefinedProfileActivated(int index)
{
    if( index < 0 ) return;
    if( index == 0 &&
            m_cbPredefinedProfile.itemText(0) == QLangManager::instance()->getResource().
            getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ) )
    {
        return;
    }

    m_RequestCmdManager.Stop();

    QString strV = m_cbPredefinedProfile.itemData( index ).toString();
    int nProfileIdx = ui->TabMouseSettingTable->getActiveTab();
    if( !m_strPrevZommValue.isEmpty() )
    {
        int nP = strV.indexOf(" 80");
        m_strPrevZommValue = strV.mid( nP+3, 10 );
        QSettings settings( "Habilience", "T3kCfg" );
        settings.beginGroup( "Options" );
        settings.setValue( "MacOSX_Gesture_Value", m_strPrevZommValue );
        settings.endGroup();
        strV = strV.replace( nP, 11, " 8000000000" );
    }
    QString strCmd(cstrMouseProfile1);
    strCmd.replace( '1', QString::number(nProfileIdx+1) );
    m_RequestCmdManager.AddItem( strCmd.toUtf8().data(), strV );

    m_RequestCmdManager.Start( m_pT3kHandle );

    if( index != 0 &&
            m_cbPredefinedProfile.itemText(0) == QLangManager::instance()->getResource().
            getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ) )
    {
        m_cbPredefinedProfile.removeItem( 0 );
        return;
    }
}

void QNMouseProfileWidget::onModifiedProfile()
{
    QString strProfile = m_MouseProfileTableWidget.mergeMouseProfile();
    QString strUserDefText = QLangManager::instance()->getResource().getResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") );

    bool bExist = false;
    for( int i=0; i<m_cbPredefinedProfile.count(); i++ )
    {
        if( strProfile.compare( m_cbPredefinedProfile.itemData(i).toString() ) == 0 )
        {
            m_cbPredefinedProfile.setCurrentIndex( i );
            bExist = true;
            if( m_cbPredefinedProfile.itemText(i).compare(strUserDefText) != 0 &&
                    m_cbPredefinedProfile.itemText(0).compare(strUserDefText) == 0 )
                m_cbPredefinedProfile.removeItem(0);
            break;
        }
    }
    if( !bExist )
    {
        if( m_cbPredefinedProfile.itemText(0).compare(strUserDefText) != 0 )
            m_cbPredefinedProfile.insertItem( 0, strUserDefText, strProfile );
        m_cbPredefinedProfile.setCurrentIndex( 0 );
    }
}

void QNMouseProfileWidget::onEnableMacOSXGesture(bool bEnable)
{
    m_RequestCmdManager.Stop();

    m_bCheckMacOSXZoom = true;
    QString strCmd;
    strCmd = cstrInputMode;
    strCmd += "?";

    m_nInputMode = -1;

    int nCount = 3;
    while( nCount > 0 )
    {
        if( m_pT3kHandle->sendCommand( strCmd ) )
        {
            if( m_nInputMode >= 0 )
                break;
        }
        nCount--;
    }

    bool bMultiTouchMode = false;
    switch( m_nInputMode )
    {
    case 0x00:
        ui->TabMouseSettingTable->selectTab( 0 );
        strCmd = cstrMouseProfile1;
        break;
    case 0x02:
        ui->TabMouseSettingTable->selectTab( 1 );
        strCmd = cstrMouseProfile2;
        bMultiTouchMode = true;
        break;
    default:
        Q_ASSERT( false );
        break;
    }

    nCount = 3;
    while( nCount > 0 )
    {
        if( m_pT3kHandle->sendCommand( strCmd + "?" ) )
        {
            break;
        }
        nCount--;
    }

    Q_ASSERT( nCount > 0 );

    QSettings settings( "Habilience", "T3kCfg" );
    settings.beginGroup( "Options" );
    QString strValue = settings.value( "MacOSX_Gesture_Value", "" ).toString();
    settings.endGroup();
    qDebug() << "prev value : " << strValue;

    if( bEnable )
    {
        m_strPrevZommValue = m_MouseProfileTableWidget.enableMacOSXZoom( bEnable, bMultiTouchMode );
        if( !strValue.isEmpty() )
            m_strPrevZommValue = strValue;
        qDebug() << m_strPrevZommValue;
        strCmd += "8000000000";
    }
    else
    {

        if( m_strPrevZommValue.isEmpty() && !strValue.isEmpty() )
            m_strPrevZommValue = strValue;
        m_MouseProfileTableWidget.enableMacOSXZoom( bEnable, bMultiTouchMode );
        strCmd += "80" + m_strPrevZommValue;
        m_strPrevZommValue.clear();
        qDebug() << "command : " << strCmd;
    }

    nCount = 3;
    while( nCount > 0 )
    {
        if( m_pT3kHandle->sendCommand( strCmd ) )
        {
            break;
        }
        nCount--;
    }

    if( nCount <= 0 ) Q_ASSERT( false );

    m_bCheckMacOSXZoom = false;

    settings.beginGroup( "Options" );
    settings.setValue( "MacOSX_Gesture_Value", m_strPrevZommValue );
    settings.endGroup();
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

    m_RequestCmdManager.Start( m_pT3kHandle );
}

void QNMouseProfileWidget::loadPredefProfiles(int nTabIndex)
{
    m_cbPredefinedProfile.clear();

    //m_cbPredefinedProfile.addItem( "Manage..." );

    QString strFilePath( QApplication::applicationDirPath() );
    strFilePath += "/config/gestureprofiles.ini";
    if( !QFile::exists( strFilePath ) )
    {
        m_cbPredefinedProfile.setVisible( false );
        return;
    }

    QString strGroup;
    switch( nTabIndex )
    {
    case 0:
        strGroup = GROUPNAME_MOUSE;
        break;
    case 1:
        strGroup = GROUPNAME_MULTITOUCH;
        break;
    default:
        Q_ASSERT( false );
        return;
    }

    QIni iniPredefProfiles;
    if( !iniPredefProfiles.load( strFilePath ) )
        return;

    QIni::QSection* pSection = iniPredefProfiles.getSectionNoCase( strGroup );
    if( pSection )
    {
        for( int i=0; i<pSection->getDataCount(); i++ )
        {
            if( !pSection->getEntry(i).isEmpty() )
                m_cbPredefinedProfile.addItem( pSection->getEntry(i), pSection->getData(i) );
        }
    }

    m_cbPredefinedProfile.setCurrentIndex( -1 );
}
