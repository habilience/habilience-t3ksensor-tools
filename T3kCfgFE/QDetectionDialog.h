#ifndef QDETECTIONDIALOG_H
#define QDETECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class QDetectionDialog;
}

class Dialog;
class QDetectionDialog : public QDialog
{
    Q_OBJECT
protected:
    virtual void paintEvent(QPaintEvent *);
public:
    explicit QDetectionDialog(Dialog *parent = 0);
    ~QDetectionDialog();
    
private slots:
    void on_btnClose_clicked();

private:
    Dialog* m_pMainDlg;
    Ui::QDetectionDialog *ui;
};

#endif // QDETECTIONDIALOG_H
