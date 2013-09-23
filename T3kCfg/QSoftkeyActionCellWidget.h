#ifndef QSOFTKEYACTIONCELLWIDGET_H
#define QSOFTKEYACTIONCELLWIDGET_H

#include <QLabel>

#include "QLangManager.h"
#include "ui/QHoverComboBox.h"
#include "QSoftkeyActionKeyWidget.h"

class QSoftkeyActionCellWidget : public QLabel, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    explicit QSoftkeyActionCellWidget(QWidget *parent = 0, int nRowIndex = -1, int nColumnIndex = -1);

    int GetRowIndex() { return m_nRowIndex; }
    int GetColumnIndex() { return m_nColumnIndex; }

    ushort GetVaule();
    int SetKeyAction( const char cConditionKey, const char cKey );
    void SetMouseAction( const char cConditionKey, const uchar cMouse );

    bool IsMouseType();

    int GetTypeIndex() { return m_cbActionWidget.currentIndex(); }

    // Change UI
    void SetWidgetMode( uint nI );

protected:
    void ChangeWidgetMode(int nIndex, bool bShowKeyCombo);

    QString GetText();

    virtual void paintEvent(QPaintEvent *evt);
    virtual void resizeEvent(QResizeEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

    virtual void onChangeLanguage();

private:
    void ResizeWidget( QSize szNewSize );

protected:
    QHoverComboBox              m_cbActionWidget;
    QSoftkeyActionKeyWidget     m_editKeyActionWidget;

    int                         m_nRowIndex;
    int                         m_nColumnIndex;

    bool                        m_bFocusIn;

    int                         m_nTimer;

    bool                        m_bHover;
    bool                        m_bShowWidget;    

private:
    bool                        m_bLoad;

signals:
    void ItemChanged(QObject* pObj, int nIndex);
    void KeyPressSignal(QObject* pSender, ushort nValue);
    void MousekeyPress(QObject* pSender, ushort nValue);

public slots:
    void on_Clicked_Show( int nIndex );

    void on_Softkey_Press( ushort nValue );
    void on_Softkey_MousePress( ushort nValue );

    void on_Softkey_Action_Changed( int nIndex );
    void on_Softkey_Action_Activated( int nIndex );

    void on_Edit_FocusOut();

    void on_Child_FocusIn();

    void on_m_cbKeyActionWidget_hidePopup();

    void on_m_cbFuncKeyWidget_activated(int nIndex);
    void on_m_cbMouse_activated(int nIndex);
};

#endif // QSOFTKEYACTIONCELLWIDGET_H
