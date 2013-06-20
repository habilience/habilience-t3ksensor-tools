#ifndef QSAVELOGWIDGET_H
#define QSAVELOGWIDGET_H

#include <QDialog>
#include "QTPDPEventMultiCaster.h"

class QSaveLogWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QSaveLogWidget(T30xHandle*& pHandle, QWidget *parent = 0);
    ~QSaveLogWidget();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void keyPressEvent(QKeyEvent *);

private:
    T30xHandle*&        m_pT3kHandle;

    int                 m_nTimer;

signals:

private slots:

};

#endif // QSAVELOGWIDGET_H
