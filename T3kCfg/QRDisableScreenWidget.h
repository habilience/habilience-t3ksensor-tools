#ifndef QRDISABLESCREENWIDGET_H
#define QRDISABLESCREENWIDGET_H

#include <QWidget>

class QRDisableScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRDisableScreenWidget(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent *);

signals:

public slots:

};

#endif // QRDISABLESCREENWIDGET_H
