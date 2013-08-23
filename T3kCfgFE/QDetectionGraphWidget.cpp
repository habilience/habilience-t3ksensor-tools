#include "QDetectionGraphWidget.h"
#include "QInitDataIni.h"
#include <QApplication>

#include <QPainter>
#include <math.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QPinchGesture>

#define MAX_ZOOM        (10.f)
#define MIN_ZOOM        (1.f)
#define ZOOM_STEP       (1.5f)

QDetectionGraphWidget::QDetectionGraphWidget(QWidget *parent) :
    QWidget(parent)
{
    m_nIRD = 0;
    m_pIRD = NULL;
    m_pIRDF = NULL;
    m_pITD = NULL;
    m_pDTC = NULL;

    m_nScrollPosX = 0;
    m_fZoomRatioX = 1.0f;
    m_nScrollRange = 0;

    m_bUpdateIRDF = false;
    m_bUpdateGraph = false;

    m_nNumberOfDTC = 0;
    m_nRangeLeft = m_nRangeRight = 0;
    m_nThreshold = 0;

    m_bShowScrollBar = false;
    m_TimerAutoHideScrollBar = 0;

    memset( &m_CrackInfo, 0, sizeof(CrackInfo) );
    m_CrackInfo.nSharpWidth = QInitDataIni::instance()->getDTCGraphSharpWidth();
    m_CrackInfo.fCrackThresholdError = QInitDataIni::instance()->getDTCGraphCrackThresholdError();
    m_CrackInfo.fCrackThresholdWarning = QInitDataIni::instance()->getDTCGraphCrackThresholdWarning();
    m_CrackInfo.fLightThresholdError = QInitDataIni::instance()->getDTCGraphLightThresholdError();
    m_CrackInfo.fLightThresholdWarning = QInitDataIni::instance()->getDTCGraphLightThresholdWarning();
    m_CrackInfo.bDisplay = true;
}

QDetectionGraphWidget::~QDetectionGraphWidget()
{
    if (m_pITD)
        delete[] m_pITD;
    m_pITD = NULL;
    if (m_pIRD)
        delete[] m_pIRD;
    m_pIRD = NULL;
    if (m_pIRDF)
        delete[] m_pIRDF;
    m_pIRDF = NULL;
}

void QDetectionGraphWidget::wheelEvent(QWheelEvent *evt)
{
    if (evt->modifiers() & Qt::ControlModifier)
    {
        float fZoom = m_fZoomRatioX;
        if (evt->delta() > 0)
        {
            fZoom *= ZOOM_STEP;
            zoomTo( evt->pos().x(), fZoom );
        }
        else
        {
            fZoom /= ZOOM_STEP;
            zoomTo( evt->pos().x(), fZoom );
        }
    }
}

void QDetectionGraphWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);

    p.save();

    //p.setPen( QPen(QColor(160, 160, 160), 1) );
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(Qt::white));
    p.drawRect(rcBody);

    //rcBody.adjust( 2, 1, -2, -1 );

    p.setRenderHint(QPainter::Antialiasing);

    drawIRD( p, rcBody );
    drawITD( p, rcBody );

    p.setRenderHint(QPainter::Antialiasing, false);

    drawRange( p, rcBody );
    drawGrid( p, rcBody );

    drawDTC( p, rcBody );

    p.setPen( QPen(QColor(160, 160, 160), 1) );
    p.setBrush(Qt::NoBrush);
    p.drawRect(rcBody);

    if (m_fZoomRatioX != 1.f)
    {
        char szZoom[16];
        snprintf( szZoom, 16, "%.1fX", m_fZoomRatioX );
        QString strZoomText = szZoom;
        QRect rcZoomText( rcBody.right()-50, rcBody.top()+5, 45, 30 );
        int flags = Qt::AlignRight|Qt::AlignTop|Qt::TextSingleLine;
        p.setPen( QColor(136, 0, 21) );
        p.drawText( rcZoomText, flags, strZoomText );
    }

    drawScrollBar(p, rcBody);

    p.restore();
}

void QDetectionGraphWidget::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerAutoHideScrollBar)
    {
        killTimer( m_TimerAutoHideScrollBar );
        m_TimerAutoHideScrollBar = 0;
        m_bShowScrollBar = false;
        update();
    }
}

void QDetectionGraphWidget::drawIRD( QPainter& p, QRect rcBody )
{
    if ( !m_pIRD )
        return;

    QRectF rectView = rcBody;

    float fScaleX = rectView.width() / m_nIRD;
    float fScaleY = rectView.height() / 0xff;

    fScaleX *= m_fZoomRatioX;

    QPointF pointX;
    QPainterPath pointPath;

    pointX = rectView.bottomLeft();
    pointPath.moveTo( pointX );

    int nMinI = int( (-fScaleX - rectView.left()) / fScaleX ) - 1;
    if( nMinI < 0 ) nMinI = 0;

    int nOldIRD = m_pIRD[0];
    for ( int i=0 ; i<m_nIRD ; i++ )
    {
        pointX.setX( i*fScaleX + rectView.left() - m_nScrollPosX );
        if (nOldIRD == 0 && m_pIRD[i] != 0)
        {
            pointX.setY( rectView.bottom() );
            pointPath.lineTo( pointX );
        }
        pointX.setY( rectView.top() + (rectView.height() - fScaleY * m_pIRD[i]) );

        nOldIRD = m_pIRD[i];

        if ( pointX.x() < rectView.left() )
            continue;
        if ( pointX.x() > rectView.right() )
            continue;

        pointPath.lineTo( pointX );

        if (i < m_nIRD-1 && (m_pIRD[i+1] == 0 && m_pIRD[i] != 0) )
        {
            pointX.setY( rectView.bottom() );
            pointPath.lineTo( pointX );
        }
    }
    pointX.setY(rectView.bottom());
    pointPath.lineTo( pointX );

    QPainterPath gpRLine;
    bool bGpRLineMoveTo = true;

    if ( false )//m_CrackInfo.bDisplay )
    {
        int nLeft = m_nRangeLeft * m_nIRD / 0xffff;
        int nRight = m_nRangeRight * m_nIRD / 0xffff;

        const int cnSharpShift = m_CrackInfo.nSharpWidth / 2;

        QRectF rcR;
        rcR.setY(rectView.y());
        rcR.setHeight(rectView.height());

        float nR0s = -1.f, nR0e;
        QPainterPath gpR0;
        float nR1s = -1.f, nR1e;
        QPainterPath gpR1;
        float nR2s = -1.f, nR2e;
        QPainterPath gpR2;
        float fCrackThrMid = (m_CrackInfo.fCrackThresholdError + m_CrackInfo.fCrackThresholdWarning) / 2.f;
        for ( int ni = nMinI; ni < m_nIRD-2; ni++ )
        {
            if ( ni <= nLeft + m_CrackInfo.nSharpWidth + 1 )
                continue;
            if ( ni >= nRight )
                break;

            float nMin, nMax;
            nMin = nMax = m_pIRDF[ni];
            for ( int nj = 0; nj < m_CrackInfo.nSharpWidth; nj++ )
            {
                float nIRD = m_pIRDF[ni - nj - 1];
                if ( nMin > nIRD )
                    nMin = nIRD;
                if ( nMax < nIRD )
                    nMax = nIRD;
            }

            float nDef = nMax - nMin;

            float fLvl = sqrtf(nDef / 255.f);
            if ( fLvl > m_CrackInfo.fCrackThresholdError )
            {
                if ( nR0s >= 0 )
                {
                    float nS = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    if ( nR0e < nS )
                    {
                        rcR.setX(nR0s);
                        rcR.setWidth(nR0e - nR0s);
                        gpR0.addRect(rcR);

                        nR0s = nS;
                        nR0e = ni * fScaleX + rectView.x();
                    }
                    else
                    {
                        nR0e = ni * fScaleX + rectView.x();
                    }
                }
                else
                {
                    nR0s = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    nR0e = ni * fScaleX + rectView.x();
                }
            }
            if ( fLvl > fCrackThrMid )
            {
                if ( nR1s >= 0 )
                {
                    float nS = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    if ( nR1e < nS )
                    {
                        rcR.setX(nR1s);
                        rcR.setWidth(nR1e - nR1s);
                        gpR1.addRect(rcR);

                        nR1s = nS;
                        nR1e = ni * fScaleX + rectView.x();
                    }
                    else
                    {
                        nR1e = ni * fScaleX + rectView.x();
                    }
                }
                else
                {
                    nR1s = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    nR1e = ni * fScaleX + rectView.x();
                }
            }
            if ( fLvl > m_CrackInfo.fCrackThresholdWarning )
            {
                if ( nR2s >= 0 )
                {
                    float nS = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    if ( nR2e < nS )
                    {
                        rcR.setX(nR2s);
                        rcR.setWidth(nR2e - nR2s);
                        gpR2.addRect(rcR);

                        nR2s = nS;
                        nR2e = ni * fScaleX + rectView.x();
                    }
                    else
                    {
                        nR2e = ni * fScaleX + rectView.x();
                    }
                }
                else
                {
                    nR2s = (ni - m_CrackInfo.nSharpWidth) * fScaleX + rectView.x();
                    nR2e = ni * fScaleX + rectView.x();
                }
            }

            pointX.setX( (ni - cnSharpShift) * fScaleX + rectView.x() );
            pointX.setY( rectView.bottom() - (fLvl * rectView.height()) );
            if (bGpRLineMoveTo)
            {
                gpRLine.moveTo(pointX);
                bGpRLineMoveTo = false;
            }
            else
            {
                gpRLine.lineTo(pointX);
            }
        }
        if ( nR0s >= 0 )
        {
            rcR.setX(nR0s);
            rcR.setWidth(nR0e - nR0s);
            gpR0.addRect(rcR);
        }
        if ( nR1s >= 0 )
        {
            rcR.setX(nR1s);
            rcR.setWidth(nR1e - nR1s);
            gpR1.addRect(rcR);
        }
        if ( nR2s >= 0 )
        {
            rcR.setX(nR2s);
            rcR.setWidth(nR2e - nR2s);
            gpR2.addRect(rcR);
        }

        p.fillPath( gpR2, QBrush(QColor(255, 220, 220)) );
        p.fillPath( gpR1, QBrush(QColor(255, 160, 160)) );
        p.fillPath( gpR0, QBrush(QColor(255, 110, 110)) );
    }

    p.setBrush( QBrush(QColor(160, 160, 160)) );
    p.setPen( QColor(100, 100, 100) );
    p.drawPath( pointPath );

    if ( m_CrackInfo.bDisplay &&
         ( (QApplication::keyboardModifiers() & (Qt::AltModifier|Qt::ControlModifier)) == (Qt::AltModifier|Qt::ControlModifier) ) )
    {
        p.setPen( QColor(200, 0, 0) );
        p.setBrush(Qt::NoBrush);
        p.drawPath(gpRLine);
    }
}

void QDetectionGraphWidget::drawITD( QPainter& p, QRect rcBody )
{
    if( !m_pITD )
        return;

    QRectF rectView = rcBody;

    float fScaleX = rectView.width() / m_nIRD;
    float fScaleY = rectView.height() / 0xff;

    fScaleX *= m_fZoomRatioX;

    QPointF pointX;
    QPainterPath pointPath;

    pointX = rectView.bottomLeft();
    pointPath.moveTo( pointX );

    int nOldITD = m_pITD[0];
    for( int i=0 ; i<m_nIRD ; i++ )
    {
        int nITD = m_pITD[i] * m_nThreshold / 0xff;

        pointX.setX(i*fScaleX + rectView.left() - m_nScrollPosX);
        if (nOldITD == 0 && m_pITD[i] != 0)
        {
            pointX.setY( rectView.bottom() );
            pointPath.lineTo( pointX );
        }
        pointX.setY( rectView.top() + (rectView.height() - fScaleY*nITD));

        nOldITD = m_pITD[i];

        if (pointX.x() < rectView.left())
            continue;
        if (pointX.x() > rectView.right())
            continue;

        pointPath.lineTo( pointX );

        if (i < m_nIRD-1 && (m_pITD[i+1] == 0 && m_pITD[i] != 0) )
        {
            pointX.setY( rectView.bottom() );
            pointPath.lineTo( pointX );
        }
    }

    pointX.setY(rectView.bottom());
    pointPath.lineTo( pointX );

    p.setBrush( QBrush(QColor(255, 255, 255, 150)) );
    p.setPen( QColor(0, 0, 0, 50) );
    p.drawPath( pointPath );
}

void QDetectionGraphWidget::drawDTC(QPainter &p, QRect rcBody)
{
    if (m_nNumberOfDTC == 0 || !m_pDTC) return;

    QPen linePen( QColor(200, 100, 0, 120) );
    QBrush brush( QColor(200, 100, 0, 100) );

    QRectF rectView = rcBody;
    float fScaleX = rectView.width() / m_nIRD;

    fScaleX *= m_fZoomRatioX;

    QRectF rectDetect;
    rectDetect.setY(rectView.y());
    rectDetect.setHeight(rectView.height());

    for (int i=0 ; i<m_nNumberOfDTC ; i++)
    {
        if( m_pDTC[i].start != -1 )
        {
            rectDetect.setX(m_pDTC[i].start*fScaleX + rectView.x() - m_nScrollPosX);
            float fDW = (m_pDTC[i].end*fScaleX - m_pDTC[i].start*fScaleX);
            fDW < 1.f ? fDW = 1.f : 0;
            rectDetect.setWidth(fDW);

            /*if (rectDetect.right() < rectView.left())
                continue;
            if (rectDetect.left() > rectView.right())
                continue;*/

            p.setBrush(brush);
            p.setPen(Qt::NoPen);
            p.drawRect( rectDetect );
            rectDetect.adjust( 0, -2.0f, 0, 2.0f );
            p.setBrush(Qt::NoBrush);
            p.setPen(linePen);
            p.drawRect( rectDetect );
        }
        else
        {
            break;
        }
    }
}

void QDetectionGraphWidget::drawRange( QPainter& p, QRect rcBody )
{
    p.setRenderHint(QPainter::Antialiasing, false );

    QPen pen( QBrush(QColor(240, 80, 80)), 1.0f, Qt::DashDotLine );

    int nLeft = m_nRangeLeft * m_nIRD / 0xffff;
    int nRight = m_nRangeRight * m_nIRD / 0xffff;

    float fScaleX = (float)rcBody.width() / m_nIRD;
    fScaleX *= m_fZoomRatioX;
    float fLeft = rcBody.left() - m_nScrollPosX + fScaleX * (nLeft);
    float fRight = rcBody.left() - m_nScrollPosX + fScaleX * (nRight-1);

    p.setPen( pen );
    if ( (m_nRangeLeft > 0) && (fLeft >= (float)rcBody.left()) )
    {
        p.drawLine( QPointF(fLeft, rcBody.top()), QPointF(fLeft, rcBody.bottom()) );
    }
    if ( (m_nRangeRight < 0xffff) && (fRight <= (float)rcBody.right()) )
    {
        p.drawLine( QPointF(fRight, rcBody.top()), QPointF(fRight, rcBody.bottom()) );
    }
}

void QDetectionGraphWidget::drawGrid( QPainter& p, QRect rcBody )
{
    p.setRenderHint(QPainter::Antialiasing, false );

    QPen gridPen(QColor(0xF5,0xF5,0xF5));
    QPen shadowPen(Qt::darkGray);
    gridPen.setStyle(Qt::DashLine);
    shadowPen.setStyle(Qt::DashLine);

    float fScaleY = rcBody.height() / 10.f;
    float fGridY = 0.f;
    p.setPen( shadowPen );
    for (int i=1 ; i<10 ; i++)
    {
        fGridY = fScaleY * i + rcBody.top() + 1.f;
        p.drawLine( rcBody.left(), fGridY, rcBody.right(), fGridY );
    }

    p.setPen( gridPen );
    for (int i=1 ; i<10 ; i++)
    {
        fGridY = fScaleY * i + rcBody.top();
        p.drawLine( rcBody.left(), fGridY, rcBody.right(), fGridY );
    }
    if (m_CrackInfo.bDisplay && (QApplication::keyboardModifiers() & (Qt::AltModifier|Qt::ControlModifier)) == (Qt::AltModifier|Qt::ControlModifier) )
    {
        QPen penRThErr( QBrush(QColor(255, 0, 0)), 3.f );
        float fV = rcBody.bottom() - (m_CrackInfo.fCrackThresholdError * rcBody.height());
        p.setPen(penRThErr);
        p.drawLine( rcBody.left(), fV, rcBody.right(), fV );
        QPen penRThWarn( QBrush(QColor(255, 0, 0, 100)), 3.f );
        fV = rcBody.bottom() - (m_CrackInfo.fCrackThresholdWarning * rcBody.height());
        p.setPen(penRThWarn);
        p.drawLine( rcBody.left(), fV, rcBody.right(), fV );

        QPen penLThErr( QBrush(QColor(0, 180, 0)), 3.f );
        fV = rcBody.bottom() - (m_CrackInfo.fLightThresholdError / 100.f * rcBody.height());
        p.setPen(penLThErr);
        p.drawLine( rcBody.left(), fV, rcBody.right(), fV );
        QPen penLThWarn( QBrush(QColor(0, 180, 0, 100)), 3.f );
        fV = rcBody.bottom() - (m_CrackInfo.fLightThresholdWarning / 100.f * rcBody.height());
        p.setPen(penLThWarn);
        p.drawLine( rcBody.left(), fV, rcBody.right(), fV );
    }
}

void QDetectionGraphWidget::drawScrollBar( QPainter& p, QRect rcBody )
{
    if (m_fZoomRatioX == 1.f || !m_bShowScrollBar)
        return;

    p.setRenderHint(QPainter::Antialiasing);

    int nScrollHeight = rcBody.height() / 40;
    if (nScrollHeight > 10) nScrollHeight = 10;
    if (nScrollHeight < 4) nScrollHeight = 4;
    QRectF rcScrollBody = rcBody;

    rcScrollBody.adjust( 5, 0, -5, 0 );

    float fBaseWidth = rcScrollBody.width();
    float fThumbWidth = fBaseWidth / m_fZoomRatioX;

    float fScrollRange = fBaseWidth - fThumbWidth;
    m_nScrollRange = (int)(rcBody.width() * m_fZoomRatioX + 0.5f);
    m_nScrollRange = m_nScrollRange - rcBody.width();
    float fScrollX = (float)m_nScrollPosX / m_nScrollRange;

    rcScrollBody.setTop( rcBody.bottom() - nScrollHeight - 5 );
    rcScrollBody.setHeight( nScrollHeight );

    rcScrollBody.setLeft( rcScrollBody.left() + fScrollRange * fScrollX );
    rcScrollBody.setWidth( fThumbWidth );

    p.setPen(Qt::NoPen);
    if (!m_bControlScrollBar)
        p.setBrush(QColor(80, 80, 80, 80));
    else
        p.setBrush(QColor(80, 80, 80, 120));

    m_rcScrollThumb.setRect((int)rcScrollBody.left(), (int)rcScrollBody.top(), (int)rcScrollBody.width(), (int)rcScrollBody.height());
    m_rcScrollThumb.adjust( -2, -5, 2, 5 );

    p.drawRect( rcScrollBody );
}

void QDetectionGraphWidget::mouseMoveEvent(QMouseEvent *evt)
{
    //if (evt->button() == Qt::LeftButton)
    {
        int nCurX = evt->pos().x();
        if (m_bControlScrollBar)
            m_nScrollPosX = m_nOldScrollPosX + (int)(m_fZoomRatioX * (-m_nPrevDownX + nCurX));
        else
            m_nScrollPosX = m_nOldScrollPosX + (m_nPrevDownX - nCurX);
        if (m_nScrollPosX > m_nScrollRange)
            m_nScrollPosX = m_nScrollRange;
        if (m_nScrollPosX < 0)
            m_nScrollPosX = 0;
        update();
    }
}

void QDetectionGraphWidget::mousePressEvent(QMouseEvent *evt)
{
    if (m_fZoomRatioX == 1.f) return;

    if (evt->button() == Qt::LeftButton)
    {
        if (m_bShowScrollBar)
        {
            if (m_rcScrollThumb.contains(evt->pos()))
            {
                m_bControlScrollBar = true;
            }
            else
            {
                m_bControlScrollBar = false;
            }
        }

        m_bShowScrollBar = true;
        if (m_TimerAutoHideScrollBar)
        {
            killTimer(m_TimerAutoHideScrollBar);
            m_TimerAutoHideScrollBar = 0;
        }
        update();

        setMouseTracking(true);
        m_nOldScrollPosX = m_nScrollPosX;
        m_nPrevDownX = evt->pos().x();
    }
}

void QDetectionGraphWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    if (m_fZoomRatioX == 1.f) return;

    if (evt->button() == Qt::LeftButton)
    {
        setMouseTracking(false);
        m_nOldScrollPosX = 0;
        m_nPrevDownX = 0;

        if (m_TimerAutoHideScrollBar)
            killTimer(m_TimerAutoHideScrollBar);
        m_TimerAutoHideScrollBar = startTimer(2000);

        if (m_bControlScrollBar)
        {
            m_bControlScrollBar = false;
            update();
        }
    }
}

void QDetectionGraphWidget::redrawGraph(int nStart, int nEnd, bool bForce)
{
    if (!m_bUpdateGraph || bForce)
    {
        updateGraph( nStart, nEnd );
        m_bUpdateGraph = true;
    }
}

void QDetectionGraphWidget::updateGraph(int nStart, int nEnd)
{
    QRect rcBody(0, 0, width(), height());

    int nScrollX = 0;

    int nLeft = nStart * m_nIRD / 0xffff;
    int nRight = nEnd * m_nIRD / 0xffff;

    float fScaleX = (float)rcBody.width() / m_nIRD;
    float fLeft = rcBody.left() - nScrollX + fScaleX * (nLeft);
    float fRight = rcBody.left() - nScrollX + fScaleX * (nRight-1);

    QRect rcUpdate( (int)(fLeft+0.5f), rcBody.top(), (int)(fRight-fLeft+0.5f), rcBody.height() );
    rcUpdate.adjust( -2, 0, 2, 0 );

    if (rcBody.intersects(rcUpdate))
        update( rcUpdate );
}

void QDetectionGraphWidget::setDisplayCrackInfo( bool bDisplay )
{
    m_CrackInfo.bDisplay = bDisplay;
    update();
}

void QDetectionGraphWidget::setGraphData( int nIRD, unsigned char* pIRD, unsigned char* pITD )
{
    if (m_nIRD != nIRD)
    {
        m_nIRD = nIRD;
        if (m_pIRD) delete[] m_pIRD;
        m_pIRD = new unsigned char[m_nIRD];
        if (m_pIRDF) delete[] m_pIRDF;
        m_pIRDF = new float[m_nIRD];
        if (m_pITD) delete[] m_pITD;
        m_pITD = new unsigned char[m_nIRD];

        memset( m_pITD, 0, sizeof(unsigned char) * m_nIRD );
        memset( m_pIRD, 0, sizeof(unsigned char) * m_nIRD );
        memset( m_pIRDF, 0, sizeof(float) * m_nIRD );
    }

    if (pIRD)
    {
        memcpy( m_pIRD, pIRD, sizeof(unsigned char) * m_nIRD );

        if (m_pIRDF && m_nNumberOfDTC <= 0)
        {
            if (m_bUpdateIRDF)
            {
goto_InitIRDF:
                for (int i=0 ; i<m_nIRD ; i++)
                {
                    m_pIRDF[i] = (float)m_pIRD[i];
                }
            }
            else
            {
                int nDCnt = 0;
                for (int i=0 ; i<m_nIRD ; i++)
                {
                    if (qAbs(m_pIRDF[i] - (float)m_pIRD[i]) > 8.f)
                    {
                        nDCnt ++;
                    }
                    m_pIRDF[i] = m_pIRDF[i] * 0.995f + (float)m_pIRD[i] * 0.005f;
                }
                if (nDCnt > m_nIRD / 8)
                    goto goto_InitIRDF;
            }
        }
    }
    if (pITD)
        memcpy( m_pITD, pITD, sizeof(unsigned char) * m_nIRD );
}

void QDetectionGraphWidget::setThreshold( int nThreshold )
{
    m_nThreshold = nThreshold;
}

void QDetectionGraphWidget::setDetectionRange( int nLeft, int nRight )
{
    if (m_nRangeLeft != nLeft)
    {
        m_nRangeLeft = nLeft;
        m_bUpdateIRDF = true;
    }
    if (m_nRangeRight != nRight)
    {
        m_nRangeRight = nRight;
        m_bUpdateIRDF = true;
    }
}

void QDetectionGraphWidget::updateDetectionData( RangeI* pDTC, int nNumberOfDTC )
{
    bool bUpdate = false;
    if (m_nNumberOfDTC != nNumberOfDTC)
    {
        delete[] m_pDTC;
        m_pDTC = NULL;
        m_nNumberOfDTC = nNumberOfDTC;
        bUpdate = false;
        if (m_nNumberOfDTC > 0)
        {
            m_pDTC = new RangeI[m_nNumberOfDTC];
        }
    }

    if (m_pDTC)
        memcpy( m_pDTC, pDTC, sizeof(RangeI) * nNumberOfDTC );

    if (m_nNumberOfDTC != 0 || bUpdate)
    {
        update();
    }
}

inline float zoomLimit( float fZoom )
{
    if (fZoom > MAX_ZOOM)
        fZoom = MAX_ZOOM;
    if (fZoom < MIN_ZOOM)
        fZoom = MIN_ZOOM;
    return fZoom;
}

void QDetectionGraphWidget::zoomIn()
{
    float fZoom = m_fZoomRatioX;
    fZoom *= ZOOM_STEP;

    QRect rcBody(0, 0, width()-1, height()-1);
    int nCenterX = rcBody.center().x();

    zoomTo( nCenterX, fZoom );
}

void QDetectionGraphWidget::zoomOut()
{
    float fZoom = m_fZoomRatioX;
    fZoom /= ZOOM_STEP;

    QRect rcBody(0, 0, width()-1, height()-1);
    int nCenterX = rcBody.center().x();

    zoomTo( nCenterX, fZoom );
}

void QDetectionGraphWidget::resetZoom()
{
    m_fZoomRatioX = 1.f;
    m_nScrollPosX = 0;
    if (m_TimerAutoHideScrollBar)
        killTimer(m_TimerAutoHideScrollBar);
    m_TimerAutoHideScrollBar = 0;
    m_bShowScrollBar = false;
    update();
}

void QDetectionGraphWidget::zoomTo( int nScreenX, float fZoom )
{
    QRect rcBody(0, 0, width()-1, height()-1);

    float fCenterX = (float)(rcBody.center().x() + m_nScrollPosX) / m_fZoomRatioX;
    float fOldScrnX = (float)(nScreenX + m_nScrollPosX) / m_fZoomRatioX;
    float fOldDeltaX = (float)(nScreenX - rcBody.center().x());

    m_fZoomRatioX = zoomLimit(fZoom);
    m_nScrollRange = (int)(rcBody.width() * m_fZoomRatioX + 0.5f);
    m_nScrollRange = m_nScrollRange - rcBody.width();

    float fNewCenterX = fCenterX * m_fZoomRatioX;
    float fNewScrnX = fOldScrnX * m_fZoomRatioX;
    float fNewDeltaX = fNewScrnX - fNewCenterX;

    float fMoveToX = fNewCenterX - (fOldDeltaX - fNewDeltaX);
    int nMoveToX = (int)(fMoveToX + 0.5f);

    m_nScrollPosX = nMoveToX  - rcBody.width()/2;

    if (m_nScrollPosX < 0)
        m_nScrollPosX = 0;
    if (m_nScrollPosX > m_nScrollRange)
        m_nScrollPosX = m_nScrollRange;

    if (m_fZoomRatioX != 1.0f)
    {
        m_bShowScrollBar = true;
        if (m_TimerAutoHideScrollBar)
            killTimer(m_TimerAutoHideScrollBar);
        m_TimerAutoHideScrollBar = startTimer(2000);
    }
    else
    {
        if (m_TimerAutoHideScrollBar)
            killTimer(m_TimerAutoHideScrollBar);
        m_TimerAutoHideScrollBar = 0;
        m_bShowScrollBar = false;
    }

    update();
}
