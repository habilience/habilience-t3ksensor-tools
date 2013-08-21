#include "QEditActionEnableDialog.h"
#include "ui_QEditActionEnableDialog.h"
#include "QLangManager.h"

#include "QKeyMapStr.h"
#include "QGestureMappingTable.h"
#include "QT3kDevice.h"
#include "../common/T3kConstStr.h"

#define RES_TAG "EDIT PROFILE ITEM"
#define MAIN_TAG "MAIN"
#define GSP_TAG "GESTURE PROFILE DIALOG"

QEditActionEnableDialog::QEditActionEnableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditActionEnableDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Dialog|Qt::WindowTitleHint|Qt::CustomizeWindowHint;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    onChangeLanguage();

    connect( ui->cbAction, SIGNAL(currentIndexChanged(int)), SLOT(onComboIndexChanged(int)));

    ui->btnCancel->setFocus();
}

QEditActionEnableDialog::~QEditActionEnableDialog()
{
    delete ui;
}

void QEditActionEnableDialog::onChangeLanguage()
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

    ui->btnApply->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_APPLY") );
    ui->btnCancel->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CANCEL") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void QEditActionEnableDialog::accept() {}
void QEditActionEnableDialog::reject() {}

void QEditActionEnableDialog::setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue, unsigned short wFlags )
{
    unsigned char cMouseKey;

    QString strLoc;
    QLangRes& res = QLangManager::getResource();
    QString strSingle = res.getResString(GSP_TAG, "TEXT_SINGLE");
    QString strDrag = res.getResString(GSP_TAG, "TEXT_DRAG");
    QString strTap = res.getResString(GSP_TAG, "TEXT_TAP");
    QString strDblTap = res.getResString(GSP_TAG, "TEXT_DOUBLE_TAP");

    switch (cKey)
    {
    case MM_GESTURE_SINGLE_MOVE:
        strLoc = strSingle + " > " + strDrag;
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_MOVE;
        break;
    case MM_GESTURE_SINGLE_TAP:
        strLoc = strSingle + " > " + strTap;
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_CLICK;
        break;
    case MM_GESTURE_SINGLE_DOUBLE_TAP:
        strLoc = strSingle + " > " + strDblTap;
        cMouseKey = MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK;
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->lineEdtProfileItem->setText( strLoc );

    ui->cbAction->clear();
    QString strDisabled = res.getResString(RES_TAG, "TEXT_PROFILE_ITEM_DISABLED");
    ui->cbAction->addItem( strDisabled );

    QString strCap = mouseKeyToString(cMouseKey);
    strCap.replace( "\r\n", " " );
    ui->cbAction->addItem( strCap );

    setCurrentIndexWithoutEvent(ui->cbAction, 0);
    ui->lblKeyDetail->setText("");

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue = wKeyValue;
    m_wProfileFlags = wFlags;

    if (m_wProfileFlags & m_wProfileValue)
    {
        QString strDetail = mouseKeyToString(cMouseKey);
        strDetail.replace( "\r\n", " " );
        strDetail.replace( "Click", "Button Click" );
        strDetail.replace( "Drag", "Button Drag" );

        ui->lblKeyDetail->setText( strDetail );
        setCurrentIndexWithoutEvent(ui->cbAction, 1);
    }
}

void QEditActionEnableDialog::onIndexChangedComboAction( QComboBox* cbAction )
{
    int index = cbAction->currentIndex();
    if (index == 0)
    {
        m_wProfileFlags &= ~m_wProfileValue;
    }
    else
    {
        m_wProfileFlags |= m_wProfileValue;
    }

    if (m_wProfileFlags & m_wProfileValue)
    {
        unsigned char cMouseKey;
        switch (m_cProfileKey)
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
        QString strDetail = mouseKeyToString(cMouseKey);
        strDetail.replace( "\r\n", " " );
        strDetail.replace( "Click", "Button Click" );
        strDetail.replace( "Drag", "Button Drag" );

        ui->lblKeyDetail->setText( strDetail );
    }
    else
    {
        ui->lblKeyDetail->setText( "" );
    }
}

void QEditActionEnableDialog::setCurrentIndexWithoutEvent( QComboBox* cbAction, int index )
{
    if (cbAction == ui->cbAction)
    {
        disconnect(ui->cbAction, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        ui->cbAction->setCurrentIndex(index);
        connect( ui->cbAction, SIGNAL(currentIndexChanged(int)), SLOT(onComboIndexChanged(int)));
    }
}

void QEditActionEnableDialog::onComboIndexChanged(int index)
{
    if (index < 0)
        return;

    onIndexChangedComboAction(ui->cbAction);
}

void QEditActionEnableDialog::on_btnApply_clicked()
{
    int nIndex = ui->cbAction->currentIndex();
    if (nIndex < 0) return;

    char szCmd[256];
    switch (m_nProfileIndex)
    {
    case 0:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile1, 0x00, m_wProfileFlags );
        break;
    case 1:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile2, 0x00, m_wProfileFlags );
        break;
    case 2:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile3, 0x00, m_wProfileFlags );
        break;
    case 3:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile4, 0x00, m_wProfileFlags );
        break;
    case 4:
        snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile5, 0x00, m_wProfileFlags );
        break;
    default:
        close();
        return;
    }
    QT3kDevice::instance()->sendCommand( szCmd, true );

    QDialog::accept();
}

void QEditActionEnableDialog::on_btnCancel_clicked()
{
    QDialog::reject();
}

