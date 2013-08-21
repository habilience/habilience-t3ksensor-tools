#ifndef QKEYDESIGNWIDGET_H
#define QKEYDESIGNWIDGET_H

#include "SoftKeyDesignToolWidget.h"

#include "Softkey.h"
#include "GraphicsButtonItem.h"
#include "ResizingGraphicsItem.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include <QToolButton>


class QKeyDesignWidget : public QGraphicsView
{
    Q_OBJECT

public:
	enum KeyAlign { KeyAlignLeft, KeyAlignCenter, KeyAlignRight, KeyAlignTop, KeyAlignMiddle, KeyAlignBottom };
	enum KeyArrange { KeyArrangeHorizontal, KeyArrangeVertical };
	enum AdjustSize { AdjustSizeSameWidth, AdjustSizeSameHeight, AdjustSizeSameBoth };
	enum Distrib { DistribHorzEqualGap, DistribVertEqualGap };
	enum ScreenSize { ScreenSizeFull, ScreenSizeFit };
	enum ScreenMode { ScreenModeKeyDesign, ScreenModePreview, ScreenModeTest };

    QKeyDesignWidget(QWidget* parent = 0);
    virtual ~QKeyDesignWidget();

    //virtual void OnDraw( CDC* pDC );

    void updateKeys();

    QRect getFirstSelectKey();

    QVector<CSoftkey*>& getSelectKeys(){ return m_SelectKeys; }
    //QGraphicsItemGroup* getSelectKeys();



//    void alignSelectKeys( KeyAlign eAlign );
//    void adjustSizeSelectKeys( AdjustSize eAdjust );
//    void distribSelectKeys( Distrib eDistrib );

//    void arrangeSelectKeys( KeyArrange eArrange, int nKeyWidth, int nKeyHeight, int nKeyInterval );
//    void makeArrangedKeys( KeyArrange eArrange, int nKeyCount, int nKeyWidth, int nKeyHeight, int nKeyInterval );

//    void keySelectAll();
//    void keyCopyToClipboard();
//    void keyPaste();

    void pushHistory();
    void popHistory();
//    void redo();
//    void undo();

    ScreenSize getScreenSize() { return m_eScrnSize; }

    void setScreenMode( ScreenMode eMode ) { m_eScreenMode = eMode; }

//    void viewTouchPoint( long lX, long lY, bool bDown );

//    void setBlinkKey( GroupKey* group, int nCalPos, bool bSet );
//    void setBlinkKey( CSoftkey* key, int nCalPos, bool bSet );
//    void cancelBlink();

//    void setOnOff( int nSKNum, bool bIsOn );

    void setInvertDrawing( bool bInvert );

    void updateTouchCount( int nTouchCount );
    void updateScreen();

protected:
    // QWidget
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *event);

    // QGraphicsView
    virtual void drawBackground(QPainter *painter, const QRectF &rect);


    void selectKey( int nIndex );

    bool isSelectKey( CSoftkey* key );

//    void updateCanvas( QPoint& ptScrnOffset );
//    void updateArea( QRect& rc, QPoint& ptScrnOffset, bool bUpdateAll=false );

//    void drawKeys( CDC* pDC );
    void drawGrid( QPainter* pDC );
//    void drawCalPos( CDC* pDC );
//    void drawTouchPoint( CDC* pDC );
//    void drawCloseButton( CDC* pDC, QRect rcClose );
//    void drawTouchCount( CDC* pDC, QRect rcClose );

    QRect deviceToScreen( const QRect rcDevice, bool bTranslate=true );
    QRect screenToDevice( const QRect rcScreen, bool bTranslate=true );

    QPoint deviceToScreen( const QPoint ptDevice, bool bTranslate=true );
    QPoint screenToDevice( const QPoint ptScreen, bool bTranslate=true );

//    void drawOutlineText( CDC* pDC, QRgb dwTextColor, QRgb dwOutlineColor, LPCTSTR lpszText, RECT rc, uint nFormat );

    int getGroupIndex( const GroupKey* pGroup );

    void ungroup( const GroupKey* pGroup );

//    GroupStatus checkGroupStatus();

    void resizeScreen();

    void init();

    QObject* findWantToParent(QObject *target, const char* strObjectName);

//    void onLButtonDown(QMouseEvent*);

protected:
    QGraphicsScene      m_GraphicScens;
    QString             m_GraphicInfoName;
    int					m_nOldTouchCount;

    QRgb			m_clrBackground;
    QRgb			m_clrKeyBorder;
    QRgb			m_clrGridMajor;
    QRgb			m_clrGridMinor;
    QRgb			m_clrCloseBtnBg;
    QRgb			m_clrCloseBtnFg;

    QVector<QString>	m_aryUndoHistoryKey;
    QVector<QString>	m_aryRedoHistoryKey;
    QVector<QString>	m_aryUndoHistoryExtra;
    QVector<QString>	m_aryRedoHistoryExtra;

    QVector<QString>    m_vUHK;
//    bool				m_bBlink;
//    bool				m_bBlinkOnOff;
//    GroupKey*			m_pBlinkGroup;
//    CSoftkey*			m_pBlinkKey;

//    bool				m_bBlinkKeySet;

//    CSoftkey*			m_pFocusKey;
//    bool				m_bFocusKeyOn;

//    int					m_nCalPos;

    QSoftKeyDesignToolWidget*	m_pSoftKeyDesignTool;

////	CBitmap*			m_pBitmap;
////	CDC					m_MemDC;

    QRect				m_rcScreen;
    QRect				m_rcScreenOrg;

//    bool				m_bMoveScreen;
//    QPoint				m_ptMouseLast;

//    //QKeyTracker			m_KeyTracker;
//    QRect				m_rcDevTracker;

    QVector<CSoftkey*>	m_SelectKeys;

//    QVector<CSoftkey*>	m_ClipboardKeys;

//    int					m_nPasteCount;

//    bool				m_bMenuGroup;

    ScreenSize			m_eScrnSize;

    ScreenMode			m_eScreenMode;

//    bool				m_bViewTouchPoint;
//    QPoint				m_ptTouchPoint;

//    QRect				m_rcCloseButton;
//    bool				m_bDownCloseButton;

    QRect				m_rcTouchCount;

    QGraphicsButtonItem* m_pGIBtnClose;

    QResizingGraphicsItem*  m_pSelectionItem;

    bool                    m_bMousePress;
    bool                    m_bMouseDrag;

    int                     m_nGridStep;

signals:
    void UpdateEnable(bool bShow);
    void UpdateResize(int nW, int nH, int nFontHeight);
    void InvertDrawing(bool bInvert);

    void keyStateCount(bool bAdd);

    void SelectedKeys(bool bGroup, int nSelectedCount);

private slots:
    void onBtnClose();
    void onSelectionChanged();

public slots:
    int onAddNewKey();
    void onRemoveSelectedKeys();
    void onScreenSize(ScreenSize eSize);
    void onGroupSelectedKeys();
    void onUngroupSelectedKeys(bool bPushHistory);
    void onReorderKeys();
    void onInvalidateKey(QGraphicsKeyItem* key);
    void onRecalcSelectionKeys( QRectF rcOld, QRectF rcNew );
    void onUpdateScreen();
    void onResetKeys();
    void onHideSelection(QPointF pos);
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnDestroy();
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
//	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
//	afx_msg void OnTrackerGroupUngroup();
//	afx_msg void OnTrackerRemove();
//	afx_msg void OnTimer(UINT nIDEvent);
};

#endif // QKEYDESIGNWIDGET_H
