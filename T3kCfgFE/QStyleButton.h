#ifndef QSTYLEBUTTON_H
#define QSTYLEBUTTON_H

#include <QPushButton>

class QStyleButton : public QPushButton
{
    Q_OBJECT
public:
    enum Alignment { AlignLeft, AlignCenter, AlignRight };
private:
    bool m_bIsHovered;
    bool m_bIsFocused;
    bool m_bIsSelected;

    QColor m_clrBackground;
    QColor m_clrNormal;
    QColor m_clrBorder;
    QColor m_clrActive;
    QColor m_clrText;
    QColor m_clrAdditionalText;
    QString m_strAdditionalText;
    int m_nAdditionalTextLineCount;

    int m_nMarginLeft;
    int m_nMarginTop;
    int m_nMarginRight;
    int m_nMarginBottom;

    QFont m_fntCaption;
    QFont m_fntAdditionalText;
    Alignment m_TextAlignment;
public:
    explicit QStyleButton(QWidget *parent = 0);

    void setMargin( int left, int top, int right, int bottom );
    void setColor( const QColor& clrNormal, const QColor& clrBorder, const QColor& clrActive, const QColor& clrText );
    void setAdditionalText( QString strText, int nLineCount, const QColor& clrText=QColor(40, 40, 40) );

    void setCaptionFontHeight( int pixelHeight );
    void setAlignment( Alignment align );

protected:
    void drawFocusRect( QPainter& p, QRect& rcBody );

    virtual void paintEvent(QPaintEvent *evt);
    virtual bool eventFilter(QObject *obj, QEvent *evt);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    
signals:
    
public slots:
    
};

#endif // QSTYLEBUTTON_H
