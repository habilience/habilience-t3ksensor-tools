#ifndef QSELECTDEVICEDIALOG_H
#define QSELECTDEVICEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "QLangManager.h"

namespace Ui {
class QSelectDeviceDialog;
}

class QSelectDeviceDialog : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    struct DEVICE_ID
    {
        unsigned short nVID;
        unsigned short nPID;
        unsigned short nMI;
        int nDeviceIndex;
    };

    DEVICE_ID getDeviceId() { return m_deviceId; }
protected:

    virtual void onChangeLanguage();
    void refreshDeviceList();
    void insertListItem( const QString& strModelName, unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDeviceIndex );

    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);

    DEVICE_ID   m_deviceId;
public:
    explicit QSelectDeviceDialog(QWidget *parent = 0);
    ~QSelectDeviceDialog();
    
private slots:
    void on_btnSelect_clicked();

    void on_btnPlayBuzzer_clicked();

    void on_tblDeviceList_doubleClicked(const QModelIndex &index);

    void on_tblDeviceList_itemSelectionChanged();

private:
    Ui::QSelectDeviceDialog *ui;
};

#endif // QSELECTDEVICEDIALOG_H
