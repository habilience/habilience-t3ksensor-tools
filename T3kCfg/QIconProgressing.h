#ifndef QICONPROGRESSING_H
#define QICONPROGRESSING_H

#include <QLabel>
#include <QList>

class QIconProgressing : public QLabel
{
    Q_OBJECT
public:
    explicit QIconProgressing(QWidget *parent = 0);
    ~QIconProgressing();

    void Start();
    void Complete();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);

protected:
    QList<QImage*>  m_listProgressImage;
    int             m_nCurrentImageIndex;
    int             m_nImageCount;

    int             m_nTimerProgress;

signals:

public slots:

};

#endif // QICONPROGRESSING_H
