#ifndef QSOFTKEYTABLEWIDGET_H
#define QSOFTKEYTABLEWIDGET_H

#include <QTableWidget>

#include "QLangManager.h"

class QSoftkeyTableWidget : public QTableWidget, public QLangManager::LangChangeNotify
{
    Q_OBJECT
public:
    explicit QSoftkeyTableWidget(QWidget *parent = 0);

    int AddSoftkeyItem( int nAddCount );
    void RemoveAllItem();

protected:
    virtual void OnChangeLanguage();

signals:
    void KeyPressSignal(int nRow, int nColumn, ushort nValue);
    void MousekeyPress(int nRow, int nColumn, ushort nValue);
    void ActionTypecellChanged(int nRow, int nColumn, int nIndex);

private slots:
    void on_cellClicked(int nRow, int nColum);
    void on_usr_ItemChanged(QObject* pObj, int nIndex);
    void onKeyPress_Signal(QObject* pSender, ushort nValue);
    void on_MousekeyPress(QObject* pSender, ushort nValue);
};

#endif // QSOFTKEYTABLEWIDGET_H
