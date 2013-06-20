#ifndef QTABREMOTEASSISTANCE_H
#define QTABREMOTEASSISTANCE_H

#include <QDialog>

#include "QLangManager.h"
#include "QTimeoutChecker.h"

#include "QT3kUserData.h"
#include "../Common/PacketStructure.h"

#include "QTPDPEventMultiCaster.h"

class QSideviewWidget;
class QOrderTouchWidget;

namespace Ui {
    class QTabRemoteAssistance;
}

class QTabRemoteAssistance : public QDialog, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QTabRemoteAssistance(T30xHandle*& pHandle, QWidget *parent = 0);
    ~QTabRemoteAssistance();

    enum eConnectState { Disconnected, Connecting, Connected };

    void ExitRemote();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *);

    // LangChangeNotify
protected:
    void OnChangeLanguage();

protected:
    void ProcessRemoteRawDataPacket( RRawDataPkt* packet );

    void LoadServerList();
    void SaveChangedPort();

    void SendAlive();
    void SendEnvironments();

protected:
    // remote
    QByteArray                  m_ReceivePacket;
    char*                       m_pProcessBuffer;

    int                         m_nTimer;
    int                         m_nWaitTime;
    eConnectState               m_eConnectState;
    int                         m_nPreviousID;

    int                         m_nRequestCount;

    QTimeoutChecker             m_TimeOutChecker;
    int                         m_nTIDClientID;

    int                         m_nTimerAlive;
    bool                        m_bCheckAlive;
    int                         m_nNotAliveCount;

    QSideviewWidget*            m_pSideviewWidget;
    QOrderTouchWidget*          m_pOrderTouchWidget;

    QString                     m_strServierListPath;

private:
    Ui::QTabRemoteAssistance *ui;

    T30xHandle*&                m_pT3kHandle;

signals:
    void StartRemoteMode();
    void EndRemoteMode();

    void ConnectedRemote();

    bool SendBufferTOHID( const char* pBuffer, unsigned short nLen );

    void ShowSideviewMode( bool bShow );
    void ShowOrderTouch( bool bShowMark, bool bOnScreen, bool bTouchOK, short nX, short nY, int nPercent );

    void InputChatMessage( QString strMsg );

private slots:
    void on_BtnRequest_clicked();
    void on_BtnExit_clicked();
    void onConnectedRemote();
    void onDisconnectedRemote();
    void onReadReadyRemote();
    void onRemoteSocketError(QAbstractSocket::SocketError err);

    void onShowSideviewMode(bool bShow);
    void onShowOrderTouch(bool bShowMark, bool bOnScreen, bool bTouchOK, short nX, short nY, int nPercent);

    void onTimeOutChecker( int nID );
};

#endif // QTABREMOTEASSISTANCE_H
