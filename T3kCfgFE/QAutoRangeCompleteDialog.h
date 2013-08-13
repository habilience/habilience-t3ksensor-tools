#ifndef QAUTORANGECOMPLETEDIALOG_H
#define QAUTORANGECOMPLETEDIALOG_H

#include <QDialog>

namespace Ui {
class QAutoRangeCompleteDialog;
}

class QAutoRangeCompleteDialog : public QDialog
{
    Q_OBJECT
private:
    int     m_nTimerCount;
    QRect   m_rcProgress;
    int     m_TimerDummyProgress;
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);
public:
    explicit QAutoRangeCompleteDialog(QWidget *parent = 0);
    ~QAutoRangeCompleteDialog();

private slots:
    virtual void accept();
    virtual void reject();

private:
    Ui::QAutoRangeCompleteDialog *ui;
};

#endif // QAUTORANGECOMPLETEDIALOG_H
