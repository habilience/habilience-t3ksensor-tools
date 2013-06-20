#ifndef QREMOTEGUIDEWIDGET_H
#define QREMOTEGUIDEWIDGET_H

#include <QWidget>

class QRemoteGuideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRemoteGuideWidget(QWidget *parent = 0);

protected:
    virtual void showEvent(QShowEvent *);
    virtual void paintEvent(QPaintEvent *);

    void ShowPointing( QPoint ptTarget );

protected:
    void MoveWidgetToPrimary();

protected:
    QPoint              m_ptTarget;

signals:

public slots:

};

#endif // QREMOTEGUIDEWIDGET_H
