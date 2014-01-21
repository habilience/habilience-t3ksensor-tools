#ifndef Q2COLORTABWIDGET_H
#define Q2COLORTABWIDGET_H

#include <QTabWidget>
#include "ColorTabBar.h"

class Q2ColorTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit Q2ColorTabWidget(QWidget *parent = 0);
    virtual ~Q2ColorTabWidget();

    void blinkTab(int index);

protected:
    QColorTabBar*        m_pTabBar;

signals:

public slots:

};

#endif // Q2COLORTABWIDGET_H
