#ifndef QHOVERSPINWIDGET_H
#define QHOVERSPINWIDGET_H

#include <QSpinBox>

#define QEVENT_SPIN_CHANGED_VALUE		(QEvent::User+500)

class QHoverSpinWidget : public QSpinBox
{
    Q_OBJECT
public:
    explicit QHoverSpinWidget(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *evt);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void focusInEvent(QFocusEvent *event);

signals:
    void ItemChanged(QObject* pObj);
public slots:

};

#endif // QHOVERSPINWIDGET_H
