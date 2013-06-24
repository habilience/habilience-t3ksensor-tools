#include "QEditActionEDWnd.h"
#include "ui_QEditActionEDWnd.h"

#include "KeyMapStr.h"
#include "T30xHandle.h"
#include "QMouseMappingTable.h"

#include "stdInclude.h"
#include "Common/nv.h"

#include "T3kPacketDef.h"

#include <QShowEvent>
#include <QHideEvent>

QEditActionEDWnd::QEditActionEDWnd(T30xHandle*& pHandle, QWidget *parent) :
    QDialog(parent), m_pTableWnd(NULL),
    ui(new Ui::QEditActionEDWnd), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    ui->setupUi(this);

#if defined(Q_OS_WIN) || defined(Q_OS_X11)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    m_bComboBoxInit = false;

    connect( ui->BtnCancel, SIGNAL(clicked()), this, SLOT(hide()) );

    m_pTableWnd = (QMouseMappingTable*)parent;

    OnChangeLanguage();

    setFixedSize( width(), height() );
}

QEditActionEDWnd::~QEditActionEDWnd()
{
    delete ui;
}

void QEditActionEDWnd::OnChangeLanguage()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TITLE_CAPTION_EDIT_PROFILE_ITEM") ) );
    ui->TitleProfileItem->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM") ) );
    ui->TitleAction->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_ACTION") ) );
    ui->BtnApply->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_APPLY") ) );
    ui->BtnCancel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_CANCEL") ) );

    if( Res.IsR2L() )
        ui->EditProfileItem->setAlignment( Qt::AlignRight );
    else
        ui->EditProfileItem->setAlignment( Qt::AlignLeft );
}

void QEditActionEDWnd::SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue, ushort wFlags )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue = wKeyValue;
    m_wProfileFlags = wFlags;

    uchar cMouseKey = 0;

    QString strLoc;
    switch( cKey )
    {
    case MM_GESTURE_SINGLE_MOVE:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_SINGLE") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DRAG") );
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_MOVE;
        break;
    case MM_GESTURE_SINGLE_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_SINGLE") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_TAP") );
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_CLICK;
        break;
    case MM_GESTURE_SINGLE_DOUBLE_TAP:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_SINGLE") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DOUBLE_TAP") );
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK;
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->EditProfileItem->setText( strLoc );

    ui->CBAction->clear();
    ui->CBAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DISABLED") ) );

    QString strCap = MouseKeyToString(cMouseKey);
    strCap.replace( "\r\n", " " );
    ui->CBAction->addItem( strCap );

    m_bComboBoxInit = true;
    ui->CBAction->setCurrentIndex(0);
    ui->LBDetail->setText( "" );

    if( m_wProfileFlags & m_wProfileValue )
    {
        QString strDetail = MouseKeyToString(cMouseKey);
        strDetail.replace( "\r\n", " " );
        strDetail.replace( "Click", "Button Click" );
        strDetail.replace( "Drag", "Button Drag" );

        ui->LBDetail->setText( strDetail );

        ui->CBAction->setCurrentIndex(1);
    }
}

void QEditActionEDWnd::on_CBAction_currentIndexChanged(int /*index*/)
{
    if( !m_bComboBoxInit ) return;
    int nSel = ui->CBAction->currentIndex();
    if( nSel < 0 ) return;

    if( nSel == 0 )
    {
        m_wProfileFlags &= ~m_wProfileValue;
    }
    else
    {
        m_wProfileFlags |= m_wProfileValue;
    }

    if( m_wProfileFlags & m_wProfileValue )
    {
        uchar cMouseKey = 0;
        switch( m_cProfileKey )
        {
        case MM_GESTURE_SINGLE_MOVE:
            cMouseKey = MM_MOUSE_KEY1_MOUSE_L_MOVE;
            break;
        case MM_GESTURE_SINGLE_TAP:
            cMouseKey = MM_MOUSE_KEY1_MOUSE_L_CLICK;
            break;
        case MM_GESTURE_SINGLE_DOUBLE_TAP:
            cMouseKey = MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK;
            break;
        default:
            break;
        }
        QString strDetail = MouseKeyToString(cMouseKey);
        strDetail.replace( "\r\n", " " );
        strDetail.replace( "Click", "Button Click" );
        strDetail.replace( "Drag", "Button Drag" );

        ui->LBDetail->setText( strDetail );
    }
    else
    {
        ui->LBDetail->setText( "" );
    }
}

void QEditActionEDWnd::hideEvent(QHideEvent *evt)
{
    if( evt->type() == QEvent::Hide )
    {
        m_pTableWnd->ResetSelect();
        m_bComboBoxInit = false;
    }
}

void QEditActionEDWnd::on_BtnApply_clicked()
{
    int nSel = ui->CBAction->currentIndex();
    if( nSel < 0 ) return;

    QString str;
    switch( m_nProfileIndex )
    {
    case 0:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile1, 0x00, m_wProfileFlags );
        break;
    case 1:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile2, 0x00, m_wProfileFlags );
        break;
    case 2:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile3, 0x00, m_wProfileFlags );
        break;
    case 3:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile4, 0x00, m_wProfileFlags );
        break;
    case 4:
        str = str.sprintf( "%s%02X%04X", cstrMouseProfile5, 0x00, m_wProfileFlags );
        break;
    default:
        hide();
        return;
    }

    m_pT3kHandle->SendCommand( (const char*)str.toUtf8().data(), true );

    hide();
}
