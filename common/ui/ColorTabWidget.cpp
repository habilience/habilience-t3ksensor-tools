#include "ColorTabWidget.h"

QColorTabWidget::QColorTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    m_pTabBar = new QColorTabBar( this );
    setTabBar( m_pTabBar );
}

QColorTabWidget::~QColorTabWidget()
{
    if( m_pTabBar )
    {
        delete m_pTabBar;
        m_pTabBar = NULL;
    }
}

void QColorTabWidget::blinkTab(int index)
{
    m_pTabBar->BlinkTab( index );
}
