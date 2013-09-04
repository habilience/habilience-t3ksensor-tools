#include "QHoverComboBox.h"

#include <QCoreApplication>
#include <QStylePainter>


QHoverComboBox::QHoverComboBox(QWidget *parent, bool bHover, int nRowIndex, int nColumnIndex) :
    QComboBox(parent),
    m_nRowIndex(nRowIndex), m_nColumnIndex(nColumnIndex), m_bUseHover(bHover)
{
    m_bHover = false;
    m_bZeroSet = false;
    m_bDrawBackColor = false;

    m_nCurIndex = -1;

    connect( this, SIGNAL(activated(int)), this, SLOT(onCurrentIndexChanged(int)) );

    installEventFilter( this );

    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    setAttribute( Qt::WA_Hover );

    setMaxVisibleItems( 255 );
    setMaxCount( 255 );
}

void QHoverComboBox::ChangeIndex(int nIndex)
{
    if( currentIndex() != nIndex )
    {
        setCurrentIndex( nIndex );
        return;
    }

    onCurrentIndexChanged( nIndex );
}

void QHoverComboBox::paintEvent(QPaintEvent *e)
{
    QPainter dc;
    dc.begin( this );
    QRect rc( 0, 0, width(), height() );
    dc.fillRect( rc, Qt::white );

    if( !m_bUseHover || m_bHover )
    {
        dc.end();
        QComboBox::paintEvent(e);
        return;
    }

    if( m_bDrawBackColor )
        dc.fillRect( rc, QColor(219,228,242) );

    dc.end();

    QStylePainter painter;
    painter.begin( this );
    painter.setPen(palette().color(QPalette::Text));
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    painter.end();
}

bool QHoverComboBox::eventFilter(QObject *target, QEvent *evt)
{
    if( isVisible() )
    {
        if( m_bUseHover && evt->type() == QEvent::HoverEnter )
        {
            m_bHover = true;
            update();
        }

        if( m_bUseHover && evt->type() == QEvent::HoverLeave )
        {
            m_bHover = false;
            update();
        }

        if( evt->type() == QEvent::MouseButtonPress )
        {
            FocusInCB();
        }

        if( evt->type() == QEvent::Wheel )
        {
            evt->accept();
            return true;
        }
    }

    return QComboBox::eventFilter(target,evt);
}

void QHoverComboBox::onCurrentIndexChanged(int nIndex)
{
    if( !m_bZeroSet )
    {
        if( nIndex < 0 )
            setCurrentIndex( 0 );
        else
            ItemChanged( this, nIndex );
    }
    else
        ItemChanged( this, nIndex );
}

void QHoverComboBox::showPopup()
{
    ShowPopupSignal();

    QComboBox::showPopup();
}

void QHoverComboBox::hidePopup()
{
    m_bHover = false;
    update();

    HidePopupSignal();

    QComboBox::hidePopup();
}

void QHoverComboBox::focusInEvent(QFocusEvent *e)
{
    FocusInSignal();

    QComboBox::focusInEvent(e);
}

void QHoverComboBox::focusOutEvent(QFocusEvent *e)
{
    FocusOutSignal();

    QComboBox::focusOutEvent(e);
}
