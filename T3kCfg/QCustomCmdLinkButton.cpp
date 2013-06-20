#include "QCustomCmdLinkButton.h"

#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>

QCustomCmdLinkButton::QCustomCmdLinkButton(QWidget *parent) :
    QCommandLinkButton(parent)
{
    nTopMargin = 10;
    nLeftMargin = 7;
    nRightMargin = 4;
    nBottomMargin = 10;
}

void QCustomCmdLinkButton::setMargin( int left, int top, int right, int bottom )
{
    nLeftMargin = left;
    nTopMargin = top;
    nRightMargin = right;
    nBottomMargin = bottom;
}

QColor QCustomCmdLinkButton::mergedColors(const QColor &a, const QColor &b, int value = 50) const
{
    Q_ASSERT(value >= 0);
    Q_ASSERT(value <= 255);
    QColor tmp = a;
    tmp.setRed((tmp.red() * value) / 255 + (b.red() * (255 - value)) / 255);
    tmp.setGreen((tmp.green() * value) / 255 + (b.green() * (255 - value)) / 255);
    tmp.setBlue((tmp.blue() * value) / 255 + (b.blue() * (255 - value)) / 255);
    return tmp;
}

QFont QCustomCmdLinkButton::titleFont() const
{
    QFont ft( QApplication::font() );
    ft.setWeight( QFont::Bold );
#if defined(Q_OS_MAC)
    ft.setPointSizeF(13.0);
#elif defined(Q_OS_X11)
    ft.setPointSizeF(11.0);
#elif defined(Q_OS_WIN)
    ft.setPointSizeF(10.0);
#endif
    //ft.resolve( QFont::WeightResolved );
    return ft;
    int resolve_mask = ft.resolve();
    QFont modifiedFont = font().resolve( ft );
    //modifiedFont.detach();
    modifiedFont.resolve( resolve_mask );
    return modifiedFont;
}

QFont QCustomCmdLinkButton::descriptionFont() const
{
    QFont ft( QApplication::font() );
#if defined(Q_OS_MAC)
    ft.setPointSizeF(12.0);
    ft.setWeight( QFont::DemiBold );
#elif defined(Q_OS_X11)
    ft.setPointSizeF(10.0);
    ft.setWeight( QFont::Normal );
#elif defined(Q_OS_WIN)
    ft.setPointSizeF(9.0);
    ft.setWeight( QFont::Normal );
#endif
    return ft;
    int resolve_mask = ft.resolve();
    QFont modifiedFont = font().resolve(ft);
    //modifiedFont.detach();
    modifiedFont.resolve(resolve_mask);
    return modifiedFont;
}

QRect QCustomCmdLinkButton::titleRect() const
{
    QRect r = rect().adjusted(textOffset(), topMargin(), -rightMargin(), 0);
    if (description().isEmpty())
    {
        QFontMetrics fm(titleFont());
        r.setTop( r.top() + qMax(0, (icon().actualSize(iconSize()).height() - fm.height()) / 2) );
    }

    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        r.setLeft( leftMargin() + (isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal) : 0) );
        r.setRight( r.right()-textOffset() );
    }

    return r;
}

QRect QCustomCmdLinkButton::descriptionRect() const
{
    QRect r = rect().adjusted(textOffset(), descriptionOffset(), -rightMargin(), -bottomMargin());
    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        r.setLeft( leftMargin() + (isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal) : 0) );
        r.setRight( r.right()-textOffset() );
    }

    return r;
}

int QCustomCmdLinkButton::textOffset() const
{
    return icon().actualSize(iconSize()).width() + leftMargin() + 6;
}

int QCustomCmdLinkButton::descriptionOffset() const
{
    QFontMetrics fm(titleFont());
    return topMargin() + fm.height() + 5; // 5 : Title, Description Margin
}

void QCustomCmdLinkButton::paintEvent(QPaintEvent */*evt*/)
{
    QStylePainter p(this);
    p.save();

    QStyleOptionButton option;
    initStyleOption(&option);

    //Enable command link appearance on Vista
    option.features |= QStyleOptionButton::CommandLinkButton;
    option.text = QString();
    option.icon = QIcon(); //we draw this ourselves
    QSize pixmapSize = icon().actualSize(iconSize());

    int vOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftVertical) : 0;
    int hOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal) : 0;

    //Draw icon
    p.drawControl(QStyle::CE_PushButton, option);
    if (!icon().isNull())
    {
        p.drawPixmap( QApplication::layoutDirection() == Qt::RightToLeft ?
                      width()-textOffset() : leftMargin() + hOffset, topMargin() + vOffset,
        icon().pixmap(pixmapSize, isEnabled() ? QIcon::Normal : QIcon::Disabled,
                                  isChecked() ? QIcon::On : QIcon::Off));
    }

    //Draw title
    QColor textColor = palette().buttonText().color();
    currentColor = textColor;
    if (isEnabled()) {
        //currentColor = textColor = palette().text().color();
        if (underMouse() && !isDown())
            textColor = QColor(55, 100, 160);//QColor(7, 64, 229);
        else
            textColor = QColor(80, 80, 80);
        //A simple text color transition
        //currentColor = textColor;//mergedColors(textColor, currentColor, 60);
        option.palette.setColor(QPalette::ButtonText, textColor);
    }

    int textflags = Qt::TextShowMnemonic;
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
        textflags |= Qt::TextHideMnemonic;

    p.setFont(titleFont());
    p.drawItemText(titleRect().translated(hOffset, vOffset),
                    textflags, option.palette, isEnabled(), text(), QPalette::ButtonText);

    //option.palette.setColor(QPalette::ButtonText, currentColor);
    //Draw description
    textflags |= Qt::TextWordWrap | Qt::ElideRight;
    p.setFont(descriptionFont());
    p.drawItemText(descriptionRect().translated(hOffset, vOffset), textflags,
                    option.palette, isEnabled(), description(), QPalette::ButtonText);
    p.restore();
}
