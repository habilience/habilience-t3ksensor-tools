#include "Q2ColorTabWidget.h"

Q2ColorTabWidget::Q2ColorTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    m_pTabBar = new QColorTabBar( this );
    setTabBar( m_pTabBar );
}

Q2ColorTabWidget::~Q2ColorTabWidget()
{
    if( m_pTabBar )
    {
        delete m_pTabBar;
        m_pTabBar = NULL;
    }
}

void Q2ColorTabWidget::blinkTab(int index)
{
    m_pTabBar->BlinkTab( index );
}
