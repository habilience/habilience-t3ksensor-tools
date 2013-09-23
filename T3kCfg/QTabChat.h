#ifndef QTABCHAT_H
#define QTABCHAT_H

#include <QWidget>
#include "QLangManager.h"

namespace Ui {
    class QTabChat;
}

class QTabChat : public QWidget, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QTabChat(QWidget *parent = 0);
    ~QTabChat();

    void StartRemoteMode();
    void EndRemoteMode();

    // QLangManager::LangChangeNotify
protected:
    virtual void onChangeLanguage();

protected:
    void SendChatMessage();

private:
    Ui::QTabChat *ui;

private slots:
    void on_BtnSend_clicked();
    void on_EditInputMessage_returnPressed();
public slots:
    void onInputAssistanceMsg(QString);
};

#endif // QTABCHAT_H
