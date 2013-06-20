#ifndef QUNDERLINELABEL_H
#define QUNDERLINELABEL_H

#include <QLabel>

class QUnderlineLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY( QString m_strIconPathName READ GetIconImage WRITE SetIconImage )
public:
    explicit QUnderlineLabel(QWidget *parent = 0);

    void SetIconImage( const QString& strPathName );
    void SetIconImage( QPixmap& pxIcon );

protected:
    QString GetIconImage() const { return m_strIconPathName; }
    virtual void paintEvent(QPaintEvent *event);

protected:
    QPixmap                 m_pxIcon;
    QString                 m_strIconPathName;
    int                     m_nMargin;
signals:

public slots:

};

#endif // QUNDERLINELABEL_H
