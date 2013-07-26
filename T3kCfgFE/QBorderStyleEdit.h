#ifndef QBORDERSTYLEEDIT_H
#define QBORDERSTYLEEDIT_H

#include <QTextEdit>

class QBorderStyleEdit : public QTextEdit
{
    Q_OBJECT
private:
    QColor      m_clrText;
    QColor      m_clrActiveText;
    QColor      m_clrBorder;
    QColor      m_clrBackground;

    double      m_dOldValue;

    bool        m_bIsFloatStyle;
    double      m_dValueRangeMin;
    double      m_dValueRangeMax;

    bool        m_bNowEditing;

    QString     m_strAllowChars;

    int         m_TimerWarning;
    bool        m_bIsBlink;
    bool        m_bIsWarning;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void timerEvent(QTimerEvent *e);

    QString getPreferStyleSheet();
public:
    void setColor( const QColor& clrBorder, const QColor& clrBackground );
    void setFloatStyle( bool bFloatStyle );
    void setWarning( bool bWarning );

    bool isEditing() { return m_bNowEditing; }
    bool isFloatStyle() { return m_bIsFloatStyle; }

    explicit QBorderStyleEdit(QWidget *parent = 0);
    
signals:
    void editModified( QBorderStyleEdit* sender, int nValue, double dValue );
public slots:
    
};

#endif // QBORDERSTYLEEDIT_H
