#ifndef QSOFTKEYACTIONKEYWIDGET_H
#define QSOFTKEYACTIONKEYWIDGET_H

#include <QLabel>
#include "QKeyEditWidget.h"
#include "ui/QHoverComboBox.h"
#include "QLangManager.h"

#define QICON_COUNT         5
#define QICON_STATE         2

enum PartInputMode { PIM_KEY, PIM_FUNC, PIM_MOUSE };

class QSoftkeyActionKeyWidget : public QLabel, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    explicit QSoftkeyActionKeyWidget(QWidget *parent = 0);
    virtual ~QSoftkeyActionKeyWidget();

    void SetRealGeometry();
    void Reset();
    void SetKeyValue( ushort wKeyValue );
    void SetMouseValue( ushort wMouseValue );
    ushort GetKeyValue();
    QString text();

    virtual void setFocus();

    int GetFuncKeyCurrentIndex();
    void SetFuncKeyCurrentIndex( int nIndex );

    PartInputMode GetWidgetInputMode() { return m_eInputMode; }
    void SetWidgetInputMode( PartInputMode eMode );

protected:
    void Init();

    virtual void onChangeLanguage();

    virtual void paintEvent(QPaintEvent *evt);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void resizeEvent(QResizeEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);
    virtual void showEvent(QShowEvent *evt);

protected:
    ushort              m_wKeyValue;
    QKeyEditWidget      m_KeyEditWidget;
    QHoverComboBox      m_cbFuncKeyWidget;
    QHoverComboBox      m_cbMouseWidget;

    QImage*		m_pIconBtn[QICON_COUNT][QICON_STATE];
    ushort		m_wIconKeyValue[QICON_COUNT];
    QRect		m_rectIconBtn[QICON_COUNT];
    int			m_nIconTotalWidth;

    bool                m_bHover;

    PartInputMode       m_eInputMode;

signals:
    void ItemChanged(QObject* pObj);
    int FocusOutKeyEdit();
    void KeyPressSignal(ushort nValue);
    void MousekeyPress(ushort nValue);

    void FuncKey_ShowPopup_S();
    void FuncKey_HidePopup_S();
    void FuncKey_FocusIn_S();
    void FuncKey_FocusOut_S();
    void FuncKey_activated(int nIndex);
    void Mouse_activated(int nIndex);

public slots:
    void OnKeyValueChange(ushort wKeyValue);
    void onKeyPress_Signal(ushort nValue);
    void on_FuncKey_ShowPopup();
    void on_FuncKey_HidePopup();
    void on_FuncKey_FocusIn();
    void on_FuncKey_FocusOut();
    void on_FuncKey_activated(int nIndex);
    void on_Mouse_activated(int nIndex);
};

#endif // QSOFTKEYACTIONKEYWIDGET_H
