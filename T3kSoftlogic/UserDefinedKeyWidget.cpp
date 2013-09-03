#include "UserDefinedKeyWidget.h"

#include "../common/QKeyMapStr.h"

#include <QPixmap>
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>


#define CTRL		0
#define ALT			1
#define SHIFT		2
#define WIN			3

#define UP			0
#define DOWN		1

#define IDC_EDIT_KEY			1
#define IDC_COMBO_FUNCKEY		2
#define IDC_COMBO_MOUSE			3

#define LBUTTON		(0x01)
#define RBUTTON		(0x02)
#define MBUTTON		(0x04)


QUserDefinedKeyWidget::QUserDefinedKeyWidget(QWidget *parent) :
    QWidget(parent)
{
	m_pIconBtn[0] =
	m_pIconBtn[1] =
	m_pIconBtn[2] = 
	m_pIconBtn[3] = NULL;

	m_eKeyType = KeyTypeNormal;

	m_pIconClearBtn = NULL;

	m_wIconKeyValue[CTRL] = MM_MOUSE_KEY0_CTRL;
	m_wIconKeyValue[ALT] = MM_MOUSE_KEY0_ALT;
	m_wIconKeyValue[SHIFT] = MM_MOUSE_KEY0_SHIFT;
	m_wIconKeyValue[WIN] = MM_MOUSE_KEY0_WINDOW;

	m_wKeyValue = 0x00;


    m_pIconBtn[CTRL]	= new QPixmap( ":/T3kSoftlogic/resources/PNG_ICON_KEY_CTRL.png" );
    m_pIconBtn[ALT]		= new QPixmap( ":/T3kSoftlogic/resources/PNG_ICON_KEY_ALT" );
    m_pIconBtn[SHIFT]	= new QPixmap( ":/T3kSoftlogic/resources/PNG_ICON_KEY_SHIFT" );
    m_pIconBtn[WIN]		= new QPixmap( ":/T3kSoftlogic/resources/PNG_ICON_KEY_WIN" );
    m_pIconClearBtn		= new QPixmap( ":/T3kSoftlogic/resources/PNG_ICON_KEY_CLEAR" );

    m_nIconTotalWidth = 0;
    m_nIconTotalWidth += m_pIconBtn[CTRL]->width() / 2;
    m_nIconTotalWidth += m_pIconBtn[ALT]->width() / 2;
    m_nIconTotalWidth += m_pIconBtn[SHIFT]->width() / 2;
    m_nIconTotalWidth += m_pIconBtn[WIN]->width() / 2;

    m_nIconTotalWidth += 4;

    m_nIconTotalHeight = m_pIconBtn[CTRL]->height();

    QRect rc( 0, 0, width(), height() );
    QRect rcEdit;
    rcEdit = rc;
    rcEdit.setTop( rc.center().y() );
    m_wndEdit.setParent( this );
    m_wndEdit.setGeometry( rcEdit );

    m_wndComboFuncKey.setParent( this );
    m_wndComboFuncKey.setGeometry( rcEdit );

    m_wndComboMouse.setParent( this );
    m_wndComboMouse.setGeometry( rcEdit );

    switch( m_eKeyType )
    {
    case KeyTypeNormal:
        m_wndComboMouse.hide();
        m_wndComboFuncKey.hide();
        m_wndEdit.show();
        break;
    case KeyTypeFunction:
        m_wndEdit.hide();
        m_wndComboMouse.hide();
        m_wndComboFuncKey.show();
        break;
    case KeyTypeMouse:
        m_wndEdit.hide();
        m_wndComboFuncKey.hide();
        m_wndComboMouse.show();
        break;
    }

    for( int i=1 ; i<=24 ; i++ )
    {
        m_wndComboFuncKey.addItem( QString("F%1").arg(i) );
        uchar cUsageId;
        int n = virtualKeyToScanCode( Qt::Key_F1+(i-1) );
        if( findUsageId( n, cUsageId ) )
        {
            m_wndComboFuncKey.setItemData( i-1, cUsageId );
        }
    }

    m_wndComboMouse.addItem( "Left Button", LBUTTON );
    m_wndComboMouse.addItem( "Right Button", RBUTTON );
    m_wndComboMouse.addItem( "Middle Button", MBUTTON );

    connect( &m_wndEdit, &QKeyLineEditWidget::textChangedKey, this, &QUserDefinedKeyWidget::onTextChangedKey );
    connect( &m_wndComboFuncKey, SIGNAL(currentIndexChanged(int)), this, SLOT(onCBChangedFunc(int)) );
    connect( &m_wndComboMouse, SIGNAL(currentIndexChanged(int)), this, SLOT(onCBChangedMouse(int)) );
}

QUserDefinedKeyWidget::~QUserDefinedKeyWidget()
{
	for( int i=0 ; i<4 ; i++ )
	{
		if( m_pIconBtn[i] != NULL )
			delete m_pIconBtn[i];
	}

	if( m_pIconClearBtn )
		delete m_pIconClearBtn;
}

void QUserDefinedKeyWidget::setKeyType( KeyType eType )
{
	if( m_eKeyType == eType )
		return;

	m_eKeyType = eType;

	switch( m_eKeyType )
	{
	case KeyTypeNormal:
        m_wndComboMouse.hide();
        m_wndComboFuncKey.hide();
        m_wndEdit.show();
		break;
	case KeyTypeFunction:
        m_wndEdit.hide();
        m_wndComboMouse.hide();
        m_wndComboFuncKey.show();
		break;
	case KeyTypeMouse:
        m_wndEdit.hide();
        m_wndComboFuncKey.hide();
        m_wndComboMouse.show();
		break;
	}
}

void QUserDefinedKeyWidget::reset()
{
	m_wKeyValue = 0x0000;
	switch( m_eKeyType )
	{
	case KeyTypeNormal:
        m_wndEdit.reset();
		break;
	case KeyTypeFunction:
        m_wndComboFuncKey.setCurrentIndex( -1 );
		break;
	case KeyTypeMouse:
        m_wndComboMouse.setCurrentIndex( -1 );
		break;
	}
}

void QUserDefinedKeyWidget::_setFunctionKeyValue( ushort wKeyValue )
{
    ushort wFnKeyValue = (wKeyValue & 0x00FF);
    m_wndComboFuncKey.setCurrentIndex( -1 );
    for( int nF = 0 ; nF<m_wndComboFuncKey.count() ; nF++ )
	{
        ushort wFnKey = (ushort)m_wndComboFuncKey.itemData( nF ).toUInt();
		if( wFnKey == wFnKeyValue )
		{
            m_wndComboFuncKey.setCurrentIndex( nF );
			break;
		}
	}
}

void QUserDefinedKeyWidget::_setMouseValue( ushort wKeyValue )
{
    ushort wMKeyValue = (wKeyValue & 0x00FF);
    m_wndComboMouse.setCurrentIndex( -1 );
    for( int nM = 0 ; nM<m_wndComboMouse.count() ; nM++ )
	{
        ushort wMKey = (ushort)m_wndComboMouse.itemData( nM ).toUInt();
		if( wMKey == wMKeyValue )
		{
            m_wndComboMouse.setCurrentIndex( nM );
			break;
		}
	}
}

void QUserDefinedKeyWidget::setKeyValue( ushort wKeyValue )
{
	m_wKeyValue = wKeyValue;
	switch( m_eKeyType )
	{
	case KeyTypeNormal:
        m_wndEdit.setKeyValue( wKeyValue );
		break;
	case KeyTypeFunction:
        _setFunctionKeyValue( wKeyValue );
		break;
	case KeyTypeMouse:
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
        _setMouseValue( wKeyValue );
		break;
	}

    update();
}

ushort QUserDefinedKeyWidget::getKeyValue()
{ 
	return m_wKeyValue; 
}

void QUserDefinedKeyWidget::paintEvent(QPaintEvent *)
{
    QRect rc( 0, 0, width(), height() );

    uchar cV0 = (uchar)(m_wKeyValue >> 8);

	int nIconW = 0;
    int nIconH = m_pIconBtn[0]->height();
	int nIconX = 0;
    int nIconY = rc.top() + (rc.height()/2-nIconH) / 2;
    bool bDown;

    QPainter painter;
    painter.begin( this );

	for( int i=0 ; i<4 ; i++ )
	{
        bDown = cV0 & m_wIconKeyValue[i] ? true : false;
        nIconW = m_pIconBtn[i]->width() / 2;
        m_rectIconBtn[i] = QRect(nIconX, nIconY, nIconW, nIconH);
        painter.drawPixmap( m_rectIconBtn[i], *m_pIconBtn[i], QRect(bDown ? nIconW : 0, 0, nIconW, nIconH) );
		nIconX += nIconW+1;
	}

    m_rectIconClearBtn = QRect( rc.right() - m_pIconClearBtn->width()/2, nIconY, nIconW, nIconH );
    painter.drawPixmap( m_rectIconClearBtn, *m_pIconClearBtn, QRect(0, 0, nIconW, nIconH) );

    painter.end();
}

void QUserDefinedKeyWidget::resizeEvent(QResizeEvent *evt)
{
    m_wndEdit.setGeometry( 0, evt->size().height()/2, evt->size().width(), evt->size().height()/2 );
    m_wndComboFuncKey.setGeometry( m_wndEdit.geometry() );
    m_wndComboMouse.setGeometry( m_wndEdit.geometry() );

    QWidget::resizeEvent(evt);
}

void QUserDefinedKeyWidget::onTextChangedKey(ushort wKeyValue)
{
	m_wKeyValue = wKeyValue;
	if( m_eKeyType == KeyTypeMouse )
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
    update();

    emit editChangedKey( m_wKeyValue );
}

void QUserDefinedKeyWidget::onCBChangedFunc(int index)
{
    if( index < 0 ) return;

    ushort wKeyValue = (ushort)m_wndComboFuncKey.itemData( index ).toUInt();
	m_wKeyValue = ((m_wKeyValue & 0xFF00) | wKeyValue);

    emit editChangedKey( m_wKeyValue );
}

void QUserDefinedKeyWidget::onCBChangedMouse(int index)
{
    if( index < 0 ) return;

    ushort wKeyValue = (ushort)m_wndComboMouse.itemData( index ).toUInt();
	m_wKeyValue = ((m_wKeyValue & 0xFF00) | wKeyValue);
	if( m_eKeyType == KeyTypeMouse )
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;

    emit editChangedKey( m_wKeyValue );
}

void QUserDefinedKeyWidget::mousePressEvent(QMouseEvent *evt)
{
    QPoint pt( evt->pos() );
	for( int i=0 ; i<4 ; i++ )
	{
        if( m_rectIconBtn[i].contains(pt) )
		{
            uchar cV0 = (uchar)(m_wKeyValue >> 8);
            uchar cV1 = (uchar)(m_wKeyValue);
			cV0 ^= m_wIconKeyValue[i];

			m_wKeyValue = cV0 << 8 | cV1;
			switch( m_eKeyType )
			{
			case KeyTypeNormal:
                m_wndEdit.setKeyValue( m_wKeyValue );
                m_wndEdit.setFocus();
				break;
			case KeyTypeFunction:
                _setFunctionKeyValue( m_wKeyValue );
				break;
			case KeyTypeMouse:
				m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
                _setMouseValue( m_wKeyValue );
				break;
			}
            update();

            emit editChangedKey(m_wKeyValue);
		}
	}

    if( m_rectIconClearBtn.contains(pt) )
	{
		m_wKeyValue = 0x00;
		switch( m_eKeyType )
		{
		case KeyTypeNormal:
            m_wndEdit.setKeyValue( m_wKeyValue );
            m_wndEdit.setFocus();
			break;
		case KeyTypeFunction:
            _setFunctionKeyValue( m_wKeyValue );
			break;
		case KeyTypeMouse:
			m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
            _setMouseValue( m_wKeyValue );
			break;
		}
        update();

        emit editChangedKey(m_wKeyValue);
	}
}

void QUserDefinedKeyWidget::focusInEvent(QFocusEvent *evt)
{
	if( m_eKeyType == KeyTypeNormal )
	{
        m_wndEdit.setFocus();
	}

    QWidget::focusInEvent(evt);
}
