#ifndef QCOLORTABWIDGET_H
#define QCOLORTABWIDGET_H

#include <QTabWidget>

#include "QColorTabBar.h"

class QColorTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit QColorTabWidget(QWidget *parent = 0);
    virtual ~QColorTabWidget();

    void BlinkTab( int nIndex );

protected:
    QColorTabBar            m_ColorTabBar;

signals:

public slots:

};

#endif // QCOLORTABWIDGET_H
