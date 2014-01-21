#include "QEditActionKey2WayDialog.h"
#include "ui_QEditActionKey2WayDialog.h"

#include "QKeyMapStr.h"
#include "QT3kDevice.h"
#include "QGestureMappingTable.h"
#include "QLangManager.h"
#include "T3kConstStr.h"

#include <QEvent>
#include <QFocusEvent>

#define RES_TAG "EDIT PROFILE ITEM"
#define MAIN_TAG "MAIN"
#define GSP_TAG "GESTURE PROFILE DIALOG"

QEditActionKey2WayDialog::QEditActionKey2WayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditActionKey2WayDialog)
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

    m_pActiveComboAction = NULL;
    onChangeLanguage();

    ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom.png") );

    setPredefineProfileTypes( ui->cbActionZoomOut );
    setPredefineProfileTypes( ui->cbActionZoomIn );

    connect(ui->widgetUserDefineKey, SIGNAL(keyValueChanged(unsigned short)), SLOT(onKeyValueChanged(unsigned short)));

    connect(ui->cbActionZoomIn, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionZoomInCurrentIndexChanged(int)));
    connect(ui->cbActionZoomOut, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionZoomOutCurrentIndexChanged(int)));

    ui->cbActionZoomIn->installEventFilter(this);
    ui->cbActionZoomOut->installEventFilter(this);

    ui->btnLink->setVisible(false);

    ui->btnCancel->setFocus();
}

QEditActionKey2WayDialog::~QEditActionKey2WayDialog()
{
    delete ui;
}

void QEditActionKey2WayDialog::onChangeLanguage()
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

void QEditActionKey2WayDialog::accept() {}
void QEditActionKey2WayDialog::reject() {}

void QEditActionKey2WayDialog::setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue1, unsigned short wKeyValue2 )
{
    setPredefineProfileTypes( ui->cbActionZoomOut );
    setPredefineProfileTypes( ui->cbActionZoomIn );

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValues[0] = wKeyValue1;		// Zoom Out
    m_wProfileValues[1] = wKeyValue2;		// Zoom In

    QLangRes& res = QLangManager::getResource();

    QString strLoc;
    switch (cKey)
    {
    case MM_GESTURE_ZOOM:
        strLoc = res.getResString(GSP_TAG, "TEXT_ZOOM");
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->lineEdtProfileItem->setText( strLoc );

    setActionWithProfileInfo( ui->cbActionZoomOut, ui->lblKeyDetailZoomOut, m_wProfileValues[0] );
    setActionWithProfileInfo( ui->cbActionZoomIn, ui->lblKeyDetailZoomIn, m_wProfileValues[1] );

    ui->btnLink->setChecked( false );

    int nSel = ui->cbActionZoomIn->currentIndex();
    if (nSel < 0) return;

    if (nSel > 2)
    {
        ui->btnLink->setChecked( true );
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom_linked.png") );
    }
}

void QEditActionKey2WayDialog::setActionWithProfileInfo( QComboBox* cbAction, QLabel* lblKeyDetail, unsigned short wKeyValue )
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

void QEditActionKey2WayDialog::setPredefineProfileTypes( QComboBox* cbAction )
{
    QLangRes& res = QLangManager::getResource();

    cbAction->clear();

    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_DISABLED"), qVariantFromValue(0x0000) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_USER_DEFINED"), qVariantFromValue(0xFFFF) );
    cbAction->addItem( res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_FUNCTION_KEY"), qVariantFromValue(0xFFFF) );

    cbAction->addItem( "Wheel", qVariantFromValue(0x80FE) );
    cbAction->addItem( "Ctrl+Wheel", qVariantFromValue(0x81FE) );
}

void QEditActionKey2WayDialog::setEditMode( EditMode mode, unsigned short wKeyValue )
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

void QEditActionKey2WayDialog::onKeyValueChanged(unsigned short wKeyValue)
{
    if (m_pActiveComboAction == NULL)
        return;

    int index = m_pActiveComboAction->currentIndex();
    if (index < 0)
        return;
    m_pActiveComboAction->setItemData( index, qVariantFromValue(wKeyValue) );

    if (ui->btnLink->isChecked())
    {
        if (m_pActiveComboAction == ui->cbActionZoomOut)
        {
            ui->cbActionZoomIn->setItemData( index, qVariantFromValue(wKeyValue) );
            onIndexChangedComboAction(ui->cbActionZoomIn);
        }
        else if (m_pActiveComboAction == ui->cbActionZoomIn)
        {
            ui->cbActionZoomOut->setItemData( index, qVariantFromValue(wKeyValue) );
            onIndexChangedComboAction(ui->cbActionZoomOut);
        }
    }

    onIndexChangedComboAction(m_pActiveComboAction);
}

void QEditActionKey2WayDialog::onIndexChangedComboAction( QComboBox* cbAction )
{
    int index = cbAction->currentIndex();
    if (index < 0) return;

    QLabel* lblKeyDetail;
    if (cbAction == ui->cbActionZoomOut)
    {
        lblKeyDetail = ui->lblKeyDetailZoomOut;
    }
    else if (cbAction == ui->cbActionZoomIn)
    {
        lblKeyDetail = ui->lblKeyDetailZoomIn;
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

void QEditActionKey2WayDialog::setCurrentIndexWithoutEvent(QComboBox *cbAction, int index)
{
    if (cbAction == ui->cbActionZoomIn)
    {
        disconnect(ui->cbActionZoomIn, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionZoomIn, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionZoomInCurrentIndexChanged(int)));
    }
    else if (cbAction == ui->cbActionZoomOut)
    {
        disconnect(ui->cbActionZoomOut, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        cbAction->setCurrentIndex(index);
        connect(ui->cbActionZoomOut, SIGNAL(currentIndexChanged(int)), SLOT(onComboActionZoomOutCurrentIndexChanged(int)));
    }
}

void QEditActionKey2WayDialog::onComboActionZoomOutCurrentIndexChanged(int index)
{
    qDebug( "onComboActionZoomOutCurrentIndexChanged: %d", index );
    if ( index < 0 ) return;

    if ( index > 2 )
    {
        ui->btnLink->setChecked( true );
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom_linked.png") );
    }
    else
    {
        if ( ui->btnLink->isChecked() )
        {
            setCurrentIndexWithoutEvent( ui->cbActionZoomIn, 0 );
            onIndexChangedComboAction( ui->cbActionZoomIn );
            ui->btnLink->setChecked( false );
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom.png") );
        }
    }

    if ( ui->btnLink->isChecked() )
    {
        setCurrentIndexWithoutEvent( ui->cbActionZoomIn, index );
        onIndexChangedComboAction( ui->cbActionZoomIn );
    }
    onIndexChangedComboAction( ui->cbActionZoomOut );
}

void QEditActionKey2WayDialog::onComboActionZoomInCurrentIndexChanged(int index)
{
    qDebug( "onComboActionZoomInCurrentIndexChanged: %d", index );

    if ( index < 0 ) return;

    if ( index > 2 )
    {
        ui->btnLink->setChecked( true );
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom_linked.png") );
    }
    else
    {
        if ( ui->btnLink->isChecked() )
        {
            setCurrentIndexWithoutEvent( ui->cbActionZoomOut, 0 );
            onIndexChangedComboAction( ui->cbActionZoomOut );
            ui->btnLink->setChecked( false );
            ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom.png") );
        }
    }

    if ( ui->btnLink->isChecked() )
    {
        setCurrentIndexWithoutEvent( ui->cbActionZoomOut, index );
        onIndexChangedComboAction( ui->cbActionZoomOut );
    }
    onIndexChangedComboAction( ui->cbActionZoomIn );
}

void QEditActionKey2WayDialog::on_btnLink_clicked()
{
    if ( ui->btnLink->isChecked() )
    {
        ui->btnLink->setChecked( false );
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom.png") );
    }
    else
    {
        ui->btnLink->setChecked( true );
        ui->lblIcon->setPixmap( QPixmap(":/T3kCfgFERes/resources/actionedit_zoom_linked.png") );
    }
}

void QEditActionKey2WayDialog::on_btnApply_clicked()
{
    unsigned short wKeyValues[2];
    int nIndex;
    QVariant var;
    nIndex = ui->cbActionZoomOut->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionZoomOut->itemData(nIndex);
    wKeyValues[0] = var.value<unsigned short>();

    if (wKeyValues[0] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    nIndex = ui->cbActionZoomIn->currentIndex();
    if (nIndex < 0) return;

    var = ui->cbActionZoomIn->itemData(nIndex);
    wKeyValues[1] = var.value<unsigned short>();

    if (wKeyValues[1] == 0xFFFF)
    {
        QDialog::accept();
        return;
    }

    char szCmd[256];
    switch (m_nProfileIndex)
    {
    case 0:
        snprintf( szCmd, 256, "%s%02X%04X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValues[0], wKeyValues[1] );
        break;
    case 1:
        snprintf( szCmd, 256, "%s%02X%04X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValues[0], wKeyValues[1] );
        break;
    default:
        QDialog::accept();
        return;
    }

    QT3kDevice::instance()->sendCommand( szCmd, true );
    QDialog::accept();
}

void QEditActionKey2WayDialog::on_btnCancel_clicked()
{
    QDialog::reject();
}

bool QEditActionKey2WayDialog::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::FocusIn)
    {
        if (obj == ui->cbActionZoomIn)
        {
            qDebug( "focus zoom in" );
            m_pActiveComboAction = ui->cbActionZoomIn;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
        else if (obj == ui->cbActionZoomOut)
        {
            qDebug( "focus zoom out" );
            m_pActiveComboAction = ui->cbActionZoomOut;
            onIndexChangedComboAction(m_pActiveComboAction);
        }
    }

    return QDialog::eventFilter(obj, evt);
}
