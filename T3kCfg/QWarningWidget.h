#ifndef QWARNINGWIDGET_H
#define QWARNINGWIDGET_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui {
    class QWarningWidget;
}

class QWarningWidget : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QWarningWidget(QWidget *parent = 0);
    ~QWarningWidget();

    void SetTextFromLanguage( QString strTitleGroup, QString strTitleText, QString strMsgGroup, QString strMsgText );
    void SetBtnTextFromLanguage( QString strOKGroup, QString strOKText, QString strCancelGroup, QString strCancelText );

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void onChangeLanguage();

private:
    Ui::QWarningWidget *ui;

    QString             m_strTitleGroup;
    QString             m_strTitleText;
    QString             m_strMsgGroup;
    QString             m_strMsgText;
    QString             m_strOKGroup;
    QString             m_strOKText;
    QString             m_strCancelGroup;
    QString             m_strCancelText;
};

#endif // QWARNINGWIDGET_H
