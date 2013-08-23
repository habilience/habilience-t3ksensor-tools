#include "stdInclude.h"

#include "QMouseMappingTable.h"

#include "../common/QKeyMapStr.h"
#include "QT3kUserData.h"

#include "T3kPacketDef.h"

#include <QPainter>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>

#define ROW_COUNT		(5)
#define COL_COUNT		(5)

#define TABLE(i,j) ( (i)+(j)*COL_COUNT )
inline void Index2Table( int n, int& i, int& j )
{
    i = n % COL_COUNT;
    j = n / COL_COUNT;
}

#define COL_MOVE	(1)
#define COL_TAP		(2)
#define COL_DBLTAP	(3)
#define COL_LONGTAP	(4)
//#define COL_ZOOM	(5)

#define ROW_SINGLE	(1)
#define ROW_FINGERS	(2)
#define ROW_PALM	(3)
#define ROW_PUTNTAP	(4)

#define EXTP_INDIVIDUAL_PUTNTAP                 (1)
#define EXTP_INERTIAL_WHEEL			(0)
#define EXTP_PUTAND_ON_MULTITOUCHDEVICE         (2)
#define EXTP_MAC_OS_MARGIN                      (3)
#define EXTP_INVERT_WHEEL			(4)
#define EXTP_ENABLE_SINGLE_DBLTAP               (5)
#define EXTP_ENABLE_SINGLE_TAP                  (6)
#define EXTP_ENABLE_SINGLE_MOVE                 (7)

QMouseMappingTable::QMouseMappingTable(QWidget *parent) :
    QLabel(parent)
{
    setFont( parent->font() );

    m_pT3kHandle = NULL;
    m_pSelectCell = NULL;
    m_pHoverCell = NULL;
    m_bIsTitleOver = false;
    m_bIsHovered = false;

    m_nProfileIndex = 1;
    m_wProfileFlags = 0x00;

    m_nCurInputMode = -1;

    m_bExtSet = false;

    setMouseTracking( true );
    installEventFilter( this );
    setAttribute( Qt::WA_Hover );

    Init();
}

QMouseMappingTable::~QMouseMappingTable()
{
    for( int nI=0 ; nI<m_vCell.count() ; nI++ )
    {
        if( GetAt(nI)->pIconImage )
            delete GetAt(nI)->pIconImage;
    }
    m_vCell.remove( 0, m_vCell.count() );

    if( m_pEditActionWnd )
    {
        delete m_pEditActionWnd;
        m_pEditActionWnd = NULL;
    }
    if( m_pEditAction2WDWnd )
    {
        delete m_pEditAction2WDWnd;
        m_pEditAction2WDWnd = NULL;
    }

    if( m_pEditAction4WDWnd )
    {
        delete m_pEditAction4WDWnd;
        m_pEditAction4WDWnd = NULL;
    }

    if( m_pEditActionEDWnd )
    {
        delete m_pEditActionEDWnd;
        m_pEditActionEDWnd = NULL;
    }
}

void QMouseMappingTable::SetProfileIndex(int nIndex)
{
    m_nProfileIndex = nIndex;
    update();
}

void QMouseMappingTable::Init()
{
    m_vCell.reserve( ROW_COUNT*COL_COUNT );
    m_vCell.clear();

    CellInfo	Dummy;
    Dummy.InitCellInfo();
    m_ciRotateHead.InitCellInfo();
    m_ciZoomHead.InitCellInfo();
    m_ciRotate.InitCellInfo();
    m_ciZoom.InitCellInfo();

    for( int i=0 ; i<COL_COUNT*ROW_COUNT ; i++ )
            m_vCell.push_back( Dummy );
    for( int i=0 ; i<COL_COUNT ; i++ )
            GetAt( TABLE(i, 0) )->bBold = true;
    for( int j=0 ; j<ROW_COUNT ; j++ )
            GetAt( TABLE(0, j) )->bBold = true;

    m_ciZoomHead.bBold = true;

    GetAt( TABLE(0, 0) )->bEnable = false;

    //m_ciRotateHead.pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_ROTATE );
    //m_ciZoomHead.pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_ZOOM );

    //GetAt( TABLE(0, 1) )->pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_SINGLE );
    //GetAt( TABLE(0, 2) )->pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_FINGERS );
    //GetAt( TABLE(0, 3) )->pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_PALM );
    //GetAt( TABLE(0, 4) )->pIconImage = GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_GST_PUTNTAP );

    GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->strText		= mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_MOVE);
    GetAt( TABLE(COL_TAP, ROW_SINGLE) )->strText		= mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_CLICK);
    GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->strText             = mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK);

    GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->bDefault		= true;
    GetAt( TABLE(COL_TAP, ROW_SINGLE) )->bDefault		= true;
    GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->bDefault            = true;

    GetAt( TABLE(COL_MOVE, ROW_PUTNTAP) )->bNoUse		= true;

    GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->cKey  		= MM_GESTURE_SINGLE_MOVE;
    GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->wKeyValue[0]  	= MM_MOUSEPROFILE_SINGLE_MOVE;
    GetAt( TABLE(COL_TAP, ROW_SINGLE) )->cKey           	= MM_GESTURE_SINGLE_TAP;
    GetAt( TABLE(COL_TAP, ROW_SINGLE) )->wKeyValue[0]   	= MM_MOUSEPROFILE_SINGLE_TAP;
    GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->cKey        	= MM_GESTURE_SINGLE_DOUBLE_TAP;
    GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->wKeyValue[0]	= MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP;

    GetAt( TABLE(COL_LONGTAP, ROW_SINGLE) )->cKey		= MM_GESTURE_SINGLE_LONG_TAP;
    GetAt( TABLE(COL_MOVE, ROW_FINGERS) )->cKey                 = MM_GESTURE_FINGERS_MOVE;
    GetAt( TABLE(COL_TAP, ROW_FINGERS) )->cKey          	= MM_GESTURE_FINGERS_TAP;
    GetAt( TABLE(COL_DBLTAP, ROW_FINGERS) )->cKey		= MM_GESTURE_FINGERS_DOUBLE_TAP;
    GetAt( TABLE(COL_LONGTAP, ROW_FINGERS) )->cKey		= MM_GESTURE_FINGERS_LONG_TAP;
    GetAt( TABLE(COL_MOVE, ROW_PALM) )->cKey            	= MM_GESTURE_PALM_MOVE;
    GetAt( TABLE(COL_TAP, ROW_PALM) )->cKey			= MM_GESTURE_PALM_TAP;
    GetAt( TABLE(COL_DBLTAP, ROW_PALM) )->cKey          	= MM_GESTURE_PALM_DOUBLE_TAP;
    GetAt( TABLE(COL_LONGTAP, ROW_PALM) )->cKey                 = MM_GESTURE_PALM_LONG_TAP;
    GetAt( TABLE(COL_TAP, ROW_PUTNTAP) )->cKey          	= MM_GESTURE_PUTAND_TAP;
    GetAt( TABLE(COL_DBLTAP, ROW_PUTNTAP) )->cKey		= MM_GESTURE_PUTAND_DOUBLE_TAP;
    GetAt( TABLE(COL_LONGTAP, ROW_PUTNTAP) )->cKey		= MM_GESTURE_PUTAND_LONG_TAP;
    m_ciZoom.cKey                                   		= MM_GESTURE_ZOOM;

    GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->eKeyType		= KeyTypeEnable;
    GetAt( TABLE(COL_TAP, ROW_SINGLE) )->eKeyType		= KeyTypeEnable;
    GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->eKeyType            = KeyTypeEnable;
    GetAt( TABLE(COL_LONGTAP, ROW_SINGLE) )->eKeyType           = KeyType1Key;
    GetAt( TABLE(COL_MOVE, ROW_FINGERS) )->eKeyType		= KeyType4Way;
    GetAt( TABLE(COL_TAP, ROW_FINGERS) )->eKeyType		= KeyType1Key;
    GetAt( TABLE(COL_DBLTAP, ROW_FINGERS) )->eKeyType           = KeyType1Key;
    GetAt( TABLE(COL_LONGTAP, ROW_FINGERS) )->eKeyType          = KeyType1Key;
    GetAt( TABLE(COL_MOVE, ROW_PALM) )->eKeyType		= KeyType4Way;
    GetAt( TABLE(COL_TAP, ROW_PALM) )->eKeyType          	= KeyType1Key;
    GetAt( TABLE(COL_DBLTAP, ROW_PALM) )->eKeyType		= KeyType1Key;
    GetAt( TABLE(COL_LONGTAP, ROW_PALM) )->eKeyType		= KeyType1Key;
    GetAt( TABLE(COL_TAP, ROW_PUTNTAP) )->eKeyType		= KeyType1Key;
    GetAt( TABLE(COL_DBLTAP, ROW_PUTNTAP) )->eKeyType           = KeyType1Key;
    GetAt( TABLE(COL_LONGTAP, ROW_PUTNTAP) )->eKeyType          = KeyType1Key;
    m_ciZoom.eKeyType        					= KeyType2Way;

    m_bCheckExtProperty[EXTP_INVERT_WHEEL]                      = false;
    m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP]                = false;
    m_bCheckExtProperty[EXTP_INERTIAL_WHEEL]                    = false;
    m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]        = false;
    m_bCheckExtProperty[EXTP_MAC_OS_MARGIN]                     = false;

    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP]                 = false;
    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP]              = false;
    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE]                = false;

    m_pEditActionWnd = new QEditActionWnd(m_pT3kHandle, this);
    m_pEditActionWnd->setFont( font() );
    m_pEditActionWnd->setModal( true );
    m_pEditActionEDWnd = new QEditActionEDWnd(m_pT3kHandle, this);
    m_pEditActionEDWnd->setFont( font() );
    m_pEditActionEDWnd->setModal( true );
    m_pEditAction2WDWnd = new QEditAction2WDWnd(m_pT3kHandle, this);
    m_pEditAction2WDWnd->setFont( font() );
    m_pEditAction2WDWnd->setModal( true );
    m_pEditAction4WDWnd = new QEditAction4WDWnd(m_pT3kHandle, this);
    m_pEditAction4WDWnd->setFont( font() );
    m_pEditAction4WDWnd->setModal( true );

    OnChangeLanguage();
}

#define EXT_HMARGIN 19
void QMouseMappingTable::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        QMatrix mtxR2L( -1.0, 0.0, 0.0, 1.0, width(), 0.0 );
        dc.setMatrix( mtxR2L );
    }
    dc.setRenderHint( QPainter::TextAntialiasing );

    QRect rcClient( 0, 0, width()-1, height()-1 );
    QRect rectBody( rcClient );
    rectBody.adjust( 2, 2, -2, 0 );

    QRect rectTable( rectBody );

    rectTable.setHeight( rectBody.height() - (rectTable.height()  - EXT_HMARGIN) / (ROW_COUNT-2) - EXT_HMARGIN );
    QRectF rectExtProperty;
    rectExtProperty.moveTo( (qreal)rectTable.x(), (qreal)rectTable.bottom() );
    rectExtProperty.setWidth( (qreal)rectTable.width() );
    rectExtProperty.setHeight( (qreal)rectBody.height() - rectTable.height() );

    // table 5x5
    // Draw Table
    qreal fTableIntervalV = (qreal)rectTable.height() / (ROW_COUNT*2 - 1);
    qreal fTableIntervalH = (qreal)rectTable.width() / (COL_COUNT*2);

    QPen GridPen( QColor(100, 100, 100) );

    qreal fLeft = (qreal)rectTable.left();
    qreal fRight = (qreal)rectTable.right();
    qreal fTop = (qreal)rectTable.top();
    qreal fBottom = (qreal)rectTable.bottom();

    for( int j=0 ; j<ROW_COUNT ; j++ )
    {
        for( int i=0 ; i<COL_COUNT ; i++ )
        {
            CellInfo* ci = GetAt( TABLE(i, j) );
            if( i != 0 )
            {
                CellInfo* ciPrev = GetAt( TABLE(i-1, j) );
                ci->rectCell.moveLeft( ciPrev->rectCell.right() );
            }
            else
                ci->rectCell.moveLeft( fLeft );
            if( j != 0 )
            {
                CellInfo* ciPrev = GetAt( TABLE(i, j-1) );
                ci->rectCell.moveTop( ciPrev->rectCell.bottom() );
            }
            else
                ci->rectCell.moveTop( fTop );

            if( i == 0 )
                ci->rectCell.setWidth( fTableIntervalH*2.f );
            else
                ci->rectCell.setWidth( fTableIntervalH*2.f );
            if( j == 0 )
                ci->rectCell.setHeight( fTableIntervalV*1.f );
            else
                ci->rectCell.setHeight( fTableIntervalV*2.f );
        }
    }

    // head color
    for( int i=0 ; i<COL_COUNT ; i++ )
    {
        CellInfo* ci = GetAt( TABLE(i, 0) );
        dc.fillRect( ci->rectCell, QColor(219, 228, 242, 255) );
    }
    // head color
    for( int j=0 ; j<ROW_COUNT ; j++ )
    {
        CellInfo* ci = GetAt( TABLE(0, j) );
        dc.fillRect( ci->rectCell, QColor(219, 228, 242, 255) );
    }

    QFont fntCellNormal( font() );
    fntCellNormal.setPixelSize( 11 );
    fntCellNormal.setBold( false );
    QFont fntCellBold( fntCellNormal );
    fntCellBold.setBold( true );

    dc.setRenderHint( QPainter::TextAntialiasing );

    // draw cell info
    for( int j=0 ; j<ROW_COUNT ; j++ )
    {
        for( int i=0 ; i<COL_COUNT ; i++ )
        {
            CellInfo* ci = GetAt( TABLE(i, j) );

            if( ci == m_pSelectCell )
            {
                dc.fillRect( ci->rectCell, QColor(255,240,240) );
            }
            else if( ci == m_pHoverCell )
            {
                dc.fillRect( ci->rectCell, QColor(240,240,255) );
            }
            DrawCellInfo( dc, *ci, fntCellNormal, fntCellBold, Qt::AlignVCenter | Qt::AlignHCenter, font().toString() );
        }
    }

    // table lines
    dc.setPen( QColor(100,100,100) );
    qreal fY = fTop;
    for( int j=1 ; j<ROW_COUNT ; j++ )
    {
        if( j == 1 )
            fY += fTableIntervalV;
        else
            fY += fTableIntervalV * 2.f;
        dc.drawLine( fLeft, fY, fRight, fY );
    }
    qreal fX = fLeft;
    for( int i=1 ; i<COL_COUNT ; i++ )
    {
        if( i == 1 )
            fX += fTableIntervalH * 2.f;
        else
            fX += fTableIntervalH * 2.f;
        dc.drawLine( fX, fTop, fX, fBottom );
    }

    GridPen.setWidth( 2 );
    dc.setPen( GridPen );
    dc.drawRect( rectTable );

    QRect rectZoomRotate;
    rectZoomRotate.moveTo( rectTable.x() + rectTable.width()*3/5, rectTable.bottom() + 4 );
    rectZoomRotate.setWidth( rectTable.width()*2/5 );
    rectZoomRotate.setHeight( rectBody.height() - rectTable.height() - 4 - EXT_HMARGIN );

    m_ciZoomHead.rectCell.moveTo( (qreal)rectZoomRotate.x(), (qreal)rectZoomRotate.y() );
    m_ciZoomHead.rectCell.setWidth( fTableIntervalH*2.f );
    m_ciZoomHead.rectCell.setHeight( rectZoomRotate.height()/2.f );

    m_ciZoom.rectCell.moveTo( (qreal)rectZoomRotate.x() + fTableIntervalH*2.f, (qreal)rectZoomRotate.y() );
    m_ciZoom.rectCell.setWidth( (qreal)rectZoomRotate.width() - fTableIntervalH*2.f );
    m_ciZoom.rectCell.setHeight( rectZoomRotate.height()/2.f );

    m_ciRotateHead.rectCell.moveTo( (qreal)rectZoomRotate.x(), (qreal)rectZoomRotate.y() + rectZoomRotate.height()/2.f );
    m_ciRotateHead.rectCell.setWidth( fTableIntervalH*2.f );
    m_ciRotateHead.rectCell.setHeight( rectZoomRotate.height()/2.f );

    m_ciRotate.rectCell.moveTo( (qreal)rectZoomRotate.x() + fTableIntervalH*2.f, (qreal)rectZoomRotate.y() + rectZoomRotate.height()/2.f );
    m_ciRotate.rectCell.setWidth( (qreal)rectZoomRotate.width() - fTableIntervalH*2.f );
    m_ciRotate.rectCell.setHeight( rectZoomRotate.height()/2.f );

    // head color (zoom/rotate)
    dc.fillRect( m_ciZoomHead.rectCell, QColor(219, 228, 242) );
#ifdef SUPPORT_ROTATE
    dc.fillRect( m_ciRotateHead.rectCell, QColor(219, 228, 242) );
#endif

    DrawCellInfo( dc, m_ciZoomHead, fntCellNormal, fntCellBold, Qt::AlignVCenter | Qt::AlignHCenter, font().toString() );
#ifdef SUPPORT_ROTATE
    DrawCellInfo( dc, m_ciRotateHead, fntCellNormal, fntCellBold, Qt::AlignVCenter | Qt::AlignHCenter, font().toString() );
#endif

    if( &m_ciZoom == m_pSelectCell )
        dc.fillRect( m_ciZoom.rectCell, QColor(255,240,240) );
    else if( &m_ciZoom == m_pHoverCell )
        dc.fillRect( m_ciZoom.rectCell, QColor(240,240,255) );

    DrawCellInfo( dc, m_ciZoom, fntCellNormal, fntCellBold, Qt::AlignVCenter | Qt::AlignHCenter, font().toString() );
#ifdef SUPPORT_ROTATE
    if( &m_ciRotate == m_pSelectCell )
    {
        dc.fillRect( m_ciRotate.rectCell, QColor(255,240,240) );
    }
    else if( &m_ciRotate == m_pHoverCell )
    {
        dc.fillRect( m_ciRotate.rectCell, QColor(240,240,255) );
    }
    DrawCellInfo( dc, m_ciRotate, fntCellNormal, fntCellBold, Qt::AlignVCenter | Qt::AlignHCenter, font().toString() );
#endif

    // grid (zoom/rotate)
    int nCY = rectZoomRotate.y() + rectZoomRotate.height()/2;
    GridPen.setWidth( 1 );
    dc.setPen( GridPen );
    dc.drawLine( rectZoomRotate.x(), nCY, rectZoomRotate.right(), nCY );
    int nCX = int(rectZoomRotate.x() + fTableIntervalH*2.f);
#ifdef SUPPORT_ROTATE
    dc.drawLine( nCX, rectZoomRotate.y(), nCX, rectZoomRotate.bottom() );
#else
    dc.drawLine( nCX, rectZoomRotate.y(), nCX, nCY );
#endif

    GridPen.setWidth( 2 );
    dc.setPen( GridPen );
#ifdef SUPPORT_ROTATE
    dc.drawRect( rectZoomRotate );
#else
    rectZoomRotate.setHeight( rectZoomRotate.height()/2 );
    dc.drawRect( rectZoomRotate );
#endif

    // draw ext-property
    rectExtProperty.adjust( 0, 3, 0, 0 );

    DrawExtProperty( dc, EXTP_INERTIAL_WHEEL, rectExtProperty, &fntCellNormal, Qt::AlignLeft | Qt::AlignVCenter );
    DrawExtProperty( dc, EXTP_INDIVIDUAL_PUTNTAP, rectExtProperty, &fntCellNormal, Qt::AlignLeft | Qt::AlignVCenter );
    DrawExtProperty( dc, EXTP_PUTAND_ON_MULTITOUCHDEVICE, rectExtProperty, &fntCellNormal, Qt::AlignLeft | Qt::AlignVCenter );

    QT3kUserData* pUD = QT3kUserData::GetInstance();
    float fVer = pUD->GetFirmwareVersion();
    if( fVer >= MM_MIN_SUPPORT_MACMARGIN_N_INVERTWHEEL_VERSION )
    {
        DrawExtProperty( dc, EXTP_MAC_OS_MARGIN, rectExtProperty, &fntCellNormal, Qt::AlignLeft | Qt::AlignVCenter );
        DrawExtProperty( dc, EXTP_INVERT_WHEEL, rectExtProperty, &fntCellNormal, Qt::AlignLeft | Qt::AlignVCenter );
    }

    if( !isEnabled() )
    {
        dc.fillRect( 0, 0, width()-1, height()-1, QColor(255,255,255,128));
    }

    dc.end();
}

void QMouseMappingTable::DrawCellInfo( QPainter& dc, CellInfo& ci, QFont& fntCellNormal, QFont& fntCellBold, int nStringFlags, QString strFamily )
{
    if( ci.bDefault && (m_pSelectCell != &ci) )
        dc.fillRect( ci.rectCell, QColor(240, 240, 240) );
    if( ci.bNoUse && (m_pSelectCell != &ci) )
        dc.fillRect( ci.rectCell, QBrush( QColor(50, 50, 50) , Qt::Dense6Pattern ) );

//    stringFormat.SetAlignment( StringAlignmentCenter );

    if( ci.bEnable && !ci.bNoUse && ci.strText.isEmpty() )
    {
        dc.setFont( ci.bBold ? fntCellBold : fntCellNormal );
        PainterDrawText( dc, ci.rectCell, nStringFlags, "-" );
    }
    else
    {
        if( ci.pIconImage )
        {
            int nIconX, nIconY, nIconW, nIconH;
            nIconW = ci.pIconImage->width();
            nIconH = ci.pIconImage->height();
            nIconX = (int)(( ci.rectCell.width() - nIconW ) / 2 + ci.rectCell.x()); //(int)( ci.rectCell.X + 5 );
            nIconY = (int)(( ci.rectCell.height() - nIconH ) / 2 + ci.rectCell.y());
            dc.drawImage( QRect(nIconX, nIconY, nIconW, nIconH), *ci.pIconImage, QRect(0, 0, nIconW, nIconH) );

            //nTextOffset = 32 + 5;
            //stringFormat.SetAlignment( StringAlignmentNear );
        }
        else
        {
            if( ci.bBold )
            {
                dc.setPen( QColor(30,53,85) );
                dc.setFont( fntCellBold );
                PainterDrawText( dc, ci.rectCell, nStringFlags, ci.strText );
            }
            else
            {
                QFontMetricsF ftMetrics( fntCellNormal );
                ftMetrics.width( ci.strText );
                qreal ftH = ftMetrics.height();
                if( ftH > 30 )
                {
                    float fR = 30.f / ftH;
                    QFont fntVariable( strFamily );
                    fntVariable.setPixelSize( 12*fR );
                    dc.setFont( fntVariable );
                }
                else
                    dc.setFont( fntCellNormal );
                PainterDrawText( dc, ci.rectCell, nStringFlags, ci.strText );
            }
        }
    }
}

void QMouseMappingTable::DrawExtProperty( QPainter& dc, int nExtIndex, QRectF &rectExtProperty, QFont* pfntCellNormal, int nStringFlags )
{
    dc.setRenderHint( QPainter::Antialiasing );

    QRectF rectCheck;
    float fCheckWH = 8.f;

    m_rectExtProperty[nExtIndex] = rectExtProperty;
    m_rectExtProperty[nExtIndex].setWidth( rectExtProperty.width()/5.f*3.f );
    m_rectExtProperty[nExtIndex].setHeight( rectExtProperty.height()/5.f );

    m_rectExtProperty[nExtIndex].translate( 0, m_rectExtProperty[nExtIndex].height()*nExtIndex );

    rectCheck = m_rectExtProperty[nExtIndex];
    rectCheck.setWidth( m_rectExtProperty[nExtIndex].height() );
    int nV = (rectCheck.width()-fCheckWH)/4.f;
    rectCheck.adjust( nV, nV, -nV, -nV );

    QRectF rectInner = rectCheck;
    rectInner.adjust( 2, 2, -2, -2 );

    dc.fillRect( rectCheck, QColor(80, 80, 80) );
    dc.fillRect( rectInner, QColor(255, 255, 255) );

    if( m_bCheckExtProperty[nExtIndex] )
    {
        rectInner.adjust( 1, 1, -1, -1 );
        dc.fillRect( rectInner, QColor(80, 80, 80) );
    }

    dc.setRenderHint( QPainter::TextAntialiasing );

    const int nOffsetX = 3;
    QFont ftExt( *pfntCellNormal );
    ftExt.setPixelSize( pfntCellNormal->pixelSize()+1 );
    dc.setFont( ftExt );
    m_rectExtProperty[nExtIndex].translate( rectCheck.width() + fCheckWH + nOffsetX, 0 );
    m_rectExtProperty[nExtIndex].setWidth( m_rectExtProperty[nExtIndex].width() - (rectCheck.width() + fCheckWH + nOffsetX) );

    PainterDrawText( dc, m_rectExtProperty[nExtIndex], nStringFlags, m_strExtProperty[nExtIndex] );

    QFontMetricsF ftMetrics( *pfntCellNormal );
    qreal ftW = ftMetrics.width( m_strExtProperty[nExtIndex] );

    m_rectExtProperty[nExtIndex].translate( -(rectCheck.width() + fCheckWH + nOffsetX), 0 );
    m_rectExtProperty[nExtIndex].setWidth( ftW + rectCheck.width() + fCheckWH + nOffsetX );

    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        m_rectExtProperty[nExtIndex].moveLeft( width()-m_rectExtProperty[nExtIndex].width() );
    }
}

void QMouseMappingTable::PainterDrawText(QPainter& dc, QRectF &rectangle, int flags, const QString &text, QRectF *boundingRect)
{
    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        QMatrix mtxL2R( 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 );
        dc.setMatrix( mtxL2R );

        rectangle.moveLeft( width()-rectangle.x()-rectangle.width() );
    }
    dc.drawText( rectangle, flags, text, boundingRect );
    if( QApplication::layoutDirection() == Qt::RightToLeft )
    {
        QMatrix mtxR2L( -1.0, 0.0, 0.0, 1.0, width(), 0.0 );
        dc.setMatrix( mtxR2L );
    }
}

void QMouseMappingTable::OnChangeLanguage()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    GetAt( TABLE(COL_MOVE, 0) )->strText                        = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DRAG") );
    GetAt( TABLE(COL_TAP, 0) )->strText                         = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_TAP") );
    GetAt( TABLE(COL_DBLTAP, 0) )->strText                      = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DOUBLE_TAP") );
    GetAt( TABLE(COL_LONGTAP, 0) )->strText                     = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_LONG_TAP") );
    m_ciRotateHead.strText                                      = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_ROTATE") );
    m_ciZoomHead.strText                                        = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_ZOOM") );
    GetAt( TABLE(0, 1) )->strText				= Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_SINGLE") );
    GetAt( TABLE(0, 2) )->strText				= Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_FINGERS") );
    GetAt( TABLE(0, 3) )->strText				= Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") );
    GetAt( TABLE(0, 4) )->strText				= Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PUTNTAP") );

    m_strExtProperty[EXTP_INDIVIDUAL_PUTNTAP]			= Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_INDIVIDUAL_PUTNTAP_DOUBLE_TAP") );
    m_strExtProperty[EXTP_INERTIAL_WHEEL]                       = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_INERTIAL_WHEEL") );
    m_strExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]           = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PUTAND_ON_MULTITOUCHDEVICE") );
    m_strExtProperty[EXTP_MAC_OS_MARGIN]                        = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_MAC_OS_MARGIN") );
    m_strExtProperty[EXTP_INVERT_WHEEL]                         = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_INVERT_WHEEL") );
    //m_strExtProperty[EXTP_ENABLE_DBLTAP]                      = _T("Enable Double-Tap");*/
}

inline bool Extract2Word( QString& str, uchar& cV )
{
    QString strWord( str.left( 2 ) );
    if( !strWord.size() ) return false;
    str.remove( 0, 2 );
    cV = (uchar)strtol( strWord.toStdString().c_str(), NULL, 16 );
    return true;
}

void QMouseMappingTable::OnRSP(ResponsePart /*Part*/, ushort /*nTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !winId() ) return;

    bool bParseProfile = false;

    switch( m_nProfileIndex )
    {
    case 0:
        if ( strstr(sCmd, cstrMouseProfile1) == sCmd )
            bParseProfile = true;
        break;
    case 1:
        if ( strstr(sCmd, cstrMouseProfile2) == sCmd )
            bParseProfile = true;
        break;
    case 2:
        if ( strstr(sCmd, cstrMouseProfile3) == sCmd )
            bParseProfile = true;
        break;
    case 3:
        if ( strstr(sCmd, cstrMouseProfile4) == sCmd )
            bParseProfile = true;
        break;
    case 4:
        if ( strstr(sCmd, cstrMouseProfile5) == sCmd )
            bParseProfile = true;
        break;
    default:
        if ( strstr(sCmd, cstrMouseProfile2) == sCmd )
            bParseProfile = true;
        break;
    }

    if ( bParseProfile )
    {
        ParseMouseProfile( sCmd );
    }
}

void QMouseMappingTable::onRecvInputMode(int nCurInputMode)
{
    m_nCurInputMode = nCurInputMode;

    if( m_bExtSet ) return;
    if( !m_wProfileFlags ) return;
    QString strCmd;
    switch( m_nProfileIndex )
    {
    case 0:
        strCmd = QString("%1?").arg(cstrMouseProfile1);
        break;
    case 1:
        strCmd = QString("%1?").arg(cstrMouseProfile2);
        break;
    case 2:
        strCmd = QString("%1?").arg(cstrMouseProfile3);
        break;
    case 3:
        strCmd = QString("%1?").arg(cstrMouseProfile4);
        break;
    case 4:
        strCmd = QString("%1?").arg(cstrMouseProfile5);
        break;
    }

    m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );
}

void QMouseMappingTable::SetCellInfo( int nCol, int nRow, uchar cV0, uchar cV1 )
{
    CellInfo* ci = GetAt(TABLE(nCol, nRow));

    ci->wKeyValue[0] = (short)cV0 << 8 | cV1;
    ci->strText = getMappingStr( cV0, cV1 );
}

void QMouseMappingTable::SetCellInfo( int nCol, int nRow, uchar cV00, uchar cV01, uchar cV10, uchar cV11, uchar cV20, uchar cV21, uchar cV30, uchar cV31 )
{
    CellInfo* ci = GetAt(TABLE(nCol, nRow));

    ci->wKeyValue[0] = (short)cV00 << 8 | cV01;
    ci->wKeyValue[1] = (short)cV10 << 8 | cV11;
    ci->wKeyValue[2] = (short)cV20 << 8 | cV21;
    ci->wKeyValue[3] = (short)cV30 << 8 | cV31;

    QString strKey1, strKey2, strKey3, strKey4;
    strKey1 = getMappingStr( cV00, cV01 );
    strKey1.replace( ("\r\n"), (" ") );
    strKey2 = getMappingStr( cV10, cV11 );
    strKey2.replace( ("\r\n"), (" ") );
    strKey3 = getMappingStr( cV20, cV21 );
    strKey3.replace( ("\r\n"), (" ") );
    strKey4 = getMappingStr( cV30, cV31 );
    strKey4.replace( ("\r\n"), (" ") );
    QString strLR, strUD;
    if( strKey1 == strKey2 )
    {
        strLR = strKey1;
    }
    else
    {
        strLR = QString("%1/%2").arg(strKey1).arg(strKey2);
    }

    if( strKey3 == strKey4 )
    {
        strUD = strKey3;
    }
    else
    {
        strUD = QString("%1/%2").arg(strKey3).arg(strKey4);
    }

    if( strLR == strUD )
    {
        strLR.replace( (" "), ("\r\n") );
        ci->strText = strLR;
    }
    else
    {
        ci->strText = QString("%1\n%2").arg(strLR).arg(strUD);
    }
}

void QMouseMappingTable::ParseMouseProfile( const char* szProfile )
{
    QString strProfile( szProfile );

    int nE = strProfile.indexOf( ('=') );
    if( nE < 0 ) return;

    strProfile.remove( 0, nE+1 );

    do
    {
        uchar cKey = 0;
        if( !Extract2Word( strProfile, cKey ) ) break;
        uchar cV[4][2];
        ushort wFlags;

        switch( cKey )
        {
        case 0x00:		// profile_flags
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            wFlags = (ushort)cV[0][0] << 8 | cV[0][1];

            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP]                      = MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP]                         = MM_MOUSEPROFILE_SINGLE_TAP & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE]                        = MM_MOUSEPROFILE_SINGLE_MOVE & wFlags ? true : false;

            GetAt( TABLE(COL_MOVE, ROW_SINGLE) )->strText                       = mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE] ? MM_MOUSE_KEY1_MOUSE_L_MOVE : 0);
            GetAt( TABLE(COL_TAP, ROW_SINGLE) )->strText                        = mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP] ? MM_MOUSE_KEY1_MOUSE_L_CLICK : 0);
            GetAt( TABLE(COL_DBLTAP, ROW_SINGLE) )->strText                     = mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP] ? MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK : 0);

            m_bCheckExtProperty[EXTP_INVERT_WHEEL]                              = MM_MOUSEPROFILE_INVERT_WHEEL & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP]                        = MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_INERTIAL_WHEEL]                            = MM_MOUSEPROFILE_INERTIAL_WHEEL & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]                = MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_MAC_OS_MARGIN]                             = MM_MOUSEPROFILE_MAC_OS_MARGIN & wFlags ? true : false;

            m_wProfileFlags = wFlags;

            switch( m_nCurInputMode )
            {
            case 0:
                for( int j=1 ; j<ROW_COUNT ; j++ )
                {
                    for( int i=1 ; i<COL_COUNT ; i++ )
                    {
                        CellInfo* ci = GetAt( TABLE(i, j) );
                        ci->bNoUse = false;
                    }
                }

                GetAt( TABLE(1,4) )->bNoUse = true;

                m_ciZoom.bNoUse = false;
                m_ciRotate.bNoUse = false;
                break;
            case 1:
            case 2:
                if( m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE] )
                {
                    for( int j=1 ; j<ROW_COUNT-2 ; j++ )
                    {
                        for( int i=1 ; i<COL_COUNT ; i++ )
                        {
                            CellInfo* ci = GetAt( TABLE(i, j) );
                            ci->bNoUse = true;
                        }
                    }

                    for( int j=ROW_COUNT-2 ; j<ROW_COUNT ; j++ )
                    {
                        for( int i=1 ; i<COL_COUNT ; i++ )
                        {
                            CellInfo* ci = GetAt( TABLE(i, j) );
                            ci->bNoUse = false;
                        }
                    }

                    GetAt( TABLE(1,4) )->bNoUse = true;
                }
                else
                {
                    for( int j=1 ; j<ROW_COUNT ; j++ )
                    {
                        for( int i=1 ; i<COL_COUNT ; i++ )
                        {
                            CellInfo* ci = GetAt( TABLE(i, j) );
                            ci->bNoUse = true;
                        }
                    }
                }

                m_ciZoom.bNoUse = true;
                m_ciRotate.bNoUse = true;
                break;
            default:
                break;
            }

            update();

            qDebug( "Profile Flags: %04X\r\n", wFlags );
            break;

        case MM_GESTURE_SINGLE_LONG_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;

            SetCellInfo( COL_LONGTAP, ROW_SINGLE, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_MOVE:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            if( !Extract2Word( strProfile, cV[1][0] ) ) break;
            if( !Extract2Word( strProfile, cV[1][1] ) ) break;
            if( !Extract2Word( strProfile, cV[2][0] ) ) break;
            if( !Extract2Word( strProfile, cV[2][1] ) ) break;
            if( !Extract2Word( strProfile, cV[3][0] ) ) break;
            if( !Extract2Word( strProfile, cV[3][1] ) ) break;
            SetCellInfo( COL_MOVE, ROW_FINGERS, cV[0][0], cV[0][1], cV[1][0], cV[1][1], cV[2][0], cV[2][1], cV[3][0], cV[3][1] );
            update();
            // LRUD
            break;

        case MM_GESTURE_FINGERS_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_TAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_DOUBLE_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_DBLTAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_LONG_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_LONGTAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_MOVE:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            if( !Extract2Word( strProfile, cV[1][0] ) ) break;
            if( !Extract2Word( strProfile, cV[1][1] ) ) break;
            if( !Extract2Word( strProfile, cV[2][0] ) ) break;
            if( !Extract2Word( strProfile, cV[2][1] ) ) break;
            if( !Extract2Word( strProfile, cV[3][0] ) ) break;
            if( !Extract2Word( strProfile, cV[3][1] ) ) break;
            SetCellInfo( COL_MOVE, ROW_PALM, cV[0][0], cV[0][1], cV[1][0], cV[1][1], cV[2][0], cV[2][1], cV[3][0], cV[3][1] );
            update();
            break;

        case MM_GESTURE_PALM_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_TAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_DOUBLE_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_DBLTAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_LONG_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_LONGTAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_TAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_DOUBLE_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_DBLTAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_LONG_TAP:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;
            SetCellInfo( COL_LONGTAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_ZOOM:
            if( !Extract2Word( strProfile, cV[0][0] ) ) break;
            if( !Extract2Word( strProfile, cV[0][1] ) ) break;

            if( !Extract2Word( strProfile, cV[1][0] ) ) break;
            if( !Extract2Word( strProfile, cV[1][1] ) ) break;

            m_ciZoom.wKeyValue[0] = (short)cV[0][0] << 8 | cV[0][1];
            m_ciZoom.wKeyValue[1] = (short)cV[1][0] << 8 | cV[1][1];

            {
                QString strKey1, strKey2;
                strKey1 = getMappingStr( cV[0][0], cV[0][1] );
                strKey1.replace( ("\r\n"), (" ") );
                strKey2 = getMappingStr( cV[1][0], cV[1][1] );
                strKey2.replace( ("\r\n"), (" ") );
                QString strZoom;
                if( strKey1 == strKey2 )
                {
                    strZoom = strKey1;
                }
                else
                {
                    strZoom = QString("%1\r\n%2").arg(strKey1).arg(strKey2);
                }

                m_ciZoom.strText = strZoom;
            }
            //SetZoomInfo( cV[0][0], cV[0][1], cV[1][0], cV[1][1] );
            update();
            break;

        default:
            break;
        }

        int nC = strProfile.indexOf( (' ') );
        if( nC >= 0 )
        {
            strProfile.remove( 0, nC+1 );
        }
    } while( strProfile.size() );
}

void QMouseMappingTable::PopEditActionWnd( CellInfo& ci, int nRow, int /*nCol*/ )
{
    switch( ci.eKeyType )
    {
    case KeyType1Key:
        m_pEditActionWnd->SetProfileInfo( m_nProfileIndex, ci.cKey, ci.wKeyValue[0], nRow == ROW_PUTNTAP ? true : false );
        m_pEditActionWnd->show();
        break;

    case KeyTypeEnable:
        m_pEditActionEDWnd->SetProfileInfo( m_nProfileIndex, ci.cKey, ci.wKeyValue[0], m_wProfileFlags );
        m_pEditActionEDWnd->show();
        break;

    case KeyType4Way:
        m_pEditAction4WDWnd->SetProfileInfo( m_nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1], ci.wKeyValue[2], ci.wKeyValue[3] );
        m_pEditAction4WDWnd->show();
        break;

    case KeyType2Way:
        m_pEditAction2WDWnd->SetProfileInfo( m_nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1] );
        m_pEditAction2WDWnd->show();
        break;

    case KeyTypeNone:
        break;
    }
}

void QMouseMappingTable::ResetSelect()
{
    m_pSelectCell = NULL;
    update();
}

void QMouseMappingTable::mousePressEvent(QMouseEvent *ev)
{
    switch( ev->button() )
    {
    case Qt::LeftButton:
        OnMouseLBDown(ev->localPos());
        break;
    default:
        break;
    }

    QWidget::mousePressEvent(ev);
}

void QMouseMappingTable::OnMouseLBDown(QPointF point)
{
    if( m_pEditActionWnd->isVisible() ||
        m_pEditActionEDWnd->isVisible() ||
        m_pEditAction2WDWnd->isVisible() ||
        m_pEditAction4WDWnd->isVisible() )
        return;

    for( int j=1 ; j<ROW_COUNT ; j++ )
    {
        for( int i=1 ; i<COL_COUNT ; i++ )
        {
            CellInfo* ci = GetAt( TABLE(i, j) );

            if( ci->bEnable && !ci->bNoUse )
            {
                if( ci->rectCell.contains( point ) )
                {
                    PopEditActionWnd( *ci, j, i );
                    m_pSelectCell = ci;
                    update();
                }
            }
        }
    }

    if( ( m_ciZoom.bEnable && !m_ciZoom.bNoUse ) && m_ciZoom.rectCell.contains( point ) )
    {
        PopEditActionWnd( m_ciZoom, 0, 0 );
        m_pSelectCell = &m_ciZoom;
        update();
    }
#ifdef SUPPORT_ROTATE
    if( ( m_ciRotate.bEnable && !m_ciRotate.bNoUse ) && m_ciRotate.rectCell.contains( point ) )
    {
        /*
        CPoint pt = CPoint((int)(m_ciRotate.rectCell.GetLeft()+.5f), (int)(m_ciRotate.rectCell.GetTop()+.5f));
        ClientToScreen( &pt );

        CRect rcWnd;
        GetWindowRect(rcWnd);
        pt = rcWnd.CenterPoint();
        pt.x -= rcPop.Width()/2; pt.y -= rcPop.Height()/2;
        m_wndEditAction.SetWindowPos( NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE );
        m_wndEditAction.ShowWindow( SW_SHOW );
        */
    }
#endif

    m_bExtSet = false;
    if( m_rectExtProperty[EXTP_INERTIAL_WHEEL].contains( point ) )
    {
        if( m_bCheckExtProperty[EXTP_INERTIAL_WHEEL] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_INERTIAL_WHEEL;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_INERTIAL_WHEEL;
        m_bExtSet = true;
    }
    if( m_rectExtProperty[EXTP_INDIVIDUAL_PUTNTAP].contains( point ) )
    {
        if( m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY;
        m_bExtSet = true;
    }
    if( m_rectExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE].contains( point ) )
    {
        if( m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE;
        m_bExtSet = true;
    }
    QT3kUserData* pUD = QT3kUserData::GetInstance();
    float fVer = pUD->GetFirmwareVersion();
    if( fVer >= MM_MIN_SUPPORT_MACMARGIN_N_INVERTWHEEL_VERSION )
    {
        if( m_rectExtProperty[EXTP_INVERT_WHEEL].contains( point ) )
        {
            if( m_bCheckExtProperty[EXTP_INVERT_WHEEL] )
                m_wProfileFlags &= ~MM_MOUSEPROFILE_INVERT_WHEEL;
            else
                m_wProfileFlags |= MM_MOUSEPROFILE_INVERT_WHEEL;
            m_bExtSet = true;
        }
        if( m_rectExtProperty[EXTP_MAC_OS_MARGIN].contains( point ) )
        {
            if( m_bCheckExtProperty[EXTP_MAC_OS_MARGIN] )
                m_wProfileFlags &= ~MM_MOUSEPROFILE_MAC_OS_MARGIN;
            else
                m_wProfileFlags |= MM_MOUSEPROFILE_MAC_OS_MARGIN;
            m_bExtSet = true;
        }
    }

    if( m_bExtSet )
    {
        m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrInputMode).toUtf8().data(), false ); // sync

        QString strCmd;
        switch( m_nProfileIndex )
        {
        case 0:
            strCmd = strCmd.sprintf("%s%02X%04X", cstrMouseProfile1, 0x00, m_wProfileFlags );
            break;
        case 1:
            strCmd = strCmd.sprintf("%s%02X%04X", cstrMouseProfile2, 0x00, m_wProfileFlags );
            break;
        case 2:
            strCmd = strCmd.sprintf("%s%02X%04X", cstrMouseProfile3, 0x00, m_wProfileFlags );
            break;
        case 3:
            strCmd = strCmd.sprintf("%s%02X%04X", cstrMouseProfile4, 0x00, m_wProfileFlags );
            break;
        case 4:
            strCmd = strCmd.sprintf("%s%02X%04X", cstrMouseProfile5, 0x00, m_wProfileFlags );
            break;
        }

        m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );

        m_bExtSet = false;
    }
}

bool QMouseMappingTable::eventFilter(QObject *target, QEvent *evt)
{
    if( isVisible() && isEnabled() )
    {
        if( evt->type() == QEvent::HoverEnter )
        {
            m_bIsHovered = true;
            update();
        }

        if( evt->type() == QEvent::HoverLeave )
        {
            m_bIsHovered = false;
            m_bIsTitleOver = false;
            if( !m_pHoverCell )
                m_pHoverCell = NULL;
            setCursor( QCursor( Qt::ArrowCursor ) );
        }
    }

    return QObject::eventFilter(target,evt);
}

void QMouseMappingTable::mouseMoveEvent(QMouseEvent *evt)
{
    if( m_pEditActionWnd->isVisible() ||
        m_pEditActionEDWnd->isVisible() ||
        m_pEditAction2WDWnd->isVisible() ||
        m_pEditAction4WDWnd->isVisible() )
        return;

    if( m_bIsHovered )
    {
        CellInfo* pHoverCell = NULL;
        bool bIsTitleOver = false;
        do{
        for( int j=1 ; j<ROW_COUNT ; j++ )
        {
            for( int i=1 ; i<COL_COUNT ; i++ )
            {
                CellInfo* ci = GetAt( TABLE(i, j) );

                if( ci->bEnable && !ci->bNoUse )
                {
                    if( ci->rectCell.contains( evt->localPos().x(), evt->localPos().y() ) )
                    {
                        pHoverCell = ci;
                        bIsTitleOver = true;
                        break;
                    }
                }
            }
            if( bIsTitleOver )
                break;
        }

        if( bIsTitleOver )
            break;

        if( ( m_ciZoom.bEnable && !m_ciZoom.bNoUse ) && m_ciZoom.rectCell.contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            pHoverCell = &m_ciZoom;
            bIsTitleOver = true;
            break;
        }
#ifdef SUPPORT_ROTATE
        if( ( m_ciRotate.bEnable && !m_ciRotate.bNoUse ) && m_ciRotate.rectCell.contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            pHoverCell = &m_ciRotate;
            bIsTitleOver = true;
            break;
        }
#endif
        if( m_rectExtProperty[EXTP_INVERT_WHEEL].contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            bIsTitleOver = true;
            break;
        }
        if( m_rectExtProperty[EXTP_INDIVIDUAL_PUTNTAP].contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            bIsTitleOver = true;
            break;
        }
        if( m_rectExtProperty[EXTP_INERTIAL_WHEEL].contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            bIsTitleOver = true;
            break;
        }
        if( m_rectExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE].contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            bIsTitleOver = true;
            break;
        }
        if( m_rectExtProperty[EXTP_MAC_OS_MARGIN].contains( evt->localPos().x(), evt->localPos().y() ) )
        {
            bIsTitleOver = true;
            break;
        }

        } while( false );
        if( m_bIsTitleOver != bIsTitleOver )
        {
            m_bIsTitleOver = bIsTitleOver;
            update();
        }

        if( m_pHoverCell != pHoverCell )
        {
            m_pHoverCell = pHoverCell;
            update();
        }

        if( bIsTitleOver )
            setCursor( QCursor( Qt::PointingHandCursor ) );
        else
            setCursor( QCursor( Qt::ArrowCursor ) );
    }

    QWidget::mouseMoveEvent(evt);
}

