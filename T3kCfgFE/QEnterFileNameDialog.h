#ifndef QENTERFILENAMEDIALOG_H
#define QENTERFILENAMEDIALOG_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui {
class QEnterFileNameDialog;
}

class QEnterFileNameDialog : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
protected:
    QString     m_strFileName;

    virtual void onChangeLanguage();
    virtual void showEvent(QShowEvent *);
public:
    QString getFileName() { return m_strFileName; }
    explicit QEnterFileNameDialog(QWidget *parent = 0);
    ~QEnterFileNameDialog();
    
private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::QEnterFileNameDialog *ui;
};

#endif // QENTERFILENAMEDIALOG_H
