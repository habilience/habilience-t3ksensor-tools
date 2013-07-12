
#include "QStyleButton.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QEvent>
#include <QPainter>
#include <QGradientStop>
#include <QSettings>

#define BORDER_ROUND        (3)

#ifdef Q_OS_WIN
inline QFont::Weight weightFromInteger(long weight)
{
    if (weight < 400)
        return QFont::Light;
    if (weight < 600)
        return QFont::Normal;
    if (weight < 700)
        return QFont::DemiBold;
    if (weight < 800)
        return QFont::Bold;
    return QFont::Black;
}
#endif

QStyleButton::QStyleButton(QWidget *parent) :
    QPushButton(parent)
{
    setBackgroundRole(QPalette::Background);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

    installEventFilter(this);

    m_bIsHovered = false;
    m_bIsFocused = false;
    m_bIsSelected = false;

    m_clrBackground = QColor(255, 255, 255);
    m_clrNormal = QColor(180, 180, 180);
    m_clrBorder = QColor(100, 100, 100);
    m_clrActive = QColor(43, 140, 238);//RGB(121, 183, 244);
    m_clrText = QColor(50, 50, 50);
    m_clrAdditionalText = QColor(50, 50, 50);
    m_strAdditionalText.clear();
    m_nAdditionalTextLineCount = 0;

    m_nMarginLeft = 0;
    m_nMarginTop = 0;
    m_nMarginRight = 0;
    m_nMarginBottom = 0;

    m_TextAlignment = AlignLeft;

#ifdef Q_OS_WIN
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONT);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    QString strFont;
    strFont = QString::fromWCharArray(ncm.lfMessageFont.lfFaceName);
    //strFont = "Arial";
    m_fntCaption = QFont(strFont);
    if (ncm.lfMessageFont.lfWeight != FW_DONTCARE)
        m_fntCaption.setWeight(weightFromInteger(ncm.lfMessageFont.lfWeight));

    QSettings s("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
    const int clearTypeEnum = 2;
    if ( clearTypeEnum == s.value("FontSmoothingType",1) )
    {
          m_fntCaption.setStyleStrategy(QFont::PreferAntialias);
    }
    m_fntCaption.setPointSizeF( font().pointSizeF() );
#else
    m_fntCaption = font();
#endif
    m_fntAdditionalText = m_fntCaption;
    m_fntAdditionalText.setPixelSize(m_fntCaption.pixelSize() * 7 / 10);
}

void QStyleButton::paintEvent(QPaintEvent */*evt*/)
{
    //QPushButton::paintEvent(evt);

    const double dPenWidth = 0.9;
    QRect rcBody(0, 0, width()-1, height()-1);

    QPainter p(this);

    p.save();

    p.setRenderHint(QPainter::Antialiasing);

    rcBody.adjust(2, 2, -2, -2);

    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_clrBackground));
    p.drawRoundedRect( rcBody, BORDER_ROUND, BORDER_ROUND );

    QRect rcI = rcBody;
    rcI.adjust(1, 1, -1, -1);

    int nAlphaOffset = 0;
    if (m_bIsHovered)
    {
        nAlphaOffset = 35;
    }
    int nTopAlpha = 0+nAlphaOffset;
    int nBtmAlpha = 220+nAlphaOffset;

    QColor color;
    if (m_bIsHovered || m_bIsSelected)
    {
        color = m_clrActive;
    }
    else
    {
        color = m_clrNormal;
    }

    if (!isEnabled())
    {
        QBrush BodyBgBrush( QColor(180, 180, 180, 200) );
        p.setPen(Qt::NoPen);
        p.setBrush(BodyBgBrush);
        p.drawRoundedRect( rcBody, BORDER_ROUND, BORDER_ROUND );
        m_clrBorder.setAlpha(240);
        p.setPen( QPen(QBrush(m_clrBorder), dPenWidth) );
        p.setBrush(Qt::NoBrush);
        m_clrBorder.setAlpha(255);
        p.drawRoundedRect( rcBody, BORDER_ROUND, BORDER_ROUND );
    }
    else
    {
        QLinearGradient grad(QPointF((double)rcBody.center().x(), (double)rcBody.top()), QPointF((double)rcBody.center().x(), (double)rcBody.bottom()));
        QGradientStops stops;
        double factors[4] = {0.0, 0.2, 0.4, 0.6};
        double positions[4] = {0.0, 0.46, 0.47, 1.0};
        if (m_bIsSelected)
        {
            positions[1] = 0.54; positions[2] = 0.55;
            factors[1] = 0.5;
            factors[2] = 0.7;
            factors[3] = 1.0;
        }
        color.setAlpha( (int)(nTopAlpha+(nBtmAlpha-nTopAlpha) * factors[0] + 0.5) );
        stops << QGradientStop(positions[0], color);
        color.setAlpha( (int)(nTopAlpha+(nBtmAlpha-nTopAlpha) * factors[1] + 0.5) );
        stops << QGradientStop(positions[1], color);
        color.setAlpha( (int)(nTopAlpha+(nBtmAlpha-nTopAlpha) * factors[2] + 0.5) );
        stops << QGradientStop(positions[2], color);
        color.setAlpha( (int)(nTopAlpha+(nBtmAlpha-nTopAlpha) * factors[3] + 0.5) );
        stops << QGradientStop(positions[3], color);
        color.setAlpha( 255 );
        grad.setStops(stops);

        p.setPen( Qt::NoPen );
        p.setBrush(grad);
        p.drawRoundedRect( rcBody, BORDER_ROUND, BORDER_ROUND );
        p.setBrush(Qt::NoBrush);

        if (m_bIsSelected)
        {
            p.setPen( QPen(QBrush(QColor().fromRgb(0xFFF5F5F5)), dPenWidth) );
        }
        else
        {
            p.setPen( QPen(QBrush(Qt::white), dPenWidth) );
        }

        p.drawRoundedRect( rcI, BORDER_ROUND, BORDER_ROUND );

        if (m_bIsFocused)
        {
            drawFocusRect( p, rcBody );
        }

        if (m_bIsSelected)
        {
            p.setPen( QPen(QBrush(m_clrBorder), dPenWidth));
        }
        else
        {
            m_clrBorder.setAlpha(240);
            p.setPen( QPen(QBrush(m_clrBorder), dPenWidth));
            m_clrBorder.setAlpha(255);
        }
        p.drawRoundedRect( rcBody, BORDER_ROUND, BORDER_ROUND );
    }

    QString strCaption = this->text();

    bool bAdditionalText = m_strAdditionalText.isEmpty() ? false : true;

    QRectF rectCaptionF( rcBody.left(), rcBody.top(), rcBody.width(), rcBody.height() );
    rectCaptionF.adjust( m_nMarginLeft, m_nMarginTop, -m_nMarginRight, -m_nMarginBottom );

    int flags = Qt::AlignLeft;
    switch (m_TextAlignment)
    {
    case AlignLeft:
        flags = Qt::AlignLeft;
        break;
    case AlignCenter:
        flags = Qt::AlignHCenter;
        break;
    case AlignRight:
        flags = Qt::AlignRight;
        break;
    }

    // Qt::AlignRight;
    // Qt::AlignHCenter;
    // Qt::AlignVCenter;

    QFontMetricsF ftMetrics( m_fntCaption );
    QRectF rectTopBBF = ftMetrics.boundingRect(rectCaptionF, flags, strCaption);
    QRectF rectBtmBBF;
    if (bAdditionalText)
    {
        QFontMetricsF ftMetricsAdditional( m_fntAdditionalText );
        QString strTemplate;
        for ( int i=0 ; i<m_nAdditionalTextLineCount ; i++ )
        {
            if (!strTemplate.isEmpty()) strTemplate += "\n";
            strTemplate += "temp";
        }
        QRectF rect1LineBBF = ftMetricsAdditional.boundingRect( rectCaptionF, flags, "temp" );
        rectBtmBBF = ftMetricsAdditional.boundingRect( rectCaptionF, flags, m_strAdditionalText );
        if (rectBtmBBF.height() > rect1LineBBF.height()*1.5)
        {
            rectBtmBBF = ftMetricsAdditional.boundingRect( rectCaptionF, flags, strTemplate );
        }
        else
        {
            rectBtmBBF = rect1LineBBF;
        }
    }

    QRectF rectTopTextF = rectCaptionF;
    rectTopTextF.setTop( rectTopBBF.top() + (rectCaptionF.height()-rectTopBBF.height()-rectBtmBBF.height()) / 2.0 );
    rectTopTextF.setHeight( rectTopBBF.height() );

    if (!isEnabled())
    {
        m_clrText.setAlpha(230);
    }
    else
    {
        m_clrText.setAlpha(255);
    }

    p.setPen( m_clrText );
    p.setFont(m_fntCaption);
    p.drawText( rectTopTextF, flags, strCaption );

    if (bAdditionalText)
    {
        if (!isEnabled())
        {
            m_clrAdditionalText.setAlpha(230);
        }
        else
        {
            m_clrAdditionalText.setAlpha(255);
        }
        QRectF rectBtmTextF;
        rectBtmTextF = rectCaptionF;
        rectBtmTextF.setTop( rectTopTextF.bottom() );
        rectBtmTextF.setHeight(rectBtmBBF.height());
        p.setFont(m_fntAdditionalText);
        p.drawText( rectBtmTextF, flags, m_strAdditionalText );
    }

    p.restore();
}

void QStyleButton::drawFocusRect( QPainter& p, QRect& rcBody )
{
    QRect rcFocus;
    QColor clrFocus = m_clrActive;
    clrFocus.setAlpha(120);

    rcFocus = rcBody;
    rcFocus.adjust( 2, 2, -2, -2 );
    p.setBrush(Qt::NoBrush);
    QBrush brush( clrFocus );
    p.setPen( QPen(brush, 4.0) );
    p.drawRoundedRect( rcFocus, BORDER_ROUND, BORDER_ROUND );

    rcFocus = rcBody;
    rcFocus.adjust( 1, 1, -1, -1 );
    p.setPen( QPen(brush, 2.0) );
    p.drawRoundedRect( rcFocus, BORDER_ROUND, BORDER_ROUND );

    rcFocus = rcBody;
    rcFocus.adjust( 1, 1, -1, -1 );
    p.setPen( QPen(brush, 1.0) );
    p.drawRoundedRect( rcFocus, BORDER_ROUND, BORDER_ROUND );
}

bool QStyleButton::eventFilter(QObject *obj, QEvent *evt)
{
    if (isEnabled())
    {
        switch (evt->type())
        {
        case QEvent::HoverEnter:
            m_bIsHovered = true;
            update();
            break;
        case QEvent::HoverLeave:
            m_bIsHovered = false;
            update();
            break;
        case QEvent::FocusIn:
            m_bIsFocused = true;
            update();
            break;
        case QEvent::FocusOut:
            m_bIsFocused = false;
            update();
            break;
        default:
            break;
        }
    }
    return QPushButton::eventFilter(obj, evt);
}

void QStyleButton::mousePressEvent(QMouseEvent *e)
{
    m_bIsSelected = true;
    update();

    QPushButton::mousePressEvent(e);
}

void QStyleButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_bIsSelected = false;
    update();

    QPushButton::mouseReleaseEvent(e);
}

void QStyleButton::setMargin(int left, int top, int right, int bottom)
{
    m_nMarginLeft = left;
    m_nMarginTop = top;
    m_nMarginRight = right;
    m_nMarginBottom = bottom;
}

void QStyleButton::setColor(const QColor &clrNormal, const QColor &clrBorder, const QColor &clrActive, const QColor &clrText )
{
    m_clrNormal = clrNormal;
    m_clrBorder = clrBorder;
    m_clrActive = clrActive;
    m_clrText = clrText;
}

void QStyleButton::setAdditionalText( QString strText, int nLineCount, const QColor& clrText/*=QColor(40, 40, 40)*/ )
{
    m_strAdditionalText = strText;
    m_nAdditionalTextLineCount = nLineCount;
    m_clrAdditionalText = clrText;
}

void QStyleButton::setCaptionFontHeight(int pixelHeight)
{
    m_fntCaption.setPixelSize( pixelHeight );
    m_fntAdditionalText.setPixelSize(m_fntCaption.pixelSize() * 7 / 10);
}

void QStyleButton::setAlignment(Alignment align)
{
    m_TextAlignment = align;
}

