#ifndef QDATAPROGRESSDIALOG_H
#define QDATAPROGRESSDIALOG_H

#include <QDialog>
#include <QLangManager.h>

namespace Ui {
class QDataProgressDialog;
}

class QT3kDevice;
class QDataProgressDialog : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
    
protected:
    QT3kDevice*     m_pDevice;
    bool            m_bSetSensorCfg;

    virtual void onChangeLanguage();
    virtual void showEvent(QShowEvent *);
public:
    explicit QDataProgressDialog(QT3kDevice* pDevice, QWidget *parent = 0, bool bSetSensorCfg=false);
    ~QDataProgressDialog();

    void insertCommand( const QString& strCmd );
    void resetCommands();
    
private slots:
    void on_btnCancel_clicked();
    void onFinish(bool bResult, int nReason);

private:
    Ui::QDataProgressDialog *ui;
};

#endif // QDATAPROGRESSDIALOG_H
