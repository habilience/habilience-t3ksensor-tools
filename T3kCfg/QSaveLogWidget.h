#ifndef QSAVELOGWIDGET_H
#define QSAVELOGWIDGET_H

#include <QDialog>
#include "TPDPEventMultiCaster.h"

class QSaveLogWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QSaveLogWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QSaveLogWidget();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void keyPressEvent(QKeyEvent *);

private:
    T3kHandle*&        m_pT3kHandle;

    int                 m_nTimer;

signals:

private slots:

};

#endif // QSAVELOGWIDGET_H
