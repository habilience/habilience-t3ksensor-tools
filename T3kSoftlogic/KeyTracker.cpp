#include "KeyTracker.h"

#include <QPaintEngine>
#include <QPainter>
#include <QWidget>
#include <QtEvents>
#include <QDebug>

#include <QApplication>

#ifndef Q_OS_WIN
#define _countof(a) (sizeof(a)/sizeof(*(a)))
#endif

//AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };
//AFX_STATIC_DATA HBRUSH _afxHatchBrush = 0;
//AFX_STATIC_DATA HPEN _afxBlackDottedPen = 0;
//AFX_STATIC_DATA int _afxHandleSize = 0;

struct AFX_HANDLEINFO_ORI
{
    size_t nOffsetX;    // offset within RECT for X coordinate
    size_t nOffsetY;    // offset within RECT for Y coordinate
    int nCenterX;       // adjust X by Width()/2 * this number
    int nCenterY;       // adjust Y by Height()/2 * this number
    int nHandleX;       // adjust X by handle size * this number
    int nHandleY;       // adjust Y by handle size * this number
    int nInvertX;       // handle converts to this when X inverted
    int nInvertY;       // handle converts to this when Y inverted
};

struct AFX_HANDLEINFO
{
    int nWayX;          // direction x (0:left,2:right)
    int nWayY;          // direction y (1:top, 2:bottom)
    int nCenterX;       // adjust X by Width()/2 * this number
    int nCenterY;       // adjust Y by Height()/2 * this number
    int nHandleX;       // adjust X by handle size * this number
    int nHandleY;       // adjust Y by handle size * this number
    int nInvertX;       // handle converts to this when X inverted
    int nInvertY;       // handle converts to this when Y inverted
};

// this array describes all 8 handles (clock-wise)

static const AFX_HANDLEINFO_ORI _afxHandleInfoOri[] =
{
    { offsetof(QKeyTracker::Rectangle, nL), offsetof(QKeyTracker::Rectangle, nT),   0, 0,  0,  0, 1, 3 },
    { offsetof(QKeyTracker::Rectangle, nR), offsetof(QKeyTracker::Rectangle, nT),   0, 0, -1,  0, 0, 2 },
    { offsetof(QKeyTracker::Rectangle, nR), offsetof(QKeyTracker::Rectangle, nB),   0, 0, -1, -1, 3, 1 },
    { offsetof(QKeyTracker::Rectangle, nL), offsetof(QKeyTracker::Rectangle, nB),   0, 0,  0, -1, 2, 0 },

    { offsetof(QKeyTracker::Rectangle, nL), offsetof(QKeyTracker::Rectangle, nT),   1, 0,  0,  0, 4, 6 },
    { offsetof(QKeyTracker::Rectangle, nR), offsetof(QKeyTracker::Rectangle, nT),   0, 1, -1,  0, 7, 5 },
    { offsetof(QKeyTracker::Rectangle, nL), offsetof(QKeyTracker::Rectangle, nB),   1, 0,  0, -1, 6, 4 },
    { offsetof(QKeyTracker::Rectangle, nL), offsetof(QKeyTracker::Rectangle, nT),   0, 1,  0,  0, 5, 7 }
};

static const AFX_HANDLEINFO _afxHandleInfo[] =
{
    { 0, 1,     0, 0,  0,  0, 1, 3 },
    { 2, 1,     0, 0, -1,  0, 0, 2 },
    { 2, 3,     0, 0, -1, -1, 3, 1 },
    { 0, 3,     0, 0,  0, -1, 2, 0 },

    { 0, 1,     1, 0,  0,  0, 4, 6 },
    { 2, 1,     0, 1, -1,  0, 7, 5 },
    { 0, 3,     1, 0,  0, -1, 6, 4 },
    { 0, 1,     0, 1,  0,  0, 5, 7 }
};

struct AFX_RECTINFO_ORI
{
    size_t nOffsetAcross;   // offset of opposite point (ie. left->right)
    int nSignAcross;        // sign relative to that point (ie. add/subtract)
};

struct AFX_RECTINFO
{
    int nWay;   // offset of opposite point (ie. left->right)
    int nSignAcross;        // sign relative to that point (ie. add/subtract)
};

static const AFX_RECTINFO_ORI _afxRectInfoOri[] =
{
    { offsetof(QKeyTracker::Rectangle, nR), +1 },
    { offsetof(QKeyTracker::Rectangle, nB), +1 },
    { offsetof(QKeyTracker::Rectangle, nL), -1 },
    { offsetof(QKeyTracker::Rectangle, nT), -1 },
};

static const AFX_RECTINFO _afxRectInfo[] =
{
    { 2, +1 },
    { 3, +1 },
    { 0, -1 },
    { 1, -1 },
};

int getHandleOffset(QRect rc, int nWay)
{
    int nOffset = -1;
    switch( nWay )
    {
    case 0: // left
        nOffset = rc.left();
        break;
    case 1: // top
        nOffset = rc.top();
        break;
    case 2: // right
        nOffset = rc.right();
        break;
    case 3: // bottom
        nOffset = rc.bottom();
        break;
    default:
        break;
    }

    return nOffset;
}

int getRectOffsetAcross(QRect rc, int nWay)
{
    int nOffset = -1;
    switch( nWay )
    {
    case 0: // left
        nOffset = rc.left();
        break;
    case 1: // top
        nOffset = rc.top();
        break;
    case 2: // right
        nOffset = rc.right();
        break;
    case 3: // bottom
        nOffset = rc.bottom();
        break;
    default:
        break;
    }

    return nOffset;
}

QKeyTracker::QKeyTracker(QObject *parent) :
    QObject(parent)
{
    m_TrackerPen.setStyle( Qt::SolidLine );
    m_TrackerPen.setWidth( 3 );
    m_TrackerPen.setColor( qRgb(0xff,0xff,150) );

    m_nHandleSize = 4;
    m_sizeMin = QSize( m_nHandleSize*2, m_nHandleSize*2 );

    m_nStyle = 0;

    m_eTrackerHit = hitNothing;
    m_bErase = false;
    m_bFinalErase = false;

    m_pTargetWidget = NULL;
    m_bTracking = false;
    m_bMove = false;

    m_bAllowInvert = false;
    m_bRubberBand = false;

    ::memset( &m_rectCalc, 0, sizeof(Rectangle) );
}

QKeyTracker::~QKeyTracker()
{
}

void QKeyTracker::draw(QPainter* painter)
{
    painter->save();

    if( !m_bMove && !m_bRubberBand )
    {
        if( m_rect.isEmpty() ) return;

        // get normalized rectangle
        QRect rect( m_rect.normalized() );

        // draw lines
//        if ((m_nStyle & (dottedLine|solidLine)) != 0)
//        {
//            painter->save();

//            rect.adjust( -1, -1, 1, 1 );
//            painter->setBrush( Qt::NoBrush );
//            //pDC->SetROP2(R2_COPYPEN);
//            painter->setCompositionMode( QPainter::RasterOp_NotDestination );

//            painter->setPen( m_TrackerPen );
//            painter-> drawRect( rect );

//            if( m_nStyle & dottedLine )
//                painter->setPen( Qt::DotLine );
//            else
//                painter->setPen( Qt::black );
//            painter->drawRect( rect );

//            painter->restore();
//        }

        // if hatchBrush is going to be used, need to unrealize it
    //	if ((m_nStyle & (hatchInside|hatchedBorder)) != 0)
    //		UnrealizeObject(_afxHatchBrush);

        // hatch inside
        if ((m_nStyle & hatchInside) != 0)
        {
            painter->save();

            painter->setPen( Qt::NoPen );
            painter->setBrush( Qt::Dense3Pattern );
            //nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
            //painter->drawRect( rect.left()+1, rect.top()+1, rect.right(), rect.bottom() );

            painter->restore();
        }

        painter->save();

        painter->setPen( Qt::black );
        painter->setBrush( Qt::white );
//        painter->setBrush( QBrush( Qt::black, Qt::Dense3Pattern ) );
        //painter->setCompositionMode( QPainter::RasterOp_NotDestination );

        QRect rectTrue( getTrueRect() );

        painter->drawRect( rectTrue.left(), rectTrue.top(), rectTrue.width()-1, getHandleSize() );
        painter->drawRect( rectTrue.left(), rect.bottom(), rectTrue.width()-1, getHandleSize() );
        painter->drawRect( rectTrue.left(), rect.top(), getHandleSize(), rect.height()-1 );
        painter->drawRect( rect.right(), rect.top(), getHandleSize(), rect.height()-1 );

        painter->restore();

        // draw hatched border
//        if ((m_nStyle & hatchedBorder) != 0)
//        {
//            painter->save();

//            painter->setPen( Qt::NoPen );
//            painter->setBrush( QBrush( Qt::black, Qt::Dense3Pattern ) );
//            painter->setBackgroundMode( Qt::OpaqueMode );
//            QRect rectTrue( getTrueRect() );

//            painter->drawRect( rectTrue.left(), rectTrue.top(), rectTrue.width()-1, getHandleSize() );
//            painter->drawRect( rectTrue.left(), rect.bottom(), rectTrue.width()-1, getHandleSize() );
//            painter->drawRect( rectTrue.left(), rect.top(), getHandleSize(), rect.height()-1 );
//            painter->drawRect( rect.right(), rect.top(), getHandleSize(), rect.height()-1 );

//            painter->restore();
//        }

        // draw resize handles
        if (!m_bMove &&  (m_nStyle & (resizeInside|resizeOutside)) != 0)
        {
            uint mask = getHandleMask();
            for (int i = 0; i < 8; ++i)
            {
                if (mask & (1<<i))
                {
                    QRect rect( m_rect.normalized() );
                    rect = getHandleRect((TrackerHit)i);
                    rect.adjust( -1, -1, 1, 1 );
                    painter->fillRect( rect, Qt::white );
                    rect.adjust( 1, 1, -1, -1 );
                    painter->fillRect( rect, Qt::black );
                }
            }
        }
    }
    else
    {
        if( m_rcTracking.isEmpty() ) return;

        painter->setPen( Qt::NoPen );
        painter->setCompositionMode( QPainter::RasterOp_NotDestination );

        painter->fillRect( m_rcTracking.left(), m_rcTracking.top(), m_rcTracking.width()-1-5, 5, Qt::Dense2Pattern );
        painter->fillRect( m_rcTracking.right()-5, m_rcTracking.top(), 5, m_rcTracking.height()-1-5, Qt::Dense2Pattern );
        painter->fillRect( m_rcTracking.left()+5, m_rcTracking.bottom()-5, m_rcTracking.width()-1-5, 5, Qt::Dense2Pattern );
        painter->fillRect( m_rcTracking.left(), m_rcTracking.top()+5, 5, m_rcTracking.height()-1-5, Qt::Dense2Pattern );
    }

	// cleanup pDC state
    painter->restore();
}

void QKeyTracker::drawTrackerRect(QRect rc, QWidget* target)
{
    // first, normalize the rectangle for drawing
    QRect rect( rc.normalized() );

    QSize size;
    if (!m_bFinalErase)
    {
        // otherwise, size depends on the style
        if (m_nStyle & hatchedBorder)
        {
            size.setWidth( qMax(1, getHandleSize(&rect)-1) );
            size.setHeight( size.width() );
            rect.adjust( -size.width(), -size.height(), size.width(), size.height() );
        }
        else
        {
            size.setWidth( m_nHandleSize );
            size.setHeight( m_nHandleSize );
        }
    }

    // and draw it

    if (m_bFinalErase || !m_bErase)
    {
        if( m_rcTracking.isEmpty() )
            target->update( m_rcTracking.adjusted( -10,-10,10,10 ) );
        else
            target->update();
        m_rcTracking = rect;
        target->update( rect.adjusted( -10,-10,10,10 ) );
    }

    // remember last rectangles
//    m_rectLast = rect;
//    m_sizeLast = size;
}

bool QKeyTracker::eventFilter(QObject *target, QEvent *evt)
{
    if( (target == m_pTargetWidget) )
    {
        if( !m_bTracking ) return false;

        bool bProcess = false;
        bool bMLBRelease = false;
        bool bEscapeKey = false;
        switch( evt->type() )
        {
        case QEvent::KeyPress:
        {
            QKeyEvent* pEvt = (QKeyEvent*)evt;
            if( pEvt->key() != Qt::Key_Escape )
                break;

            bEscapeKey = true;
        }
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* pEvt = (QMouseEvent*)evt;
            if( bEscapeKey || pEvt->button() == Qt::RightButton )
            {
                if (m_bMove)
                {
                    m_bErase = m_bFinalErase = true;
                    drawTrackerRect(m_rect, m_pTargetWidget);
                }
                m_rect = m_rcSave;

                m_bFinalErase = false;
                m_bErase = false;

                m_rcTracking.setRect( 0,0,0,0 );

                m_pTargetWidget->removeEventFilter( this );
                m_bTracking = false;
                m_bRubberBand = false;
                m_bMove = false;

                emit finish(!(m_rcSave == m_rect));

                bProcess = true;
            }
        }
            break;
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent* pEvt = (QMouseEvent*)evt;
            if( pEvt->button() != Qt::LeftButton )
                break;

            bMLBRelease = true;
        }
        case QEvent::MouseMove:
        {
            QMouseEvent* pEvt = (QMouseEvent*)evt;
            m_oldRect = m_rect;

            if (px != NULL)
                *px = pEvt->pos().x() - xDiff;
            if (py != NULL)
                *py = pEvt->pos().y() - yDiff;

            m_rect.setRect( m_rectCalc.nL, m_rectCalc.nT, m_rectCalc.nR-m_rectCalc.nL+1, m_rectCalc.nB-m_rectCalc.nT );

            // handle move case
            if (m_eTrackerHit == hitMiddle)
            {
                m_rect.setRight( m_rect.left() + m_rcSave.width() - 1 );
                m_rect.setBottom( m_rect.top() + m_rcSave.height() - 1 );
            }

            adjustRect(m_eTrackerHit);

            // only redraw and callback if the rect actually changed!
            m_bFinalErase = bMLBRelease;
            if (! (m_oldRect == m_rect) || m_bFinalErase)
            {
                if (m_bMove)
                {
                    m_bErase = true;
                    drawTrackerRect(m_oldRect, m_pTargetWidget);
                }

                if (!bMLBRelease)
                    m_bMove = true;
            }
            if (m_bFinalErase)
            {
                if (!m_bMove)
                    m_rect = m_rcSave;
                m_bFinalErase = false;
                m_bErase = false;

                m_rcTracking.setRect( 0,0,0,0 );

                m_pTargetWidget->removeEventFilter( this );
                m_bTracking = false;
                m_bRubberBand = false;
                m_bMove = false;

                m_pTargetWidget->update();

                emit finish(!(m_rcSave == m_rect));
            }

            if (!(m_oldRect == m_rect))
            {
                m_bErase = false;
                drawTrackerRect(m_rect, m_pTargetWidget);
            }

            bProcess = true;
        }
            break;
        default:
            break;
        }

        return bProcess;
    }

    return QObject::eventFilter(target, evt);
}

//bool QKeyTracker::track(QWidget* pWidget, QPoint point, bool bAllowInvert,
//    QWidget* pWndClipTo)
//{
//	// perform hit testing on the handles
//    int nHandle = hitTestHandles(point);
//	if (nHandle < 0)
//	{
//		// didn't hit a handle, so just return FALSE
//        return false;
//	}

//	// otherwise, call helper function to do the tracking
//	m_bAllowInvert = bAllowInvert;
//    return trackHandle(nHandle, pWnd, point, pWndClipTo);
//}

//bool QKeyTracker::trackRubberBand(QWidget* pWidget, QPoint point, bool bAllowInvert)
//{
//	// simply call helper function to track from bottom right handle
//	m_bAllowInvert = bAllowInvert;
//    m_rect.setRect( point.x(), point.y(), point.x(), point.y() );
//    return trackHandle(hitBottomRight, pWidget, point, NULL);
//}

bool QKeyTracker::track(QWidget* target, QPoint ptStart, bool bAllowInvert)
{
    int nHandle = hitTestHandles(ptStart);
    if( nHandle < 0 )
        return false;

    m_eTrackerHit = (TrackerHit)nHandle;
    Q_ASSERT( target != NULL );

    Q_ASSERT( m_eTrackerHit >= 0 );
    Q_ASSERT( m_eTrackerHit <= 8 );

    Q_ASSERT( !m_bFinalErase );

    m_rcSave = m_rect;

    m_bTracking = true;

    m_bAllowInvert = bAllowInvert;

    getModifyPointers(m_eTrackerHit, &px, &py, &xDiff, &yDiff);

    xDiff = ptStart.x() - xDiff;
    yDiff = ptStart.y() - yDiff;

    m_pTargetWidget = target;

    m_ptStart = ptStart;

    m_pTargetWidget->installEventFilter( this );

    return true;
}

void QKeyTracker::trackRubberBand(QWidget* target, QPoint ptStart, bool bAllowInvert)
{
    Q_ASSERT( target != NULL );

    Q_ASSERT( !m_bFinalErase );

    m_rcSave = m_rect;

    m_bTracking = true;
    m_bRubberBand = true;

    m_eTrackerHit = hitBottomRight;

    m_bAllowInvert = bAllowInvert;

    getModifyPointers(m_eTrackerHit, &px, &py, &xDiff, &yDiff);

    xDiff = ptStart.x() - xDiff;
    yDiff = ptStart.y() - yDiff;

    m_pTargetWidget = target;

    m_ptStart = ptStart;

    m_pTargetWidget->installEventFilter( this );
}

QRect QKeyTracker::getTrueRect() const
{
    //ASSERT(AfxIsValidAddress(lpTrueRect, sizeof(RECT)));

    QRect rect( m_rect.normalized() );

    int nInflateBy = 0;
    if ((m_nStyle & (resizeOutside|hatchedBorder)) != 0)
        nInflateBy += getHandleSize() - 1;
    if ((m_nStyle & (solidLine|dottedLine)) != 0)
        ++nInflateBy;
    rect.adjust( -nInflateBy, -nInflateBy, nInflateBy, nInflateBy );

    return rect;
}

int QKeyTracker::getHandleSize(QRect* lpRect) const
{
    QRect rect;
    if (lpRect == NULL)
        rect = m_rect;
    else
        rect = *lpRect;

    int size = m_nHandleSize;
    if (!(m_nStyle & resizeOutside))
    {
        // make sure size is small enough for the size of the rect
        int sizeMax = qMin(qAbs(rect.right() - rect.left()),
                           qAbs(rect.bottom() - rect.top()));
        if (size * 2 > sizeMax)
            size = sizeMax / 2;
    }
    return size;
}

uint QKeyTracker::getHandleMask() const
{
    uint mask = 0x0F;   // always have 4 corner handles
    int size = m_nHandleSize*3;
    if (abs(m_rect.width()) - size > 4)
        mask |= 0x50;
    if (abs(m_rect.height()) - size > 4)
        mask |= 0xA0;
    return mask;
}

int QKeyTracker::hitTest(QPoint point)
{
    TrackerHit hitResult = hitNothing;

    QRect rectTrue( getTrueRect() );
    Q_ASSERT(rectTrue.left() <= rectTrue.right());
    Q_ASSERT(rectTrue.top() <= rectTrue.bottom());
    if( rectTrue.contains( point ) )
    {
        if( (m_nStyle & (resizeInside|resizeOutside)) != 0 )
            hitResult = (TrackerHit)hitTestHandles(point);
        else
            hitResult = hitMiddle;
    }
    return hitResult;
}

int QKeyTracker::hitTestHandles(QPoint point)
{
    QRect rect;
    uint mask = getHandleMask();

    // see if hit anywhere inside the tracker
    rect = getTrueRect();
    if (!rect.contains(point))
        return hitNothing;  // totally missed

    // see if we hit a handle
    for (int i = 0; i < 8; ++i)
    {
        if (mask & (1<<i))
        {
            rect = getHandleRect(i);
            if (rect.contains(point))
                return (TrackerHit)i;
        }
    }

    // last of all, check for non-hit outside of object, between resize handles
    if ((m_nStyle & hatchedBorder) == 0)
    {
        QRect rect( m_rect );
        rect = rect.normalized();
        if ((m_nStyle & dottedLine|solidLine) != 0)
            rect.adjust( -1, -1, 1, 1 );
        if (!rect.contains(point))
            return hitNothing;  // must have been between resize handles
    }
    return hitMiddle;   // no handle hit, but hit object (or object border)
}

QRect QKeyTracker::getHandleRect(int nHandle)
{
    Q_ASSERT(nHandle < 8);

    // get normalized rectangle of the tracker
    QRect rectT = m_rect;
    rectT = rectT.normalized();
    if ((m_nStyle & (solidLine|dottedLine)) != 0)
        rectT.adjust( -1, -1, 1, 1 );

    // since the rectangle itself was normalized, we also have to invert the
    //  resize handles.
    nHandle = normalizeHit(nHandle);

    // handle case of resize handles outside the tracker
    int size = getHandleSize();
    if (m_nStyle & resizeOutside)
    {
        int nV = size-1;
        rectT.adjust( -nV, -nV, nV, nV );
    }

    // calculate position of the resize handle
    int nWidth = rectT.width();
    int nHeight = rectT.height();
    QRect rect;
    const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
    rect.setLeft( getHandleOffset( rectT, pHandleInfo->nWayX ) + (size * pHandleInfo->nHandleX)
                  + (pHandleInfo->nCenterX * (nWidth - size) / 2) );
    rect.setTop( getHandleOffset( rectT, pHandleInfo->nWayY ) + (size * pHandleInfo->nHandleY)
                 + (pHandleInfo->nCenterY * (nHeight - size) / 2) );
    rect.setRight( rect.left() + size );
    rect.setBottom( rect.top() + size );

    return rect;
}

int QKeyTracker::normalizeHit(int nHandle)
{
    Q_ASSERT(nHandle <= 8 && nHandle >= -1);
    if (nHandle == hitMiddle || nHandle == hitNothing)
        return nHandle;
    Q_ASSERT(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
    const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
    if (m_rect.width() < 0)
    {
        nHandle = (TrackerHit)pHandleInfo->nInvertX;
        Q_ASSERT(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
        pHandleInfo = &_afxHandleInfo[nHandle];
    }
    if (m_rect.height() < 0)
        nHandle = (TrackerHit)pHandleInfo->nInvertY;
    return nHandle;
}

void QKeyTracker::getModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py)
{
    Q_ASSERT(nHandle >= 0);
    Q_ASSERT(nHandle <= 8);

    if (nHandle == hitMiddle)
        nHandle = hitTopLeft;   // same as hitting top-left

    *ppx = NULL;
    *ppy = NULL;

    // fill in the part of the rect that this handle modifies
    //  (Note: handles that map to themselves along a given axis when that
    //   axis is inverted don't modify the value on that axis)
    m_rectCalc.nL = m_rect.left();
    m_rectCalc.nT = m_rect.top();
    m_rectCalc.nR = m_rect.right();
    m_rectCalc.nB = m_rect.bottom();

    const AFX_HANDLEINFO_ORI* pHandleInfo = &_afxHandleInfoOri[nHandle];
    if (pHandleInfo->nInvertX != nHandle)
    {
        *ppx = (int*)((uchar*)&m_rectCalc + pHandleInfo->nOffsetX);
        if (px != NULL)
            *px = **ppx;
    }
    else
    {
        // middle handle on X axis
        if (px != NULL)
            *px = m_rectCalc.nT + abs(m_rectCalc.nR-m_rectCalc.nL) / 2;
    }
    if (pHandleInfo->nInvertY != nHandle)
    {
        *ppy = (int*)((uchar*)&m_rectCalc + pHandleInfo->nOffsetY);
        if (py != NULL)
            *py = **ppy;
    }
    else
    {
        // middle handle on Y axis
        if (py != NULL)
            *py = m_rectCalc.nT + abs(m_rectCalc.nB-m_rectCalc.nT) / 2;
    }

    m_rect.setRect( m_rectCalc.nL, m_rectCalc.nT, m_rectCalc.nR-m_rectCalc.nL + 1, m_rectCalc.nB-m_rectCalc.nT + 1 );
}

void QKeyTracker::adjustRect(int nHandle)
{
    if (nHandle == hitMiddle)
        return;

    // convert the handle into locations within m_rect
    int *px, *py;
    getModifyPointers(nHandle, &px, &py, NULL, NULL);

    m_rectCalc.nL = m_rect.left();
    m_rectCalc.nT = m_rect.top();
    m_rectCalc.nR = m_rect.right();
    m_rectCalc.nB = m_rect.bottom();

    // enforce minimum width
    int nNewWidth = m_rect.width();
    int nAbsWidth = m_bAllowInvert ? qAbs(nNewWidth) : nNewWidth;
    if (px != NULL && nAbsWidth < m_sizeMin.width())
    {
        nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;

        ptrdiff_t iRectInfo = (int*)px - (int*)&m_rectCalc;
        Q_ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
        const AFX_RECTINFO_ORI* pRectInfo = &_afxRectInfoOri[iRectInfo];
        *px = *(int*)((uchar*)&m_rectCalc + pRectInfo->nOffsetAcross) +
            nNewWidth * m_sizeMin.width() * -pRectInfo->nSignAcross;
    }

    // enforce minimum height
    int nNewHeight = m_rect.height();
    int nAbsHeight = m_bAllowInvert ? qAbs(nNewHeight) : nNewHeight;
    if (py != NULL && nAbsHeight < m_sizeMin.height())
    {
        nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
        ptrdiff_t iRectInfo = (int*)py - (int*)&m_rectCalc;
        Q_ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
        const AFX_RECTINFO_ORI* pRectInfo = &_afxRectInfoOri[iRectInfo];
        *py = *(int*)((uchar*)&m_rectCalc + pRectInfo->nOffsetAcross) +
            nNewHeight * m_sizeMin.height() * -pRectInfo->nSignAcross;
    }

    m_rect.setRect( m_rectCalc.nL, m_rectCalc.nT, m_rectCalc.nR-m_rectCalc.nL + 1, m_rectCalc.nB-m_rectCalc.nT + 1 );
}
