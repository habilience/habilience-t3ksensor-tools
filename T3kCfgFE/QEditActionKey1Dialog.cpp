#include "QEditActionKey1Dialog.h"
#include "ui_QEditActionKey1Dialog.h"

#include "QKeyMapStr.h"
#include "QT3kDevice.h"
#include "QGestureMappingTable.h"
#include "QLangManager.h"
#include "T3kConstStr.h"

#define RES_TAG "EDIT PROFILE ITEM"
#define MAIN_TAG "MAIN"
#define GSP_TAG "GESTURE PROFILE DIALOG"

QEditActionKey1Dialog::QEditActionKey1Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditActionKey1Dialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::WindowTitleHint|Qt::CustomizeWindowHint;
#if defined(Q_OS_WIN)
    flags |= Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint;
#elif defined(Q_OS_MAC)
    flags |= Qt::Popup;
#else
    flags |= Qt::Dialog;
#endif
    setWindowFlags(flags);
    setWindowModality( Qt::WindowModal );
    setModal( true );
    setFixedSize(this->size());

    onChangeLanguage();

    connect(ui->widgetUserDefineKey, SIGNAL(keyValueChanged(unsigned short)), SLOT(onKeyValueChanged(unsigned short)));

    connect( ui->cbAction, SIGNAL(currentIndexChanged(int)), SLOT(onComboIndexChanged(int)));

    ui->btnCancel->setFocus();
}

QEditActionKey1Dialog::~QEditActionKey1Dialog()
{
    delete ui;
}

void QEditActionKey1Dialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString(RES_TAG, "TITLE") );

    ui->lblProfileItem->setText( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM") );
    ui->lblAction->setText( res.getResString(RES_TAG, "TEXT_ACTION") );
    ui->gbUserDefinedKey->setTitle( res.getResString(RES_TAG, "TEXT_USER_DEFINE_KEY") );
    ui->lblKey->setText( res.getResString(RES_TAG, "TEXT_KEY") );

    ui->btnApply->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_APPLY") );
    ui->btnCancel->setText( res.getResString("MESSAGEBOX", "BTN_CAPTION_CANCEL") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QEditActionKey1Dialog::accept() {}
void QEditActionKey1Dialog::reject() {}

void QEditActionKey1Dialog::setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue )
{
    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue = wKeyValue;

    QString strLoc;
    ProfileType type = TypeSingle;
    bool bUseTaskSwitcher = false;

    QLangRes& res = QLangManager::getResource();
    QString strSingle = res.getResString(GSP_TAG, "TEXT_SINGLE");
    QString strFingers = res.getResString(GSP_TAG, "TEXT_FINGERS");
    QString strPalm = res.getResString(GSP_TAG, "TEXT_PALM");
    QString strPutNTap = res.getResString(GSP_TAG, "TEXT_PUTNTAP");
    QString strDrag = res.getResString(GSP_TAG, "TEXT_DRAG");
    QString strTap = res.getResString(GSP_TAG, "TEXT_TAP");
    QString strLongTap = res.getResString(GSP_TAG, "TEXT_LONG_TAP");
    QString strDblTap = res.getResString(GSP_TAG, "TEXT_DOUBLE_TAP");
    switch (cKey)
    {
    case MM_GESTURE_SINGLE_LONG_TAP:
        strLoc = strSingle + " > " + strLongTap;
        type = TypeSingle;
        break;
    case MM_GESTURE_FINGERS_TAP:
        strLoc = strFingers + " > " + strTap;
        type = TypeSingle;
        break;
    case MM_GESTURE_FINGERS_DOUBLE_TAP:
        strLoc = strFingers + " > " + strDblTap;
        type = TypeDouble;
        break;
    case MM_GESTURE_FINGERS_LONG_TAP:
        strLoc = strFingers + " > " + strLongTap;
        type = TypeSingle;
        break;
    case MM_GESTURE_PALM_MOVE:
        strLoc = strPalm + " > " + strDrag;
        type = TypeMove;
        break;
    case MM_GESTURE_PALM_TAP:
        strLoc = strPalm + " > " + strTap;
        type = TypeSingle;
        break;
    case MM_GESTURE_PALM_DOUBLE_TAP:
        strLoc = strPalm + " > " + strDblTap;
        type = TypeDouble;
        break;
    case MM_GESTURE_PALM_LONG_TAP:
        strLoc = strPalm + " > " + strLongTap;
        type = TypeSingle;
        break;
    case MM_GESTURE_PUTAND_TAP:
        strLoc = strPutNTap + " > " + strTap;
        type = TypeSingle;
        bUseTaskSwitcher = true;
        break;
    case MM_GESTURE_PUTAND_DOUBLE_TAP:
        strLoc = strPutNTap + " > " + strDblTap;
        type = TypeDouble;
        bUseTaskSwitcher = true;
        break;
    case MM_GESTURE_PUTAND_LONG_TAP:
        strLoc = strPutNTap + " > " + strLongTap;
        type = TypeSingle;
        bUseTaskSwitcher = true;
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    setPredefineProfileTypes( type, bUseTaskSwitcher );

    ui->lineEdtProfileItem->setText( strLoc );

    setCurrentIndexWithoutEvent(ui->cbAction, -1);
    ui->lblKeyDetail->setText( "" );

    QString strDetail = getMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    bool bFound = false;
    int nSelIndex = -1;
    for (int nI = 0 ; nI < ui->cbAction->count() ; nI ++)
    {
        QVariant var = ui->cbAction->itemData(nI);
        unsigned short wItemValue = var.value<unsigned short>();
        if (wItemValue == m_wProfileValue)
        {
            setCurrentIndexWithoutEvent(ui->cbAction, nI);
            nSelIndex = nI;
            bFound = true;
            break;
        }
    }

    ui->lblKeyDetail->setText( strDetail );

    if (!bFound)
    {
        bool bFunctionKey = false;
        if (!(wKeyValue >> 8 & 0x80))
        {
            int nScanCode;
            if ( findScanCode(wKeyValue & 0xFF, nScanCode) )
            {
                unsigned long nVk = scanCodeToVirtualKey(nScanCode);
                if (nVk >= Qt::Key_F1 && nVk <= Qt::Key_F24)
                {
                    bFunctionKey = true;
                }
            }
        }
        if (bFunctionKey)
        {
            setCurrentIndexWithoutEvent(ui->cbAction, 2);
            ui->cbAction->setItemData( 2, qVariantFromValue(wKeyValue) );

            setEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            setCurrentIndexWithoutEvent(ui->cbAction, 1);
            ui->cbAction->setItemData( 1, qVariantFromValue(wKeyValue) );

            setEditMode( ModeUserDefined, wKeyValue );
        }
    }
    else
    {
        if (nSelIndex == 2)
        {
            ui->cbAction->setItemData( 2, qVariantFromValue(wKeyValue) );

            setEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            setEditMode( ModeNone, wKeyValue );
        }
    }
}

void QEditActionKey1Dialog::setPredefineProfileTypes( ProfileType type, bool bUseTaskSwitcher )
{
    QLangRes& res = QLangManager::getResource();

    ui->cbAction->clear();

    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_DISABLED"), qVariantFromValue(0x0000) );
    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_USER_DEFINED"), qVariantFromValue(0xFFFF) );
    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FUNCTION_KEY"), qVariantFromValue(0xFFFF) );

    switch( type )
    {
    case TypeMove:
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_LEFT_DRAG"), qVariantFromValue(0x8001) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_RIGHT_DRAG"), qVariantFromValue(0x8004) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_MIDDLE_DRAG"), qVariantFromValue(0x8007) );
        ui->cbAction->addItem( "Wheel", qVariantFromValue(0x80FE) );
        ui->cbAction->addItem( "Ctrl+Wheel", qVariantFromValue(0x81FE) );
        break;
    case TypeSingle:
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_LEFT_CLICK"), qVariantFromValue(0x8002) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_RIGHT_CLICK"), qVariantFromValue(0x8005) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_MIDDLE_CLICK"), qVariantFromValue(0x8008) );
        break;
    case TypeDouble:
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_LEFT_DBLCLICK"), qVariantFromValue(0x8003) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_RIGHT_DBLCLICK"), qVariantFromValue(0x8006) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_MIDDLE_DBLCLICK"), qVariantFromValue(0x8009) );
        break;
    }

    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FORWARD"), qVariantFromValue(0x0450) );
    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_BACK"), qVariantFromValue(0x044F) );

    if (bUseTaskSwitcher)
    {
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_TASK_SWITCH1"), qVariantFromValue(0x042B) );
        ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_TASK_SWITCH2"), qVariantFromValue(0x082B) );
    }

    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_TASK_MANAGER"), qVariantFromValue(0x0329) );
    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_EXPLORER"), qVariantFromValue(0x0808) );
    ui->cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_DESKTOP"), qVariantFromValue(0x0807) );
}

void QEditActionKey1Dialog::setEditMode( EditMode mode, unsigned short wKeyValue )
{
    QLangRes& res = QLangManager::getResource();
    switch ( mode )
    {
    case ModeNone:
        ui->gbUserDefinedKey->setVisible(false);
        ui->lblKey->setVisible(false);
        ui->widgetUserDefineKey->reset();
        ui->widgetUserDefineKey->setVisible(false);
        break;
    case ModeUserDefined:
        ui->widgetUserDefineKey->setKeyType( QUserDefinedKeyCtrl::KeyTypeNormal );
        if (wKeyValue != 0xFFFF)
        {
            ui->widgetUserDefineKey->setKeyValue( wKeyValue );
        }
        ui->gbUserDefinedKey->setTitle( res.getResString(RES_TAG, "TEXT_USER_DEFINE_KEY") );
        ui->gbUserDefinedKey->setVisible(true);
        ui->lblKey->setVisible(true);
        ui->widgetUserDefineKey->setVisible( true );
        break;
    case ModeFunctionKey:
        ui->widgetUserDefineKey->setKeyType( QUserDefinedKeyCtrl::KeyTypeFunction );
        if (wKeyValue != 0xFFFF)
        {
            ui->widgetUserDefineKey->setKeyValue( wKeyValue );
        }
        ui->gbUserDefinedKey->setTitle( res.getResString(RES_TAG, "TEXT_FUNCTION_KEY") );
        ui->gbUserDefinedKey->setVisible(true);
        ui->lblKey->setVisible(true);
        ui->widgetUserDefineKey->setVisible( true );
        break;
    }
}

void QEditActionKey1Dialog::onKeyValueChanged(unsigned short wKeyValue)
{
    int index = ui->cbAction->currentIndex();
    if ( index >= 0 )
    {
        ui->cbAction->setItemData( index, qVariantFromValue(wKeyValue) );
    }
    onIndexChangedComboAction(ui->cbAction);
}

void QEditActionKey1Dialog::onIndexChangedComboAction( QComboBox* cbAction )
{
    int index = cbAction->currentIndex();
    if (index < 0) return;

    QLangRes& res = QLangManager::getResource();

    QVariant var = cbAction->itemData(index);
    unsigned short wKeyValue = var.value<unsigned short>();

    if (index == 1)
    {
        setEditMode( ModeUserDefined, wKeyValue );
    }
    else if(index == 2)
    {
        setEditMode( ModeFunctionKey, wKeyValue );
    }
    else
    {
        setEditMode( ModeNone, wKeyValue );
    }

    if (wKeyValue == 0xFFFF)
    {
        ui->lblKeyDetail->setText( res.getResString(RES_TAG, "TEXT_NOT_DEFINED") );
        return;
    }
    QString strDetail = getMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    ui->lblKeyDetail->setText( strDetail );
}

void QEditActionKey1Dialog::setCurrentIndexWithoutEvent( QComboBox* cbAction, int index )
{
    if (cbAction == ui->cbAction)
    {
        disconnect(ui->cbAction, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        ui->cbAction->setCurrentIndex(index);
        connect( ui->cbAction, SIGNAL(currentIndexChanged(int)), SLOT(onComboIndexChanged(int)));
    }
}

void QEditActionKey1Dialog::onComboIndexChanged(int index)
{
    if (index < 0) return;

    onIndexChangedComboAction(ui->cbAction);
}

void QEditActionKey1Dialog::on_btnApply_clicked()
{
    int nIndex = ui->cbAction->currentIndex();
    if (nIndex < 0) return;

    QVariant var = ui->cbAction->itemData(nIndex);
    unsigned short wKeyValue = var.value<unsigned short>();

    if (wKeyValue == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    char szCmd[256];
    switch (m_nProfileIndex)
    {
    case 0:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValue );
        break;
    case 1:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValue );
        break;
    default:
        QDialog::accept();
        return;
    }

    QT3kDevice::instance()->sendCommand( szCmd, true );
    QDialog::accept();
}

void QEditActionKey1Dialog::on_btnCancel_clicked()
{
    QDialog::reject();
}
