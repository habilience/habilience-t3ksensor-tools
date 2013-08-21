#ifndef QEDITACTIONKEY4WAYDIALOG_H
#define QEDITACTIONKEY4WAYDIALOG_H

#include <QDialog>
#include "QUserDefinedKeyCtrl.h"

namespace Ui {
class QEditActionKey4WayDialog;
}

class QComboBox;
class QLabel;
class QEditActionKey4WayDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QEditActionKey4WayDialog(QWidget *parent = 0);
    ~QEditActionKey4WayDialog();

    void setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue1, unsigned short wKeyValue2, unsigned short wKeyValue3, unsigned short wKeyValue4 );

protected:
    enum ProfileType { TypeSingle, TypeDouble, TypeMove };
    enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

    int             m_nProfileIndex;
    unsigned char   m_cProfileKey;
    unsigned short  m_wProfileValues[4];

    void setActionWithProfileInfo( QComboBox* cbAction, QLabel* lblKeyDetail, unsigned short wKeyValue );
    void setPredefineProfileTypes( QComboBox* cbAction, bool bAlter=false );
    void setEditMode( EditMode mode, unsigned short wKeyValue );

    virtual void accept();
    virtual void reject();

    void onChangeLanguage();

    QComboBox* m_pActiveComboAction;

    void onIndexChangedComboAction( QComboBox* cbAction );
    virtual bool eventFilter(QObject *, QEvent *);
    void setCurrentIndexWithoutEvent( QComboBox* cbAction, int index );

    bool m_bLinkLock;

private slots:
    void onKeyValueChanged(unsigned short wKeyValue);
    void onComboActionLeftCurrentIndexChanged(int index);
    void onComboActionRightCurrentIndexChanged(int index);
    void onComboActionUpCurrentIndexChanged(int index);
    void onComboActionDownCurrentIndexChanged(int index);
    
    void on_btnLinkVert_clicked();

    void on_btnLinkHorz_clicked();

    void on_btnApply_clicked();

    void on_btnCancel_clicked();

private:
    Ui::QEditActionKey4WayDialog *ui;
};

#endif // QEDITACTIONKEY4WAYDIALOG_H
