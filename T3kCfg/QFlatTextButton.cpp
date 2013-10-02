#include "QFlatTextButton.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

QFlatTextButton::QFlatTextButton(QWidget *parent) :
    QPushButton(parent)
{
    m_bIsHovered = false;
    m_bIsMouseDown = false;
    m_bIsPressed = false;
    m_pImageIcon = NULL;
    m_bDrawBorder = false;

    m_eFBA = FBA_LEFT;

    const QFont& pFont = font();
    m_fntUnderline = m_fntNormal = pFont;
    m_fntUnderline.setUnderline( true );

    setBackgroundRole( QPalette::Background );
    setMouseTracking( true );
    setAttribute( Qt::WA_Hover );

    installEventFilter( this );
}

QFlatTextButton::~QFlatTextButton()
{
    if( m_pImageIcon )
    {
        delete m_pImageIcon;
        m_pImageIcon = NULL;
    }
}

void QFlatTextButton::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rcBody( 0,0,width()-1,height()-1 );

    dc.fillRect( rcBody, Qt::white );

//    bool bEnabled = lpDrawItemStruct->itemState & ODS_DISABLED ? false : true;
//    bool bFocused = lpDrawItemStruct->itemState & ODS_FOCUS ? true : false;

    DrawButton( &dc, rcBody );

    if( !isEnabled() )
        dc.fillRect( rcBody, QColor(255,255,255,128) );

    dc.end();
}

void QFlatTextButton::SetIconImage(const QString &strPathName)
{
    if( m_pImageIcon )
        delete m_pImageIcon;

    m_pImageIcon = new QImage( strPathName );
}

QPainterPath QFlatTextButton::CreateRoundRect( const QRect& rect, int nRadius )
{
    int nDiameter = nRadius << 1;

    int x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    QRect arcRect( x1, y1, nDiameter, nDiameter );

    QPainterPath path;
    path.moveTo(x1, y1+nRadius);
    path.arcTo(arcRect, 180.0, -90.0);
    path.lineTo(x2-nRadius,y1);
    arcRect.moveTo( x2-nDiameter, y1);
    path.arcTo(arcRect, 90.0, -90.0);
    path.lineTo(x2,y2-nRadius);
    arcRect.moveTo(x2-nDiameter, y2-nDiameter);
    path.arcTo(arcRect, 0.0, -90.0);
    path.lineTo(x1+nRadius,y2);
    arcRect.moveTo(x1,y2-nDiameter);
    path.arcTo(arcRect, -90.0, -90.0);
    path.lineTo(x1, y1+nRadius);

    path.closeSubpath();

    return path;
}

void QFlatTextButton::DrawButton( QPainter* pDC, QRect rcBody )
{
    QString strText( text() );

    /*
    UINT nBtnAlign;
    if( (GetStyle() & BS_CENTER) == BS_CENTER )
    {
        nBtnAlign = DT_CENTER;
    }
    else
    {
        nBtnAlign = GetStyle() & BS_LEFT ? DT_LEFT : DT_RIGHT;
    }*/

    pDC->setRenderHint( QPainter::Antialiasing );

    QRect rectBody( rcBody.left(), rcBody.top(), rcBody.width(), rcBody.height() );
    if( m_bDrawBorder )
    {
        QRect rectBorder( rectBody );
        rectBorder.adjust( 2,2,-2,-2 );
        QPainterPath BorderPath = CreateRoundRect( rectBorder, 3 );
        pDC->setPen( QColor(100, 100, 100) );
        pDC->drawPath( BorderPath );
    }
/*    if(  )
    {
        rectBody.adjust( 3,3,-3,-3 );
        //DrawFocusRect( g, rectBody );
    }*/

    const int nOff = 3;
    rcBody.adjust( nOff, nOff, -nOff, -nOff );

    QFontMetricsF ftMetrics( font() );
    qreal ftW = ftMetrics.width( strText );
//    qreal ftH = ftMetrics.height();
    Qt::AlignmentFlag eFlag = Qt::AlignLeft;
    switch( m_eFBA )
    {
    case FBA_LEFT:
        break;
    case FBA_RIGHT:
        eFlag = Qt::AlignRight;
        break;
    case FBA_CENTER:
        eFlag = Qt::AlignCenter;
        break;
    default:
        break;
    }

    if( m_pImageIcon )
    {
        int nIconW = m_pImageIcon->width();
        int nIconH = m_pImageIcon->height();
        const int nOffsetX = 3;

        int nIconX = 0;//rcBody.left() + (rcBody.width()-nIconW-ftW) / 2 - nOffsetX;
        switch( m_eFBA )
        {
        case FBA_LEFT:
            nIconX = rcBody.left();
            break;
        case FBA_RIGHT:
            nIconX = rcBody.right() - ftW - nOffsetX - nIconW;
            break;
        case FBA_CENTER:
            nIconX = rcBody.left() + (rcBody.width()-nIconW-ftW) / 2 - nOffsetX;
            break;
        }
        int nIconY = rcBody.top() + (rcBody.height()-nIconH) / 2;

        bool bR2L = QApplication::layoutDirection() == Qt::RightToLeft;
        if( bR2L )
            nIconX = rcBody.right() - nIconX - nIconW;

        pDC->drawImage( nIconX, nIconY, /*QRect(nIconX,nIconY,nIconW,nIconH),*/ *m_pImageIcon );

        if( bR2L )
            rcBody.setRight( rcBody.right() - nIconW - nOffsetX*2 );
        else
            rcBody.setLeft( nIconX + nIconW + nOffsetX );

        //m_pImageIcon
    }

    pDC->setFont( m_bIsHovered ? m_fntUnderline : m_fntNormal );
    pDC->setPen( m_bIsHovered ? (m_bIsMouseDown ? QColor(255, 30, 30) : QColor(30, 30, 255)) : (m_bIsMouseDown ? QColor(255, 30, 30) : QColor(30, 30, 30)) );

    pDC->drawText( rcBody, strText, QTextOption(Qt::AlignVCenter|eFlag) );

    /*
    Rect rectBody( rcBody.left, rcBody.top, rcBody.Width(), rcBody.Height() );
    rectBody.Inflate( -2, -2 );

    BSTR bstrTitle = m_strTitleText.AllocSysString();
    FontFamily fntFamily( L"Arial" );
    Font fntTitle( &fntFamily, 12, FontStyleBold, UnitPixel );

    RectF rectTitle;
    rectTitle.X = (float)nIconOffset;
    rectTitle.Y = (float)rectBody.Y + nOffsetXY;
    rectTitle.Width = (float)rectBody.Width - nOffsetXY*2;
    rectTitle.Height = 14.f;

    StringFormat stringFormat;
    stringFormat.SetLineAlignment( StringAlignmentCenter );
    stringFormat.SetAlignment( StringAlignmentNear );
    g.DrawString( bstrTitle, -1, &fntTitle, rectTitle, &stringFormat, &SolidBrush( Color(80, 80, 80) ) );

    ::SysFreeString( bstrTitle );
    */
    //if( bFocused )
    //	DrawFocusRect( g, rectBody );
}

void QFlatTextButton::DrawFocusRect( QPainter& dc, QRect& rectBody )
{
    QRect rectFocus( rectBody );
    rectFocus.adjust( 2,2,-2,-2 );
    QPainterPath pRoundPath = CreateRoundRect( rectFocus, 3 );
    QPen penDot( QColor(100, 100, 100, 100) );
    penDot.setCapStyle( Qt::RoundCap );
    penDot.setStyle( Qt::DashLine );
    dc.setPen( penDot );
    dc.drawPath( pRoundPath );
}

bool QFlatTextButton::eventFilter(QObject *obj, QEvent *evt)
{
    if( isEnabled() )
    {
        if( evt->type() == QEvent::HoverEnter )
        {
            m_bIsHovered = true;
            update();
        }

        if( evt->type() == QEvent::HoverLeave )
        {
            m_bIsHovered = false;
            update();
        }
    }

    return QPushButton::eventFilter(obj,evt);
}

void QFlatTextButton::mousePressEvent(QMouseEvent *e)
{
    if( e->type() == QEvent::MouseButtonPress )
    {
        m_bIsMouseDown = true;
    }

    QPushButton::mousePressEvent(e);
}

void QFlatTextButton::mouseReleaseEvent(QMouseEvent *e)
{
    if( e->type() == QEvent::MouseButtonRelease )
    {
        m_bIsMouseDown = false;
    }
    QPushButton::mouseReleaseEvent(e);
}
