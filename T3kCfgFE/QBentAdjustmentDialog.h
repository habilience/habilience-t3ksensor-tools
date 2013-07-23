#ifndef QBENTADJUSTMENTDIALOG_H
#define QBENTADJUSTMENTDIALOG_H

#include <QDialog>

namespace Ui {
class QBentAdjustmentDialog;
}

class Dialog;
class QBentAdjustmentDialog : public QDialog
{
    Q_OBJECT
    
protected:
    virtual void paintEvent(QPaintEvent *);
public:
    explicit QBentAdjustmentDialog(Dialog *parent = 0);
    ~QBentAdjustmentDialog();
    
private slots:
    void on_btnClose_clicked();

private:
    Dialog* m_pMainDlg;
    Ui::QBentAdjustmentDialog *ui;
};

#endif // QBENTADJUSTMENTDIALOG_H
