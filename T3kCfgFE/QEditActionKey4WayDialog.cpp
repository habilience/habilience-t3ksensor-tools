#include "QEditActionKey4WayDialog.h"
#include "ui_QEditActionKey4WayDialog.h"

#include "QKeyMapStr.h"
#include "QT3kDevice.h"
#include "QGestureMappingTable.h"
#include "QLangManager.h"
#include "../common/T3kConstStr.h"

#include <QEvent>
#include <QFocusEvent>

#define RES_TAG "EDIT PROFILE ITEM"
#define MAIN_TAG "MAIN"
#define GSP_TAG "GESTURE PROFILE DIALOG"

QEditActionKey4WayDialog::QEditActionKey4WayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditActionKey4WayDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Dialog|Qt::WindowTitleHint|Qt::CustomizeWindowHint;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    m_pActiveComboAction = NULL;
    m_bLinkLock = false;

    onChangeLanguage();

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );

    setPredefineProfileTypes( ui->cbActionLeft );
    setPredefineProfileTypes( ui->cbActionRight, true );
    setPredefineProfileTypes( ui->cbActionUp );
    setPredefineProfileTypes( ui->cbActionDown, true );

    connect(ui->widgetUserDefineKey, SIGNAL(keyValueChanged(unsigned short)), SLOT(onKeyValueChanged(unsigned short)));

    connect(ui->cbActionLeft, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionLeftCurrentIndexChanged(int)));
    connect(ui->cbActionRight, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionRightCurrentIndexChanged(int)));
    connect(ui->cbActionUp, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionUpCurrentIndexChanged(int)));
    connect(ui->cbActionDown, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionDownCurrentIndexChanged(int)));

    ui->cbActionLeft->installEventFilter(this);
    ui->cbActionRight->installEventFilter(this);
    ui->cbActionUp->installEventFilter(this);
    ui->cbActionDown->installEventFilter(this);

    ui->btnLinkHorz->setVisible(false);
    ui->btnLinkVert->setVisible(false);

    ui->btnCancel->setFocus();
}

QEditActionKey4WayDialog::~QEditActionKey4WayDialog()
{
    delete ui;
}

void QEditActionKey4WayDialog::onChangeLanguage()
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
    ui->btnCancel->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CANCEL") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QEditActionKey4WayDialog::accept() {}
void QEditActionKey4WayDialog::reject() {}

void QEditActionKey4WayDialog::setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue1, unsigned short wKeyValue2, unsigned short wKeyValue3, unsigned short wKeyValue4 )
{
    setPredefineProfileTypes( ui->cbActionLeft );
    setPredefineProfileTypes( ui->cbActionRight, true );
    setPredefineProfileTypes( ui->cbActionUp );
    setPredefineProfileTypes( ui->cbActionDown, true );

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValues[0] = wKeyValue1;		// left
    m_wProfileValues[1] = wKeyValue2;		// right
    m_wProfileValues[2] = wKeyValue3;       // up
    m_wProfileValues[3] = wKeyValue4;       // down

    QLangRes& res = QLangManager::getResource();
    QString strFingers = res.getResString(GSP_TAG, "TEXT_FINGERS");
    QString strPalm = res.getResString(GSP_TAG, "TEXT_PALM");
    QString strDrag = res.getResString(GSP_TAG, "TEXT_DRAG");

    QString strLoc;
    switch (cKey)
    {
    case MM_GESTURE_FINGERS_MOVE:
        strLoc = strFingers + " > " + strDrag;
        break;
    case MM_GESTURE_PALM_MOVE:
        strLoc = strPalm + " > " + strDrag;
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->lineEdtProfileItem->setText( strLoc );

    setActionWithProfileInfo( ui->cbActionLeft, ui->lblKeyDetailLeft, m_wProfileValues[0] );
    setActionWithProfileInfo( ui->cbActionRight, ui->lblKeyDetailRight, m_wProfileValues[1] );
    setActionWithProfileInfo( ui->cbActionUp, ui->lblKeyDetailUp, m_wProfileValues[2] );
    setActionWithProfileInfo( ui->cbActionDown, ui->lblKeyDetailDown, m_wProfileValues[3] );

    ui->btnLinkHorz->setChecked( false );
    ui->btnLinkVert->setChecked( false );

    int nSel = ui->cbActionLeft->currentIndex();
    if (nSel < 0) return;

    m_bLinkLock = false;
    if ((nSel > 2) && (nSel < 6))
        m_bLinkLock = true;

    if (!m_bLinkLock)
    {
        if (nSel > 2)
        {
            ui->btnLinkHorz->setChecked( true );
            if (ui->btnLinkVert->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
        }

        nSel = ui->cbActionUp->currentIndex();
        if (nSel < 0) return;

        if (nSel > 2)
        {
            ui->btnLinkVert->setChecked( true );
            if (ui->btnLinkHorz->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
        }
    }
    else
    {
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_locked.png") );
    }
}

void QEditActionKey4WayDialog::setActionWithProfileInfo( QComboBox* cbAction, QLabel* lblKeyDetail, unsigned short wKeyValue )
{
    setCurrentIndexWithoutEvent(cbAction, -1);
    lblKeyDetail->setText("");

    QString strDetail = getMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    bool bFound = false;
    int nSelIndex = -1;
    for ( int nI = 0 ; nI < cbAction->count() ; nI ++ )
    {
        QVariant var = cbAction->itemData(nI);
        unsigned short wItemValue = var.value<unsigned short>();

        if (wItemValue == wKeyValue)
        {
            setCurrentIndexWithoutEvent(cbAction, nI);
            nSelIndex = nI;
            bFound = true;
            break;
        }
    }

    lblKeyDetail->setText( strDetail );

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
            setCurrentIndexWithoutEvent( cbAction, 2 );
            cbAction->setItemData( 2, qVariantFromValue(wKeyValue) );

            setEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            setCurrentIndexWithoutEvent( cbAction, 1 );
            cbAction->setItemData( 1, qVariantFromValue(wKeyValue) );

            setEditMode( ModeUserDefined, wKeyValue );
        }
    }
    else
    {
        if (nSelIndex == 2)
        {
            cbAction->setItemData( 2, qVariantFromValue(wKeyValue) );

            setEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            setEditMode( ModeNone, wKeyValue );
        }
    }
}

void QEditActionKey4WayDialog::setPredefineProfileTypes( QComboBox* cbAction, bool bAlter/*=false*/ )
{
    QLangRes& res = QLangManager::getResource();

    cbAction->clear();

    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_DISABLED"), qVariantFromValue(0x0000) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_USER_DEFINED"), qVariantFromValue(0xFFFF) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FUNCTION_KEY"), qVariantFromValue(0xFFFF) );

    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_LEFT_DRAG"), qVariantFromValue(0x8001) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_RIGHT_DRAG"), qVariantFromValue(0x8004) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_MIDDLE_DRAG"), qVariantFromValue(0x8007) );

    cbAction->addItem( "Wheel", qVariantFromValue(0x80FE) );
    cbAction->addItem( "Ctrl+Wheel", qVariantFromValue(0x81FE) );

    if( !bAlter )
    {
        cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FORWARD"), qVariantFromValue(0x0450) );
        cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_BACK"), qVariantFromValue(0x044F) );
    }
    else
    {
        cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_BACK"), qVariantFromValue(0x044F) );
        cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FORWARD"), qVariantFromValue(0x0450) );
    }
}

void QEditActionKey4WayDialog::setEditMode( EditMode mode, unsigned short wKeyValue )
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

void QEditActionKey4WayDialog::onKeyValueChanged(unsigned short wKeyValue)
{
    if (m_pActiveComboAction == NULL)
        return;

    m_pActiveComboAction->setItemData( 1, qVariantFromValue(wKeyValue) );   // user defined key
    m_pActiveComboAction->setItemData( 2, qVariantFromValue(wKeyValue) );   // function key

    if ( (m_pActiveComboAction == ui->cbActionLeft)
         || (m_pActiveComboAction == ui->cbActionRight) )
    {
        if (ui->btnLinkHorz->isChecked())
        {
            if (m_pActiveComboAction == ui->cbActionLeft)
            {
                ui->cbActionRight->setItemData( 1, qVariantFromValue(wKeyValue) );
                onIndexChangedComboAction( ui->cbActionRight );
            }
            else if (m_pActiveComboAction == ui->cbActionRight)
            {
                ui->cbActionLeft->setItemData( 1, qVariantFromValue(wKeyValue) );
                onIndexChangedComboAction( ui->cbActionLeft );
            }
        }
    }
    else if ( (m_pActiveComboAction == ui->cbActionUp)
              || (m_pActiveComboAction == ui->cbActionDown) )
    {
        if (ui->btnLinkVert->isChecked())
        {
            if (m_pActiveComboAction == ui->cbActionUp)
            {
                ui->cbActionDown->setItemData( 1, qVariantFromValue(wKeyValue) );
                onIndexChangedComboAction( ui->cbActionDown );
            }
            else if (m_pActiveComboAction == ui->cbActionDown)
            {
                ui->cbActionUp->setItemData( 1, qVariantFromValue(wKeyValue) );
                onIndexChangedComboAction( ui->cbActionUp );
            }
        }
    }

    onIndexChangedComboAction( m_pActiveComboAction );
}

void QEditActionKey4WayDialog::onIndexChangedComboAction( QComboBox* cbAction )
{
    int index = cbAction->currentIndex();
    if (index < 0) return;

    QLabel* lblKeyDetail;
    if (cbAction == ui->cbActionLeft)
    {
        lblKeyDetail = ui->lblKeyDetailLeft;
    }
    else if (cbAction == ui->cbActionRight)
    {
        lblKeyDetail = ui->lblKeyDetailRight;
    }
    else if (cbAction == ui->cbActionUp)
    {
        lblKeyDetail = ui->lblKeyDetailUp;
    }
    else if (cbAction == ui->cbActionDown)
    {
        lblKeyDetail = ui->lblKeyDetailDown;
    }
    else
    {
        return;
    }

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
        lblKeyDetail->setText( res.getResString(RES_TAG, "TEXT_NOT_DEFINED") );
        return;
    }
    QString strDetail = getMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    lblKeyDetail->setText( strDetail );
}

void QEditActionKey4WayDialog::setCurrentIndexWithoutEvent(QComboBox *cbAction, int index)
{
    if (cbAction == ui->cbActionLeft)
    {
        disconnect(ui->cbActionLeft, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionLeft, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionLeftCurrentIndexChanged(int)));
    }
    else if (cbAction == ui->cbActionRight)
    {
        disconnect(ui->cbActionRight, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionRight, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionRightCurrentIndexChanged(int)));
    }
    else if (cbAction == ui->cbActionUp)
    {
        disconnect(ui->cbActionUp, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionUp, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionUpCurrentIndexChanged(int)));
    }
    else if (cbAction == ui->cbActionDown)
    {
        disconnect(ui->cbActionDown, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionDown, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionDownCurrentIndexChanged(int)));
    }
}

void QEditActionKey4WayDialog::onComboActionLeftCurrentIndexChanged(int index)
{
    //qDebug( "onComboActionLeftCurrentIndexChanged: %d", index );
    if ( index < 0 ) return;

    bool bLinkLock = false;
    if ( (index > 2) && (index < 6) )
        bLinkLock = true;

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );

    if (!bLinkLock)
    {
        if (index > 2)
        {
            ui->btnLinkHorz->setChecked( true );
            if (ui->btnLinkVert->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
        }
        else
        {
            if (ui->btnLinkHorz->isChecked())
            {
                setCurrentIndexWithoutEvent(ui->cbActionRight, 0);
                onIndexChangedComboAction( ui->cbActionRight );

                ui->btnLinkHorz->setChecked( false );
                if (ui->btnLinkVert->isChecked())
                    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
            }
        }
    }
    else
    {
        ui->btnLinkHorz->setChecked( false );
        ui->btnLinkVert->setChecked( false );

        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_locked.png") );
    }

    if (!bLinkLock)
    {
        if (m_bLinkLock)
        {
            // reset
            //setCurrentIndexWithoutEvent(ui->cbActionLeft, 0);
            //onIndexChangedComboAction( ui->cbActionLeft );
            setCurrentIndexWithoutEvent(ui->cbActionRight, 0);
            //onIndexChangedComboAction( ui->cbActionRight );
            setCurrentIndexWithoutEvent(ui->cbActionUp, 0);
            //onIndexChangedComboAction( ui->cbActionUp );
            setCurrentIndexWithoutEvent(ui->cbActionDown, 0);
            //onIndexChangedComboAction( ui->cbActionDown );
        }

        if (ui->btnLinkHorz->isChecked())
        {
            setCurrentIndexWithoutEvent(ui->cbActionRight, index);
            onIndexChangedComboAction( ui->cbActionRight );
        }

        onIndexChangedComboAction( ui->cbActionLeft );
    }
    else
    {
        setCurrentIndexWithoutEvent(ui->cbActionRight, index);
        onIndexChangedComboAction( ui->cbActionRight );
        setCurrentIndexWithoutEvent(ui->cbActionUp, index);
        onIndexChangedComboAction( ui->cbActionUp );
        setCurrentIndexWithoutEvent(ui->cbActionDown, index);
        onIndexChangedComboAction( ui->cbActionDown );

        onIndexChangedComboAction( ui->cbActionLeft );
    }

    m_bLinkLock = bLinkLock;
}

void QEditActionKey4WayDialog::onComboActionRightCurrentIndexChanged(int index)
{
    //qDebug( "onComboActionRightCurrentIndexChanged: %d", index );
    if ( index < 0 ) return;

    bool bLinkLock = false;
    if ( (index > 2) && (index < 6) )
        bLinkLock = true;

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );

    if (!bLinkLock)
    {
        if (index > 2)
        {
            ui->btnLinkHorz->setChecked( true );
            if (ui->btnLinkVert->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
        }
        else
        {
            if (ui->btnLinkHorz->isChecked())
            {
                setCurrentIndexWithoutEvent(ui->cbActionLeft, 0);
                onIndexChangedComboAction( ui->cbActionLeft );

                ui->btnLinkHorz->setChecked( false );
                if (ui->btnLinkVert->isChecked())
                    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
            }
        }
    }
    else
    {
        ui->btnLinkHorz->setChecked( false );
        ui->btnLinkVert->setChecked( false );

        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_locked.png") );
    }

    if (!bLinkLock)
    {
        if (m_bLinkLock)
        {
            // reset
            setCurrentIndexWithoutEvent(ui->cbActionLeft, 0);
            //onIndexChangedComboAction( ui->cbActionLeft );
            //setCurrentIndexWithoutEvent(ui->cbActionRight, 0);
            //onIndexChangedComboAction( ui->cbActionRight );
            setCurrentIndexWithoutEvent(ui->cbActionUp, 0);
            //onIndexChangedComboAction( ui->cbActionUp );
            setCurrentIndexWithoutEvent(ui->cbActionDown, 0);
            //onIndexChangedComboAction( ui->cbActionDown );
        }

        if (ui->btnLinkHorz->isChecked())
        {
            setCurrentIndexWithoutEvent(ui->cbActionLeft, index);
            onIndexChangedComboAction( ui->cbActionLeft );
        }

        onIndexChangedComboAction( ui->cbActionRight );
    }
    else
    {
        setCurrentIndexWithoutEvent(ui->cbActionLeft, index);
        onIndexChangedComboAction( ui->cbActionLeft );
        setCurrentIndexWithoutEvent(ui->cbActionUp, index);
        onIndexChangedComboAction( ui->cbActionUp );
        setCurrentIndexWithoutEvent(ui->cbActionDown, index);
        onIndexChangedComboAction( ui->cbActionDown );

        onIndexChangedComboAction( ui->cbActionRight );
    }

    m_bLinkLock = bLinkLock;
}

void QEditActionKey4WayDialog::onComboActionUpCurrentIndexChanged(int index)
{
    //qDebug( "onComboActionUpCurrentIndexChanged: %d", index );
    if ( index < 0 ) return;

    bool bLinkLock = false;
    if ( (index > 2) && (index < 6) )
        bLinkLock = true;

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );

    if (!bLinkLock)
    {
        if (index > 2)
        {
            ui->btnLinkVert->setChecked( true );
            if (ui->btnLinkHorz->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
        }
        else
        {
            if (ui->btnLinkVert->isChecked())
            {
                setCurrentIndexWithoutEvent(ui->cbActionDown, 0);
                onIndexChangedComboAction( ui->cbActionDown );

                ui->btnLinkVert->setChecked( false );
                if (ui->btnLinkHorz->isChecked())
                    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
            }
        }
    }
    else
    {
        ui->btnLinkHorz->setChecked( false );
        ui->btnLinkVert->setChecked( false );

        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_locked.png") );
    }

    if (!bLinkLock)
    {
        if (m_bLinkLock)
        {
            // reset
            setCurrentIndexWithoutEvent(ui->cbActionLeft, 0);
            //onIndexChangedComboAction( ui->cbActionLeft );
            setCurrentIndexWithoutEvent(ui->cbActionRight, 0);
            //onIndexChangedComboAction( ui->cbActionRight );
            //setCurrentIndexWithoutEvent(ui->cbActionUp, 0);
            //onIndexChangedComboAction( ui->cbActionUp );
            setCurrentIndexWithoutEvent(ui->cbActionDown, 0);
            //onIndexChangedComboAction( ui->cbActionDown );
        }

        if (ui->btnLinkVert->isChecked())
        {
            setCurrentIndexWithoutEvent(ui->cbActionDown, index);
            onIndexChangedComboAction( ui->cbActionDown );
        }

        onIndexChangedComboAction( ui->cbActionUp );
    }
    else
    {
        setCurrentIndexWithoutEvent(ui->cbActionLeft, index);
        onIndexChangedComboAction( ui->cbActionLeft );
        setCurrentIndexWithoutEvent(ui->cbActionRight, index);
        onIndexChangedComboAction( ui->cbActionRight );
        setCurrentIndexWithoutEvent(ui->cbActionDown, index);
        onIndexChangedComboAction( ui->cbActionDown );

        onIndexChangedComboAction( ui->cbActionUp );
    }

    m_bLinkLock = bLinkLock;
}

void QEditActionKey4WayDialog::onComboActionDownCurrentIndexChanged(int index)
{
    //qDebug( "onComboActionDownCurrentIndexChanged: %d", index );
    if ( index < 0 ) return;

    bool bLinkLock = false;
    if ( (index > 2) && (index < 6) )
        bLinkLock = true;

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );

    if (!bLinkLock)
    {
        if (index > 2)
        {
            ui->btnLinkVert->setChecked( true );
            if (ui->btnLinkHorz->isChecked())
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
            else
                ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
        }
        else
        {
            if (ui->btnLinkVert->isChecked())
            {
                setCurrentIndexWithoutEvent(ui->cbActionUp, 0);
                onIndexChangedComboAction( ui->cbActionUp );

                ui->btnLinkVert->setChecked( false );
                if (ui->btnLinkHorz->isChecked())
                    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
            }
        }
    }
    else
    {
        ui->btnLinkHorz->setChecked( false );
        ui->btnLinkVert->setChecked( false );

        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_locked.png") );
    }

    if (!bLinkLock)
    {
        if (m_bLinkLock)
        {
            // reset
            setCurrentIndexWithoutEvent(ui->cbActionLeft, 0);
            //onIndexChangedComboAction( ui->cbActionLeft );
            setCurrentIndexWithoutEvent(ui->cbActionRight, 0);
            //onIndexChangedComboAction( ui->cbActionRight );
            setCurrentIndexWithoutEvent(ui->cbActionUp, 0);
            //onIndexChangedComboAction( ui->cbActionUp );
            //setCurrentIndexWithoutEvent(ui->cbActionDown, 0);
            //onIndexChangedComboAction( ui->cbActionDown );
        }

        if (ui->btnLinkVert->isChecked())
        {
            setCurrentIndexWithoutEvent(ui->cbActionUp, index);
            onIndexChangedComboAction( ui->cbActionUp );
        }

        onIndexChangedComboAction( ui->cbActionDown );
    }
    else
    {
        setCurrentIndexWithoutEvent(ui->cbActionLeft, index);
        onIndexChangedComboAction( ui->cbActionLeft );
        setCurrentIndexWithoutEvent(ui->cbActionRight, index);
        onIndexChangedComboAction( ui->cbActionRight );
        setCurrentIndexWithoutEvent(ui->cbActionUp, index);
        onIndexChangedComboAction( ui->cbActionUp );

        onIndexChangedComboAction( ui->cbActionDown );
    }

    m_bLinkLock = bLinkLock;
}

void QEditActionKey4WayDialog::on_btnLinkVert_clicked()
{

}

void QEditActionKey4WayDialog::on_btnLinkHorz_clicked()
{
    if (ui->btnLinkHorz->isChecked())
    {
        ui->btnLinkHorz->setChecked(false);
        if (ui->btnLinkVert->isChecked())
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_vert_linked.png") );
        else
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way.png") );
    }
    else
    {
        ui->btnLinkHorz->setChecked(true);
        if (ui->btnLinkVert->isChecked())
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_linked.png") );
        else
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_4way_horz_linked.png") );
    }
}

void QEditActionKey4WayDialog::on_btnApply_clicked()
{
    unsigned short wKeyValues[4];
    int nIndex;
    QVariant var;
    nIndex = ui->cbActionLeft->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionLeft->itemData(nIndex);
    wKeyValues[0] = var.value<unsigned short>();

    if (wKeyValues[0] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    nIndex = ui->cbActionRight->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionRight->itemData(nIndex);
    wKeyValues[1] = var.value<unsigned short>();

    if (wKeyValues[1] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    nIndex = ui->cbActionUp->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionUp->itemData(nIndex);
    wKeyValues[2] = var.value<unsigned short>();

    if (wKeyValues[2] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    nIndex = ui->cbActionDown->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionDown->itemData(nIndex);
    wKeyValues[3] = var.value<unsigned short>();

    if (wKeyValues[3] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    char szCmd[256];
    switch (m_nProfileIndex)
    {
    case 0:
        snprintf( szCmd, 256, "%s%02X%04X%04X%04X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValues[0], wKeyValues[1], wKeyValues[2], wKeyValues[3] );
        break;
    case 1:
        snprintf( szCmd, 256, "%s%02X%04X%04X%04X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValues[0], wKeyValues[1], wKeyValues[2], wKeyValues[3] );
        break;
    case 2:
        snprintf( szCmd, 256, "%s%02X%04X%04X%04X%04X", cstrMouseProfile3, m_cProfileKey, wKeyValues[0], wKeyValues[1], wKeyValues[2], wKeyValues[3] );
        break;
    case 3:
        snprintf( szCmd, 256, "%s%02X%04X%04X%04X%04X", cstrMouseProfile4, m_cProfileKey, wKeyValues[0], wKeyValues[1], wKeyValues[2], wKeyValues[3] );
        break;
    case 4:
        snprintf( szCmd, 256, "%s%02X%04X%04X%04X%04X", cstrMouseProfile5, m_cProfileKey, wKeyValues[0], wKeyValues[1], wKeyValues[2], wKeyValues[3] );
        break;
    default:
        QDialog::accept();
        return;
    }

    QT3kDevice::instance()->sendCommand( szCmd, true );
    QDialog::accept();
}

void QEditActionKey4WayDialog::on_btnCancel_clicked()
{
    QDialog::reject();
}

bool QEditActionKey4WayDialog::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::FocusIn)
    {
        if (obj == ui->cbActionLeft)
        {
            m_pActiveComboAction = ui->cbActionLeft;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
        else if (obj == ui->cbActionRight)
        {
            m_pActiveComboAction = ui->cbActionRight;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
        else if (obj == ui->cbActionUp)
        {
            m_pActiveComboAction = ui->cbActionUp;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
        else if (obj == ui->cbActionDown)
        {
            m_pActiveComboAction = ui->cbActionDown;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
    }

    return QDialog::eventFilter(obj, evt);
}

