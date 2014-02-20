#ifndef QBRIEFINGDIALOG_H
#define QBRIEFINGDIALOG_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class QBriefingDialog;
}

class QPartCheckBox;
class QBriefingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QBriefingDialog(bool bPartialUpgrade = false, QWidget *parent = 0);
    ~QBriefingDialog();

    void addPart(QString strPart, QString strModel, QString strCurrentVer, QString strTargetVer);
    
private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();

    void onTogglePart(QString strPart, bool bChecked);

private:
    Ui::QBriefingDialog *ui;
    bool        m_bPartialUpgrade;
    int         m_nSelected;

signals:
    void togglePart(QString strPart, bool bClicked);

};

#endif // QBRIEFINGDIALOG_H
