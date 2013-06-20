#include "QTabChat.h"
#include "ui_QTabChat.h"

#include "QT3kUserData.h"

#include "../Common/PacketStructure.h"

#include <QScrollBar>
#include <QTime>


QTabChat::QTabChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTabChat)
{
    ui->setupUi(this);
    setFont( parent->font() );

    ui->EditInputMessage->setEnabled( false );
    ui->BtnSend->setEnabled( false );

    OnChangeLanguage();
}

QTabChat::~QTabChat()
{
    delete ui;
}

void QTabChat::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->TitleChat->setText( Res.GetResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("TITLE_CHAT")) );
    ui->BtnSend->setText( Res.GetResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("BTN_SEND")) );
}

void QTabChat::StartRemoteMode()
{
    ui->EditInputMessage->setEnabled( true );
    ui->BtnSend->setEnabled( true );
}

void QTabChat::EndRemoteMode()
{
    if( !ui->EditInputMessage->isEnabled() ) return;
    ui->EditInputMessage->setEnabled( false );
    ui->BtnSend->setEnabled( false );

    QString strMsg( QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("ASSISTANCE"),QString::fromUtf8("MSG_CHAT_DISCONNECTED")) );
    QTime curTime( QTime::currentTime() );
    QString str( "<p><font color=gray>*** %1 (%2h:%3m:%4s) ***</p>\r\n" );
    str = str.arg(strMsg).arg(curTime.hour(), 2).arg(curTime.minute(), 2).arg(curTime.second(), 2);
    ui->TextMessage->append( str );
    ui->TextMessage->verticalScrollBar()->setSliderPosition( ui->TextMessage->verticalScrollBar()->maximum() );
}

void QTabChat::SendChatMessage()
{
    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
    if( pSocket->state() != QTcpSocket::ConnectedState ) return;

    QString strMsg( ui->EditInputMessage->text() );
    QByteArray baMsg( strMsg.toUtf8() );

    int nLen = baMsg.size();
    int nPktSize = sizeof(RHeaderPkt) + nLen + 1;

    char* pBuffer = new char[nPktSize];

    RChatMessage* pPkt = (RChatMessage*)pBuffer;
    pPkt->Header.nType = Client | TranChatMessage;
    pPkt->Header.nPktSize = nPktSize;
    ::memcpy( pPkt->szString, baMsg.data(), nLen );
    pPkt->szString[nLen] = '\0';

    int nReqCnt = 3;
    char* pRemainPkt = (char*)pBuffer;
    qint64 nRet = 0;
    qint64 nRemainPkt = nPktSize;
    while( nRemainPkt && nReqCnt )
    {
        nRet = pSocket->write( (const char*)pRemainPkt, nRemainPkt );
        nRemainPkt -= nRet;
        pRemainPkt += nRet;
        nReqCnt--;
    }

    delete[] pBuffer;

    if( nRemainPkt != 0 )
    {
        qDebug( "Send Error : Chat Message" );
        return;
    }

    ui->EditInputMessage->setText( "" );
    QTime curTime( QTime::currentTime() );
    QString str( "<p><font color=gray> Me :    (%1h:%2m:%3s)</font><br/>&nbsp;&nbsp;&nbsp;%4</p>\r\n" );
    str = str.arg(curTime.hour(), 2).arg(curTime.minute(), 2).arg(curTime.second(), 2).arg(strMsg);
    ui->TextMessage->append( str );

    ui->TextMessage->verticalScrollBar()->setSliderPosition( ui->TextMessage->verticalScrollBar()->maximum() );
}

void QTabChat::on_EditInputMessage_returnPressed()
{
    if( ui->EditInputMessage->text().isEmpty() ) return;
    SendChatMessage();

    ui->EditInputMessage->setFocus();
}

void QTabChat::on_BtnSend_clicked()
{
    if( ui->EditInputMessage->text().isEmpty() ) return;

    SendChatMessage();

    ui->EditInputMessage->setFocus();
}

void QTabChat::onInputAssistanceMsg(QString strMsg)
{
    ui->TextMessage->append( "<p><font color=gray> Admin :</font><br/>&nbsp;&nbsp;&nbsp;" + strMsg + "</p>\r\n" );
    ui->TextMessage->verticalScrollBar()->setSliderPosition( ui->TextMessage->verticalScrollBar()->maximum() );

    activateWindow();
}
