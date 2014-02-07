#ifndef QSAVELOGWIDGET_H
#define QSAVELOGWIDGET_H

#include <QDialog>
#include "QT3kDevice.h"

class QSaveLogWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QSaveLogWidget(QT3kDevice*& pHandle, QWidget *parent = 0);
    ~QSaveLogWidget();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void keyPressEvent(QKeyEvent *);

private:
    QT3kDevice*&        m_pT3kHandle;

    int                 m_nTimer;

signals:

private slots:

};

#endif // QSAVELOGWIDGET_H
