#include "QEditActionWnd.h"
#include "ui_QEditActionWnd.h"

#include "T30xHandle.h"
#include "QMouseMappingTable.h"

#include "KeyMapStr.h"
#include "stdInclude.h"

#include "Common/nv.h"

#include "T3kPacketDef.h"

#include <QShowEvent>
#include <QHideEvent>

QEditActionWnd::QEditActionWnd(T30xHandle*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditActionWnd), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    ui->setupUi(this);

#if defined(Q_OS_WIN) || defined(Q_OS_X11)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    connect( ui->BtnCancel, SIGNAL(clicked()), this, SLOT(hide()) );
    connect( ui->LBUserDefKey, SIGNAL(KeyPressSignal(ushort)), this ,SLOT(onKeyPress_Signal(ushort)) );
    connect( ui->LBUserDefKey, SIGNAL(FuncKey_activated(int)), this, SLOT(onFunctionKeyActivated(int)) );

    m_pTableWnd = (QMouseMappingTable*)parent;

    Init();

    setFixedSize( width(), height() );
}

QEditActionWnd::~QEditActionWnd()
{
    delete ui;
}

void QEditActionWnd::OnChangeLanguage()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TITLE_CAPTION_EDIT_PROFILE_ITEM") ) );
    ui->TitleProfileItem->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM") ) );
    ui->TitleAction->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_ACTION") ) );
    ui->LBUserDefKey->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_USER_DEFINE_KEY") ) );
    ui->LBKey->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_KEY") ) );
    ui->BtnApply->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_APPLY") ) );
    ui->BtnCancel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_CANCEL") ) );

    if( Res.IsR2L() )
        ui->EditProfileItem->setAlignment( Qt::AlignRight );
    else
        ui->EditProfileItem->setAlignment( Qt::AlignLeft );

    ui->LBUserDefKey->SetRealGeometry();
}

void QEditActionWnd::Init()
{
    OnChangeLanguage();
}

void QEditActionWnd::SetPredefineProfileTypes( ProfileType Type, bool bTaskSwitch )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->CBAction->clear();

    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DISABLED") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0000 );
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0xFFFF );
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FUNCTION_KEY") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0xFFFF );

    switch( Type )
    {
    case TypeMove:
        ui->CBAction->addItem( "Left Drag" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8001 );
        ui->CBAction->addItem( "Right Drag" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8004 );
        ui->CBAction->addItem( "Middle Drag" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8007 );
        ui->CBAction->addItem( "Wheel" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x80FE );
        ui->CBAction->addItem( "Ctrl+Wheel" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x81FE );
        break;
    case TypeSingle:
        ui->CBAction->addItem( "Left Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8002 );
        ui->CBAction->addItem( "Right Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8005 );
        ui->CBAction->addItem( "Middle Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8008 );
        break;
    case TypeDouble:
        ui->CBAction->addItem( "Left Double Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8003 );
        ui->CBAction->addItem( "Right Double Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8006 );
        ui->CBAction->addItem( "Middle Double Click" );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x8009 );
        break;
    default:
        break;
    }
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FORWARD") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x044F );
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_BACK") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0450 );

#if defined(Q_OS_MAC)
    if( bTaskSwitch )
    {
        ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH1") ) );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x082B );
    }
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_EXPLORER") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0811 );
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DESKTOP") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0044 );
#else
#if defined(Q_OS_WIN)
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_MANAGER") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0329 );
#endif
    if( bTaskSwitch )
    {
        ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH1") ) );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x042B );
    }
#if defined(Q_OS_WIN)
    if( bTaskSwitch )
    {
        ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_TASK_SWITCH2") ) );
        ui->CBAction->setItemData( ui->CBAction->count()-1, 0x082B );
    }
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_EXPLORER") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0808 );
#endif
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DESKTOP") ) );
    ui->CBAction->setItemData( ui->CBAction->count()-1, 0x0807 );
#endif
}

void QEditActionWnd::SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue, bool bTaskSwitch )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue = wKeyValue;

    QString strLoc;
    ProfileType Type = TypeError;
    switch( cKey )
    {
    case MM_GESTURE_SINGLE_LONG_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_SINGLE") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_LONG_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_FINGERS_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_FINGERS") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_FINGERS_DOUBLE_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_FINGERS") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DOUBLE_TAP") );
        Type = TypeDouble;
        break;
    case MM_GESTURE_FINGERS_LONG_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_FINGERS") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_LONG_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_PALM_MOVE:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DRAG") );
        Type = TypeMove;
        break;
    case MM_GESTURE_PALM_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_PALM_DOUBLE_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DOUBLE_TAP") );
        Type = TypeDouble;
        break;
    case MM_GESTURE_PALM_LONG_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_LONG_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_PUTAND_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PUTNTAP") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_TAP") );
        Type = TypeSingle;
        break;
    case MM_GESTURE_PUTAND_DOUBLE_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PUTNTAP") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DOUBLE_TAP") );
        Type = TypeDouble;
        break;
    case MM_GESTURE_PUTAND_LONG_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PUTNTAP") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_LONG_TAP") );
        Type = TypeSingle;
        break;
    default:
        strLoc = QString::fromUtf8("Unknown");
        break;
    }

    SetPredefineProfileTypes( Type, bTaskSwitch );

    ui->EditProfileItem->setText( strLoc );

    ui->CBAction->setCurrentIndex(0);
    ui->LBDetail->setText( ("") );

    QString strDetail = GetMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    bool bFound = false;
    int nSelIndex = 0;
    for( int nI = 0 ; nI < ui->CBAction->count() ; nI ++ )
    {
        ushort wItemValue = (ushort)ui->CBAction->itemData(nI).toInt();
        if( wItemValue == m_wProfileValue )
        {
            ui->CBAction->setCurrentIndex( nI );
            nSelIndex = nI;
            bFound = true;
            break;
        }
    }

    ui->LBDetail->setText( strDetail );

    if( !bFound )
    {
        bool bFunctionKey = false;
        if( !(wKeyValue >> 8 & 0x80) )
        {
            int nScanCode;
            if( FindScanCode( wKeyValue & 0xFF, nScanCode ) )
            {
                int nVk = FromMapVirtualKey( nScanCode );
                if( nVk >= Qt::Key_F1 && nVk <= Qt::Key_F24 )
                {
                    bFunctionKey = true;
                }
            }
        }
        if( bFunctionKey )
        {
            ui->CBAction->setCurrentIndex( 2 );
            ui->CBAction->setItemData( 2, wKeyValue );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            ui->CBAction->setCurrentIndex( 1 );
            ui->CBAction->setItemData( 1, wKeyValue );

            SetEditMode( ModeUserDefined, wKeyValue );
        }
    }
    else
    {
        if( nSelIndex == 2 )
        {
            ui->CBAction->setItemData( 2, wKeyValue );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            SetEditMode( ModeNone, wKeyValue );
        }
    }
}

void QEditActionWnd::SetEditMode( EditMode Mode, ushort wKeyValue )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    switch( Mode )
    {
    case ModeNone:
        ui->GBUserDefKey->hide();
        ui->LBKey->hide();
        ui->LBUserDefKey->show();
        ui->LBUserDefKey->hide();
        ui->LBUserDefKey->Reset();
        break;
    case ModeUserDefined:
        if( wKeyValue == 0xFFFF )
            ui->LBUserDefKey->Reset();
        else
            ui->LBUserDefKey->SetKeyValue( wKeyValue );

        ui->GBUserDefKey->setTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_USER_DEFINE_KEY") ) );
        ui->GBUserDefKey->show();
        ui->LBKey->show();
        ui->LBUserDefKey->SetUseFuncKey( false );
        ui->LBUserDefKey->show();
        ui->LBUserDefKey->setFocus();
        break;
    case ModeFunctionKey:
        ui->LBUserDefKey->hide();
        ui->LBUserDefKey->SetUseFuncKey( true );
        ui->LBUserDefKey->show();
        ui->GBUserDefKey->setTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_FUNCTION_KEY") ) );
        ui->GBUserDefKey->show();
        ui->LBKey->show();

        ui->LBUserDefKey->Reset();
        if( wKeyValue == 0xFFFF )
        {
            ui->LBDetail->setText( GetMappingStr(58, 00) );
            break;
        }
        ui->LBUserDefKey->SetKeyValue( wKeyValue );

        ui->LBUserDefKey->SetFuncKeyCurrentIndex( -1 );
        for( int nF = 0 ; nF<ui->LBUserDefKey->GetFuncKeyCount(); nF++ )
        {
            ushort wFnKey = (ushort)ui->LBUserDefKey->GetItemData( nF ).toInt();
            if( wFnKey == (wKeyValue&0xFF) )
            {
                ui->LBUserDefKey->SetFuncKeyCurrentIndex( nF );
                break;
            }
        }
        break;
    }
}

void QEditActionWnd::on_CBAction_currentIndexChanged(int /*index*/)
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    int nSel = ui->CBAction->currentIndex();
    if( nSel < 0 ) return;

    ushort wKeyValue = (ushort)ui->CBAction->itemData(nSel).toInt();

    if( nSel == 1 )
    {
        ui->CBAction->setItemData( 2, 0x0000 );
        SetEditMode( ModeUserDefined, wKeyValue );
    }
    else if( nSel == 2 )
    {
        ui->CBAction->setItemData( 1, 0x0000 );
        SetEditMode( ModeFunctionKey, wKeyValue );
        setFocusPolicy( Qt::StrongFocus );
    }
    else
    {
        SetEditMode( ModeNone, wKeyValue );
        setFocusPolicy( Qt::NoFocus );
    }

    if( wKeyValue == 0xFFFF )
    {
        ui->LBDetail->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NOT_DEFINED") ) );
        return;
    }

    QString strDetail( GetMappingStr(wKeyValue>>8, wKeyValue&0xFF) );
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    ui->LBDetail->setText( strDetail );
}

void QEditActionWnd::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        if( ui->CBAction->currentIndex() == 1 || ui->CBAction->currentIndex() == 2 )
        {
            ushort wKeyValue = ui->LBUserDefKey->GetKeyValue();
            ui->LBDetail->setText( GetMappingStr(wKeyValue>>8, wKeyValue&0xFF) );
        }
        ui->LBUserDefKey->update();
    }
}

void QEditActionWnd::hideEvent(QHideEvent *evt)
{
    QDialog::hideEvent(evt);

    if( evt->type() == QEvent::Hide )
    {
        m_pTableWnd->ResetSelect();
    }
}

void QEditActionWnd::on_BtnApply_clicked()
{
    int nSel = ui->CBAction->currentIndex();
    if( nSel < 0 ) return;

    ushort wKeyValue = (ushort)ui->CBAction->itemData(nSel).toInt();

    if( wKeyValue == 0xFFFF )
    {
        hide();
        return;
    }

    QString str;
    switch( m_nProfileIndex )
    {
    case 0:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValue );
        break;
    case 1:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValue );
        break;
    case 2:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile3, m_cProfileKey, wKeyValue );
        break;
    case 3:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile4, m_cProfileKey, wKeyValue );
        break;
    case 4:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile5, m_cProfileKey, wKeyValue );
        break;
    default:
        hide();
        return;
    }

    m_pT3kHandle->SendCommand( (const char*)str.toUtf8().data(), true );

    hide();
}

void QEditActionWnd::OnKeyEditChange( ushort wKeyValue )
{
    ui->CBAction->setItemData( 1, wKeyValue );
    ui->CBAction->setItemData( 2, wKeyValue );
    on_CBAction_currentIndexChanged(0);
}

void QEditActionWnd::onFunctionKeyActivated(int /*index*/)
{
    int nFuncSelIndex = ui->LBUserDefKey->GetFuncKeyCurrentIndex();
    if( nFuncSelIndex < 0 )
        return;

    ushort wKeyValue = (ushort)ui->LBUserDefKey->GetKeyValue();//itemData( nFuncSelIndex ).toInt();
    ui->CBAction->setItemData( 2, wKeyValue );

    QString strDetail( GetMappingStr(wKeyValue>>8, wKeyValue&0xFF) );
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    ui->LBDetail->setText( strDetail );
}

void QEditActionWnd::on_CBAction_activated(int index)
{
    on_CBAction_currentIndexChanged(index);
}

void QEditActionWnd::onKeyPress_Signal(ushort nValue)
{
    OnKeyEditChange( nValue );
}
