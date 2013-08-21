#ifndef QEDITACTIONKEY1DIALOG_H
#define QEDITACTIONKEY1DIALOG_H

#include <QDialog>
#include "QUserDefinedKeyCtrl.h"

namespace Ui {
class QEditActionKey1Dialog;
}

class QEditActionKey1Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QEditActionKey1Dialog(QWidget *parent = 0);
    ~QEditActionKey1Dialog();

    void setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue );

protected:
    enum ProfileType { TypeSingle, TypeDouble, TypeMove };
    enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

    int             m_nProfileIndex;
    unsigned char   m_cProfileKey;
    unsigned short  m_wProfileValue;

    void setPredefineProfileTypes( ProfileType type, bool bUseTaskSwitcher );
    void setEditMode( EditMode mode, unsigned short wKeyValue );

    virtual void accept();
    virtual void reject();

    void onChangeLanguage();

    void onIndexChangedComboAction( QComboBox* cbAction );
    void setCurrentIndexWithoutEvent( QComboBox* cbAction, int index );

private slots:
    void onKeyValueChanged(unsigned short wKeyValue);
    void onComboIndexChanged(int index);

    void on_btnApply_clicked();
    void on_btnCancel_clicked();

private:
    Ui::QEditActionKey1Dialog *ui;
};

#endif // QEDITACTIONKEY1DIALOG_H
