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

    int         m_nMaxTextLength;

    Qt::Alignment m_alignText;
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

    bool isEditing() const { return m_bNowEditing; }
    bool isFloatStyle() const { return m_bIsFloatStyle; }

    int maxTextLength() const { return m_nMaxTextLength; }
    void setMaxTextLength( int nMax ) { m_nMaxTextLength = nMax; }

    explicit QBorderStyleEdit(QWidget *parent = 0);
    
signals:
    void editModified( QBorderStyleEdit* sender, int nValue, double dValue );
public slots:
    void setAlignment(Qt::Alignment a);
    void setText(const QString &text);
    
};

#endif // QBORDERSTYLEEDIT_H
