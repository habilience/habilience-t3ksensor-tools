#ifndef QICONLABEL_H
#define QICONLABEL_H

#include <QLabel>

class QIconLabel : public QWidget
{
    Q_OBJECT
public:
    explicit QIconLabel(QWidget *parent = 0);

    void SetIconImage( QString strImagePathName );

protected:
    virtual void paintEvent(QPaintEvent *evt);

    QString             m_strImagePathName;

signals:

public slots:

};

#endif // QICONLABEL_H
