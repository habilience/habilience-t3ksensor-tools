#ifndef COLORTABWIDGET_H
#define COLORTABWIDGET_H

#include <QTabWidget>
#include "ColorTabBar.h"

class QColorTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit QColorTabWidget(QWidget *parent = 0);
    virtual ~QColorTabWidget();

    void blinkTab(int index);

protected:
    QColorTabBar*        m_pTabBar;

signals:

public slots:

};

#endif // COLORTABWIDGET_H
