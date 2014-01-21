#ifndef QEDITACTIONEDWND_H
#define QEDITACTIONEDWND_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui{
    class QEditActionEDWnd;
}

class QMouseMappingTable;
class QEditActionEDWnd : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QEditActionEDWnd(QWidget *parent = 0);
    ~QEditActionEDWnd();

    void SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue, ushort wFlags );
    virtual void onChangeLanguage();

protected:
    virtual void hideEvent(QHideEvent *evt);

protected:
    QMouseMappingTable*	m_pTableWnd;

    int			m_nProfileIndex;
    uchar		m_cProfileKey;
    ushort		m_wProfileValue;
    ushort		m_wProfileFlags;

    bool                m_bComboBoxInit;

private:
    Ui::QEditActionEDWnd* ui;

signals:
    void sendCommand(QString strCmd, bool bAsync = false, unsigned short nTimeout = 1000);

private slots:
    void on_BtnApply_clicked();
    void on_CBAction_currentIndexChanged(int index);
};

#endif // QEDITACTIONEDWND_H
