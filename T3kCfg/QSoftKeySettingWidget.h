#ifndef QSOFTKEYSETTINGWIDGET_H
#define QSOFTKEYSETTINGWIDGET_H

#include <QWidget>

#include "QT3kDeviceREventHandler.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"
#include "QSoftkey.h"

#include "QSoftkeyActionCellWidget.h"

namespace Ui {
    class QSoftKeySettingWidget;
}

class QSoftKeySettingWidget : public QWidget, public QT3kDeviceREventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QSoftKeySettingWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QSoftKeySettingWidget();

    void SetDefault();
    void Refresh();

protected:
    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

    virtual void onChangeLanguage();
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);


    int ParseSoftKey( const char* psSoftKey );
    int ParseSoftLogic( const char* psSoftLogic );

    bool AddLogicArray( QSoftlogic::PortType eType, QSoftlogic::Port e1InPort, ushort nValue );
    bool ReplaceSoftlogic( int nIndex, ushort nKeyValue, int nRow, int nColumn );

    int FindSoftlogic( QSoftlogicArray& aryLogic, QSoftlogic::Port eInputPort );
    int FindSoftlogic( QSoftlogicArray& aryLogic, int nIndex );
    int SoftkeyInputIndex( QSoftlogic::Port ePort );
    bool VerifyIndexLogicArray( int nIndex );

    void UpdateListEnable();

    bool Save();

    void RequestSensorData( bool bDefault );

protected:
    QRequestHIDManager          m_RequestSensorData;

    QSoftkeyArray               m_arySoftKey;
    QSoftlogicArray             m_arySoftLogic;    

    bool                        m_bInit;
    bool                        m_bDefault;
    bool                        m_bSave;
    bool                        m_bUpdateList;
    bool                        m_bLoad;

    int                         m_nAddCount;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);

private:
    Ui::QSoftKeySettingWidget   *ui;
    QT3kDeviceR*&                m_pT3kHandle;

private slots:
    void onslot_TableSoftkeyMap_cellChanged(int nRow, int nColumn, int nIndex);
    void onTableSoftkeyMap_KeyPress_Signal(int nRow, int nColumn, ushort nValue);
    void on_TableSoftkeyMap_MousekeyPress(int nRow, int nColumn, ushort nValue);
};

#endif // QSOFTKEYSETTINGWIDGET_H
