#ifndef QPREDEFPROFILEEDITDIALOG_H
#define QPREDEFPROFILEEDITDIALOG_H

#include "ui/QEditActionWnd.h"
#include "ui/QEditActionEDWnd.h"
#include "ui/QEditAction2WDWnd.h"
#include "ui/QEditAction4WDWnd.h"
#include "ui/QGestureMappingTable.h"
#include "QLangManager.h"

#include <QDialog>

namespace Ui {
class QPredefProfileEditDialog;
}

class QPredefProfileEditDialog : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QPredefProfileEditDialog(QWidget *parent = 0);
    ~QPredefProfileEditDialog();

protected:
    // QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

protected:
    QEditActionWnd*             m_pEditActionWnd;
    QEditActionEDWnd*           m_pEditActionEWnd;
    QEditAction2WDWnd*          m_pEditAction2WDWnd;
    QEditAction4WDWnd*          m_pEditAction4WDWnd;

private:
    Ui::QPredefProfileEditDialog *ui;

protected slots:
    void onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo& ci, ushort nProfileFlags);
    void onSendCommand(QString strCmd, bool bAsync = false, unsigned short nTimeout = 1000);
private slots:
    void on_CBProfileList_activated(int index);
    void on_BtnAdd_clicked();
    void on_BtnDelete_clicked();
};

#endif // QPREDEFPROFILEEDITDIALOG_H
