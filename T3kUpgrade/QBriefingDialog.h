#ifndef QBRIEFINGDIALOG_H
#define QBRIEFINGDIALOG_H

#include <QDialog>

namespace Ui {
class QBriefingDialog;
}

class QBriefingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QBriefingDialog(QString& strDetailHTML, QWidget *parent = 0);
    ~QBriefingDialog();
    
private slots:
    void on_pushButtonOK_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::QBriefingDialog *ui;
    QString& m_strDetailHTML;
};

#endif // QBRIEFINGDIALOG_H
