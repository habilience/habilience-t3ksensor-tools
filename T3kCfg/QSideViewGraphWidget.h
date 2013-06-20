#ifndef QSIDEVIEWGRAPHWIDGET_H
#define QSIDEVIEWGRAPHWIDGET_H

#include <QWidget>

class QSideViewGraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSideViewGraphWidget(QWidget *parent = 0);
    virtual ~QSideViewGraphWidget();

    void SetData( const uchar* pData, const int nCnt );
    void UpdateGraph();
    void ClearData() { memset( m_pData, 0, sizeof(uchar)*m_nCnt ); }

protected:
    void OnDraw( QPainter* pDC );

    virtual void paintEvent(QPaintEvent *evt);

protected:
    uchar*	m_pData;
    int		m_nCnt;
//    CBitmap*	m_pBitmap;
//    CDC			m_MemDC;
signals:

public slots:

};

#endif // QSIDEVIEWGRAPHWIDGET_H
