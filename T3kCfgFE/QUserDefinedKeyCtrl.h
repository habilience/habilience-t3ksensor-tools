#ifndef QUSERDEFINEDKEYCTRL_H
#define QUSERDEFINEDKEYCTRL_H

#include <QWidget>
#include <QComboBox>
#include <QImage>
#include "QKeyEdit.h"

class QUserDefinedKeyCtrl : public QWidget
{
    Q_OBJECT
public:
    enum KeyType { KeyTypeNormal, KeyTypeFunction, KeyTypeMouse };
private:
    QKeyEdit    m_keyEdit;
    QComboBox   m_cbFuncKey;
    QComboBox   m_cbMouse;

    KeyType         m_keyType;
    unsigned short  m_wKeyValue;
    QImage          m_iconButtons[4];
    QRect           m_rectIconButtons[4];
    unsigned short  m_wIconKeyValue[4];

    QImage          m_iconClearButton;
    QRect           m_rectIconClearButton;

    int             m_nIconTotalWidth;

protected:
    void setMouseValue( unsigned short wKeyValue );
    void setFunctionKeyValue( unsigned short wKeyValue );

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

    virtual void mousePressEvent(QMouseEvent *);
    virtual void focusInEvent(QFocusEvent *);

    void onIndexChangedComboAction( QComboBox* cbAction );
    void setCurrentIndexWithoutEvent( QComboBox* cbAction, int index );

public:
    explicit QUserDefinedKeyCtrl(QWidget *parent = 0);

    void reset();
    void setKeyValue( unsigned short wKeyValue );
    unsigned short keyValue();

    void setKeyType( KeyType type );
    KeyType keyType() const { return m_keyType; }
    
signals:
    void keyValueChanged(unsigned short wKeyValue);
    
private slots:
    void onKeyValueChanged(QKeyEdit* keyEdit, unsigned short wKeyValue);
    void onComboMouseIndexChanged( int index );
    void onComboFunctionKeyIndexChanged( int index );
    
};

#endif // QUSERDEFINEDKEYCTRL_H
