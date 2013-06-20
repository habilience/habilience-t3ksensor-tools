#ifndef QDRAWINGWIDGET_H
#define QDRAWINGWIDGET_H

#include <QWidget>

class QLegendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QLegendWidget(QWidget *parent = 0);

    void SetText( QString strText ) { m_strText = strText; }

protected:
    QString         m_strText;

protected:
    virtual void paintEvent(QPaintEvent *evt);

signals:

public slots:

};

#endif // QDRAWINGWIDGET_H
