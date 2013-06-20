#ifndef QEDITACTION4WDWND_H
#define QEDITACTION4WDWND_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui{
    class QEditAction4WDWnd;
}

class T30xHandle;
class QMouseMappingTable;
class QLabel;
class QComboBox;

class QEditAction4WDWnd : public QDialog, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QEditAction4WDWnd(T30xHandle*& pHandle, QWidget *parent = 0);
    ~QEditAction4WDWnd();

    enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

    void SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue1, ushort wKeyValue2, ushort wKeyValue3, ushort wKeyValue4 );

    virtual void OnChangeLanguage();

protected:
    void SetEditMode( EditMode Mode, ushort wKeyValue );
    void SetActionWithProfileInfo( QComboBox& pWndComboAction, QLabel& nDetailLabel, ushort wKeyValue );
    void SetPredefineProfileTypes( QComboBox& pWndComboAction, bool bAlter=false );

    void OnCbnSelchangeComboAction( QComboBox* pWndComboAction );

    void Init();

    void OnKeyEditChange( ushort wKeyValue );

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);

protected:
    QMouseMappingTable*         m_pTableWnd;
    QComboBox*                  m_pWndActiveComboAction;

    bool                        m_bLinkLock;

    int                         m_nProfileIndex;
    uchar                       m_cProfileKey;
    ushort                      m_wProfileValue[4];

    int                         m_nLoad;

private:
    Ui::QEditAction4WDWnd* ui;

    T30xHandle*&                m_pT3kHandle;

private slots:
    void on_ChkLinkHorz_toggled(bool checked);
    void on_ChkLinkVert_toggled(bool checked);
    void on_CBActionDown_activated(int index);
    void on_CBActionUp_activated(int index);
    void on_CBActionRight_activated(int index);
    void on_CBActionLeft_activated(int index);
    void onFunctionKeyActivated(int index);
    void on_BtnApply_clicked();
    void on_CBActionDown_currentIndexChanged(int index);
    void on_CBActionUp_currentIndexChanged(int index);
    void on_CBActionRight_currentIndexChanged(int index);
    void on_CBActionLeft_currentIndexChanged(int index);
    void onKeyPress_Signal(ushort nValue);
};

#endif // QEDITACTION4WDWND_H
