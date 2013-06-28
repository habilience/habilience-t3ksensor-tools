#include "dialog.h"
#include "ui_dialog.h"

#include <QPropertyAnimation>
#include <QMessageBox>

#define RETRY_CONNECTION_INTERVAL        (3000)

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    m_TimerConnectDevice = 0;

    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    setWindowIcon( QIcon(":/T3kUpgradeRes/resources/T3kUpgrade.png") );

    ui->stackedWidget->setCurrentIndex(0);
    connect( &m_Packet, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::QueuedConnection );
}

Dialog::~Dialog()
{
    m_Packet.close();
    delete ui;
}

void Dialog::onDisconnected()
{
    qDebug( "disconnected" );

    if (m_Packet.isOpen())
    {
        m_Packet.close();
    }
    connectDevice();
}

void Dialog::timerEvent(QTimerEvent *evt)
{
    if( evt->type() == QEvent::Timer )
    {
        if( evt->timerId() == m_TimerConnectDevice )
        {
            killTimer(m_TimerConnectDevice);
            m_TimerConnectDevice = 0;
            connectDevice();
        }
    }
}

void Dialog::connectDevice()
{
    qDebug( "try connect..." );
    if (m_Packet.open())
    {
        qDebug( "connection ok" );
    }
    else
    {
        qDebug( "connection fail" );
        m_TimerConnectDevice = startTimer(RETRY_CONNECTION_INTERVAL);
    }
}

void Dialog::showEvent(QShowEvent *evt)
{
    if ( evt->type() == QEvent::Show )
    {
        connectDevice();
    }

    QDialog::showEvent(evt);
}

void Dialog::closeEvent(QCloseEvent *evt)
{
    if ( evt->type() == QEvent::Close )
    {
        qDebug( "closeEvent" );
        if (m_Packet.isOpen())
        {
            m_Packet.close();
        }
        if (m_TimerConnectDevice != 0)
        {
            killTimer(m_TimerConnectDevice);
            m_TimerConnectDevice = 0;
        }
    }

    QDialog::closeEvent(evt);
}

void Dialog::on_pushButtonUpgrade_clicked()
{
    emit ui->stackedWidget->slideInNext();
}

void Dialog::on_pushButtonCancel_clicked()
{
    // TODO: confirm cancel

    emit ui->stackedWidget->slideInPrev();
}
