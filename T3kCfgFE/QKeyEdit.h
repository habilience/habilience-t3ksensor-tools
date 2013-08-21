#ifndef QKEYEDIT_H
#define QKEYEDIT_H

#include <QLineEdit>

class QKeyEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QKeyEdit(QWidget *parent = 0);

    void reset();
    void setKeyValue( unsigned short wKeyValue );
    unsigned short keyValue() { return m_wKeyValue; }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);
    void setToNone();
    unsigned short m_wKeyValue;
    
signals:
    void keyEditValueChanged(QKeyEdit* keyEdit, unsigned short wKeyValue);
    
public slots:
    
};

#endif // QKEYEDIT_H
