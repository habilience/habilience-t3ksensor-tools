#ifndef QKEYDESIGNWIDGET_H
#define QKEYDESIGNWIDGET_H

#include "SoftKeyDesignToolWidget.h"

#include "SoftKey.h"
#include "KeyTracker.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include <QToolButton>


class QKeyDesignWidget : public QWidget
{
    Q_OBJECT

public:
	enum ScreenSize { ScreenSizeFull, ScreenSizeFit };
	enum ScreenMode { ScreenModeKeyDesign, ScreenModePreview, ScreenModeTest };

    QKeyDesignWidget(QWidget* parent = 0);
    virtual ~QKeyDesignWidget();

    void updateKeys();

    void selectKey( int nIndex );

    QVector<CSoftkey*>& getSelectKeys() { return m_SelectKeys; }
    QRect getFirstSelectKey();

    void alignSelectKeys( KeyAlign eAlign );
    void adjustSizeSelectKeys( AdjustSize eAdjust );
    void distribSelectKeys( Distrib eDistrib );

    void arrangeSelectKeys( KeyArrange eArrange, int nKeyWidth, int nKeyHeight, int nKeyInterval );
    void makeArrangedKeys( KeyArrange eArrange, int nKeyCount, int nKeyWidth, int nKeyHeight, int nKeyInterval );

    void keySelectAll();
    void keyCopyToClipboard();
    void keyPaste();

    void pushHistory();
    void popHistory();
    void redo();
    void undo();

    void setScreenSize( ScreenSize eScrnSize );
    ScreenSize getScreenSize() { return m_eScrnSize; }

    void setScreenMode( ScreenMode eMode ) { m_eScreenMode = eMode; }

    void viewTouchPoint( long lX, long lY, bool bDown );

    void setBlinkKey( GroupKey* group, int nCalPos, bool bSet );
    void setBlinkKey( CSoftkey* key, int nCalPos, bool bSet );
    void cancelBlink();

    void setOnOff( int nSKNum, bool bIsOn );

    void setInvertDrawing( bool bInvert );

    void updateTouchCount( int nTouchCount );

    void init();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);

    void draw( QPainter* painter );

    void mouseLButtonDown(QMouseEvent* evt);

protected:
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
    bool				m_bBlink;
    bool				m_bBlinkOnOff;
    GroupKey*			m_pBlinkGroup;
    CSoftkey*			m_pBlinkKey;

    bool				m_bBlinkKeySet;

    CSoftkey*			m_pFocusKey;
    bool				m_bFocusKeyOn;

    int					m_nCalPos;

    QSoftKeyDesignToolWidget*	m_pSoftKeyDesignTool;

    QRect				m_rcScreen;
    QRect				m_rcScreenOrg;

    bool				m_bMoveScreen;
    QPoint				m_ptMouseLast;

    float				m_fGridStep;

    QKeyTracker			m_KeyTracker;
    QRect				m_rcDevTracker;

    QVector<CSoftkey*>	m_SelectKeys;

    QVector<CSoftkey*>	m_ClipboardKeys;

    int					m_nPasteCount;

    bool				m_bMenuGroup;

    ScreenSize			m_eScrnSize;

    ScreenMode			m_eScreenMode;

    bool				m_bViewTouchPoint;
    QPoint				m_ptTouchPoint;

    QRect				m_rcCloseButton;
    bool				m_bDownCloseButton;

    QRect				m_rcTouchCount;

    int                 m_nTimerBlink;

    enum InputButton { btnLeft, btnRight, btnMiddle };

    typedef struct _MouseState
    {
        InputButton             eInput;
        bool                    bPress;
        Qt::KeyboardModifier    keyModifier;
    } MouseState;

    bool                    m_bCheckRubberBand;

    bool                    m_bMouseLeftPress;
    Qt::KeyboardModifier    m_keyModifier;

    bool                    m_bShiftDown;
    bool                    m_bCtrlDown;

    QRect                   m_rcOldTracker;
    QRect                   m_rcOld;

    bool                    m_bCapture;

    bool isSelectKey( CSoftkey* key );

    void updateCanvas( QPoint& ptScrnOffset );
    void updateArea( QRect& rc, QPoint& ptScrnOffset, bool bUpdateAll=false );

    void drawKeys( QPainter* painter );
    void drawGrid( QPainter* painter );
    void drawCalPos( QPainter* painter );
    void drawTouchPoint( QPainter* painter );
    void drawCloseButton( QPainter* painter, QRect rcClose );
    void drawTouchCount( QPainter* painter, QRect rcClose );

    QRect deviceToScreen( const QRect& rcDevice, bool bTranslate=true );
    QRect screenToDevice( const QRect& rcScreen, bool bTranslate=true );

    QPoint deviceToScreen( const QPoint& ptDevice, bool bTranslate=true );
    QPoint screenToDevice( const QPoint& ptScreen, bool bTranslate=true );

    void drawOutlineText( QPainter* painter, QRgb dwTextColor, QRgb dwOutlineColor, QString strText, QRect rc, QTextOption nFormat );

    int getGroupIndex( const GroupKey* pGroup );

    void ungroup( const GroupKey* pGroup );

    GroupStatus checkGroupStatus();

    void resizeScreen();

signals:
    void updateEnable(bool bShow);
    void updateResize(int nW, int nH, int nFontHeight);
    void invertDrawing(bool bInvert);

    void keyStateCount(bool bAdd);

    void closeWidget();

public slots:
    void onCustomContextMenuRequested(const QPoint& pos);
    void onContextMenuGrouping();
    void onContextMenuRemove();

    int onAddNewKey();
    void onRemoveSelectKeys();
    void onGroupSelectKeys();
    void onUngroupSelectKeys( bool bPushHistory );
    void onInvalidateKey( CSoftkey* key );
    void onRecalcSelectionKeys( QRect rcOld, QRect rcNew );
    void onUpdateScreen();
    void onScreenSize( int eScreen );

    // QKeyTracker
    void onRubberBandFinish(bool bChanged);

    // SoftkeyDesignToolWidget
    void onReorderKeys();
    void onResetKeys();

    void onGenerateKeys(KeyArrange eArrange, int nCount, int nW, int nH, int nInterval);
    void onAlignSelectedKeys(KeyAlign eAlign);
    void onAdjustSizeSelectedKeys(AdjustSize eSize);
    void onDistribSelectKeys(Distrib eDistrib);
};

#endif // QKEYDESIGNWIDGET_H
