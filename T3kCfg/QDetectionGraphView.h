#ifndef QDETECTIONGRAPHVIEW_H
#define QDETECTIONGRAPHVIEW_H

#include <QScrollArea>
#include <QTimer>
#include "stdInclude.h"


class QDetectionGraphView : public QScrollArea
{
    Q_OBJECT
public:
    explicit QDetectionGraphView(QWidget *parent = 0);
    virtual ~QDetectionGraphView();

    void SetGraphData( int nIRD, uchar* pIRD, uchar* pITD );
    void UpdateDetectionData( QPoint* pObjs, int nNumberOfDTC );
    void RedrawGraph();
    void SetThreshold( int nThreshold ) { m_nThreshold = nThreshold; }
    void SetDetectionRange( int nLeft, int nRight );
    void ClearGraph();

    void SetCaption( QString& strLeft, QString& strRight );

    void ZoomIn();
    void ZoomOut();
    void ResetZoom();

    void DisplayAutoOffset( int nOffset );

    virtual void wheelEvent(QWheelEvent *evt);

    void OnTimer(int nTimerId);

protected:
    float CalcValidZoom( float fZoom );

    void SetZoomRatio( float fZoomW, float fZoomH, bool bInvalidate=true );
    void MoveTo( QPoint ptGraph );
    void ZoomTo( QPoint ptGraph, float fZoomW, float fZoomH );

    void UpdateGraph();

    void DrawGraph( QPainter& dc );
    void DrawITD( QPainter& dc, const QRectF& rectGraph );
    void DrawIRD( QPainter& dc, const QRectF& rectGraph );
    void DrawDTC( QPainter& dc, const QRectF& rectGraph, QPoint* pObj );
    void DrawGrid( QPainter& dc, const QRectF& rectBody );
    void DrawRange( QPainter& dc, const QRectF& rectGraph );

    void GetScrollBarDimension( int& nSWW, int &nSWH );

    void MousePan( QPoint& point );

    QPoint GetDeviceScrollPosition();

    virtual void mouseMoveEvent(QMouseEvent *evt);
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseReleaseEvent(QMouseEvent *evt);
    virtual void paintEvent(QPaintEvent *evt);

    virtual void resizeEvent(QResizeEvent *evt);

protected:
    uchar*              m_pITD;
    uchar*              m_pIRD;
    int                 m_nIRD;

    int                 m_nNumberOfDTC;

    QString             m_strCaptionLeft;
    QString             m_strCaptionRight;

    QPoint              m_ptViewLastMouse;

    int                 m_nRangeLeft;
    int                 m_nRangeRight;

    int                 m_nThreshold;

    QRectF              m_rectGraph;

    float               m_fZoomMin;
    float               m_fZoomMax;

    int                 m_nBaseWidth;
    int                 m_nBaseHeight;
    float               m_fZoomRatioW;
    float               m_fZoomRatioH;

    QPixmap*		m_pImageCanvas;
    QPixmap*		m_pImageGraph;
    QPixmap*		m_pImageDetection;

    bool                m_bUpdateGraph;
    bool                m_bStopUpdate;
    bool                m_bUpdateDetection;

    int                 m_nAutoOffset;

signals:

public slots:

private slots:
    void OnVScrollActionTrig( int nAction );
    void OnHScrollActionTrig( int nAction );

};

inline float QDetectionGraphView::CalcValidZoom( float fZoom )
{
    if( fZoom == 1.f ) return fZoom;
    if( m_fZoomMax < fZoom ) return m_fZoomMax;
    if( m_fZoomMin > fZoom ) return m_fZoomMin;
    return fZoom;
}


#endif // QDETECTIONGRAPHVIEW_H
