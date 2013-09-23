#include "QTabRemoteAssistance.h"
#include "ui_QTabRemoteAssistance.h"

#include <QSettings>
#include <QFile>
#include <QMovie>
#include <QDesktopWidget>
#include <QHostAddress>
#include <QDir>
#include <QStandardPaths>

#include "PacketStructure.h"

#include "QSideviewWidget.h"
#include "QOrderTouchWidget.h"
#include "QT3kLoadEnvironmentObject.h"

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#define SVC_TABLETINPUT         "TabletInputService"

#define MAX_PROCESS_BUFFER      1024
#define RECONNECT_WAIT_TIME     60      // 60 sec
#define REQUEST_COUNT           3

QString GetAbstractSocketErrorStr(QAbstractSocket::SocketError err);

QTabRemoteAssistance::QTabRemoteAssistance(T3kHandle *&pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTabRemoteAssistance), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);

    setFont( parent->font() );

    m_pProcessBuffer = new char[MAX_PROCESS_BUFFER];
    m_eConnectState = Disconnected;
    m_nTimer = 0;
    m_nWaitTime = RECONNECT_WAIT_TIME;
    m_nPreviousID = 0;
    m_nRequestCount = 0;

    m_nTIDClientID = 0;
    m_nTimerAlive = 0;
    m_bCheckAlive = false;
    m_nNotAliveCount = 0;

    ui->LBLed->SetStatus( QSimpleLed::SDisconnect );

    ui->StackedWidget->setCurrentIndex( 0 );

    LoadServerList();

    m_pSideviewWidget = new QSideviewWidget( parentWidget() );
    m_pSideviewWidget->hide();
    m_pSideviewWidget->SetT3kHandle(pHandle);

    m_pOrderTouchWidget = new QOrderTouchWidget();
    m_pOrderTouchWidget->hide();

    connect( &m_TimeOutChecker, SIGNAL(TimeOut(int)), this, SLOT(onTimeOutChecker(int)) );

    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
    connect( pSocket, SIGNAL(connected()), this, SLOT(onConnectedRemote()) );
    connect( pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnectedRemote()) );
    connect( pSocket, SIGNAL(readyRead()), this, SLOT(onReadReadyRemote()) );

    connect( this, SIGNAL(ShowSideviewMode(bool)), this, SLOT(onShowSideviewMode(bool)), Qt::QueuedConnection );
    connect( this, SIGNAL(ShowOrderTouch(bool,bool,bool,short,short,int)), this, SLOT(onShowOrderTouch(bool,bool,bool,short,short,int)), Qt::QueuedConnection );

    onChangeLanguage();
}

QTabRemoteAssistance::~QTabRemoteAssistance()
{
    if( m_nTimer )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;
    }
    if( m_nTimerAlive )
    {
        killTimer( m_nTimerAlive );
        m_nTimerAlive = 0;
    }
    m_TimeOutChecker.Stop();

    if( m_pSideviewWidget )
    {
        delete m_pSideviewWidget;
        m_pSideviewWidget = NULL;
    }

    delete ui;

    if( m_pProcessBuffer )
    {
        delete[] m_pProcessBuffer;
        m_pProcessBuffer = NULL;
    }
}

void QTabRemoteAssistance::LoadServerList()
{
    QString strPath = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    strPath += "t3kcfg/config/";

    m_strServierListPath = strPath;
    if( !QFile::exists( m_strServierListPath + "serverlist.txt" ) )
    {
        QDir().mkpath( strPath );
        QFile fResource(":/T3kCfgRes/Config/serverlist.txt");
        if( fResource.open( QIODevice::ReadOnly ) )
        {
            QFile fSaveFile( m_strServierListPath + "serverlist.txt" );
            if( fSaveFile.open( QIODevice::WriteOnly ) )
            {
                fSaveFile.write( fResource.readAll() );
                fSaveFile.close();
            }

            fResource.close();
        }
    }

    QString strPort;
    QSettings iniServer( m_strServierListPath + "serverlist.txt", QSettings::IniFormat );
    iniServer.beginGroup( "SERVER_LIST" );
    QStringList strServerList = iniServer.childKeys();
    foreach( QString str, strServerList )
    {
        if( str.contains("server",Qt::CaseInsensitive) )
            ui->CBSelectServer->addItem( iniServer.value( str ).toString() );
        else if( str.contains("port",Qt::CaseInsensitive) )
            strPort = iniServer.value( str ).toString();
    }

    if( strPort.isEmpty() )
        strPort = QString("%1").arg(RemoteClientPortDef);

    iniServer.endGroup();

    ui->EditPort->setText( strPort );
}

void QTabRemoteAssistance::SaveChangedPort()
{
    if( !QFile::exists( m_strServierListPath + "serverlist.txt" ) )
    {
        QDir().mkpath( m_strServierListPath );
        QFile fResource(":/T3kCfgRes/Config/serverlist.txt");
        if( fResource.open( QIODevice::ReadOnly ) )
        {
            QFile fSaveFile( m_strServierListPath + "serverlist.txt" );
            if( fSaveFile.open( QIODevice::WriteOnly ) )
            {
                fSaveFile.write( fResource.readAll() );
                fSaveFile.close();
            }

            fResource.close();
        }
    }

    QSettings iniServer( m_strServierListPath + "serverlist.txt", QSettings::IniFormat );
    iniServer.beginGroup( "SERVER_LIST" );
    iniServer.setValue( "port", ui->EditPort->text().toInt() );
    iniServer.endGroup();
}

void QTabRemoteAssistance::ExitRemote()
{
    if( m_pT3kHandle->GetExpireTime() == 0 )
        m_pT3kHandle->SetExpireTime( 5000 );
    // disconnect
    m_nPreviousID = 0;
    m_eConnectState = Disconnected;

    ui->LBWaiting->Complete();
    ui->LBState->setText( "" );
    ui->LBMessage->setText( "" );
    ui->LBIdentify->setText( "" );
    ui->StackedWidget->setCurrentIndex( 0 );
}

void QTabRemoteAssistance::SendAlive()
{
    m_bCheckAlive = false;
    ui->LBLed->SetStatus( QSimpleLed::SConnectOFF );
    m_nNotAliveCount = 0;

    RHeaderPkt pkt;
    pkt.nType = Client | KeepAlive;
    pkt.nPktSize = sizeof(RHeaderPkt);

    QT3kUserData::GetInstance()->GetRemoteSocket()->write( (const char*)&pkt, pkt.nPktSize );
}

void QTabRemoteAssistance::SendEnvironments()
{
    QByteArray btIP( QT3kUserData::GetInstance()->GetRemoteSocket()->localAddress().toString().toUtf8() );
    QByteArray btOS( QT3kLoadEnvironmentObject::GetOSDisplayString().toUtf8() );

#ifdef Q_OS_WIN
    QVector<PairRSP> vHIDState = QT3kLoadEnvironmentObject::GetHIDState();
    unsigned long dwHIDState = 0x00000000;
    for( int i=0; i<vHIDState.count(); i++ )
    {
        PairRSP stEnv = vHIDState.at(i);
        if( stEnv.strData.compare("OK", Qt::CaseInsensitive) == 0 )
            dwHIDState |= (0x00000001 << i);
    }
#endif
    QDesktopWidget w;
    int nPrimaryIndex = w.primaryScreen();

    QRect rcScreen( w.screenGeometry(nPrimaryIndex) );

    QByteArray btScreen( QString("%1,%2,%3,%4").arg(rcScreen.x()).arg(rcScreen.y()).
                         arg(rcScreen.width()).arg(rcScreen.height()).toUtf8() );

    unsigned short nPktLen = sizeof(RClientEnvironments) + btScreen.size() + btIP.size() + btOS.size() + 3;
    char *pBuffer = new char[nPktLen];

    RClientEnvironments* pktEnv = (RClientEnvironments*)pBuffer;
    pktEnv->Header.nType = Client | TranClientEnv;
    pktEnv->Header.nPktSize = nPktLen;
    pktEnv->nID = m_nPreviousID;
    pktEnv->nDisplayCnt = w.screenCount();
    pktEnv->nDPrimaryIdx = nPrimaryIndex;
#ifdef Q_OS_WIN
    pktEnv->nOrientation = (unsigned short)QT3kLoadEnvironmentObject::GetScreenOrientation();
    pktEnv->bTabletInput = QT3kLoadEnvironmentObject::IsServiceRunning( SVC_TABLETINPUT );
    pktEnv->dwHIDState = dwHIDState;
#endif
    ::memcpy( pktEnv->szIPOSString, btScreen.data(), btScreen.size() );
    pktEnv->szIPOSString[btScreen.size()] = '\0';
    ::memcpy( pktEnv->szIPOSString+btScreen.size()+1, btIP.data(), btIP.size() );
    pktEnv->szIPOSString[btScreen.size()+btIP.size()+1] = '\0';
    ::memcpy( pktEnv->szIPOSString+btScreen.size()+btIP.size()+2, btOS.data(), btOS.size() );
    pktEnv->szIPOSString[btScreen.size()+btIP.size()+btOS.size()+2] = '\0';

    int nReqCnt = 3;
    char* pRemainPkt = (char*)pBuffer;
    qint64 nRet = 0;
    qint64 nRemainPkt = nPktLen;
    while( nRemainPkt && nReqCnt )
    {
        nRet = QT3kUserData::GetInstance()->GetRemoteSocket()->write( (const char*)pRemainPkt, nRemainPkt );
        nRemainPkt -= nRet;
        pRemainPkt += nRet;
        nReqCnt--;
    }

    delete[] pBuffer;

    if( nRemainPkt != 0 )
    {
        qDebug( "Send Error : Client Environments" );
        return;
    }
}

void QTabRemoteAssistance::showEvent(QShowEvent *)
{
}

void QTabRemoteAssistance::closeEvent(QCloseEvent *)
{   
    if( QT3kUserData::GetInstance()->GetRemoteSocket()->state() == QAbstractSocket::ConnectedState )
    {
        ExitRemote();
        QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();
    }
}

void QTabRemoteAssistance::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimer )
    {
        m_nWaitTime--;
        ui->LBState->setText( QLangManager::instance()->getResource().getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_RECONNECTING")).arg(m_nWaitTime) );
        if( m_nWaitTime <= 0 )
        {
            killTimer( m_nTimer );
            m_nTimer = 0;
            ui->BtnRequest->setEnabled( true );
            ui->StackedWidget->setCurrentIndex( 0 );
        }
    }
    else if( evt->timerId() == m_nTimerAlive )
    {
        if( !m_bCheckAlive )
        {
            m_nNotAliveCount++;
            if( m_nNotAliveCount > 2 )
            {
                killTimer( m_nTimerAlive );
                m_nTimerAlive = 0;
                ui->LBLed->SetStatus( QSimpleLed::SDisconnect );
                m_bCheckAlive = false;
                m_nNotAliveCount = 0;
                if( QT3kUserData::GetInstance()->GetRemoteSocket()->state() == QAbstractSocket::ConnectedState )
                {
                    ExitRemote();
                    QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();
                }
            }
            else
                SendAlive();
        }
        else
            SendAlive();
    }

    QDialog::timerEvent(evt);
}

void QTabRemoteAssistance::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->TitleRemote->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TITLE_REMOTE")) );
    ui->BtnRequest->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_REQUEST")) );
    ui->LBSelectServer->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_SELECT_SERVER")) );
    ui->LBUserName->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_USERNAME")) );
    ui->LBPort->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_PORT")) );

    ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_EXIT")) );
}

void QTabRemoteAssistance::on_BtnRequest_clicked()
{
    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
    connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onRemoteSocketError(QAbstractSocket::SocketError)) );
    pSocket->connectToHost( ui->CBSelectServer->currentText(), ui->EditPort->text().toInt() );

    m_nPreviousID = 0;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_CANCEL")) );
    ui->LBWaiting->Start();
    ui->LBState->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_CONNECTING")) );
    ui->StackedWidget->setCurrentIndex( 1 );

    SaveChangedPort();
}

void QTabRemoteAssistance::on_BtnExit_clicked()
{
    if( m_eConnectState == Connected )
    {
        m_nPreviousID = 0;
        m_eConnectState = Disconnected;

        RHeaderPkt pkt;
        pkt.nType = Client | NotifyRemoteFinish;
        pkt.nPktSize = sizeof(RHeaderPkt);
        QT3kUserData::GetInstance()->GetRemoteSocket()->write( (const char*)&pkt, pkt.nPktSize );
        QT3kUserData::GetInstance()->GetRemoteSocket()->disconnectFromHost();

        ExitRemote();
    }
    else
    {
        QT3kUserData::GetInstance()->GetRemoteSocket()->abort();

        ui->LBWaiting->Complete();
        ui->LBState->setText( "" );
        ui->LBMessage->setText( "" );
        ui->LBIdentify->setText( "" );
        ui->StackedWidget->setCurrentIndex( 0 );
    }
}

void QTabRemoteAssistance::onConnectedRemote()
{
    m_eConnectState = Connected;
    if( m_nTimer )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;
    }

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_DISCONNECT")) );
    ui->LBState->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_WAIT_MSG")) );
}

void QTabRemoteAssistance::onDisconnectedRemote()
{
    m_ReceivePacket.clear();

    emit ShowSideviewMode( false );
    emit ShowOrderTouch( false, false, false, 0, 0, 0 );

    if( m_eConnectState == Disconnected )
        ui->StackedWidget->setCurrentIndex( 0 );
    else
    {
        m_pT3kHandle->SetExpireTime( 5000 );
        m_pT3kHandle->onReceiveRawDataFlag( false );

        m_eConnectState = Connecting;

        QLangRes& Res = QLangManager::instance()->getResource();

        ui->LBIdentify->setText( "" );
        ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_CANCEL")) );
        ui->LBState->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_RECONNECTING")).arg(60) );

        QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
        pSocket->connectToHost( ui->CBSelectServer->currentText(), ui->EditPort->text().toInt() );

        if( m_nTimer )
            killTimer( m_nTimer );

        m_nWaitTime = RECONNECT_WAIT_TIME;
        m_nTimer = startTimer( 1000 );

        m_TimeOutChecker.Stop( -1 );
    }

    emit EndRemoteMode();
}

void QTabRemoteAssistance::onReadReadyRemote()
{
    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();

    qint64 nAvailableSize = pSocket->bytesAvailable();
    if( !nAvailableSize ) return;

    QByteArray bt = pSocket->readAll();
    m_ReceivePacket.push_back( bt );

    while( m_ReceivePacket.size() >= (int)sizeof(RHeaderPkt) )
    {
        RHeaderPkt* pHeader = (RHeaderPkt*)m_ReceivePacket.data();
        if( pHeader->nPktSize > m_ReceivePacket.size() ) return;

        Q_ASSERT( pHeader->nPktSize );
        Q_ASSERT( MAX_PROCESS_BUFFER >= pHeader->nPktSize );
        ::memcpy( m_pProcessBuffer, m_ReceivePacket.data(), pHeader->nPktSize );
        m_ReceivePacket.remove( 0, pHeader->nPktSize );

        ProcessRemoteRawDataPacket( (RRawDataPkt*)m_pProcessBuffer );
    }
}

void QTabRemoteAssistance::ProcessRemoteRawDataPacket(RRawDataPkt *packet)
{
    switch( packet->Header.nType & 0xF000 )
    {
    case Server:
        {
            switch( packet->Header.nType & 0x0FFF )
            {
            case NotifyPairConnected:
                {
                    // Start Remote mode
                    m_pT3kHandle->SetExpireTime( 0 );
                    m_pT3kHandle->onReceiveRawDataFlag( true );

                    QLangRes& Res = QLangManager::instance()->getResource();

                    ui->LBState->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_REMOTE_CONTROL")) );
                    ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_DISCONNECT")) );

                    emit StartRemoteMode();

                    SendEnvironments();
                }
                break;
            case NotifyPairDisconnected:
                {
                    m_pT3kHandle->SetExpireTime( 5000 );
                    m_pT3kHandle->onReceiveRawDataFlag( false );

                    emit ShowSideviewMode( false );
                    emit ShowOrderTouch( false, false, false, 0, 0, 0 );

                    QLangRes& Res = QLangManager::instance()->getResource();
                    ui->BtnExit->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_CANCEL")) );
                    ui->LBState->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_REPAIRCONNECTION")) );

                    emit EndRemoteMode();
                }
                break;
            case ReqConnectionInfo:
                {
                    // send user name (hostname:inputname, user or controller)
                    char szHostName[128] = "";

                    if( ::gethostname( szHostName, sizeof(szHostName) ) != 0 )
                       sprintf( szHostName, "unknown" );

                    //qDebug( "Host Info : %s", szHostName );

                    RAdditionalPkt info;
                    info.Header.nType = Client | ResConnectionInfo;
                    info.Header.nPktSize = sizeof(RAdditionalPkt);
                    info.uoData.RClientConnectInfo.nPrevID = m_nPreviousID;
                    info.uoData.RClientConnectInfo.nModel = QT3kUserData::GetInstance()->GetModel();
                    QString strUserName = ui->EditUserName->text().isEmpty() ? "unknown" : ui->EditUserName->text();
                    strUserName += ':';
                    strUserName += szHostName;
                    QByteArray baUserName( strUserName.toUtf8() );
                    int nMaxByteLen = baUserName.size() > MAX_NAME ? MAX_NAME : baUserName.size();
                    ::memcpy( info.uoData.RClientConnectInfo.szUserName, baUserName.data(), nMaxByteLen );
                    info.uoData.RClientConnectInfo.szUserName[nMaxByteLen] = '\0';

                    QT3kUserData::GetInstance()->GetRemoteSocket()->write( (const char*)&info, info.Header.nPktSize );

                    if( !m_nTIDClientID )
                        m_nTIDClientID = m_TimeOutChecker.Start( 3000 );
                }
                break;
            case NotifyClientID:
                {
                    m_TimeOutChecker.Stop( m_nTIDClientID );
                    m_nTIDClientID = 0;

                    RAdditionalPkt* pPacket = (RAdditionalPkt*)packet;
                    m_nPreviousID = pPacket->uoData.RClientID.nID;

                    QLangRes& Res = QLangManager::instance()->getResource();
                    ui->LBIdentify->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_ID")).arg(m_nPreviousID) );

                    if( !m_nTimerAlive )
                        m_nTimerAlive = startTimer( 4000 );

                    SendAlive();
                }
                break;
            case KeepAlive:
                {
                    ui->LBLed->SetStatus( QSimpleLed::SConnectON );
                    m_bCheckAlive = true;
                }
                break;
            case ReqClientEnv:
                {
                    SendEnvironments();
                }
                break;
            default:
                break;
            }
        }
        break;
    case Client:
        break;
    case CtrlManager:
        {
            switch( packet->Header.nType & 0x0FFF )
            {
            case TranChatMessage:
                {
                    RChatMessage* pPkt = (RChatMessage*)packet;
                    QString strMsg( QString::fromUtf8( pPkt->szString ) );
                    emit InputChatMessage( strMsg );
                }
                break;
            default:
                break;
            }
        }
        break;
    case CtrlDriver:
        {
            switch( packet->Header.nType & 0x0FFF )
            {
            case TranSensorRawData:
                {
                    RRawDataPkt* pPkt = (RRawDataPkt*)packet;
                    if( m_pT3kHandle->IsOpen() )
                    {
                        //bool bRet =
                        m_pT3kHandle->SendBuffer( (const unsigned char*)pPkt->data, RAWDATA_BLOCK, 1, 1000 );
                        //qDebug( "Send to HID : %d - %s", bRet, (char*)pBuffer+11 );
                    }
                }
                break;
            case TranInstantMode:
                {
                    RInstantMode* pPkt = (RInstantMode*)packet;
                    m_pT3kHandle->SetInstantMode( pPkt->nMode, pPkt->nExpireTime, pPkt->dwGstFlag );
                }
                break;
            case SideviewMode:
                {
                    RAdditionalPkt* pPacket = (RAdditionalPkt*)packet;
                    bool bShow = pPacket->uoData.RSideviewMode.bShow;
                    emit ShowSideviewMode( bShow );
                }
                break;
            case OrderTouch:
                {
                    RAdditionalPkt* pPacket = (RAdditionalPkt*)packet;
                    bool bShowMark = (bool)pPacket->uoData.ROrderTouch.bShowMark;
                    bool bOnScreen = (bool)pPacket->uoData.ROrderTouch.bOnScreen;
                    bool bTouchOK = (bool)pPacket->uoData.ROrderTouch.bTouchOK;
                    short nX = pPacket->uoData.ROrderTouch.nX;
                    short nY = pPacket->uoData.ROrderTouch.nY;
                    short nPercent = pPacket->uoData.ROrderTouch.nPercent;
                    emit ShowOrderTouch( bShowMark, bOnScreen, bTouchOK, nX, nY, nPercent );
                }
                break;
            default:
                Q_ASSERT( false );
                break;
            }
        }
        break;
    default:
        break;
    }
}

void QTabRemoteAssistance::onTimeOutChecker( int nID )
{
    if( nID == m_nTIDClientID )
    {
        if( m_nRequestCount < REQUEST_COUNT )
        {
            m_nRequestCount++;

            RHeaderPkt ReqID;
            ReqID.nType = Client | ReqAssignedID;
            ReqID.nPktSize = sizeof(RHeaderPkt);

            QT3kUserData::GetInstance()->GetRemoteSocket()->write( (const char*)&ReqID, ReqID.nPktSize );
        }
        else
        {
            m_nRequestCount = 0;
            m_TimeOutChecker.Stop( m_nTIDClientID );

            ui->LBWaiting->Complete();

            QLangRes& Res = QLangManager::instance()->getResource();
            ui->LBState->setText( "" );
            ui->LBMessage->setText( Res.getResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TEXT_SERV_NOT_RES")) );
            ui->StackedWidget->setCurrentIndex( 0 );

            m_eConnectState = Disconnected;
            ExitRemote();
            QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
            if( pSocket->state() == QAbstractSocket::ConnectedState )
                pSocket->disconnectFromHost();
        }
    }
}

void QTabRemoteAssistance::onRemoteSocketError(QAbstractSocket::SocketError err)
{
    qDebug( "%s", GetAbstractSocketErrorStr(err).toUtf8().data() );

    if( m_eConnectState == Disconnected )
    {
        ui->LBMessage->setText( GetAbstractSocketErrorStr(err) );
        ui->StackedWidget->setCurrentIndex( 0 );
    }
    else
    {
        QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
        pSocket->connectToHost( ui->CBSelectServer->currentText(), ui->EditPort->text().toInt() );
    }
}

void QTabRemoteAssistance::onShowSideviewMode(bool bShow)
{
    // sideview window
    if( bShow )
        m_pSideviewWidget->show();
    else
        m_pSideviewWidget->hide();
}

void QTabRemoteAssistance::onShowOrderTouch(bool bShowMark, bool bOnScreen, bool bTouchOK, short nX, short nY, int nPercent)
{
    // cross mark window
    if( !m_pOrderTouchWidget->isVisible() )
        m_pOrderTouchWidget->Init();

    m_pOrderTouchWidget->onOrderTouch( bShowMark, bOnScreen, bTouchOK, nX, nY, nPercent );
}

QString GetAbstractSocketErrorStr(QAbstractSocket::SocketError err)
{
    QString str;
    switch( err )
    {
    case QAbstractSocket::ConnectionRefusedError:                str = "Connection Refused Error";
        break;
    case QAbstractSocket::RemoteHostClosedError:                 str = "Remote Host Closed Error";
        break;
    case QAbstractSocket::HostNotFoundError:                     str = "Host Not Found Error";
        break;
    case QAbstractSocket::SocketAccessError:                     str = "Socket Access Error";
        break;
    case QAbstractSocket::SocketResourceError:                   str = "Socket Resource Error";
        break;
    case QAbstractSocket::SocketTimeoutError:                    str = "Socket Timeout Error";
        break;
    case QAbstractSocket::DatagramTooLargeError:                 str = "Datagram Too Large Error";
        break;
    case QAbstractSocket::NetworkError:                          str = "Network Error";
        break;
    case QAbstractSocket::AddressInUseError:                     str = "Address In Use Error";
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:        str = "Socket Address Not Available Error";
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:       str = "Unsupported Socket Operation Error";
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:        str = "Unfinished Socket Operation Error";
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:      str = "Proxy Authentication Required Error";
        break;
    case QAbstractSocket::SslHandshakeFailedError:               str = "Ssl Hand shake Failed Error";
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:           str = "Proxy Connection Refused Error";
        break;
    case QAbstractSocket::ProxyConnectionClosedError:            str = "Proxy Connection Closed Error";
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:           str = "Proxy Connection Timeout Error";
        break;
    case QAbstractSocket::ProxyNotFoundError:                    str = "Proxy Not Found Error";
        break;
    case QAbstractSocket::ProxyProtocolError:                    str = "Proxy Protocol Error";
        break;
    case QAbstractSocket::UnknownSocketError:                    str = "Unknown Socket Error";
    default:
        break;
    };

    return str;
}
