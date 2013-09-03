#ifndef QKEYLINEEDITWIDGET_H
#define QKEYLINEEDITWIDGET_H

#include <QLineEdit>


class QKeyLineEditWidget : public QLineEdit
{
    Q_OBJECT

public:
    QKeyLineEditWidget(QWidget* parent=0);
    virtual ~QKeyLineEditWidget();

    void reset();
    void setKeyValue( ushort wKeyValue );
    ushort getKeyValue() { return m_wKeyValue; }

protected:
    virtual void keyPressEvent(QKeyEvent *);

protected:
    ushort	m_wKeyValue;

signals:
    void textChangedKey(ushort nVale);
public slots:

};

#endif // QKEYLINEEDITWIDGET_H
