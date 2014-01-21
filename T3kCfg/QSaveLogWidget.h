#ifndef QSAVELOGWIDGET_H
#define QSAVELOGWIDGET_H

#include <QDialog>
#include "QT3kDeviceR.h"

class QSaveLogWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QSaveLogWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QSaveLogWidget();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void keyPressEvent(QKeyEvent *);

private:
    QT3kDeviceR*&        m_pT3kHandle;

    int                 m_nTimer;

signals:

private slots:

};

#endif // QSAVELOGWIDGET_H
