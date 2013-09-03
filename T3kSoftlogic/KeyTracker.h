#ifndef QKEYTRACKER_H
#define QKEYTRACKER_H

#include <QObject>
#include <QPen>
#include <QSize>
#include <QMouseEvent>

class QKeyTracker : public QObject
{
    Q_OBJECT

public:
    enum StyleType
    {
        solidLine = 1, dottedLine = 2, hatchedBorder = 4,
        resizeInside = 8, resizeOutside = 16, hatchInside = 32
    };

    Q_DECLARE_FLAGS(StyleFlags, StyleType)
    enum TrackerHit
    {
        hitNothing = -1,
        hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
        hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
    };

public:
    QKeyTracker(QObject* parent = 0);
    virtual ~QKeyTracker();

//    bool track(QWidget* pWidget, QPoint point, bool bAllowInvert = false,
//        QWidget* pWndClipTo = NULL);
//    bool trackRubberBand(QWidget* pWidget, QPoint point, bool bAllowInvert = true);

	
    void draw(QPainter* painter);

    int hitTest(QPoint point);

    void trackRubberBand(QWidget* target, QPixmap* pm, QPoint ptStart, bool bAllowInvert = false);
    bool track(QWidget* target, QPixmap* pm, QPoint ptStart, bool bAllowInvert = false);

    QRect       m_oldRect;
    QRect       m_rect;
    int         m_nStyle;
    QSize       m_sizeMin;

    bool        m_bErase;
    bool        m_bFinalErase;

    bool        m_bMove;

    QRect       m_rcSave;
    QPoint      m_ptStart;
    QRect       m_rcTracking;

    bool        m_bAllowInvert;

    typedef struct _Rectangle
    {
        int nL;
        int nT;
        int nR;
        int nB;
    } Rectangle;

    Rectangle   m_rectCalc;
    bool        m_bTracking;
    bool        m_bRubberBand;

protected:
    virtual bool eventFilter(QObject *, QEvent *);

    //bool TrackHandle(int nHandle, QWidget* pWidget, QPoint point, QWidget* pWndClipTo);
    void getModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py);

    void drawTrackerRect(QRect rc, QWidget* target, QPixmap* pm);

    QRect getTrueRect() const;
    int getHandleSize(QRect* lpRect=NULL) const;
    uint getHandleMask() const;
    QRect getHandleRect(int nHandle);
    int normalizeHit(int nHandle);
    int hitTestHandles(QPoint point);

    void adjustRect(int nHandle);

protected:
    QPen		m_TrackerPen;

    QWidget*    m_pTargetWidget;
    QPixmap*    m_pixmap;

    int         m_nHandleSize;



    TrackerHit  m_eTrackerHit;

    int *px, *py;
    int xDiff, yDiff;

signals:
    void finish(bool bChanged);

public slots:

};

#endif // QKEYTRACKER_H
