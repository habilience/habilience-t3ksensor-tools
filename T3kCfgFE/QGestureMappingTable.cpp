#include "QGestureMappingTable.h"
#include <QPainter>

#include "QKeyMapStr.h"
#include "T3kConstStr.h"
#include "QUtils.h"
#include "QGUIUtils.h"
#include "QT3kDevice.h"
#include "QLangManager.h"
#include <QMouseEvent>

//#define SUPPORT_ROTATE

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

#define EXTP_INERTIAL_WHEEL						(0)
#define EXTP_INDIVIDUAL_PUTNTAP                 (1)
#define EXTP_PUTAND_ON_MULTITOUCHDEVICE         (2)
#define EXTP_MAC_OS_MARGIN                      (3)
#define EXTP_INVERT_WHEEL						(4)
#define EXTP_ENABLE_SINGLE_DBLTAP               (5)
#define EXTP_ENABLE_SINGLE_TAP                  (6)
#define EXTP_ENABLE_SINGLE_MOVE                 (7)

#define MM_MOUSEPROFILE_SINGLE_MOVE                     0x0001
#define MM_MOUSEPROFILE_SINGLE_TAP                      0x0002
#define MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP               0x0004

#define MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY    0x0200
        // Use PUT&, PALM action and Multi-touch device action
#define MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE         0x0400
        // Inertial WHEEL
#define MM_MOUSEPROFILE_INERTIAL_WHEEL                  0x1000
        // Invert wheel
#define MM_MOUSEPROFILE_INVERT_WHEEL                    0x2000
        // Use MacOSX margin
#define MM_MOUSEPROFILE_MAC_OS_MARGIN                   0x4000

inline bool extract2Word( QString& str, unsigned char& cV )
{
    QString strWord = str.left( 2 );
    if ( strWord.isEmpty() ) return false;
    str.remove( 0, 2 );
    cV = (unsigned char)strWord.toInt(0, 16);
    return true;
}

#define RES_TAG     "GESTURE PROFILE DIALOG"

QGestureMappingTable::QGestureMappingTable(QWidget *parent) :
    QWidget(parent)
{
    m_nProfileIndex = 0;
    m_wProfileFlags = 0x00;
    m_pSelectCell = NULL;

    m_bIsTitleOver = false;
    m_bIsHovered = false;

    m_pHoverCell = NULL;
    m_nHoverExtProperty = -1;

    m_fntSystem = getSystemFont(this);

    setMinimumSize( 400, 300 );

    initControl();

    setMouseTracking(true);     // for mousemove event
    installEventFilter( this ); // for mousemove event!
}

void QGestureMappingTable::initControl()
{
    m_aryCell.clear();

    CellInfo	Dummy;
    initCellInfo( Dummy );
    initCellInfo( m_ciRotateHead );
    initCellInfo( m_ciZoomHead );
    initCellInfo( m_ciRotate );
    initCellInfo( m_ciZoom );

    for ( int i=0 ; i<COL_COUNT*ROW_COUNT ; i++ )
        m_aryCell.push_back( Dummy );
    for ( int i=0 ; i<COL_COUNT ; i++ )
        m_aryCell[TABLE(i, 0)].bBold = true;
    for ( int j=0 ; j<ROW_COUNT ; j++ )
        m_aryCell[TABLE(0, j)].bBold = true;

    m_ciZoomHead.bBold = true;

    m_aryCell[TABLE(0, 0)].bEnable = false;

    m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].strText		= mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_MOVE);
    m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].strText		= mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_CLICK);
    m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].strText	= mouseKeyToString(MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK);

    m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].bDefault		= true;
    m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].bDefault		= true;
    m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].bDefault	= true;

    m_aryCell[TABLE(COL_MOVE, ROW_PUTNTAP)].bNotUsed	= true;

    m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].cKey				= MM_GESTURE_SINGLE_MOVE;
    m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].wKeyValue[0]		= MM_MOUSEPROFILE_SINGLE_MOVE;
    m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].cKey				= MM_GESTURE_SINGLE_TAP;
    m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].wKeyValue[0]		= MM_MOUSEPROFILE_SINGLE_TAP;
    m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].cKey			= MM_GESTURE_SINGLE_DOUBLE_TAP;
    m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].wKeyValue[0]	= MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP;

    m_aryCell[TABLE(COL_LONGTAP, ROW_SINGLE)].cKey		= MM_GESTURE_SINGLE_LONG_TAP;
    m_aryCell[TABLE(COL_MOVE, ROW_FINGERS)].cKey		= MM_GESTURE_FINGERS_MOVE;
    m_aryCell[TABLE(COL_TAP, ROW_FINGERS)].cKey			= MM_GESTURE_FINGERS_TAP;
    m_aryCell[TABLE(COL_DBLTAP, ROW_FINGERS)].cKey		= MM_GESTURE_FINGERS_DOUBLE_TAP;
    m_aryCell[TABLE(COL_LONGTAP, ROW_FINGERS)].cKey		= MM_GESTURE_FINGERS_LONG_TAP;
    m_aryCell[TABLE(COL_MOVE, ROW_PALM)].cKey			= MM_GESTURE_PALM_MOVE;
    m_aryCell[TABLE(COL_TAP, ROW_PALM)].cKey			= MM_GESTURE_PALM_TAP;
    m_aryCell[TABLE(COL_DBLTAP, ROW_PALM)].cKey			= MM_GESTURE_PALM_DOUBLE_TAP;
    m_aryCell[TABLE(COL_LONGTAP, ROW_PALM)].cKey		= MM_GESTURE_PALM_LONG_TAP;
    m_aryCell[TABLE(COL_TAP, ROW_PUTNTAP)].cKey			= MM_GESTURE_PUTAND_TAP;
    m_aryCell[TABLE(COL_DBLTAP, ROW_PUTNTAP)].cKey		= MM_GESTURE_PUTAND_DOUBLE_TAP;
    m_aryCell[TABLE(COL_LONGTAP, ROW_PUTNTAP)].cKey		= MM_GESTURE_PUTAND_LONG_TAP;
    m_ciZoom.cKey										= MM_GESTURE_ZOOM;

    m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].keyType		= KeyTypeEnable;
    m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].keyType		= KeyTypeEnable;
    m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].keyType	= KeyTypeEnable;
    m_aryCell[TABLE(COL_LONGTAP, ROW_SINGLE)].keyType	= KeyType1Key;
    m_aryCell[TABLE(COL_MOVE, ROW_FINGERS)].keyType		= KeyType4Way;
    m_aryCell[TABLE(COL_TAP, ROW_FINGERS)].keyType		= KeyType1Key;
    m_aryCell[TABLE(COL_DBLTAP, ROW_FINGERS)].keyType	= KeyType1Key;
    m_aryCell[TABLE(COL_LONGTAP, ROW_FINGERS)].keyType	= KeyType1Key;
    m_aryCell[TABLE(COL_MOVE, ROW_PALM)].keyType		= KeyType4Way;
    m_aryCell[TABLE(COL_TAP, ROW_PALM)].keyType			= KeyType1Key;
    m_aryCell[TABLE(COL_DBLTAP, ROW_PALM)].keyType		= KeyType1Key;
    m_aryCell[TABLE(COL_LONGTAP, ROW_PALM)].keyType		= KeyType1Key;
    m_aryCell[TABLE(COL_TAP, ROW_PUTNTAP)].keyType		= KeyType1Key;
    m_aryCell[TABLE(COL_DBLTAP, ROW_PUTNTAP)].keyType	= KeyType1Key;
    m_aryCell[TABLE(COL_LONGTAP, ROW_PUTNTAP)].keyType	= KeyType1Key;
    m_ciZoom.keyType									= KeyType2Way;

    m_bCheckExtProperty[EXTP_INVERT_WHEEL]						= false;
    m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP]				= false;
    m_bCheckExtProperty[EXTP_INERTIAL_WHEEL]					= false;
    m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]		= false;
    m_bCheckExtProperty[EXTP_MAC_OS_MARGIN]						= false;

    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP]		= false;
    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP]	= false;
    m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE]	= false;

    QLangRes& res = QLangManager::getResource();

    m_aryCell[TABLE(COL_MOVE, 0)].strText		= res.getResString(RES_TAG, "TEXT_DRAG");
    m_aryCell[TABLE(COL_TAP, 0)].strText		= res.getResString(RES_TAG, "TEXT_TAP");
    m_aryCell[TABLE(COL_DBLTAP, 0)].strText		= res.getResString(RES_TAG, "TEXT_DOUBLE_TAP");
    m_aryCell[TABLE(COL_LONGTAP, 0)].strText	= res.getResString(RES_TAG, "TEXT_LONG_TAP");
    m_ciRotateHead.strText						= res.getResString(RES_TAG, "TEXT_ROTATE");
    m_ciZoomHead.strText						= res.getResString(RES_TAG, "TEXT_ZOOM");
    m_aryCell[TABLE(0, 1)].strText				= res.getResString(RES_TAG, "TEXT_SINGLE");
    m_aryCell[TABLE(0, 2)].strText				= res.getResString(RES_TAG, "TEXT_FINGERS");
    m_aryCell[TABLE(0, 3)].strText				= res.getResString(RES_TAG, "TEXT_PALM");
    m_aryCell[TABLE(0, 4)].strText				= res.getResString(RES_TAG, "TEXT_PUTNTAP");

    m_strExtProperty[EXTP_INDIVIDUAL_PUTNTAP]			= res.getResString(RES_TAG, "TEXT_INDIVIDUAL_PUTNTAP");
    m_strExtProperty[EXTP_INERTIAL_WHEEL]				= res.getResString(RES_TAG, "TEXT_INERTIAL_WHEEL");
    m_strExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]	= res.getResString(RES_TAG, "TEXT_PUTNAD_ON_MULTITOUCHDEVICE");
    m_strExtProperty[EXTP_MAC_OS_MARGIN]				= res.getResString(RES_TAG, "TEXT_MAC_OS_MARGIN");
    m_strExtProperty[EXTP_INVERT_WHEEL]					= res.getResString(RES_TAG, "TEXT_INVERT_WHEEL");
}

void QGestureMappingTable::setProfileIndex( int nIndex )
{
    m_nProfileIndex = nIndex;
    update();
}

void QGestureMappingTable::resetSelect()
{
    m_pSelectCell = NULL;
    update();
}

void QGestureMappingTable::setCellInfo( int nCol, int nRow,
                  unsigned char cV00, unsigned char cV01,
                  unsigned char cV10, unsigned char cV11,
                  unsigned char cV20, unsigned char cV21,
                  unsigned char cV30, unsigned char cV31 )
{
    CellInfo& ci = m_aryCell[TABLE(nCol, nRow)];

    ci.wKeyValue[0] = (short)cV00 << 8 | cV01;
    ci.wKeyValue[1] = (short)cV10 << 8 | cV11;
    ci.wKeyValue[2] = (short)cV20 << 8 | cV21;
    ci.wKeyValue[3] = (short)cV30 << 8 | cV31;

    QString strKey1, strKey2, strKey3, strKey4;
    strKey1 = getMappingStr( cV00, cV01 );
    strKey1.replace( "\r\n", " " );
    strKey2 = getMappingStr( cV10, cV11 );
    strKey2.replace( "\r\n", " " );
    strKey3 = getMappingStr( cV20, cV21 );
    strKey3.replace( "\r\n", " " );
    strKey4 = getMappingStr( cV30, cV31 );
    strKey4.replace( "\r\n", " " );
    QString strLR, strUD;
    if (strKey1 == strKey2)
    {
        strLR = strKey1;
    }
    else
    {
        strLR = strKey1 + "/" + strKey2;
    }

    if (strKey3 == strKey4)
    {
        strUD = strKey3;
    }
    else
    {
        strUD = strKey3 + "/" + strKey4;
    }

    if (strLR == strUD)
    {
        strLR.replace( " ", "\r\n" );
        ci.strText = strLR;
    }
    else
    {
        ci.strText = strLR + "\n" + strUD;
    }
}

void QGestureMappingTable::setCellInfo( int nCol, int nRow,
                  unsigned char cV0, unsigned char cV1 )
{
    CellInfo& ci = m_aryCell[TABLE(nCol, nRow)];

    ci.wKeyValue[0] = (short)cV0 << 8 | cV1;
    ci.strText = getMappingStr( cV0, cV1 );
}

void QGestureMappingTable::parseMouseProfile( const char* szProfile )
{
    QString strProfile = szProfile;

    int nE = strProfile.indexOf('=');
    if ( nE < 0 ) return;

    strProfile.remove( 0, nE+1 );

    do
    {
        unsigned char cKey;
        if ( !extract2Word( strProfile, cKey ) ) break;
        unsigned char cV[4][2];
        unsigned short wFlags;

        switch ( cKey )
        {
        case 0x00:		// profile_flags
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            wFlags = (unsigned short)cV[0][0] << 8 | cV[0][1];

            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP]	= MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP]		= MM_MOUSEPROFILE_SINGLE_TAP & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE]	= MM_MOUSEPROFILE_SINGLE_MOVE & wFlags ? true : false;

            m_aryCell[TABLE(COL_MOVE, ROW_SINGLE)].strText		= mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_MOVE] ? MM_MOUSE_KEY1_MOUSE_L_MOVE : 0);
            m_aryCell[TABLE(COL_TAP, ROW_SINGLE)].strText		= mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_TAP] ? MM_MOUSE_KEY1_MOUSE_L_CLICK : 0);
            m_aryCell[TABLE(COL_DBLTAP, ROW_SINGLE)].strText	= mouseKeyToString(m_bCheckExtProperty[EXTP_ENABLE_SINGLE_DBLTAP] ? MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK : 0);

            m_bCheckExtProperty[EXTP_INVERT_WHEEL]					= MM_MOUSEPROFILE_INVERT_WHEEL & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP]			= MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_INERTIAL_WHEEL]				= MM_MOUSEPROFILE_INERTIAL_WHEEL & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_INVERT_WHEEL]					= MM_MOUSEPROFILE_INVERT_WHEEL & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE]	= MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE & wFlags ? true : false;
            m_bCheckExtProperty[EXTP_MAC_OS_MARGIN]					= MM_MOUSEPROFILE_MAC_OS_MARGIN & wFlags ? true : false;

            m_wProfileFlags = wFlags;
            update();
            qDebug( "Profile Flags: %04X", wFlags );

            break;
        case MM_GESTURE_SINGLE_LONG_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;

            setCellInfo( COL_LONGTAP, ROW_SINGLE, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_MOVE:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            if ( !extract2Word( strProfile, cV[1][0] ) ) break;
            if ( !extract2Word( strProfile, cV[1][1] ) ) break;
            if ( !extract2Word( strProfile, cV[2][0] ) ) break;
            if ( !extract2Word( strProfile, cV[2][1] ) ) break;
            if ( !extract2Word( strProfile, cV[3][0] ) ) break;
            if ( !extract2Word( strProfile, cV[3][1] ) ) break;
            setCellInfo( COL_MOVE, ROW_FINGERS, cV[0][0], cV[0][1], cV[1][0], cV[1][1], cV[2][0], cV[2][1], cV[3][0], cV[3][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_TAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_DOUBLE_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_DBLTAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_FINGERS_LONG_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_LONGTAP, ROW_FINGERS, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_MOVE:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            if ( !extract2Word( strProfile, cV[1][0] ) ) break;
            if ( !extract2Word( strProfile, cV[1][1] ) ) break;
            if ( !extract2Word( strProfile, cV[2][0] ) ) break;
            if ( !extract2Word( strProfile, cV[2][1] ) ) break;
            if ( !extract2Word( strProfile, cV[3][0] ) ) break;
            if ( !extract2Word( strProfile, cV[3][1] ) ) break;
            setCellInfo( COL_MOVE, ROW_PALM, cV[0][0], cV[0][1], cV[1][0], cV[1][1], cV[2][0], cV[2][1], cV[3][0], cV[3][1] );
            update();
            break;

        case MM_GESTURE_PALM_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_TAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_DOUBLE_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_DBLTAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PALM_LONG_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_LONGTAP, ROW_PALM, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_TAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_DOUBLE_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_DBLTAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_PUTAND_LONG_TAP:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;
            setCellInfo( COL_LONGTAP, ROW_PUTNTAP, cV[0][0], cV[0][1] );
            update();
            break;

        case MM_GESTURE_ZOOM:
            if ( !extract2Word( strProfile, cV[0][0] ) ) break;
            if ( !extract2Word( strProfile, cV[0][1] ) ) break;

            if ( !extract2Word( strProfile, cV[1][0] ) ) break;
            if ( !extract2Word( strProfile, cV[1][1] ) ) break;

            m_ciZoom.wKeyValue[0] = (short)cV[0][0] << 8 | cV[0][1];
            m_ciZoom.wKeyValue[1] = (short)cV[1][0] << 8 | cV[1][1];

            {
                QString strKey1, strKey2;
                strKey1 = getMappingStr( cV[0][0], cV[0][1] );
                strKey1.replace( "\r\n", " " );
                strKey2 = getMappingStr( cV[1][0], cV[1][1] );
                strKey2.replace( "\r\n", " " );
                QString strZoom;
                if (strKey1 == strKey2)
                {
                    strZoom = strKey1;
                }
                else
                {
                    strZoom = strKey1 + "\n" + strKey2;
                }

                m_ciZoom.strText = strZoom;
            }
            update();

            break;
        default:
            break;
        }

        int nC = strProfile.indexOf( ' ' );
        if ( nC >= 0 )
        {
            strProfile.remove( 0, nC+1 );
        }
    } while (!strProfile.isEmpty());
}

void QGestureMappingTable::drawExtProperty( QPainter& p, int nExtIndex, QRectF &rectExtProperty, QFont& fntCellNormal, int flags, const QColor& cellFontColor )
{
    p.setRenderHint( QPainter::Antialiasing );
    QRectF rectCheck;

    QRectF rectBounds;
    m_rectExtProperty[nExtIndex] = rectExtProperty;
    m_rectExtProperty[nExtIndex].setY( m_rectExtProperty[nExtIndex].y() + (rectExtProperty.height()/5.f)*nExtIndex );
    m_rectExtProperty[nExtIndex].setHeight(rectExtProperty.height()/5.f);

    QString& strExt = m_strExtProperty[nExtIndex];

    QFontMetricsF fmFont(fntCellNormal);
    QRectF rectFont = fmFont.boundingRect(strExt);

    rectCheck = m_rectExtProperty[nExtIndex];
    rectCheck.setWidth(rectFont.height());
    rectCheck.setTop( m_rectExtProperty[nExtIndex].top() + (m_rectExtProperty[nExtIndex].height() - rectFont.height()) / 2.0f );
    rectCheck.setHeight(rectFont.height());

    QRectF rectInner = rectCheck;
    rectInner.adjust( 2.f, 2.f, -2.f, -2.f );

    p.fillRect( rectCheck, QColor(80,80,80) );
    p.fillRect( rectInner, QColor(255,255,255) );

    if (m_bCheckExtProperty[nExtIndex])
    {
        rectInner.adjust( 1.f, 1.f, -1.f, -1.f );
        p.fillRect( rectInner, QColor(80,80,80) );
    }

    const int nOffsetX = 5;
    m_rectExtProperty[nExtIndex].translate( rectCheck.width() + nOffsetX, 0 );
    m_rectExtProperty[nExtIndex].setWidth( m_rectExtProperty[nExtIndex].width() - (rectCheck.width() + nOffsetX) );
    p.setFont( fntCellNormal );
    p.setPen( m_nHoverExtProperty == nExtIndex ? QColor(55,100,160) : cellFontColor);
    strExt = fmFont.elidedText( strExt, Qt::ElideRight, m_rectExtProperty[nExtIndex].width() );
    p.drawText( m_rectExtProperty[nExtIndex], flags, strExt );

    p.drawText( m_rectExtProperty[nExtIndex], flags, strExt, &rectBounds );
    m_rectExtProperty[nExtIndex].translate( -(rectCheck.width() + nOffsetX), 0 );
    m_rectExtProperty[nExtIndex].setWidth( rectBounds.width() + rectCheck.width() + nOffsetX);
    m_rectExtProperty[nExtIndex].setTop( rectBounds.top() );
    m_rectExtProperty[nExtIndex].setHeight( rectBounds.height() );
}

void QGestureMappingTable::drawCellInfo(QPainter& p, const CellInfo& ci, QFont& fntCellNormal, QFont& fntCellBold, const QColor &cellFontColor, int& flags, const QString& strFontFamily )
{
    if( ci.bDefault && (m_pSelectCell != &ci) )
        p.fillRect( ci.rectCell, QColor(240,240,240) );
    if( ci.bNotUsed && (m_pSelectCell != &ci) )
        p.fillRect( ci.rectCell, QBrush(QColor(50, 50, 50), Qt::Dense6Pattern) );

    flags = Qt::AlignCenter|Qt::AlignVCenter;

    if (ci.bEnable && !ci.bNotUsed && ci.strText.isEmpty())
    {
        p.setPen( cellFontColor );
        p.setFont( ci.bBold ? fntCellBold : fntCellNormal );
        p.drawText( ci.rectCell, flags, "-" );
    }
    else
    {
        int nTextOffset = 0;
        QRectF rectText = ci.rectCell;
        rectText.adjust( nTextOffset, 0, 0, 0 );
        if (ci.bBold)
        {
            QFontMetricsF ftMetrics(fntCellBold);
            QString strText = ftMetrics.elidedText(ci.strText, Qt::ElideRight, rectText.width());
            p.setPen( QColor(30, 53, 85) );
            p.setFont( fntCellBold );
            p.drawText( rectText, flags, strText );
        }
        else
        {
            QFontMetricsF ftMetrics(fntCellNormal);
            QRectF rectBB = ftMetrics.boundingRect( rectText, flags, ci.strText );
            if ( rectBB.height() >= rectText.height()-2.0f )
            {
                float fFontHeight = fntCellNormal.pixelSize();

goto_FontRecalc:
                fFontHeight *= 0.9f;
                QFont fntVariable(strFontFamily);
                fntVariable.setPixelSize(fFontHeight);
                p.setFont( fntVariable );
                p.drawText( rectText, flags, ci.strText, &rectBB );
                if( rectBB.height() >= rectText.height()-2.0f )
                    goto goto_FontRecalc;
                p.setPen( cellFontColor );
                QFontMetricsF fm(fntVariable);
                rectBB = fm.boundingRect( rectText, flags, ci.strText );
                if (rectBB.width() >= rectText.width()-2.0f)
                {
                    QString strOrg = ci.strText;
                    QString strText;
                    if (strOrg.indexOf('\n') >= 0)
                    {
                        QString strLeft = extractLeft(strOrg, '\n');
                        QString strRight = strOrg;
                        strLeft = fm.elidedText(strLeft, Qt::ElideRight, rectText.width());
                        strRight = fm.elidedText(strRight, Qt::ElideRight, rectText.width());
                        strText = strLeft + '\n' + strRight;
                    }
                    else
                    {
                        strText = fm.elidedText(ci.strText, Qt::ElideRight, rectText.width());
                    }
                    p.drawText( rectText, flags, strText );
                }
                else
                {
                    p.drawText( rectText, flags, ci.strText );
                }
            }
            else
            {
                p.setFont( fntCellNormal );
                p.setPen( cellFontColor );
                if (rectBB.width() >= rectText.width()-2.0f)
                {
                    QString strOrg = ci.strText;
                    QString strText;
                    if (strOrg.indexOf('\n') >= 0)
                    {
                        QString strLeft = extractLeft(strOrg, '\n');
                        QString strRight = strOrg;
                        strLeft = ftMetrics.elidedText(strLeft, Qt::ElideRight, rectText.width());
                        strRight = ftMetrics.elidedText(strRight, Qt::ElideRight, rectText.width());
                        strText = strLeft + '\n' + strRight;
                    }
                    else
                    {
                        strText = ftMetrics.elidedText(ci.strText, Qt::ElideRight, rectText.width());
                    }
                    p.drawText( rectText, flags, strText );
                }
                else
                {
                    p.drawText( rectText, flags, ci.strText );
                }
            }
        }
    }
}

void QGestureMappingTable::popEditActionWnd( const CellInfo& ci )
{
    updateProfile( m_nProfileIndex, ci, m_wProfileFlags );

    resetSelect();
}

void QGestureMappingTable::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    bool bParseProfile = false;

    switch (m_nProfileIndex)
    {
    case 0:
        if ( strstr(szCmd, cstrMouseProfile1) == szCmd )
            bParseProfile = true;
        break;
    case 1:
        if ( strstr(szCmd, cstrMouseProfile2) == szCmd )
            bParseProfile = true;
        break;
    default:
        if ( strstr(szCmd, cstrMouseProfile2) == szCmd )
            bParseProfile = true;
        break;
    }

    if ( bParseProfile )
    {
        parseMouseProfile( szCmd );
    }
}

void QGestureMappingTable::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QRect rectBody( 0, 0, width()-1, height()-1 );
    QRect rcClient = rectBody;
    rectBody.adjust( 10, 16, -10, -16 );

    QRect rectTable = rectBody;
    rectTable.setHeight( rectTable.height() - rectTable.height()/(ROW_COUNT-2) );
    QRectF rectExtProperty( rectTable.left(), rectTable.bottom(), rectTable.width(), rectBody.height() - rectTable.height() );

    float fTableIntervalV = (float)rectTable.height() / (ROW_COUNT*2 - 1);
    float fTableIntervalH = (float)rectTable.width() / (COL_COUNT*2);

    QPen gridPen( QColor(100, 100, 100), 1.f );

    float fLeft = (float)rectTable.left();
    float fRight = (float)rectTable.right();
    float fTop = (float)rectTable.top();
    float fBottom = (float)rectTable.bottom();

    for ( int j=0 ; j<ROW_COUNT ; j++ )
    {
        for ( int i=0 ; i<COL_COUNT ; i++ )
        {
            CellInfo& ci = m_aryCell[TABLE(i, j)];
            if ( i != 0 )
            {
                const CellInfo& ciPrev = m_aryCell.at( TABLE(i-1, j) );
                ci.rectCell.setX(ciPrev.rectCell.right());
            }
            else
            {
                ci.rectCell.setX((int)fLeft);
            }
            if ( j != 0 )
            {
                const CellInfo& ciPrev = m_aryCell.at( TABLE(i, j-1) );
                ci.rectCell.setY(ciPrev.rectCell.bottom());
            }
            else
            {
                ci.rectCell.setY((int)fTop);
            }

            if ( i == 0 )
                ci.rectCell.setWidth(fTableIntervalH*2.f);
            else
                ci.rectCell.setWidth(fTableIntervalH*2.f);
            if ( j == 0 )
                ci.rectCell.setHeight(fTableIntervalV*1.f);
            else
                ci.rectCell.setHeight(fTableIntervalV*2.f);
        }
    }

    // head color
    for ( int i=0 ; i<COL_COUNT ; i++ )
    {
        const CellInfo& ci = m_aryCell.at( TABLE(i, 0) );
        p.fillRect( ci.rectCell, QColor(219, 228, 242) );
    }
    // head color
    for ( int j=0 ; j<ROW_COUNT ; j++ )
    {
        const CellInfo& ci = m_aryCell.at( TABLE(0, j) );
        p.fillRect( ci.rectCell, QColor(219, 228, 242) );
    }

    QString strFamily = m_fntSystem.family();

    QFont fntCellNormal( strFamily );
    fntCellNormal.setPixelSize(12);
    QFont fntCellBold = fntCellNormal;
    fntCellBold.setBold(true);

    int flags = Qt::AlignCenter|Qt::AlignVCenter;

    QColor cellFontColor(0,0,0);
    // draw cell info
    for ( int j=0 ; j<ROW_COUNT ; j++ )
    {
        for ( int i=0 ; i<COL_COUNT ; i++ )
        {
            const CellInfo& ci = m_aryCell.at( TABLE(i, j) );

            if ( &ci == m_pSelectCell )
            {
                p.fillRect( ci.rectCell, QColor(255, 240, 240) );
            }
            else if( &ci == m_pHoverCell )
            {
                p.fillRect( ci.rectCell, QColor(240, 240, 255) );
            }
            drawCellInfo( p, ci, fntCellNormal, fntCellBold, cellFontColor, flags, strFamily );
        }
    }

    p.setPen( gridPen );
    // table lines
    float fY = fTop;
    for ( int j=1 ; j<ROW_COUNT ; j++ )
    {
        if( j == 1 )
            fY += fTableIntervalV;
        else
            fY += fTableIntervalV * 2.f;
        p.drawLine( fLeft, fY, fRight, fY );
    }
    float fX = fLeft;
    for ( int i=1 ; i<COL_COUNT ; i++ )
    {
        if( i == 1 )
            fX += fTableIntervalH * 2.f;
        else
            fX += fTableIntervalH * 2.f;
        p.drawLine( fX, fTop, fX, fBottom );
    }

    p.setBrush(Qt::NoBrush);
    p.setPen( QPen(QColor(100,100,100), 2.f) );
    p.drawRect( rectTable );

    QRect rectZoomRotate(rectTable.x() + rectTable.width()*3/5, rectTable.bottom()+4, rectTable.width()*2/5, rectBody.height()-rectTable.height()-4);

    m_ciZoomHead.rectCell.setRect( rectZoomRotate.x(), rectZoomRotate.y(), fTableIntervalH*2.f, rectZoomRotate.height()/2.f );

    m_ciZoom.rectCell.setRect( rectZoomRotate.x() + fTableIntervalH*2.f, rectZoomRotate.y(), rectZoomRotate.width() - fTableIntervalH*2.f, rectZoomRotate.height()/2.f );

    m_ciRotateHead.rectCell.setRect( rectZoomRotate.x(), rectZoomRotate.y() + rectZoomRotate.height()/2.f, fTableIntervalH*2.f, rectZoomRotate.height()/2.f );

    m_ciRotate.rectCell.setRect( rectZoomRotate.x() + fTableIntervalH*2.f, rectZoomRotate.y() + rectZoomRotate.height()/2.f, rectZoomRotate.width() - fTableIntervalH*2.f, rectZoomRotate.height()/2.f );

    // head color (zoom/rotate)
    p.fillRect( m_ciZoomHead.rectCell, QColor(219, 228, 242) );
#ifdef SUPPORT_ROTATE
    p.fillRect( m_ciRotateHead.rectCell, QColor(219, 228, 242) );
#endif

    drawCellInfo( p, m_ciZoomHead, fntCellNormal, fntCellBold, cellFontColor, flags, strFamily );
#ifdef SUPPORT_ROTATE
    drawCellInfo( p, m_ciRotateHead, fntCellNormal, fntCellBold, cellFontColor, flags, strFamily );
#endif

    if ( &m_ciZoom == m_pSelectCell )
    {
        p.fillRect( m_ciZoom.rectCell, QColor(255,240,240) );
    }
    else if( &m_ciZoom == m_pHoverCell )
    {
        p.fillRect( m_ciZoom.rectCell, QColor(240,240,255) );
    }
    drawCellInfo( p, m_ciZoom, fntCellNormal, fntCellBold, cellFontColor, flags, strFamily );
#ifdef SUPPORT_ROTATE
    if ( &m_ciRotate == m_pSelectCell )
    {
        p.fillRect( m_ciRotate.rectCell, QColor(255,240,240) );
    }
    else if( &m_ciRotate == m_pHoverCell )
    {
        p.fillRect( m_ciRotate.rectCell, QColor(240,240,255) );
    }
    drawCellInfo( p, m_ciRotate, fntCellNormal, fntCellBold, cellFontColor, flags, strFamily );
#endif

    // grid (zoom/rotate)
    int nCY = rectZoomRotate.y() + rectZoomRotate.height()/2;
    p.setPen( gridPen );
    p.drawLine( rectZoomRotate.x(), nCY, rectZoomRotate.right(), nCY );
    int nCX = int(rectZoomRotate.x()+fTableIntervalH*2.f);
#ifdef SUPPORT_ROTATE
    p.drawLine( nCX, rectZoomRotate.top(), nCX, rectZoomRotate.bottom() );
#else
    p.drawLine( nCX, rectZoomRotate.top(), nCX, nCY );
#endif

#ifdef SUPPORT_ROTATE
    p.setPen( QPen(QColor(100,100,100), 2.f) );
    p.setBrush(Qt::NoBrush);
    p.drawRect( rectZoomRotate );
#else
    rectZoomRotate.setHeight( rectZoomRotate.height()/2 );
    p.setPen( QPen(QColor(100,100,100), 2.f) );
    p.setBrush(Qt::NoBrush);
    p.drawRect( rectZoomRotate );
#endif

    // draw ext-property

    rectExtProperty.setRight( rectZoomRotate.left()-3 );

    rectExtProperty.adjust(0, 3, 0, -3);
    flags = Qt::AlignLeft|Qt::AlignVCenter;

    //p.drawRect( rectExtProperty );

    drawExtProperty( p, EXTP_INERTIAL_WHEEL, rectExtProperty, fntCellNormal, flags, cellFontColor );
    drawExtProperty( p, EXTP_INDIVIDUAL_PUTNTAP, rectExtProperty, fntCellNormal, flags, cellFontColor );
    drawExtProperty( p, EXTP_PUTAND_ON_MULTITOUCHDEVICE, rectExtProperty, fntCellNormal, flags, cellFontColor );
    drawExtProperty( p, EXTP_MAC_OS_MARGIN, rectExtProperty, fntCellNormal, flags, cellFontColor );
    drawExtProperty( p, EXTP_INVERT_WHEEL, rectExtProperty, fntCellNormal, flags, cellFontColor );

    if ( !isEnabled() )
    {
        p.fillRect( rcClient, QColor(255, 255, 255, 128) );
        QPen XPen( QColor(250, 100, 100, 200), 10.f );
        p.drawLine( rcClient.topLeft(), rcClient.bottomRight() );
        p.drawLine( rcClient.topRight(), rcClient.bottomLeft() );
    }
}

void QGestureMappingTable::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    update();
}

void QGestureMappingTable::mousePressEvent(QMouseEvent *e)
{
    for ( int j=1 ; j<ROW_COUNT ; j++ )
    {
        for ( int i=1 ; i<COL_COUNT ; i++ )
        {
            const CellInfo& ci = m_aryCell.at( TABLE(i, j) );

            if ( ci.bEnable && !ci.bNotUsed )
            {
                if ( ci.rectCell.contains( e->pos() ) )
                {
                    m_pSelectCell = const_cast<CellInfo*>(&ci);
                    update();
                    popEditActionWnd( ci );
                    return;
                }
            }
        }
    }

    if ( m_ciZoom.rectCell.contains( e->pos() ) )
    {
        m_pSelectCell = &m_ciZoom;
        update();
        popEditActionWnd( m_ciZoom );
        return;
    }
#ifdef SUPPORT_ROTATE
    if ( m_ciRotate.rectCell.contains( e->pos() ) )
    {
        m_pSelectCell = &m_ciRotate;
        update();
        popEditActionWnd( m_ciRotate );
        return;
    }
#endif

    bool bExtSet = false;
    if ( m_rectExtProperty[EXTP_INERTIAL_WHEEL].contains( e->pos() ) )
    {
        if ( m_bCheckExtProperty[EXTP_INERTIAL_WHEEL] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_INERTIAL_WHEEL;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_INERTIAL_WHEEL;
        bExtSet = true;
    }
    else if ( m_rectExtProperty[EXTP_INDIVIDUAL_PUTNTAP].contains( e->pos() ) )
    {
        if ( m_bCheckExtProperty[EXTP_INDIVIDUAL_PUTNTAP] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY;
        bExtSet = true;
    }
    else if ( m_rectExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE].contains( e->pos() ) )
    {
        if ( m_bCheckExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE;
        bExtSet = true;
    }
    else if ( m_rectExtProperty[EXTP_INVERT_WHEEL].contains( e->pos() ) )
    {
        if ( m_bCheckExtProperty[EXTP_INVERT_WHEEL] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_INVERT_WHEEL;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_INVERT_WHEEL;
        bExtSet = true;
    }
    else if ( m_rectExtProperty[EXTP_MAC_OS_MARGIN].contains( e->pos() ) )
    {
        if ( m_bCheckExtProperty[EXTP_MAC_OS_MARGIN] )
            m_wProfileFlags &= ~MM_MOUSEPROFILE_MAC_OS_MARGIN;
        else
            m_wProfileFlags |= MM_MOUSEPROFILE_MAC_OS_MARGIN;
        bExtSet = true;
    }

    if ( bExtSet )
    {
        char szCmd[256];
        switch ( m_nProfileIndex )
        {
        case 0:
            snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile1, 0x00, m_wProfileFlags );
            break;
        case 1:
            snprintf( szCmd, 256, "%s%02X%04X", cstrMouseProfile2, 0x00, m_wProfileFlags );
            break;
        }
        QT3kDevice::instance()->sendCommand( szCmd, true );
    }
}

void QGestureMappingTable::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
}

void QGestureMappingTable::mouseMoveEvent(QMouseEvent *e)
{
    int nHoverExtProperty = -1;
    if (m_bIsHovered)
    {
        bool bIsTitleOver = false;
        do {
            for ( int j=1 ; j<ROW_COUNT ; j++ )
            {
                for ( int i=1 ; i<COL_COUNT ; i++ )
                {
                    const CellInfo& ci = m_aryCell.at( TABLE(i, j) );

                    if ( ci.bEnable && !ci.bNotUsed )
                    {
                        if ( ci.rectCell.contains( e->pos() ) )
                        {
                            bIsTitleOver = true;
                            break;
                        }
                    }
                }
                if ( bIsTitleOver )
                    break;
            }

            if ( bIsTitleOver )
                break;

            if ( m_ciZoom.rectCell.contains( e->pos() ) )
            {
                bIsTitleOver = true;
                break;
            }
#ifdef SUPPORT_ROTATE
            if ( m_ciRotate.rectCell.contains( e->pos() ) )
            {
                bIsTitleOver = true;
                break;
            }
#endif
            if ( m_rectExtProperty[EXTP_INVERT_WHEEL].contains( e->pos() ) )
            {
                bIsTitleOver = true;
                nHoverExtProperty = EXTP_INVERT_WHEEL;
                break;
            }
            if ( m_rectExtProperty[EXTP_INDIVIDUAL_PUTNTAP].contains( e->pos() ) )
            {
                bIsTitleOver = true;
                nHoverExtProperty = EXTP_INDIVIDUAL_PUTNTAP;
                break;
            }
            if ( m_rectExtProperty[EXTP_INERTIAL_WHEEL].contains( e->pos() ) )
            {
                bIsTitleOver = true;
                nHoverExtProperty = EXTP_INERTIAL_WHEEL;
                break;
            }
            if ( m_rectExtProperty[EXTP_PUTAND_ON_MULTITOUCHDEVICE].contains( e->pos() ) )
            {
                bIsTitleOver = true;
                nHoverExtProperty = EXTP_PUTAND_ON_MULTITOUCHDEVICE;
                break;
            }
            if ( m_rectExtProperty[EXTP_MAC_OS_MARGIN].contains( e->pos() ) )
            {
                bIsTitleOver = true;
                nHoverExtProperty = EXTP_MAC_OS_MARGIN;
                break;
            }

        } while( false );
        if ( m_bIsTitleOver != bIsTitleOver )
        {
            m_bIsTitleOver = bIsTitleOver;
            if (m_bIsTitleOver)
            {
                setCursor(Qt::PointingHandCursor);
            }
            else
            {
                setCursor(Qt::ArrowCursor);
            }
            update();
        }
    }

    CellInfo* pHoverCell = NULL;
    for ( int j=1 ; j<ROW_COUNT ; j++ )
    {
        for ( int i=1 ; i<COL_COUNT ; i++ )
        {
            const CellInfo& ci = m_aryCell.at( TABLE(i, j) );

            if ( ci.bEnable && !ci.bNotUsed )
            {
                if ( ci.rectCell.contains( e->pos() ) )
                {
                    pHoverCell = const_cast<CellInfo*>(&ci);
                }
            }
        }
    }

    if ( m_ciZoom.rectCell.contains( e->pos() ) )
    {
        pHoverCell = &m_ciZoom;
    }
#ifdef SUPPORT_ROTATE
    if ( m_ciRotate.rectCell.contains( e->pos() ) )
    {
        pHoverCell = &m_ciRotate;
    }
#endif

    if ( m_pHoverCell != pHoverCell )
    {
        m_pHoverCell = pHoverCell;
        update();
    }

    if (m_nHoverExtProperty != nHoverExtProperty)
    {
        m_nHoverExtProperty = nHoverExtProperty;
        update();
    }

    QWidget::mouseMoveEvent(e);
}

bool QGestureMappingTable::event(QEvent *e)
{
    if (isEnabled())
    {
        switch (e->type())
        {
        case QEvent::DragEnter:
        case QEvent::Enter:
        case QEvent::HoverEnter:
            m_bIsHovered = true;
            update();
            break;
        case QEvent::DragLeave:
        case QEvent::Leave:
        case QEvent::HoverLeave:
            m_bIsHovered = false;
            m_bIsTitleOver = false;
            if (!m_pHoverCell)
            {
                m_pHoverCell = NULL;
                update();
            }
            break;
        default:
            break;
        }
    }
    else
    {
        m_bIsTitleOver = false;
        m_bIsHovered = false;
    }

    return QWidget::event(e);
}
