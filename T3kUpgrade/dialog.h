#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFWDPacket.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
private:
    QFWDPacket  m_Packet;
    int         m_TimerConnectDevice;
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    virtual void timerEvent(QTimerEvent *evt);
    virtual void showEvent(QShowEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    void connectDevice();
    
private slots:
    void on_pushButtonUpgrade_clicked();

    void on_pushButtonCancel_clicked();

    void onDisconnected();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
