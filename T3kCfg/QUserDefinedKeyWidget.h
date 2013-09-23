#ifndef QUSERDEFINEDKEYWIDGET_H
#define QUSERDEFINEDKEYWIDGET_H

#include <QLabel>
#include "QKeyEditWidget.h"
#include "ui/QHoverComboBox.h"

#define QICON_COUNT         5
#define QICON_STATE         2

class QUserDefinedKeyWidget : public QLabel
{
    Q_OBJECT
public:
    explicit QUserDefinedKeyWidget(QWidget *parent = 0);
    virtual ~QUserDefinedKeyWidget();

    void SetRealGeometry();
    void Reset();
    void SetKeyValue( ushort wKeyValue );
    ushort GetKeyValue();

    virtual void setFocus();

    void SetUseFuncKey( bool bFuncKey );
    void SetFuncKeyCurrentIndex( int nIndex );
    int GetFuncKeyCurrentIndex();
    int GetFuncKeyCount();
    QVariant GetItemData( int nIndex ) const;

protected:
    void Init();

    virtual void paintEvent(QPaintEvent *evt);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual bool eventFilter(QObject *target, QEvent *evt);

protected:
    ushort              m_wKeyValue;
    QKeyEditWidget      m_KeyEditWidget;
    QHoverComboBox      m_cbFuncKeyWidget;

    QImage*		m_pIconBtn[QICON_COUNT][QICON_STATE];
    ushort		m_wIconKeyValue[QICON_COUNT];
    QRect		m_rectIconBtn[QICON_COUNT];
    int			m_nIconTotalWidth;

    bool                m_bFuncKeyMode;

signals:
    void KeyPressSignal(ushort nValue);
    void FuncKey_activated(int nIndex);

public slots:
    void OnKeyValueChange(ushort wKeyValue);
    void onKeyPress_Signal(ushort nValue);
    void on_FuncKey_activated(int nIndex);
};

#endif // QUSERDEFINEDKEYWIDGET_H
