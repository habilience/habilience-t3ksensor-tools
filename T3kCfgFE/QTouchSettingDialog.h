#ifndef QTOUCHSETTINGDIALOG_H
#define QTOUCHSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class QTouchSettingDialog;
}

class Dialog;
class QTouchSettingDialog : public QDialog
{
    Q_OBJECT
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent *);

    virtual void reject();
    virtual void accept();
public:
    explicit QTouchSettingDialog(Dialog *parent = 0);
    ~QTouchSettingDialog();
    bool canClose();
    
private slots:
    void on_btnClose_clicked();

private:
    Dialog* m_pMainDlg;
    Ui::QTouchSettingDialog *ui;
};

#endif // QTOUCHSETTINGDIALOG_H
