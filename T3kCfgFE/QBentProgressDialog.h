#ifndef QBENTPROGRESSDIALOG_H
#define QBENTPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class QBentProgressDialog;
}

struct BentItem;
class QBentProgressDialog : public QDialog
{
    Q_OBJECT
protected:
    virtual void paintEvent(QPaintEvent *);
public:
    explicit QBentProgressDialog(BentItem& item, QWidget *parent = 0);
    ~QBentProgressDialog();

private slots:
    virtual void accept();
    virtual void reject();
private:
    BentItem&   m_item;
    Ui::QBentProgressDialog *ui;
};

#endif // QBENTPROGRESSDIALOG_H
