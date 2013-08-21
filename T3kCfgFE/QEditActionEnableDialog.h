#ifndef QEDITACTIONENABLEDIALOG_H
#define QEDITACTIONENABLEDIALOG_H

#include <QDialog>

namespace Ui {
class QEditActionEnableDialog;
}

class QComboBox;
class QEditActionEnableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QEditActionEnableDialog(QWidget *parent = 0);
    ~QEditActionEnableDialog();

    void setProfileInfo( int nProfileIndex, unsigned char cKey, unsigned short wKeyValue, unsigned short wFlags );

protected:
    int             m_nProfileIndex;
    unsigned char   m_cProfileKey;
    unsigned short  m_wProfileValue;
    unsigned short  m_wProfileFlags;

    virtual void accept();
    virtual void reject();

    void onChangeLanguage();

    void onIndexChangedComboAction( QComboBox* cbAction );
    void setCurrentIndexWithoutEvent( QComboBox* cbAction, int index );

signals:

private slots:
    void onComboIndexChanged(int index);

    void on_btnApply_clicked();
    void on_btnCancel_clicked();

private:
    Ui::QEditActionEnableDialog *ui;
};

#endif // QEDITACTIONENABLEDIALOG_H
