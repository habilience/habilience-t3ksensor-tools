#include "QUserDefinedKeyCtrl.h"
#include <QResizeEvent>
#include <QPainter>

#include "QKeyMapStr.h"

#define LBUTTON		(0x01)
#define RBUTTON		(0x02)
#define MBUTTON		(0x04)

#define CTRL		0
#define ALT			1
#define SHIFT		2
#define WIN			3

QUserDefinedKeyCtrl::QUserDefinedKeyCtrl(QWidget *parent) :
    QWidget(parent)
  , m_keyEdit(this)
  , m_cbFuncKey(this)
  , m_cbMouse(this)
{
    m_iconButtons[CTRL].load(":/T3kCfgFERes/resources/keyicon_ctrl.png");
    m_iconButtons[ALT].load(":/T3kCfgFERes/resources/keyicon_alt.png");
    m_iconButtons[SHIFT].load(":/T3kCfgFERes/resources/keyicon_shift.png");
#ifdef Q_WS_MAC
    m_iconButtons[WIN].load(":/T3kCfgFERes/resources/keyicon_cmd.png");
#else
    m_iconButtons[WIN].load(":/T3kCfgFERes/resources/keyicon_win.png");
#endif

    m_iconClearButton.load(":/T3kCfgFERes/resources/keyicon_clear.png");

    m_keyType = KeyTypeNormal;
    m_wIconKeyValue[CTRL] = MM_MOUSE_KEY0_CTRL;
    m_wIconKeyValue[ALT] = MM_MOUSE_KEY0_ALT;
    m_wIconKeyValue[SHIFT] = MM_MOUSE_KEY0_SHIFT;
    m_wIconKeyValue[WIN] = MM_MOUSE_KEY0_WINDOW;

    m_wKeyValue = 0x00;

    m_nIconTotalWidth = 0;
    m_nIconTotalWidth += m_iconButtons[CTRL].width() / 2;
    m_nIconTotalWidth += m_iconButtons[ALT].width() / 2;
    m_nIconTotalWidth += m_iconButtons[SHIFT].width() / 2;
    m_nIconTotalWidth += m_iconButtons[WIN].width() / 2;
    m_nIconTotalWidth += 4;

    m_keyEdit.setVisible(false);
    m_cbFuncKey.setVisible(false);
    m_cbMouse.setVisible(false);

    m_keyEdit.setFocusPolicy(Qt::StrongFocus);
    m_cbFuncKey.setFocusPolicy(Qt::StrongFocus);
    m_cbMouse.setFocusPolicy(Qt::StrongFocus);

    QString strFn;
    for ( int i=1 ; i<=24 ; i++ )
    {
        int nScanCode = virtualKeyToScanCode( Qt::Key_F1+i-1 );
        unsigned char cUsageId;
        if (findUsageId( nScanCode, cUsageId ))
        {
            strFn = QString("F%1").arg(i);
            m_cbFuncKey.addItem( strFn, qVariantFromValue(cUsageId));
        }
    }

    m_cbMouse.addItem( "Left Button", qVariantFromValue(LBUTTON) );
    m_cbMouse.addItem( "Right Button", qVariantFromValue(RBUTTON) );
    m_cbMouse.addItem( "Middle Button", qVariantFromValue(MBUTTON) );

    connect( &m_keyEdit, SIGNAL(keyEditValueChanged(QKeyEdit*,unsigned short)), SLOT(onKeyValueChanged(QKeyEdit*,unsigned short)) );

    connect( &m_cbMouse, SIGNAL(currentIndexChanged(int)), SLOT(onComboMouseIndexChanged(int)) );
    connect( &m_cbFuncKey, SIGNAL(currentIndexChanged(int)), SLOT(onComboFunctionKeyIndexChanged(int)) );
}

void QUserDefinedKeyCtrl::reset()
{
    m_wKeyValue = 0x0000;
    switch (m_keyType)
    {
    case KeyTypeNormal:
        m_keyEdit.reset();
        break;
    case KeyTypeFunction:
        setCurrentIndexWithoutEvent(&m_cbFuncKey, -1);
        break;
    case KeyTypeMouse:
        setCurrentIndexWithoutEvent(&m_cbMouse, -1);
        break;
    }
}

void QUserDefinedKeyCtrl::setKeyValue( unsigned short wKeyValue )
{
    m_wKeyValue = wKeyValue;
    switch (m_keyType)
    {
    case KeyTypeNormal:
        m_keyEdit.setKeyValue( wKeyValue );
        break;
    case KeyTypeFunction:
        setFunctionKeyValue( wKeyValue );
        break;
    case KeyTypeMouse:
        m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
        setMouseValue( wKeyValue );
        break;
    }

    update();
}

unsigned short QUserDefinedKeyCtrl::keyValue()
{
    return m_wKeyValue;
}

void QUserDefinedKeyCtrl::setKeyType( KeyType type )
{
    if( m_keyType == type )
        return;

    m_keyType = type;

    switch (m_keyType)
    {
    case KeyTypeNormal:
        m_cbMouse.setVisible(false);
        m_cbFuncKey.setVisible(false);
        m_keyEdit.setVisible(true);
        break;
    case KeyTypeFunction:
        m_cbMouse.setVisible(false);
        m_keyEdit.setVisible(false);
        m_cbFuncKey.setVisible(true);
        break;
    case KeyTypeMouse:
        m_cbFuncKey.setVisible(false);
        m_keyEdit.setVisible(false);
        m_cbMouse.setVisible(true);
    }
}

void QUserDefinedKeyCtrl::setMouseValue( unsigned short wKeyValue )
{
    unsigned short wMKeyValue = (wKeyValue & 0x00FF);
    setCurrentIndexWithoutEvent(&m_cbMouse, -1);
    for (int nM = 0 ; nM<m_cbMouse.count() ; nM++)
    {
        QVariant var = m_cbMouse.itemData(nM);
        unsigned short wMKey = var.value<unsigned short>();
        if( wMKey == wMKeyValue )
        {
            setCurrentIndexWithoutEvent(&m_cbMouse, nM);
            break;
        }
    }
}

void QUserDefinedKeyCtrl::setFunctionKeyValue( unsigned short wKeyValue )
{
    unsigned short wFnKeyValue = (wKeyValue & 0x00FF);
    setCurrentIndexWithoutEvent(&m_cbFuncKey, -1);
    for( int nF = 0 ; nF<m_cbFuncKey.count() ; nF++ )
    {
        QVariant var = m_cbFuncKey.itemData(nF);
        unsigned short wFnKey = var.value<unsigned short>();
        if( wFnKey == wFnKeyValue )
        {
            setCurrentIndexWithoutEvent(&m_cbFuncKey, nF);
            break;
        }
    }
}

void QUserDefinedKeyCtrl::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rc(0, 0, width(), height());

    unsigned char cV0 = (unsigned char)(m_wKeyValue >> 8);

    int nIconW = 0;
    int nIconH = m_iconButtons[0].height();
    int nIconX = 0;
    int nIconY = rc.top() + (rc.height()/2-nIconH) / 2;
    bool bDown;
    for (int i=0 ; i<4 ; i++)
    {
        bDown = cV0 & m_wIconKeyValue[i] ? true : false;
        nIconW = m_iconButtons[i].width() / 2;
        m_rectIconButtons[i] = QRect(nIconX, nIconY, nIconW, nIconH);
        p.drawImage( nIconX, nIconY, m_iconButtons[i], bDown ? nIconW : 0, 0, nIconW, nIconH );
        nIconX += nIconW+1;
    }

    m_rectIconClearButton = QRect( rc.right() - m_iconClearButton.width()/2, nIconY, nIconW, nIconH );
    p.drawImage( m_rectIconClearButton.left(), m_rectIconClearButton.top(), m_iconClearButton, 0, 0, nIconW, nIconH );
}

void QUserDefinedKeyCtrl::resizeEvent(QResizeEvent *evt)
{
    const QSize& size = evt->size();
    if (size.width() <= 0 || size.height() <= 0)
        return;

    QRect rcBody(0, 0, size.width(), size.height());

    QRect rcEdit = rcBody;
    rcEdit.adjust( 0, rcEdit.height()/2, 0, 0 );

    m_keyEdit.move( rcEdit.topLeft() );
    m_keyEdit.resize( rcEdit.size() );
    m_cbFuncKey.move( rcEdit.topLeft() );
    m_cbFuncKey.resize( rcEdit.size() );
    m_cbMouse.move( rcEdit.topLeft() );
    m_cbMouse.resize( rcEdit.size() );

    switch (m_keyType)
    {
    case KeyTypeNormal:
        m_cbMouse.setVisible(false);
        m_cbFuncKey.setVisible(false);
        m_keyEdit.setVisible(true);
        break;
    case KeyTypeFunction:
        m_cbMouse.setVisible(false);
        m_keyEdit.setVisible(false);
        m_cbFuncKey.setVisible(true);
        break;
    case KeyTypeMouse:
        m_cbFuncKey.setVisible(false);
        m_keyEdit.setVisible(false);
        m_cbMouse.setVisible(true);
    }
}

void QUserDefinedKeyCtrl::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        QPoint pt = evt->pos();
        for (int i=0 ; i<4 ; i++)
        {
            if (m_rectIconButtons[i].contains(pt))
            {
                unsigned char cV0 = (unsigned char)(m_wKeyValue >> 8);
                unsigned char cV1 = (unsigned char)(m_wKeyValue);
                cV0 ^= m_wIconKeyValue[i];

                m_wKeyValue = cV0 << 8 | cV1;
                switch (m_keyType)
                {
                case KeyTypeNormal:
                    m_keyEdit.setKeyValue( m_wKeyValue );
                    m_keyEdit.setFocus();
                    break;
                case KeyTypeFunction:
                    setFunctionKeyValue( m_wKeyValue );
                    break;
                case KeyTypeMouse:
                    m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
                    setMouseValue( m_wKeyValue );
                    break;
                }
                update();

                emit keyValueChanged( m_wKeyValue );
            }
        }

        if (m_rectIconClearButton.contains(pt))
        {
            m_wKeyValue = 0x00;
            switch (m_keyType)
            {
            case KeyTypeNormal:
                m_keyEdit.setKeyValue( m_wKeyValue );
                m_keyEdit.setFocus();
                break;
            case KeyTypeFunction:
                setFunctionKeyValue( m_wKeyValue );
                break;
            case KeyTypeMouse:
                m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
                setMouseValue( m_wKeyValue );
                break;
            }
            update();

            emit keyValueChanged( m_wKeyValue );
        }
    }
}

void QUserDefinedKeyCtrl::focusInEvent(QFocusEvent *evt)
{
    QWidget::focusInEvent(evt);

    if (m_keyType == KeyTypeNormal)
    {
        m_keyEdit.setFocus();
    }
}

void QUserDefinedKeyCtrl::onKeyValueChanged(QKeyEdit* /*keyEdit*/, unsigned short wKeyValue)
{
    m_wKeyValue = wKeyValue;
    if (m_keyType == KeyTypeMouse)
        m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
    update();

    emit keyValueChanged(m_wKeyValue);
}

void QUserDefinedKeyCtrl::onIndexChangedComboAction( QComboBox* cbAction )
{
    int index = cbAction->currentIndex();
    if (index < 0) return;

    QVariant var = cbAction->itemData(index);
    unsigned short wKeyValue = var.value<unsigned short>();
    m_wKeyValue = ((m_wKeyValue & 0xFF00) | wKeyValue);
    if (m_keyType == KeyTypeMouse)
        m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;

    emit keyValueChanged(m_wKeyValue);
}

void QUserDefinedKeyCtrl::setCurrentIndexWithoutEvent( QComboBox* cbAction, int index )
{
    if (cbAction == &m_cbMouse)
    {
        disconnect(&m_cbMouse, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        m_cbMouse.setCurrentIndex(index);
        connect( &m_cbMouse, SIGNAL(currentIndexChanged(int)), SLOT(onComboMouseIndexChanged(int)));
    }
    else if (cbAction == &m_cbFuncKey)
    {
        disconnect(&m_cbFuncKey, SIGNAL(currentIndexChanged(int)), NULL, NULL);
        m_cbFuncKey.setCurrentIndex(index);
        connect(&m_cbFuncKey, SIGNAL(currentIndexChanged(int)), SLOT(onComboFunctionKeyIndexChanged(int)));
    }
}

void QUserDefinedKeyCtrl::onComboMouseIndexChanged( int index )
{
    if (index < 0)
        return;

    onIndexChangedComboAction( &m_cbMouse );
}

void QUserDefinedKeyCtrl::onComboFunctionKeyIndexChanged( int index )
{
    if (index < 0)
        return;

    onIndexChangedComboAction( &m_cbFuncKey );
}
