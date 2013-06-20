#ifndef QRAISEPUSHBUTTON_H
#define QRAISEPUSHBUTTON_H

#include <QToolButton>

class QRaisePushButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QRaisePushButton(QWidget *parent = 0);

#ifndef Q_OS_WIN
    void setVisible(bool visible);

protected:
    virtual void paintEvent(QPaintEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

protected:
    bool            m_bHover;
#endif
signals:

public slots:

};

#endif // QRAISEPUSHBUTTON_H
