#include "QAssistanceWidget.h"
#include "ui_QAssistanceWidget.h"

#include "stdInclude.h"

#include "Common/nv.h"

#include <QShowEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QStandardPaths>

#include "QT3kUserData.h"
#include "QWidgetCloseEventManager.h"
#include "T3kCfgWnd.h"

#include "QTabSensorStatus.h"
#include "QTabRemoteAssistance.h"
#include "QTabChat.h"

#include "CfgCustomCmdDef.h"


QAssistanceWidget::QAssistanceWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAssistanceWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    setWindowModality( Qt::WindowModal );
    setModal( true );

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );
    setFixedSize( width(), height() );

    m_nCurrentTab = 0;
    m_bForcedClose = false;

    connect( ui->BtnClose, SIGNAL(clicked()), this, SLOT(close()) );

    ui->TabCamWidget->setFont( font() );

    m_pTabStatus = new QTabSensorStatus( m_pT3kHandle, this );
    m_pTabStatus->setFont( font() );
    m_pTabRemote = NULL;
    m_pTabChat = NULL;

    ui->TabCamWidget->addTab( m_pTabStatus, "Sensor status" );

    QString strPath = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    if( QFile::exists( strPath + "/t3kcfg/config/serverlist.txt" ) )
    {
        m_pTabRemote = new QTabRemoteAssistance( m_pT3kHandle, this );
        m_pTabRemote->setFont( font() );
        m_pTabChat = new QTabChat( this );
        m_pTabChat->setFont( font() );

        connect( m_pTabRemote, SIGNAL(StartRemoteMode()), this, SLOT(onStartRemoteMode()) );
        connect( m_pTabRemote, SIGNAL(EndRemoteMode()), this, SLOT(onEndRemoteMode()) );
        connect( m_pTabRemote, SIGNAL(InputChatMessage(QString)), m_pTabChat, SLOT(onInputAssistanceMsg(QString)) );
        connect( m_pTabRemote, SIGNAL(InputChatMessage(QString)), this, SLOT(onBlinkChat(QString)) );

        ui->TabCamWidget->addTab( m_pTabRemote, "Remote assistance" );
        ui->TabCamWidget->addTab( m_pTabChat, "Chat" );
    }

    onChangeLanguage();
}

QAssistanceWidget::~QAssistanceWidget()
{
    if( m_pTabStatus )
    {
        delete m_pTabStatus;
        m_pTabStatus = NULL;
    }
    if( m_pTabRemote )
    {
        delete m_pTabRemote;
        m_pTabRemote = NULL;
    }
    if( m_pTabChat )
    {
        delete m_pTabChat;
        m_pTabChat = NULL;
    }

    delete ui;
}

void QAssistanceWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("TITLE_CAPTION")) );
    ui->BtnClose->setText( Res.getResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_CAPTION_CLOSE")) );

    ui->TabCamWidget->setTabText( 0, Res.getResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_CAPTION_SAVELOG")) );

    QString strPath = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    if( QFile::exists( strPath + "/t3kcfg/config/serverlist.txt" ) )
    {
        ui->TabCamWidget->setTabText( 1, Res.getResString(QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_REMOTE_ASSISTANCE")) );
        ui->TabCamWidget->setTabText( 2, Res.getResString(QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("TITLE_CHAT")) );
    }
}

void QAssistanceWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    if( strstr(cmd, cstrCalibrationMode) == cmd )
    {
        int nMode;
        nMode = atoi(cmd + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                QWidgetCloseEventManager::instance()->AddClosedWidget( this, 500 );
            }
        }
    }
}

void QAssistanceWidget::TPDP_OnSTT(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, const char *status)
{
    if( !isVisible() ) return;

    if( strstr(status, cstrCalibrationMode) == status )
    {
        int nMode;
        nMode = atoi(status + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                QWidgetCloseEventManager::instance()->AddClosedWidget( this, 500 );
            }
        }
    }
}

void QAssistanceWidget::closeEvent(QCloseEvent *evt)
{
    if( m_bForcedClose )
    {
        m_pTabRemote->ExitRemote();
        QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();
        m_bForcedClose = false;
    }
    else if( QT3kUserData::GetInstance()->GetRemoteSocket()->state() == QAbstractSocket::ConnectedState )
    {
        QLangRes& Res = QLangManager::instance()->getResource();
        QString strMessage = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("MSG_REMOTE_DISCONNECT") );
        QString strMsgTitle = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("TITLE_REMOTE_MSG") );

        QMessageBox msgBox( this );
        msgBox.setWindowTitle( strMsgTitle );
        msgBox.setText( strMessage );
        msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        msgBox.setIcon( QMessageBox::Question );
        msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
        msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
        msgBox.setFont( font() );

        if( msgBox.exec() != QMessageBox::Yes )
        {
            evt->ignore();
            return;
        }

        m_pTabRemote->ExitRemote();
        QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();
    }

    QDialog::closeEvent(evt);
}

void QAssistanceWidget::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);
}

void QAssistanceWidget::hideEvent(QHideEvent *evt)
{
    if( evt->type() == QEvent::Hide )
    {
        QList<QWidget*> pChildren = findChildren<QWidget*>();
        QWidget* pChild = NULL;
        foreach( pChild, pChildren )
        {
            if( pChild )
                pChild->close();
        }
    }
}

void QAssistanceWidget::on_TabCamWidget_currentChanged(int index)
{
    if( !isActiveWindow() ) return;
    switch( index )
    {
    case 0:
        {
            QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
            if( pSocket->state() == QAbstractSocket::ConnectedState )
            {
                QLangRes& Res = QLangManager::instance()->getResource();
                QString strMessage = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("MSG_REMOTE_DISCONNECT") );
                QString strMsgTitle = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("TITLE_REMOTE_MSG") );

                QMessageBox msgBox( this );
                msgBox.setWindowTitle( strMsgTitle );
                msgBox.setText( strMessage );
                msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
                msgBox.setIcon( QMessageBox::Question );
                msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
                msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
                msgBox.setFont( font() );

                if( msgBox.exec() != QMessageBox::Yes )
                {
                    ui->TabCamWidget->setCurrentIndex( m_nCurrentTab );
                    break;
                }

                m_nCurrentTab = index;

                m_pTabRemote->ExitRemote();
                QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();
            }
            else
                m_nCurrentTab = index;
        }
        break;
    case 1:
        //ui->BtnDefault->setEnabled( false );
        m_nCurrentTab = index;
        break;
    case 2:
        if( QT3kUserData::GetInstance()->GetRemoteSocket()->state() != QAbstractSocket::ConnectedState )
        {
            ui->TabCamWidget->setCurrentIndex( m_nCurrentTab );
            break;
        }

        ui->TabCamWidget->blinkTab( -1 );

        m_nCurrentTab = index;
        break;
    default:
        break;
    }
}

void QAssistanceWidget::onStartRemoteMode()
{
    m_pTabChat->StartRemoteMode();
}

void QAssistanceWidget::onEndRemoteMode()
{
    m_pTabChat->EndRemoteMode();

    if( m_nCurrentTab == 2 )
        ui->TabCamWidget->setCurrentIndex( 1 );
    else
        ui->TabCamWidget->setCurrentIndex( m_nCurrentTab );
}

void QAssistanceWidget::onBlinkChat(QString)
{
    if( m_nCurrentTab != 2 )
    {
        ui->TabCamWidget->blinkTab( 2 );
        activateWindow();
    }
}
