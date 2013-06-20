#ifndef QKEYEDITWIDGET_H
#define QKEYEDITWIDGET_H

#include <QLineEdit>

class QKeyEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    explicit QKeyEditWidget(QWidget *parent = 0);

    void Reset();
    void SetKeyValue( ushort wKeyValue );
    ushort GetKeyValue() { return m_wKeyValue; }
    void SetNotKeyInputMode( bool bNotInput );

protected:
    virtual void keyPressEvent(QKeyEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

protected:
    ushort      m_wKeyValue;
    bool        m_bNotInput;

signals:
    void KeyPressSignal( ushort nValue );

public slots:

};

#endif // QKEYEDITWIDGET_H
