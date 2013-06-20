#include "QRaisePushButton.h"

#include <QStylePainter>
#include <QStyleOptionToolButton>

#include <QEvent>

QRaisePushButton::QRaisePushButton(QWidget *parent) :
    QToolButton(parent)
{
#ifndef Q_OS_WIN
    m_bHover = false;

    setAttribute( Qt::WA_Hover );

    installEventFilter( this );
#endif
}
#ifndef Q_OS_WIN
void QRaisePushButton::paintEvent(QPaintEvent *evt)
{
    if( m_bHover )
    {
        QToolButton::paintEvent(evt);
        return;
    }

    QStylePainter dc;
    dc.begin( this );
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    dc.drawControl(QStyle::CE_ToolButtonLabel, opt);
    dc.end();
}

bool QRaisePushButton::eventFilter(QObject *target, QEvent *evt)
{
    if( target == this )
    {
        if( evt->type() == QEvent::HoverEnter )
        {
            m_bHover = true;
        }

        if( evt->type() == QEvent::HoverLeave )
        {
            m_bHover = false;
        }
    }

    return QToolButton::eventFilter(target, evt);
}

void QRaisePushButton::setVisible(bool visible)
{
    setAttribute( Qt::WA_Hover );

    QToolButton::setVisible( visible );
}

#endif
