#include "QHoverSpinWidget.h"

#include <QKeyEvent>
#include <QCoreApplication>

#include "QSendDataEvent.h"

QHoverSpinWidget::QHoverSpinWidget(QWidget *parent) :
    QSpinBox(parent)
{
    setMaximum( 63 );
    hide();
}

void QHoverSpinWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QKeyEvent::KeyPress )
    {
        int nVKey = evt->key();
        if( nVKey < Qt::Key_0 || nVKey > Qt::Key_9 )
        {
            evt->accept();
            return;
        }
        ItemChanged( parent() );
    }

    QSpinBox::keyPressEvent(evt);
}

void QHoverSpinWidget::focusInEvent(QFocusEvent *event)
{
    if( !isVisible() )
    {
        event->accept();
        return;
    }
    QSpinBox::focusInEvent(event);
}

void QHoverSpinWidget::focusOutEvent(QFocusEvent *event)
{
    hide();
    QSendDataEvent e( QEVENT_SPIN_CHANGED_VALUE, text().toInt(), parent() );
    QCoreApplication::sendEvent( parent(), &e );
}
