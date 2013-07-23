#ifndef QSELECTSENSORDATADIALOG_H
#define QSELECTSENSORDATADIALOG_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui {
class QSelectSensorDataDialog;
}

class QSelectSensorDataDialog : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
protected:
    bool    m_bSaveMode;
    QString m_strFileName;
    bool    m_bIsNewData;

    virtual void onChangeLanguage();

    void onInitDialog();

    virtual void showEvent(QShowEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);

    void insertListItem( const QString& strDateTime, const QString& strFileName );
public:
    void setSaveMode( bool bSaveMode ) { m_bSaveMode = bSaveMode; }
    QString getFileName() { return m_strFileName; }

    explicit QSelectSensorDataDialog(QWidget *parent = 0);
    ~QSelectSensorDataDialog();
    
private slots:
    void on_btnNewData_clicked();

    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_tblDataList_doubleClicked(const QModelIndex &index);

    void on_tblDataList_itemSelectionChanged();

private:
    Ui::QSelectSensorDataDialog *ui;
};

#endif // QSELECTSENSORDATADIALOG_H
