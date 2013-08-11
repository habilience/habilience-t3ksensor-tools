#ifndef QDETECTIONGRAPHWIDGET_H
#define QDETECTIONGRAPHWIDGET_H

#include <QWidget>
#include "QT3kDeviceEventHandler.h"

class QGestureEvent;
class QPinchGesture;
class QDetectionGraphWidget : public QWidget
{
    Q_OBJECT
protected:
    struct CrackInfo {
        int nSharpWidth;
        float fCrackThresholdError;
        float fCrackThresholdWarning;
        float fLightThresholdError;
        float fLightThresholdWarning;
        bool bDisplay;
    };

    CrackInfo   m_CrackInfo;

    unsigned char*  m_pITD;
    unsigned char*  m_pIRD;
    float*          m_pIRDF;
    bool            m_bUpdateIRDF;
    int             m_nIRD;

    RangeI*         m_pDTC;

    int             m_nNumberOfDTC;
    int             m_nRangeLeft;
    int             m_nRangeRight;

    int             m_nThreshold;

    bool            m_bUpdateGraph;

    int             m_nScrollPosX;
    float           m_fZoomRatioX;
    int             m_nScrollRange;
    bool            m_bShowScrollBar;
    int             m_TimerAutoHideScrollBar;

    int             m_nPrevDownX;
    int             m_nOldScrollPosX;

    QRect           m_rcScrollThumb;
    bool            m_bControlScrollBar;

    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *evt);
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseReleaseEvent(QMouseEvent *evt);
    virtual void wheelEvent(QWheelEvent *evt);
    virtual void timerEvent(QTimerEvent *evt);

    void drawIRD( QPainter& p, QRect rcBody );
    void drawITD( QPainter& p, QRect rcBody );
    void drawDTC( QPainter& p, QRect rcBody );
    void drawRange( QPainter& p, QRect rcBody );
    void drawGrid( QPainter& p, QRect rcBody );

    void drawScrollBar( QPainter& p, QRect rcBody );

    void updateGraph( int nStart, int nEnd );

    void zoomTo( int nScreenX, float fZoom );

public:
    explicit QDetectionGraphWidget(QWidget *parent = 0);
    ~QDetectionGraphWidget();

    bool isSetUpdateGraph() { return m_bUpdateGraph; }
    void setUpdateGraph( bool bSet ) { m_bUpdateGraph = bSet; }

    void setGraphData( int nIRD, unsigned char* pIRD, unsigned char* pITD );
    void setThreshold( int nThreshold );
    void setDetectionRange( int nLeft, int nRight );
    void updateDetectionData( RangeI* pDTC, int nNumberOfDTC );

    void redrawGraph( int nStart, int nEnd, bool bForce );

    void setDisplayCrackInfo( bool bDisplay );

    void zoomIn();
    void zoomOut();
    void resetZoom();
    
signals:
    
public slots:
    
};

#endif // QDETECTIONGRAPHWIDGET_H
