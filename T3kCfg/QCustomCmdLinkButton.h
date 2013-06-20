#ifndef QCUSTOMCMDLINKBUTTON_H
#define QCUSTOMCMDLINKBUTTON_H

#include <QCommandLinkButton>

class QCustomCmdLinkButton : public QCommandLinkButton
{
    Q_OBJECT
public:
    explicit QCustomCmdLinkButton(QWidget *parent = 0);

    void setMargin( int left, int top, int right, int bottom );
protected:
    virtual void paintEvent(QPaintEvent *evt);

protected:
    void init();
    qreal titleSize() const;
    bool usingVistaStyle() const;

    QFont titleFont() const;
    QFont descriptionFont() const;

    QRect titleRect() const;
    QRect descriptionRect() const;

    int textOffset() const;
    int descriptionOffset() const;
    int descriptionHeight(int width) const;
    QColor mergedColors(const QColor &a, const QColor &b, int value) const;

    int topMargin() const { return nTopMargin; }
    int leftMargin() const { return nLeftMargin; }
    int rightMargin() const { return nRightMargin; }
    int bottomMargin() const { return nBottomMargin; }

    int nLeftMargin;
    int nTopMargin;
    int nRightMargin;
    int nBottomMargin;

    QColor currentColor;
};

#endif // QCUSTOMCMDLINKBUTTON_H
