#include "QColorTabWidget.h"

#include <QPainter>
#include <QTabBar>
#include <QTimerEvent>

QColorTabWidget::QColorTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar( &m_ColorTabBar );
}

QColorTabWidget::~QColorTabWidget()
{
}

void QColorTabWidget::BlinkTab(int nIndex)
{
    m_ColorTabBar.BlinkTab( nIndex );
}
