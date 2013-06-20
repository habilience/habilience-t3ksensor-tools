#ifndef QFLATTEXTBUTTON_H
#define QFLATTEXTBUTTON_H

#include <QPushButton>

class QFlatTextButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QFlatTextButton(QWidget *parent = 0);
    virtual ~QFlatTextButton();

    enum FlatBtnAlign { FBA_LEFT, FBA_RIGHT, FBA_CENTER };

    void SetBorder( bool bBorder ) { m_bDrawBorder = bBorder; }
    void SetIconImage( const QString &strPathName );
    void SetAlignmentText( FlatBtnAlign eFBA ) { m_eFBA= eFBA; }

protected:
    void DrawButton( QPainter* pDC, QRect rcBody );
    void DrawFocusRect( QPainter& dc, QRect& rectBody );

    QPainterPath CreateRoundRect( const QRect& rect, int nRadius );

    void OnMouseEnter();

    virtual void paintEvent(QPaintEvent *evt);
    virtual bool eventFilter(QObject *obj, QEvent *evt);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    bool		m_bIsHovered;
    bool		m_bIsMouseDown;
    bool		m_bIsPressed;

    QFont		m_fntNormal;
    QFont		m_fntUnderline;

    QImage*		m_pImageIcon;

    bool		m_bDrawBorder;

    FlatBtnAlign        m_eFBA;
signals:

public slots:

};

#endif // QFLATTEXTBUTTON_H
