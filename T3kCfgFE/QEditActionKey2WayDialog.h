#ifndef QEDITACTIONKEY2WAYDIALOG_H
#define QEDITACTIONKEY2WAYDIALOG_H

#include <QDialog>
#include "QUserDefinedKeyCtrl.h"

namespace Ui {
class QEditActionKey2WayDialog;
}

class QComboBox;
class QLabel;
class QEditActionKey2WayDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QEditActionKey2WayDialog(QWidget *parent = 0);
    ~QEditActionKey2WayDialog();

    void setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue1, unsigned short wKeyValue2 );

protected:
    enum ProfileType { TypeSingle, TypeDouble, TypeMove };
    enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

    int             m_nProfileIndex;
    unsigned char   m_cProfileKey;
    unsigned short  m_wProfileValues[2];

    void setActionWithProfileInfo( QComboBox* cbAction, QLabel* lblKeyDetail, unsigned short wKeyValue );
    void setPredefineProfileTypes( QComboBox* cbAction );
    void setEditMode( EditMode mode, unsigned short wKeyValue );

    virtual void accept();
    virtual void reject();

    void onChangeLanguage();

    QComboBox* m_pActiveComboAction;

    void onIndexChangedComboAction( QComboBox* cbAction );

    virtual bool eventFilter(QObject *, QEvent *);

    void setCurrentIndexWithoutEvent( QComboBox* cbAction, int index );
private slots:
    void onKeyValueChanged(unsigned short wKeyValue);

    void onComboActionZoomOutCurrentIndexChanged(int index);
    void onComboActionZoomInCurrentIndexChanged(int index);

    void on_btnLink_clicked();
    void on_btnApply_clicked();
    void on_btnCancel_clicked();

private:
    Ui::QEditActionKey2WayDialog *ui;
};

#endif // QEDITACTIONKEY2WAYDIALOG_H
