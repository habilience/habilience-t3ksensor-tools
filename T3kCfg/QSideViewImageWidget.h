#ifndef QSIDEVIEWIMAGEWIDGET_H
#define QSIDEVIEWIMAGEWIDGET_H

#include <QLabel>

class QSideViewImageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit QSideViewImageWidget(QWidget *parent = 0);
    virtual ~QSideViewImageWidget();

    void SetModel( QString strModel ) { m_strModelName = strModel; }
    void SetSimpleDetection( bool bSimple ) { m_bSimpleDetection = bSimple; }
    void SetNoCam( bool bNoCam ) { m_bNoCam = bNoCam; }

    void SetAutoDetection( int nAutoLine ) { m_nAutoLine = nAutoLine; }

    void SetImage( QImage* bmp );
    void SetProgress( int nPos );

protected:
    virtual void paintEvent(QPaintEvent *evt);

protected:
    int			m_nOrignImageHeight;
    QImage*     m_bmpImage;
    int			m_nProgress;

    QString     m_strModelName;
    bool        m_bSimpleDetection;
    bool        m_bNoCam;
    int         m_nAutoLine;

signals:

public slots:

};

#endif // QSIDEVIEWIMAGEWIDGET_H
