#ifndef QSIDEVIEWDIALOG_H
#define QSIDEVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class QSideviewDialog;
}

class Dialog;
class QSideviewDialog : public QDialog
{
    Q_OBJECT

protected:
    virtual void paintEvent(QPaintEvent *);
public:
    explicit QSideviewDialog(Dialog *parent);
    ~QSideviewDialog();
    
private slots:
    void on_btnClose_clicked();

private:
    Dialog*     m_pMainDlg;
    Ui::QSideviewDialog *ui;
};

#endif // QSIDEVIEWDIALOG_H
