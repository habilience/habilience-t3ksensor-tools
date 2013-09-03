#ifndef QLOGICDESIGNWIDGET_H
#define QLOGICDESIGNWIDGET_H

#include "Softkey.h"
#include "EditGateWidget.h"
#include "LogicGalleryWidget.h"

#include <QWidget>
#include <QPen>


class QLogicDesignWidget : public QWidget
{
    Q_OBJECT

public:
    QLogicDesignWidget(QWidget* parent=0);
    virtual ~QLogicDesignWidget();

	enum ScreenMode { ScreenModeLogicDesign, ScreenModePreview };

    void setScreenMode( ScreenMode eMode ) { m_eScreenMode = eMode; }

    void setInvertDrawing( bool bInvert );

protected:
    QRgb			m_clrBackground;
    QRgb			m_clrCloseBtnBg;
    QRgb			m_clrCloseBtnFg;
    QRgb			m_clrGrid;
    QRgb			m_clrBindSelect;
    QRgb			m_clrBindNormal;
    QRgb			m_clrKeyBg;
    QRgb			m_clrKeyFg;
    QRgb			m_clrKeyBorderActive;
    QRgb			m_clrKeyBorderNormal;
    QRgb			m_clrKeyBorderBind;

    QRgb			m_clrLogicBorderNormal;
    QRgb			m_clrLogicBorderActive;
    QRgb			m_clrLogicFg;
    QRgb			m_clrLogicBg;

    QRgb			m_clrConnector;

    QEditGateWidget     m_wndEditGate;
    QLogicGalleryWidget	m_wndLogicGallery;

    QPixmap*            m_pImageCanvas;

	CSoftlogic*			m_pSoftlogicForRemove;

	ScreenMode			m_eScreenMode;
    QRect				m_rcCloseButton;
    bool				m_bDownCloseButton;

    bool				m_bShowText;

    QPixmap*            m_pImageConsumerButton;

    QPointF				m_ptSoftkeyPositionBackup;
	CSoftkey*			m_pToBindKey;
	CSoftkey*			m_pCheckKey;

    QPen                m_penFocus;
    QFont               m_ftDrawText;
    QBrush              m_brshHatchGate;

    bool                m_bCpaute;

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

    void mouseLBtnDown(QPoint pos);
    void mouseLBtnUp(QPoint pos);
    void mouseRBtnUp(QPoint pos);
    void mouseLBtnDoubleClick(QPoint pos);

protected:
    void draw(QPainter* painter);

    void drawKeyBind( QPainter * painter, QRect rcClient, CSoftkey * key, int id, int size );
    void drawKey( QPainter * painter, QRect rcClient, CSoftkey * key, int id, int nbid, int size, bool bBound );
    void drawLogic( QPainter * painter, QRect rcClient, CSoftlogic * logic, bool bShow, int size, bool bBound, int ID );

    void _drawBufferGate( QPainter * painter, QPoint pt, int size, CSoftlogic::Port enable_port, bool enable_not, bool in_not, bool bShow );
    void _drawAndGate( QPainter * painter, QPoint pt, int size, CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow );
    void _drawOrGate( QPainter * painter, QPoint pt, int size, CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow );
    void _drawXorGate( QPainter * painter, QPoint pt, int size, CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow );
    void _drawAndRisingedgeGate( QPainter * painter, QPoint pt, int size, CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow );
    void _drawTrigger( QPainter * painter, QPoint pt, int size, CSoftlogic::Trigger trigger );
    void _drawLine( QPainter * painter, QPoint pt1, QPoint pt2, float fD );
    void _drawLine2( QPainter* painter, QPen& LinePen, QPoint pt1, QPoint pt2, float fD );
    void _drawInPort( QPainter * painter, CSoftlogic::Port port, QPoint pt, int size, bool bUp, bool bDefaultTrue, bool bNoGateOK = false );

    void _drawEnable( QPainter * painter, CSoftlogic::Port enable_port, bool enable_not, int xe, int ye, int xsize, int ysize );
    void _drawOutKey( QPainter * painter, QPoint pt, int size, uchar ckey, uchar key );
    void _drawOutMouse( QPainter * painter, QPoint pt, int size, uchar ckey, uchar mkey );
    void _drawOutState( QPainter * painter, QPoint pt, int size, CSoftlogic::Port out_port );
    void _drawOutGPIO( QPainter * painter, QPoint pt, int size, CSoftlogic::Port out_port );
    void _drawOutReport( QPainter * painter, QPoint pt, int size, int id );

    bool getOutPortPosI( CSoftlogic::Port port, QPoint * ptPos, QRect rcClient );

	CSoftlogic::Port m_port_hit;
    QPoint           m_SelOffset;
	enum GatePart
	{
        epqNill, egpBody, epgIn1, epgIn2
	} m_SelPart;
    CSoftlogic::Port hitTest( QPoint pt, QPoint * ptPos, GatePart * part, CSoftkey* pDontCheckKey=NULL );

	CSoftlogic::Port m_port_dst;
    QPoint           m_pt_dst;

    bool  m_bShowNumber;

    void drawCloseButton( QPainter* painter, QRect rcClose );

    bool isAssociate( CSoftkey* pSoftkey, CSoftlogic* pLogic );
    bool isAssociate( CSoftlogic* pLogicCheck, CSoftlogic* pLogic );
    QRect getKeyBound( CSoftkey* pSoftkey, bool bFocusRect );
    QRect getLogicBound( CSoftlogic* pLogic );

signals:
    void closeWidget();

public slots:
    void onUpdate();
    void onAddLogicFromGallery(int nGalleryItemType, CSoftlogic::Port portKey, QPoint ptMouse);
};

#endif // QLOGICDESIGNWIDGET_H
