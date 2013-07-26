#ifndef QUNDERLINETITLE_H
#define QUNDERLINETITLE_H

#include <QLabel>

class QUnderlineTitle : public QLabel
{
    Q_OBJECT
private:
    QImage      m_imgIcon;
protected:
    virtual void paintEvent(QPaintEvent *);
public:
    void setIconImage(const QString& strIconPath);
    explicit QUnderlineTitle(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // QUNDERLINETITLE_H
