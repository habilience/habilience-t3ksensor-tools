#ifndef QUSERDEFINEDKEYWIDGET_H
#define QUSERDEFINEDKEYWIDGET_H

#include "KeyLineEditWidget.h"

#include <QWidget>
#include <QComboBox>


class QUserDefinedKeyWidget : public QWidget
{
    Q_OBJECT

public:
	enum KeyType { KeyTypeNormal, KeyTypeFunction, KeyTypeMouse };

    QUserDefinedKeyWidget(QWidget* parent=0);
    virtual ~QUserDefinedKeyWidget();

    void reset();
    void setKeyValue( ushort wKeyValue );
    ushort getKeyValue();

    void setKeyType( KeyType eType );
    KeyType getKeyType() const { return m_eKeyType; }

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void focusInEvent(QFocusEvent *);

    void _setMouseValue( ushort wKeyValue );
    void _setFunctionKeyValue( ushort wKeyValue );

protected:
    KeyType             m_eKeyType;

    ushort              m_wKeyValue;
    QKeyLineEditWidget  m_wndEdit;
    QComboBox           m_wndComboFuncKey;
    QComboBox           m_wndComboMouse;

    QPixmap*            m_pIconBtn[4];
    ushort              m_wIconKeyValue[4];

    QPixmap*            m_pIconClearBtn;
    QRect               m_rectIconClearBtn;

    QRect               m_rectIconBtn[4];

    int                 m_nIconTotalWidth;
    int                 m_nIconTotalHeight;

signals:
    void editChangedKey(ushort wKeyValue);

public slots:
    void onTextChangedKey(ushort wKeyValue);
    void onCBChangedFunc(int index);
    void onCBChangedMouse(int index);
};

#endif // QUSERDEFINEDKEYWIDGET_H
