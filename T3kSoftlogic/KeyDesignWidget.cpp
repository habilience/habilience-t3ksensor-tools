#include "KeyDesignWidget.h"

#include "T3kSoftlogicDlg.h"
#include "T3kCommonData.h"

#include <QDebug>
#include <QPainter>
#include <QtEvents>
#include <QApplication>
#include <QStackedLayout>
#include <QMenu>

#define TRACK_OFFSETXY		(1)
#define MAX_GROUP			(13)

QRgb s_dwNormalColor = qRgb(180, 180, 180);

static QRgb s_dwGroupColor1[MAX_GROUP] =
{
    qRgb(156, 90, 60),
    qRgb(255, 163, 177),
    qRgb(229, 170, 122),
    qRgb(245, 228, 156),
    qRgb(255, 249, 189),
    qRgb(211, 249, 188),
    qRgb(157, 187, 97),
    qRgb(153, 217, 234),
    qRgb(112, 154, 209),
    qRgb(84, 109, 142),
    qRgb(181, 165, 213),
    qRgb(168, 230, 29),
    qRgb(255, 194, 14)
};

static QRgb s_dwGroupColor2[MAX_GROUP] =
{
    qRgb(149, 179, 215),
    qRgb(217, 150, 148),
    qRgb(195, 214, 155),
    qRgb(197, 162, 199),
    qRgb(147, 205, 221),
    qRgb(250, 192, 144),
    qRgb(192, 80, 77),
    qRgb(155, 187, 89),
    qRgb(128, 100, 162),
    qRgb(75, 172, 198),
    qRgb(247, 150, 70),
    qRgb(255, 192, 0),
    qRgb(79, 129, 189)
};

QRgb* s_dwGroupColor = NULL;

static bool isContainGroup( QVector<const GroupKey*>& GroupArray, const GroupKey* pGroup )
{
    for ( int nI=0 ; nI<GroupArray.count() ; nI++ )
	{
        if ( GroupArray.at(nI) == pGroup ) return true;
	}
    return false;
}

static void removeGroup( QVector<GroupKey*>& GroupArray, const GroupKey* pGroup )
{
    for ( int nI=0 ; nI<GroupArray.count() ; nI++ )
	{
        if ( GroupArray.at(nI) == pGroup )
        {
            GroupArray.remove(nI);
            delete pGroup;
            return;
        }
	}
}

QKeyDesignWidget::QKeyDesignWidget(QWidget *parent) :
    QWidget(parent)
{
    m_eScreenMode = ScreenModePreview;

    m_pSoftKeyDesignTool = NULL;

    //m_bMousePress = false;
    //m_bMouseDrag = false;

    m_bCheckRubberBand = false;

    m_keyModifier = Qt::NoModifier;

    m_bCtrlDown = false;
    m_bShiftDown = false;
    m_bCapture = false;

    m_nTimerBlink = 0;

    if( parent == NULL )
    {
        setWindowFlags( Qt::FramelessWindowHint );
        setMouseTracking( true );

        m_eScreenMode = ScreenModeKeyDesign;

        m_pSoftKeyDesignTool = new QSoftKeyDesignToolWidget( &m_SelectKeys, this );

        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::closeWidget, this, &QKeyDesignWidget::close );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::addNewKey, this, &QKeyDesignWidget::onAddNewKey );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::removeSelectedKeys, this, &QKeyDesignWidget::onRemoveSelectKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::groupSelectedKeys, this, &QKeyDesignWidget::onGroupSelectKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::ungroupSelectedKeys, this, &QKeyDesignWidget::onUngroupSelectKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::reorderKeys, this, &QKeyDesignWidget::onReorderKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::invalidateKey, this, &QKeyDesignWidget::onInvalidateKey );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::recalcSelectionKeys, this, &QKeyDesignWidget::onRecalcSelectionKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::updateScreen, this, &QKeyDesignWidget::onUpdateScreen );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::resetKeys, this, &QKeyDesignWidget::onResetKeys );

        connect( this, &QKeyDesignWidget::keyStateCount, m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::onKeyStateCount );

        connect( &m_KeyTracker, &QKeyTracker::finish, this, &QKeyDesignWidget::onRubberBandFinish );

        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::generateKeys, this, &QKeyDesignWidget::onGenerateKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::alignSelectedKeys, this, &QKeyDesignWidget::onAlignSelectedKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::adjustSizeSelectedKeys, this, &QKeyDesignWidget::onAdjustSizeSelectedKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::distribSelectKeys, this, &QKeyDesignWidget::onDistribSelectKeys );

        setContextMenuPolicy( Qt::CustomContextMenu );
        connect( this, &QWidget::customContextMenuRequested, this, &QKeyDesignWidget::onCustomContextMenuRequested );
    }

    m_rcScreen.setRect( 0, 0, width(), height() );
    m_rcScreenOrg = m_rcScreen;
    m_bMoveScreen = false;
    m_eScrnSize = ScreenSizeFit;

    m_bBlink = false;
    m_bBlinkOnOff = false;
    m_pBlinkGroup = NULL;
    m_pBlinkKey = NULL;
    m_pFocusKey = NULL;
    m_bFocusKeyOn = false;

    m_bViewTouchPoint = false;
    m_bDownCloseButton = false;

    m_bBlinkKeySet = false;

    m_nCalPos = 0;

    m_nOldTouchCount = 0;

    setInvertDrawing( false );

    init();
}

QKeyDesignWidget::~QKeyDesignWidget()
{
}

void QKeyDesignWidget::init()
{
    if ( m_eScreenMode == ScreenModeKeyDesign )
    {
        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        double dScrnDimWidth, dScrnDimHeight;
        Keys.getScreenDimension( dScrnDimWidth, dScrnDimHeight );

        m_pSoftKeyDesignTool->setScaleFactor( 0.0, 0.0 );
        if ( dScrnDimWidth != 0.0 && dScrnDimHeight != 0.0 )
        {
            double dScaleDimWidth = dScrnDimWidth / DEV_COORD;		// mm/dev
            double dScaleDimHeight = dScrnDimHeight / DEV_COORD;

            if( m_pSoftKeyDesignTool )
                m_pSoftKeyDesignTool->setScaleFactor( dScaleDimWidth, dScaleDimHeight );
        }

        m_KeyTracker.m_nStyle = QKeyTracker::solidLine|QKeyTracker::hatchedBorder|QKeyTracker::resizeOutside|QKeyTracker::resizeInside;
        m_KeyTracker.m_sizeMin = QSize( 0, 0 );

        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
        m_rcDevTracker.setRect( 0,0,0,0 );

        m_rcScreen.setRect( 0, 0, width(), height() );
    }
}

void QKeyDesignWidget::updateTouchCount( int nTouchCount )
{
    if ( nTouchCount < 0 )
    {
        update( m_rcTouchCount );
        return;
    }
    if ( m_nOldTouchCount != nTouchCount )
    {
        m_nOldTouchCount = nTouchCount;
        update( m_rcTouchCount );
    }
}

void QKeyDesignWidget::setInvertDrawing( bool bInvert )
{
    if ( bInvert )
    {
        m_clrBackground = qRgb(255, 255, 255);
        m_clrKeyBorder = qRgb(56, 93, 138);
        m_clrGridMajor = qRgb(200, 200, 200);
        m_clrGridMinor = qRgb(230, 230, 230);
        m_clrCloseBtnBg = qRgb(230, 230, 230);
        m_clrCloseBtnFg = qRgb(100, 100, 100);

        s_dwGroupColor = s_dwGroupColor2;
    }
    else
    {
        m_clrBackground = qRgb(0, 0, 0);
        m_clrKeyBorder = qRgb(240, 240, 240);
        m_clrGridMajor = qRgb(80, 80, 80);
        m_clrGridMinor = qRgb(40, 40, 40);
        m_clrCloseBtnBg = qRgb(100, 100, 100);
        m_clrCloseBtnFg = qRgb(0, 0, 0);

        s_dwGroupColor = s_dwGroupColor1;
    }

    if( isVisible() )
        update();
}

void QKeyDesignWidget::selectKey( int nIndex )
{
    m_SelectKeys.clear();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    if ( nIndex < 0 || nIndex >= Keys.getSize() ) return;

    CSoftkey* key = Keys[nIndex];

    if ( !isSelectKey(key) )
        m_SelectKeys.push_back( key );

    QRect rcKey = deviceToScreen( key->getPosition() );
    m_KeyTracker.m_rect = rcKey;
    m_rcDevTracker = key->getPosition();
    m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );

    update();
}

void QKeyDesignWidget::viewTouchPoint( long lX, long lY, bool bDown )
{
    if ( !m_bViewTouchPoint && bDown )
    {
        m_ptTouchPoint = deviceToScreen( QPoint(lX, lY), true );
    }

    m_bViewTouchPoint = bDown;

    update( QRect( m_ptTouchPoint.x()-10, m_ptTouchPoint.y()-10, m_ptTouchPoint.x()+10, m_ptTouchPoint.y()+10 ) );

    m_ptTouchPoint = deviceToScreen( QPoint(lX, lY), true );

    update( QRect( m_ptTouchPoint.x()-10, m_ptTouchPoint.y()-10, m_ptTouchPoint.x()+10, m_ptTouchPoint.y()+10 ) );
}

void QKeyDesignWidget::updateKeys()
{
    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    m_rcDevTracker.setRect( 0,0,0,0 );

    m_SelectKeys.clear();
    m_ClipboardKeys.clear();

    m_bBlink = false;
    m_bBlinkOnOff = false;
    m_pBlinkGroup = NULL;
    m_pBlinkKey = NULL;
    m_pFocusKey = NULL;
    m_bFocusKeyOn = false;

    if ( winId() )
    {
        resizeScreen();

        if( m_nTimerBlink )
        {
            killTimer( m_nTimerBlink );
            m_nTimerBlink = 0;
        }

        update();
    }
}

QRect QKeyDesignWidget::getFirstSelectKey()
{
    QRect rc;

    if ( m_SelectKeys.count() != 0 )
        rc = m_SelectKeys.at(0)->getPosition();

    return rc;
}

void QKeyDesignWidget::arrangeSelectKeys( KeyArrange eArrange, int nKeyWidth, int nKeyHeight, int nKeyInterval )
{
    if ( m_SelectKeys.count() <= 1 ) return;

    // sort
    QVector<CSoftkey*> SortKeys( m_SelectKeys );

    CSoftkey *key, *keyNext;
    QRect rcKey, rcKeyNext;

    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
    {
        for ( int nJ=nI+1 ; nJ<SortKeys.count() ; nJ++ )
        {
            key = SortKeys.at(nI);
            keyNext = SortKeys.at(nJ);
            rcKey = key->getPosition();
            rcKeyNext = keyNext->getPosition();
            qDebug() << QString("key Name: %1").arg(key->getName());

            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
            {
                // swap
                CSoftkey* pI = SortKeys.at(nI);
                CSoftkey* pJ = SortKeys.at(nJ);
                SortKeys.replace( nJ, pI );
                SortKeys.replace( nI, pJ );
            }
        }
    }

    CSoftkey* pFirstKey = SortKeys.at(0);

    QRect rcFirst = pFirstKey->getPosition();
    QPoint ptFirst = rcFirst.topLeft();

    QRect rcMergedTrack;
    for ( int nI=0 ; nI<SortKeys.count() ; nI++ )
    {
        CSoftkey* key = SortKeys.at(nI);
        rcKey.setLeft( ptFirst.x() );
        rcKey.setTop( ptFirst.y() );
        rcKey.setRight( rcKey.left() + nKeyWidth );
        rcKey.setBottom( rcKey.top() + nKeyHeight );

        switch ( eArrange )
        {
        case KeyArrangeHorizontal:
            rcKey.setLeft( rcKey.left() + (nKeyWidth + nKeyInterval) * (int)nI );
            rcKey.setRight( rcKey.right() + (nKeyWidth + nKeyInterval) * (int)nI );
            break;
        case KeyArrangeVertical:
            rcKey.setTop( rcKey.top() + (nKeyHeight + nKeyInterval) * (int)nI );
            rcKey.setBottom( rcKey.bottom() + (nKeyHeight + nKeyInterval) * (int)nI );
            break;
        }

        key->setPosition( rcKey );

        rcMergedTrack = rcMergedTrack.united( rcKey );
    }

    if ( !rcMergedTrack.isNull() && !rcMergedTrack.isEmpty() )
    {
        m_rcDevTracker = rcMergedTrack;
        m_KeyTracker.m_rect = deviceToScreen( rcMergedTrack );
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
    }
    else
    {
        m_rcDevTracker.setRect( 0,0,0,0 );
        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    }
    update();
}

void QKeyDesignWidget::makeArrangedKeys( KeyArrange eArrange, int nKeyCount, int nKeyWidth, int nKeyHeight, int nKeyInterval )
{
    QRect rcKey;
    int nV = DEV_COORD * 5 / 100 / 2;
    rcKey.setLeft( nV );
    rcKey.setTop( nV );
    nV = DEV_COORD * 10 / 100 / 2;
    rcKey.setRight( rcKey.left() + nV );
    rcKey.setBottom( rcKey.top() + nV );

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    updateKeys();

    nV = DEV_COORD * 1 / 100;
    for ( int i=0 ; i<nKeyCount ; i++ )
    {
        int nIndex = Keys.addSoftkey( false, rcKey );

        if ( nIndex < 0 ) break;

        m_SelectKeys.push_back( Keys[nIndex] );


        rcKey.adjust( nV, nV, nV, nV );
    }

    arrangeSelectKeys( eArrange, nKeyWidth, nKeyHeight, nKeyInterval );
}


void QKeyDesignWidget::alignSelectKeys( KeyAlign eAlign )
{
    if ( m_SelectKeys.count() <= 1 ) return;

    QRect rcBase;
    CSoftkey *key, *keyNext;
    QRect rcKey, rcKeyNext;

    // sort
    QVector<CSoftkey*> SortKeys( m_SelectKeys );

    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
    {
        for ( int nJ=nI ; nJ<SortKeys.count() ; nJ++ )
        {
            key = SortKeys.at(nI);
            keyNext = SortKeys.at(nJ);
            rcKey = key->getPosition();
            rcKeyNext = keyNext->getPosition();
            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
            {
                // swap
                CSoftkey* pI = SortKeys.at(nI);
                CSoftkey* pJ = SortKeys.at(nJ);
                SortKeys.replace( nJ, pI );
                SortKeys.replace( nI, pJ );
            }
        }
    }

    switch ( eAlign )
    {
    case KeyAlignLeft:
        rcBase = SortKeys.at(0)->getPosition();
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDx = rcKey.left() - rcBase.left();
            rcKey.setLeft( rcKey.left() - nDx );
            rcKey.setRight( rcKey.right() - nDx );
            key->setPosition( rcKey );
        }
        break;
    case KeyAlignCenter:
        rcBase.setRect( 0,0,0,0 );
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            rcBase = rcBase.united( rcKey );
        }
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDx = rcKey.center().x() - rcBase.center().x();
            rcKey.setLeft( rcKey.left() - nDx );
            rcKey.setRight( rcKey.right() - nDx );
            key->setPosition( rcKey );
        }
        break;
    case KeyAlignRight:
        rcBase = SortKeys.at(SortKeys.count()-1)->getPosition();
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDx = rcKey.right() - rcBase.right();
            rcKey.setLeft( rcKey.left() - nDx );
            rcKey.setRight( rcKey.right() - nDx );
            key->setPosition( rcKey );
        }
        break;
    case KeyAlignTop:
        rcBase = SortKeys.at(0)->getPosition();
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDy = rcKey.top() - rcBase.top();
            rcKey.setTop( rcKey.top() - nDy );
            rcKey.setBottom( rcKey.bottom() - nDy );
            key->setPosition( rcKey );
        }
        break;
    case KeyAlignMiddle:
        rcBase.setRect( 0,0,0,0 );
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            rcBase = rcBase.united( rcKey );
        }
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDy = rcKey.center().y() - rcBase.center().y();
            rcKey.setTop( rcKey.top() - nDy );
            rcKey.setBottom( rcKey.bottom() - nDy );
            key->setPosition( rcKey );
        }
        break;
    case KeyAlignBottom:
        rcBase = SortKeys.at(SortKeys.count()-1)->getPosition();
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            int nDy = rcKey.bottom() - rcBase.bottom();
            rcKey.setTop( rcKey.top() - nDy );
            rcKey.setBottom( rcKey.bottom() - nDy );
            key->setPosition( rcKey );
        }
        break;
    }

    QRect rcMergedTrack;
    rcMergedTrack.setRect( 0,0,0,0 );
	
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);
        QRect rcKey = key->getPosition();
        rcMergedTrack = rcMergedTrack.united( rcKey );
    }

    if ( !rcMergedTrack.isNull() && !rcMergedTrack.isEmpty() )
    {
        m_rcDevTracker = rcMergedTrack;
        m_KeyTracker.m_rect = deviceToScreen( rcMergedTrack );
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
    }
    else
    {
        m_rcDevTracker.setRect( 0,0,0,0 );
        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    }

    update();
}

void QKeyDesignWidget::distribSelectKeys( Distrib eDistrib )
{
    if ( m_SelectKeys.count() <= 1 ) return;

    CSoftkey* key;
    QRect rcKey;

    CSoftkey* keyNext;
    QRect rcKeyNext;

    QVector<CSoftkey*> SortKeys( m_SelectKeys );

    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
    {
        for ( int nJ=nI ; nJ<SortKeys.count() ; nJ++ )
        {
            key = SortKeys.at(nI);
            keyNext = SortKeys.at(nJ);
            rcKey = key->getPosition();
            rcKeyNext = keyNext->getPosition();
            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
            {
                // swap
                CSoftkey* pI = SortKeys.at(nI);
                CSoftkey* pJ = SortKeys.at(nJ);
                SortKeys.replace( nJ, pI );
                SortKeys.replace( nI, pJ );
            }
        }
    }

    float fAveInterval = 0.0f;
    int nInterCount = 0;
    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
    {
        key = SortKeys.at(nI);
        keyNext = SortKeys.at(nI+1);

        rcKey = key->getPosition();
        rcKeyNext = keyNext->getPosition();

        switch ( eDistrib )
        {
        case DistribHorzEqualGap:
            fAveInterval += (rcKeyNext.left() - rcKey.right());
            break;
        case DistribVertEqualGap:
            fAveInterval += (rcKeyNext.top() - rcKey.bottom());
            break;
        }
        nInterCount ++;
    }
    if ( nInterCount != 0 )
        fAveInterval /= nInterCount;

    if ( fAveInterval != 0.0f )
    {
        key = SortKeys.at(0);
        QRect rcBase = key->getPosition();

        float fBaseXY;
        switch ( eDistrib )
        {
        case DistribHorzEqualGap:
            fBaseXY = (float)rcBase.right();
            break;
        case DistribVertEqualGap:
            fBaseXY = (float)rcBase.bottom();
            break;
        }

        for ( int nI=1 ; nI<SortKeys.count() ; nI++ )
        {
            key = SortKeys.at(nI);

            rcKey = key->getPosition();

            int nT;
            switch ( eDistrib )
            {
            case DistribHorzEqualGap:
                nT = rcKey.width();
                rcKey.setLeft( (int)(fBaseXY + fAveInterval + 0.5f) );
                rcKey.setRight( rcKey.left() + nT );

                fBaseXY += fAveInterval + nT;
                break;
            case DistribVertEqualGap:
                nT = rcKey.height();
                rcKey.setTop( (int)(fBaseXY + fAveInterval + 0.5f) );
                rcKey.setBottom( rcKey.top() + nT );

                fBaseXY += fAveInterval + nT;
                break;
            }

            key->setPosition( rcKey );
        }
    }

    QRect rcMergedTrack;
    rcMergedTrack.setRect( 0,0,0,0 );
	
    for ( int nI=0 ; nI<SortKeys.count() ; nI++ )
    {
        CSoftkey* key = SortKeys.at(nI);
        QRect rcKey = key->getPosition();
        rcMergedTrack = rcMergedTrack.united( rcKey );
    }

    if ( !rcMergedTrack.isNull() && !rcMergedTrack.isEmpty() )
    {
        m_rcDevTracker = rcMergedTrack;
        m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
    }
    else
    {
        m_rcDevTracker.setRect( 0,0,0,0 );
        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    }

    update();
}

void QKeyDesignWidget::setScreenSize( ScreenSize eScrnSize )
{
    m_eScrnSize = eScrnSize;

    m_SelectKeys.clear();
    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    m_rcDevTracker.setRect( 0,0,0,0 );

    resizeScreen();

    update();
}

void QKeyDesignWidget::adjustSizeSelectKeys( AdjustSize eAdjust )
{
    if ( m_SelectKeys.count() <= 1 ) return;

    CSoftkey* key = m_SelectKeys.at(0);
    QRect rcBase = key->getPosition();

    QRect rcKey;

    switch ( eAdjust )
    {
    case AdjustSizeSameWidth:
        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            rcKey.setRight( rcKey.left() + rcBase.width() );
            key->setPosition( rcKey );
        }
        break;
    case AdjustSizeSameHeight:
        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            rcKey.setBottom( rcKey.top() + rcBase.height() );
            key->setPosition( rcKey );
        }
        break;
    case AdjustSizeSameBoth:
        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
        {
            key = m_SelectKeys.at(nI);
            rcKey = key->getPosition();
            rcKey.setRight( rcKey.left() + rcBase.width() );
            rcKey.setBottom( rcKey.top() + rcBase.height() );
            key->setPosition( rcKey );
        }
        break;
    }

    QRect rcMergedTrack;
    rcMergedTrack.setRect( 0,0,0,0 );
	
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);
        QRect rcKey = key->getPosition();
        rcMergedTrack = rcMergedTrack.united( rcKey );
    }

    if ( !rcMergedTrack.isNull() && !rcMergedTrack.isEmpty() )
    {
        m_rcDevTracker = rcMergedTrack;
        m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
    }
    else
    {
        m_rcDevTracker.setRect( 0,0,0,0 );
        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    }

    update();
}

QRect QKeyDesignWidget::deviceToScreen( const QRect& rcDevice, bool bTranslate/*=true*/ )
{
    QRect rcScreen( rcDevice );

    if ( bTranslate )
    {
        QPoint ptOffset = screenToDevice( m_rcScreen.topLeft(), false );
        rcScreen.adjust( ptOffset.x(), ptOffset.y(), ptOffset.x(), ptOffset.y() );
    }

    rcScreen.setLeft( rcScreen.left() * m_rcScreen.width() / DEV_COORD );
    rcScreen.setTop( rcScreen.top() * m_rcScreen.height() / DEV_COORD );
    rcScreen.setRight( rcScreen.right() * m_rcScreen.width() / DEV_COORD );
    rcScreen.setBottom( rcScreen.bottom() * m_rcScreen.height() / DEV_COORD );

    return rcScreen;
}

QRect QKeyDesignWidget::screenToDevice( const QRect& rcScreen, bool bTranslate/*=true*/ )
{
    QRect rcDevice( rcScreen );

    rcDevice.setLeft( rcScreen.left() * DEV_COORD / m_rcScreen.width() );
    rcDevice.setTop( rcScreen.top() * DEV_COORD / m_rcScreen.height() );
    rcDevice.setRight( rcScreen.right() * DEV_COORD / m_rcScreen.width() );
    rcDevice.setBottom( rcScreen.bottom() * DEV_COORD / m_rcScreen.height() );

    if ( bTranslate )
    {
        QPoint ptOffset = screenToDevice( m_rcScreen.topLeft(), false );
        rcDevice.adjust( -ptOffset.x(), -ptOffset.y(), -ptOffset.x(), -ptOffset.y() );
    }

    return rcDevice;
}

QPoint QKeyDesignWidget::deviceToScreen( const QPoint& ptDevice, bool bTranslate/*=true*/ )
{
    QPoint ptScreen( ptDevice );

    ptScreen.setX( ptScreen.x() * m_rcScreen.width() / DEV_COORD );
    ptScreen.setY( ptScreen.y() * m_rcScreen.height() / DEV_COORD );

    if ( bTranslate )
    {
        ptScreen.setX( ptScreen.x() + m_rcScreen.left() );
        ptScreen.setY( ptScreen.y() + m_rcScreen.top() );
    }

    return ptScreen;
}

QPoint QKeyDesignWidget::screenToDevice( const QPoint& ptScreen, bool bTranslate/*=true*/ )
{
    QPoint ptDevice( ptScreen );

    if ( bTranslate )
    {
        ptDevice.setX( ptDevice.x() - m_rcScreen.left() );
        ptDevice.setY( ptDevice.y() - m_rcScreen.top() );
    }

    ptDevice.setX( ptDevice.x() * DEV_COORD / m_rcScreen.width() );
    ptDevice.setY( ptDevice.y() * DEV_COORD / m_rcScreen.height() );

    return ptDevice;
}

int QKeyDesignWidget::getGroupIndex( const GroupKey* pGroup )
{
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    if ( pGroup == NULL )
        return -1;

    for ( int nI = 0 ; nI < GroupKeys.count() ; nI ++ )
    {
        if ( GroupKeys.at(nI) == pGroup )
            return (int)nI;
    }
    return -1;
}

void QKeyDesignWidget::draw( QPainter* painter )
{
    painter->save();

    QPen ScreenPen;
    ScreenPen.setColor( qRgb(77, 109, 243) );

    if ( m_eScreenMode != ScreenModePreview )
    {
        ScreenPen.setStyle( Qt::SolidLine );
        ScreenPen.setWidth( 4 );
    }
    else
    {
        ScreenPen.setStyle( Qt::DotLine );
        ScreenPen.setWidth( 1 );
    }

    painter->setPen( ScreenPen );
    painter->setBrush( Qt::NoBrush );

    QRect rcScreen = m_rcScreen;
    rcScreen.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

    painter->drawRect( rcScreen );

    painter->drawLine( rcScreen.center().x(), rcScreen.top(), rcScreen.center().x(), rcScreen.top()+5 );
    painter->drawLine( rcScreen.center().x(), rcScreen.bottom(), rcScreen.center().x(), rcScreen.bottom()-5 );
    painter->drawLine( rcScreen.left(), rcScreen.center().y(), rcScreen.left()+5, rcScreen.center().y() );
    painter->drawLine( rcScreen.right(), rcScreen.center().y(), rcScreen.right()-5, rcScreen.center().y() );

    if ( !m_bMoveScreen )
    {
        drawKeys( painter );
    }

    drawCalPos( painter );

    if ( m_bViewTouchPoint )
    {
        drawTouchPoint( painter );
    }

    painter->restore();

    if ( !m_bMoveScreen )
    {
        if ( m_eScreenMode == ScreenModeKeyDesign )
        {
            m_KeyTracker.draw( painter );
        }
    }

    QRect rcClient( 0 , 0, width(), height() );

    QRect rcClose;
    int nCloseWH = rcClient.width() / 36;
    rcClose.setLeft( rcClient.right() - nCloseWH );
    rcClose.setRight( rcClient.right() );
    rcClose.setTop( rcClient.top() );
    rcClose.setBottom( rcClient.top() + nCloseWH );

    rcClose.adjust( -20, 20, -20, 20 );

    m_rcCloseButton = rcClose;
    drawCloseButton( painter, rcClose );

    QRect rcTouchCount = rcClient;
    rcTouchCount.setLeft( rcClient.center().x() - 40 );
    rcTouchCount.setRight( rcClient.center().x() + 40 );
    rcTouchCount.setTop( rcClient.center().x() - 40 );
    rcTouchCount.setBottom( rcClient.center().y() + 40 );
    m_rcTouchCount = rcTouchCount;
    drawTouchCount( painter, m_rcTouchCount );
}

void QKeyDesignWidget::drawGrid( QPainter* painter )
{
    painter->save();

    QRect rc( 0, 0, width(), height() );
    QPen GridMajorPen( Qt::SolidLine );
    GridMajorPen.setWidth( 1 );
    GridMajorPen.setColor( m_clrGridMajor );
    QPen GridMinorPen( Qt::SolidLine );
    GridMinorPen.setWidth( 1 );
    GridMinorPen.setColor( m_clrGridMinor );
    painter->setPen( GridMinorPen );

    float fGridStep = m_rcScreen.width() / 100.0f;

    QPoint ptOffset = m_rcScreen.topLeft();

    float fX = ptOffset.x() - ((int)(ptOffset.x()  / fGridStep)) * fGridStep;
    for ( float i=fX ; i<(float)rc.width() ; i+=fGridStep )
    {
        painter->drawLine( (int)(i+0.5f), 0, (int)(i+0.5f), rc.height() );
    }

    float fY = ptOffset.y() - ((int)(ptOffset.y()  / fGridStep)) * fGridStep;
    for ( float i=fY ; i<rc.height() ; i+=fGridStep )
    {
        painter->drawLine( 0, (int)(i+0.5f), rc.width(), (int)(i+0.5f) );
    }

    // draw major grid
    {
        painter->setPen( GridMajorPen );

        float fMajorGridStep = fGridStep * 5.0f;
        float fX = ptOffset.x() - ((int)(ptOffset.x()  / fMajorGridStep)) * fMajorGridStep;
        for ( float i=fX ; i<(float)rc.width() ; i+=fMajorGridStep )
        {
            painter->drawLine( (int)(i+0.5f), 0, (int)(i+0.5f), rc.height() );
        }

        float fY = ptOffset.y() - ((int)(ptOffset.y()  / fMajorGridStep)) * fMajorGridStep;
        for ( float i=fY ; i<(float)rc.height() ; i+=fMajorGridStep )
        {
            painter->drawLine( 0, (int)(i+0.5f), rc.width(), (int)(i+0.5f) );
        }
    }

    painter->restore();
}

void QKeyDesignWidget::drawOutlineText( QPainter* painter, QRgb dwTextColor, QRgb dwOutlineColor, QString strText, QRect rc, QTextOption nFormat )
{
    painter->save();

    painter->setPen( dwOutlineColor );

    QRect rcText( rc );

    rcText.adjust( -1, 0, -1, 0 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( +2, 0, +2, 0 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( -1, -1, -1, -1 );
    painter->drawText( rcText, strText, nFormat );
    rcText.adjust( 0, +2, 0, +2 );
    painter->drawText( rcText, strText, nFormat );

    painter->setPen( dwTextColor );
    painter->drawText( rc, strText, nFormat );

    painter->restore();
}

void QKeyDesignWidget::drawKeys( QPainter* painter )
{
    painter->save();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QPen KeyPen( Qt::SolidLine );
    KeyPen.setWidth( 1 );
    KeyPen.setColor( m_clrKeyBorder );
    painter->setPen( KeyPen );

    QRect rcClient( 0, 0, width(), height() );
    QRect rcClip( 0, 0, rcClient.width(), rcClient.height() );
    QRect rcInter;

    QString strCaption;

    QRect rcKey;

    bool bShowNumber = false;
    int nFntHeight = 14;
    if ( Keys.getSize() > 0 )
    {
        CSoftkey* key = Keys[0];
        rcKey = deviceToScreen( key->getPosition() );
        bShowNumber = rcKey.height() > 30 ? true : false;

        int nKH = ( (rcKey.height() < rcKey.width()) ? rcKey.height() : rcKey.width() ) * 3 / 7;
        nFntHeight = nKH;

        if ( nFntHeight > 15 ) nFntHeight = 15;
    }

    for ( int i=0 ; i<Keys.getSize() ; i++ )
    {
        CSoftkey* key = Keys[i];

        rcKey = deviceToScreen( key->getPosition() );

        if ( !rcKey.intersects( rcClip ) )
            continue;

        QBrush brushKey( key->getShow() ? Qt::SolidPattern : Qt::BDiagPattern );

        if ( key->getGroup() == NO_GROUP )
        {
            QRgb dwNormalColor = s_dwNormalColor;

            if ( m_bBlink && m_bBlinkOnOff && m_pBlinkKey == key )
            {
                dwNormalColor = qRgb(255, 0, 0);
            }

            if ( m_pFocusKey == key )
            {
                if ( m_bFocusKeyOn )
                {
                    dwNormalColor = qRgb(255, 0, 0);
                }
            }

            if ( key->getShow() )
                brushKey.setColor( dwNormalColor );
            else
            {
                brushKey.setColor( qRgb(50, 50, 50) );

                painter->setBackgroundMode( Qt::OpaqueMode );
                painter->setBackground( QColor(dwNormalColor) );
            }
        }
        else
        {
            QRgb dwGroupColor = s_dwGroupColor[getGroupIndex(key->getGroup())];
            if ( m_bBlink && m_bBlinkOnOff && m_pBlinkGroup == key->getGroup() )
            {
                dwGroupColor = qRgb(255, 0, 0);
            }

            if ( m_pFocusKey == key )
            {
                if ( m_bFocusKeyOn )
                {
                    dwGroupColor = qRgb(255, 0, 0);
                }
            }

            if ( key->getShow() )
                brushKey.setColor( dwGroupColor );
            else
            {
                brushKey.setColor( qRgb(50, 50, 50) );

                painter->setBackgroundMode( Qt::OpaqueMode );
                painter->setBackground( QColor(dwGroupColor) );
            }
        }

        painter->setBrush( brushKey );

        painter->drawRect( rcKey );

        painter->setBackgroundMode( Qt::TransparentMode );

        //if ( m_eScreenMode != ScreenModePreview )
        {
            rcKey.adjust( 5, 5, -5, -5 );

            QString strName;
            strName = key->getName();
            if ( !strName.isEmpty() )
            {
                QFont fntText;
                if ( nFntHeight > 11 )
                {
                    painter->save();

                    fntText.setFamily( "Arial" );
                    fntText.setPixelSize( nFntHeight );

                    painter->setFont( fntText );
                    QRect rcText( rcKey );
                    rcText.setTop( rcKey.top() + rcKey.height()/2 );
                    QTextOption option( Qt::AlignCenter );
                    QFontMetrics fm( fntText );
                    drawOutlineText( painter, qRgb(50,50,50), qRgb(255, 255, 255), fm.elidedText( strName, Qt::ElideRight, rcText.width(), Qt::TextSingleLine ), rcText, option );
                    rcKey.setBottom ( rcText.top() );

                    painter->restore();
                }
            }

            if ( bShowNumber )
            {
                QString strCaption = QString("%1").arg(i+1);
                QTextOption option( Qt::AlignCenter );
                QFont ft( painter->font() );
                ft.setBold( true );
                painter->setFont( ft );
                QFontMetrics fm( ft );
                drawOutlineText( painter, qRgb(0,0,0), qRgb(255, 255, 255), fm.elidedText( strCaption, Qt::ElideRight, rcKey.width(), Qt::TextSingleLine ), rcKey, option );
            }
        }
    }

    painter->setCompositionMode( QPainter::RasterOp_NotDestination );

    QPen SelectPen( Qt::DashLine );
    SelectPen.setWidth( 2 );
    SelectPen.setColor( qRgb(77, 243, 109) );

    if ( m_SelectKeys.count() > 0 )
    {
        painter->setPen( SelectPen );
        painter->setBrush( Qt::NoBrush );

        CSoftkey* key1 = m_SelectKeys.at(0);

        rcKey = deviceToScreen( key1->getPosition() );

        rcKey.adjust( 2, 2, -2, -2 );

        if ( rcKey.intersects( rcClip ) )
        {
            painter->drawText( rcKey, "*", Qt::AlignLeft|Qt::AlignTop );
        }

        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            CSoftkey* key = m_SelectKeys.at(nI);

            rcKey = deviceToScreen( key->getPosition() );

            rcKey.adjust( 2, 2, -2, -2 );

            if ( !rcKey.intersects( rcClip ) )
                continue;

            painter->drawRect( rcKey );
        }

        painter->setPen( Qt::DashDotLine );
        painter->drawRect( m_KeyTracker.m_rect );
    }

    painter->restore();
}

void QKeyDesignWidget::drawCloseButton( QPainter* painter, QRect rcClose )
{
    if ( m_eScreenMode == ScreenModePreview )
        return;

    QPen penClose( Qt::SolidLine );
    penClose.setWidth( 1 );
    penClose.setColor( qRgb(200, 200, 200) );
    QBrush brushClose( m_bDownCloseButton ? qRgb(237, 28, 36) : m_clrCloseBtnBg );
    QBrush brushX( m_clrCloseBtnFg );

    painter->setPen( penClose );
    painter->setBrush( brushClose );

    painter->drawRoundedRect( rcClose, 7, 7 );

    QPoint ptC = rcClose.center();
    int nWH = rcClose.width() / 4;
    int nC = 4;
    QPainterPath path;
    path.moveTo( ptC.x()-nWH+nC, ptC.y()-nWH );
    path.lineTo( ptC.x(), ptC.y()-nC );
    path.lineTo( ptC.x()+nWH-nC, ptC.y()-nWH );
    path.lineTo( ptC.x()+nWH, ptC.y()-nWH+nC );
    path.lineTo( ptC.x()+nC, ptC.y() );
    path.lineTo( ptC.x()+nWH, ptC.y()+nWH-nC );
    path.lineTo( ptC.x()+nWH-nC, ptC.y()+nWH );
    path.lineTo( ptC.x(), ptC.y()+nC );
    path.lineTo( ptC.x()-nWH+nC, ptC.y()+nWH );
    path.lineTo( ptC.x()-nWH, ptC.y()+nWH-nC );
    path.lineTo( ptC.x()-nC, ptC.y() );
    path.lineTo( ptC.x()-nWH, ptC.y()-nWH+nC );
    path.closeSubpath();

    painter->setBrush( brushX );
    painter->drawPath( path );
}

void QKeyDesignWidget::drawTouchPoint( QPainter* painter )
{
    painter->save();

    QBrush brushTouch( qRgb(0, 255, 0) );
    QPen penTouch;
    penTouch.setStyle( Qt::SolidLine );
    penTouch.setWidth( 1 );
    penTouch.setColor( qRgb(0, 0, 0) );

    painter->setPen( penTouch );
    painter->setBrush( brushTouch );

    painter->drawEllipse( m_ptTouchPoint.x(), m_ptTouchPoint.y(), 5, 5 );
}

void QKeyDesignWidget::drawTouchCount( QPainter* painter, QRect rcTouchCount )
{
    if ( m_eScreenMode != ScreenModePreview || !m_bBlink )
        return;

    painter->save();

    QPen penTouchCount( Qt::SolidLine );
    penTouchCount.setWidth( 1 );
    penTouchCount.setColor( qRgb(200, 200, 200) );
    QBrush brushTouchCount ( m_bBlinkKeySet ? qRgb(237, 28, 28) : m_clrCloseBtnBg );//qRgb(28, 28, 237) );
    painter->setPen( penTouchCount );
    painter->setBrush( brushTouchCount );

    painter->drawRoundRect( rcTouchCount, rcTouchCount.width()/10, rcTouchCount.height()/10 );

    // draw text
    int nTextHeight = rcTouchCount.height() * 2 / 3;
    QFont fntText( "Arial" ); // size ?
    fntText.setPixelSize( nTextHeight );
    painter->setFont( fntText );

    QString strText = QString("%1").arg(m_nOldTouchCount);
    painter->setPen( m_bBlinkKeySet ? qRgb(255, 255, 255) : m_clrCloseBtnFg );//qRgb(128, 128, 128) );

    painter->drawText( rcTouchCount, strText, Qt::AlignHCenter|Qt::AlignVCenter );

    painter->restore();
}


void QKeyDesignWidget::drawCalPos( QPainter* painter )
{
    if ( !m_bBlink ) return;

    QRect rcCalKey;

    if ( m_pBlinkKey )
    {
        rcCalKey = deviceToScreen( m_pBlinkKey->getPosition() );
    }
    if ( m_pBlinkGroup )
    {
        for ( int nG = 0 ; nG < m_pBlinkGroup->getCount() ; nG++ )
        {
            CSoftkey* key = m_pBlinkGroup->getAt(nG);

            QRect rcKey = deviceToScreen( key->getPosition() );
            rcCalKey = rcCalKey.united( rcKey );
        }
    }

    if ( !rcCalKey.isEmpty() )
    {
        QPoint ptCal;
        switch ( m_nCalPos )
        {
        case 0:	// left-top
            ptCal.setX( rcCalKey.left() ); ptCal.setY( rcCalKey.top() );
            break;
        case 1: // right-top
            ptCal.setX( rcCalKey.right() ); ptCal.setY( rcCalKey.top() );
            break;
        case 2: // right-bottom
            ptCal.setX( rcCalKey.right() ); ptCal.setY( rcCalKey.bottom() );
            break;
        case 3: // left-bottom
            ptCal.setX( rcCalKey.left() ); ptCal.setY( rcCalKey.bottom() );
            break;
        }

        QRect rcFocus( ptCal.x()-2, ptCal.y()-2, 5, 5 );
        QPen penCal( Qt::SolidLine );
        penCal.setColor( qRgb(0, 255, 0) );
        if ( m_bBlinkKeySet )
            penCal.setWidth( 3 );
        else
            penCal.setWidth( 1 );

        painter->setBrush( Qt::NoBrush );
        painter->setPen( penCal );

        if ( m_bBlinkKeySet )
        {
            painter->drawLine( ptCal.x() - 15, ptCal.y(), ptCal.x() + 16, ptCal.y() );
            painter->drawLine( ptCal.x(), ptCal.y() - 15, ptCal.x(), ptCal.y() + 16 );
        }
        else
        {
            painter->drawLine( ptCal.x() - 9, ptCal.y(), ptCal.x() + 10, ptCal.y() );
            painter->drawLine( ptCal.x(), ptCal.y() - 9, ptCal.x(), ptCal.y() + 10 );
        }
        painter->drawRect( rcFocus );
    }
}

void QKeyDesignWidget::resizeScreen()
{
    m_rcScreen = m_rcScreenOrg;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QRect rcKey;
    QRect rcScreenFit( m_rcScreen );
    for ( int i=0 ; i<Keys.getSize() ; i++ )
    {
        CSoftkey* key = Keys[i];

        rcKey = deviceToScreen( key->getPosition() );

        rcScreenFit = rcScreenFit.united( rcKey );
    }

    int nOffsetX, nOffsetY;

    switch ( m_eScrnSize )
    {
    case ScreenSizeFit:
        {
        float fRW = (float)m_rcScreenOrg.width() / rcScreenFit.width();
        float fRH = (float)m_rcScreenOrg.height() / rcScreenFit.height();
        m_rcScreen.setLeft( (int)(fRW * m_rcScreenOrg.left() - rcScreenFit.left() * fRW + .5f) );
        m_rcScreen.setTop( (int)(fRH * m_rcScreenOrg.top() - rcScreenFit.top() * fRH + .5f) );
        m_rcScreen.setRight( (int)(fRW * m_rcScreenOrg.right() - rcScreenFit.left() * fRW + .5f) );
        m_rcScreen.setBottom( (int)(fRH * m_rcScreenOrg.bottom() - rcScreenFit.top() * fRH + .5f) );
        }

        nOffsetX = m_rcScreen.width() * 5 / 100;		// 5%
        nOffsetY = m_rcScreen.height() * 5 / 100;
        break;
    case ScreenSizeFull:
        nOffsetX = nOffsetY = 0;
        break;
    }

    m_rcScreen.adjust( nOffsetX, nOffsetY, -nOffsetX, -nOffsetY );
}

void QKeyDesignWidget::pushHistory()
{
    m_aryRedoHistoryKey.clear();
    m_aryRedoHistoryExtra.clear();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QString strExtra;
    QString strKey( Keys.save( strExtra, NULL ) );

    m_aryUndoHistoryKey.push_back( strKey );
    m_aryUndoHistoryExtra.push_back( strExtra );

    if ( m_aryUndoHistoryKey.count() >= 30 )
    {
        m_aryUndoHistoryKey.remove( 0 );
        m_aryUndoHistoryExtra.remove( 0 );
    }
}

void QKeyDesignWidget::popHistory()
{
    int nUndoSize = m_aryUndoHistoryKey.count();
    if ( nUndoSize <= 0 )
        return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QString strExtra;
    QString strKey( Keys.save( strExtra, NULL ) );

    m_aryRedoHistoryKey.push_back( strKey );
    m_aryRedoHistoryExtra.push_back( strExtra );

    strKey = m_aryUndoHistoryKey.at( nUndoSize - 1 );
    m_aryUndoHistoryKey.remove( nUndoSize - 1 );
    strExtra = m_aryUndoHistoryExtra.at( nUndoSize - 1 );
    m_aryUndoHistoryExtra.remove( nUndoSize - 1 );

    Keys.load( strKey, strExtra, NULL );
}

void QKeyDesignWidget::undo()
{
    popHistory();
    updateKeys();
    update();
}

void QKeyDesignWidget::redo()
{
    int nRedoSize = m_aryRedoHistoryKey.count();
    if ( nRedoSize <= 0 )
        return;

    updateKeys();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    QString strExtra;
    QString strKey( Keys.save( strExtra, NULL ) );

    m_aryUndoHistoryKey.push_back( strKey );
    m_aryUndoHistoryExtra.push_back( strExtra );

    if ( m_aryUndoHistoryKey.count() >= 30 )
    {
        m_aryUndoHistoryKey.remove( 0 );
        m_aryUndoHistoryExtra.remove( 0 );
    }

    strKey = m_aryRedoHistoryKey.at( nRedoSize - 1 );
    m_aryRedoHistoryKey.remove( nRedoSize - 1 );
    strExtra = m_aryRedoHistoryExtra.at( nRedoSize - 1 );
    m_aryRedoHistoryExtra.remove( nRedoSize - 1 );

    Keys.load( strKey, strExtra, NULL );

    update();
}

void QKeyDesignWidget::keySelectAll()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    m_SelectKeys.clear();
    QRect rcMergedTrack;

    for ( int i=0 ; i<Keys.getSize() ; i++ )
    {
        CSoftkey* key = Keys[i];
        m_SelectKeys.push_back( key );
        QRect rcKey = key->getPosition();
        rcMergedTrack = rcMergedTrack.united( rcKey );
    }

    if ( !rcMergedTrack.isNull() && !rcMergedTrack.isEmpty() )
    {
        m_rcDevTracker = rcMergedTrack;
        m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
    }
    else
    {
        m_rcDevTracker.setRect( 0,0,0,0 );
        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    }

    update();
}

void QKeyDesignWidget::keyCopyToClipboard()
{
    m_nPasteCount = 0;
    qDebug( "Copy" );
    m_ClipboardKeys.clear();
    if ( m_SelectKeys.count() > 0 )
    {
        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
        {
            m_ClipboardKeys.push_back( m_SelectKeys.at(nI) );
        }
    }
    else
    {
        QApplication::beep();
    }
}

void QKeyDesignWidget::keyPaste()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    if ( m_ClipboardKeys.count() > 0 )
    {
        if ( Keys.getSize() >= SOFT_KEY_MAX )
        {
            QApplication::beep();
            return;
        }

        pushHistory();
        m_SelectKeys.clear();

        QRect rcMergedTrack;
		
        m_nPasteCount++;
        QPoint ptOffsetXY( 20, 20 );
        ptOffsetXY = screenToDevice( ptOffsetXY, false );
        for ( int nI=0 ; nI<m_ClipboardKeys.count() ; nI++ )
        {
            CSoftkey* key = m_ClipboardKeys.at(nI);
            QRect rcPos = key->getPosition();
            int nX = ptOffsetXY.x() * m_nPasteCount;
            int nY = ptOffsetXY.y() * m_nPasteCount;
            rcPos.adjust( nX, nY, nX, nY );
            int nIndex = Keys.addSoftkey( key->getShow(), rcPos );
            if ( nIndex < 0 ) break;

            Keys[nIndex]->setName( key->getName() );
            Keys[nIndex]->setShow( key->getShow() );
            m_SelectKeys.push_back( Keys[nIndex] );
            QRect rcKey( Keys[nIndex]->getPosition() );
            rcMergedTrack = rcMergedTrack.united( rcKey );
        }

        if ( !rcMergedTrack.isEmpty() )
        {
            m_rcDevTracker = rcMergedTrack;
            m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
            m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
        }
        else
        {
            m_rcDevTracker.setRect( 0,0,0,0 );
            m_KeyTracker.m_rect.setRect( 0,0,0,0 );
        }

        update();
    }
    else
    {
        QApplication::beep();
    }
}

bool QKeyDesignWidget::isSelectKey( CSoftkey* key )
{
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        if ( m_SelectKeys.at(nI) == key ) return true;
    }
    return false;
}

GroupStatus QKeyDesignWidget::checkGroupStatus()
{
    bool bIsExistNoGroup = false;
    QVector<const GroupKey*> TempGroupArray;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);
        if ( key->getGroup() != NO_GROUP )
        {
            if ( !isContainGroup( TempGroupArray, key->getGroup() ) )
            {
                TempGroupArray.push_back(key->getGroup());
            }
        }
        else
        {
            bIsExistNoGroup = true;
        }
    }

    switch ( TempGroupArray.count() )
    {
    case 1:
        if ( bIsExistNoGroup )
        {
            return EnableGroup;
        }
        else
        {
            return EnableUngroup;
        }
        break;
    case 0:
        if ( bIsExistNoGroup )
        {
            return EnableGroup;
        }
    default:
        break;
    }

    return DisableGroup;
}

void QKeyDesignWidget::ungroup( const GroupKey* pGroup )
{
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    for ( int nG=0; nG<pGroup->getCount() ; nG++ )
    {
        CSoftkey* key = pGroup->getAt(nG);
        key->setGroup( NULL );
    }
    removeGroup( GroupKeys, pGroup );
}

void QKeyDesignWidget::setBlinkKey( GroupKey* group, int nCalPos, bool bSet )
{
    if ( m_bBlink == true )
    {
        cancelBlink();
    }

    m_pBlinkGroup = group;
    m_bBlink = true;
    m_nCalPos = nCalPos;
    m_bBlinkKeySet = bSet;

    m_nTimerBlink = startTimer( 200 );
}

void QKeyDesignWidget::setBlinkKey( CSoftkey* key, int nCalPos, bool bSet )
{
    if ( m_bBlink == true )
    {
        cancelBlink();
    }

    m_pBlinkKey = key;
    m_bBlink = true;
    m_nCalPos = nCalPos;
    m_bBlinkKeySet = bSet;

    m_nTimerBlink = startTimer( 500 );

    //if ( bSet )
    {
        update( m_rcTouchCount );
    }
}

void QKeyDesignWidget::setOnOff( int nSKNum, bool bIsOn )
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    if ( Keys.getSize() <= nSKNum || nSKNum < 0 )
        return;

    CSoftkey* key = Keys[nSKNum];

    m_pFocusKey = key;
    m_bFocusKeyOn = bIsOn;

    onInvalidateKey( key );
}

void QKeyDesignWidget::cancelBlink()
{
    if( m_nTimerBlink )
    {
        killTimer( m_nTimerBlink );
        m_nTimerBlink = 0;
    }

    m_bBlink = false;
    m_bBlinkOnOff = false;
    m_pBlinkGroup = NULL;
    m_pBlinkKey = NULL;

    update();
}

void QKeyDesignWidget::paintEvent(QPaintEvent *)
{
    if ( m_pSoftKeyDesignTool )
        m_pSoftKeyDesignTool->updateUIButtonState( (int)m_SelectKeys.count(), checkGroupStatus(), m_SelectKeys );

    QRect rcClient( 0, 0, width()-1, height()-1 );

    QPainter painterCanvas;

    painterCanvas.begin( this );

    //if( bInit )
    {
        painterCanvas.fillRect( rcClient, m_clrBackground );
        if ( m_eScreenMode != ScreenModePreview )
            drawGrid( &painterCanvas );
    }

    draw( &painterCanvas );

    if( m_eScreenMode == ScreenModePreview )
    {
        painterCanvas.setPen( Qt::black );
        painterCanvas.setBrush( Qt::NoBrush );
        painterCanvas.drawRect( rcClient.adjusted(1,1,-1,-1) );
    }

    painterCanvas.end();
}

void QKeyDesignWidget::resizeEvent(QResizeEvent *evt)
{
    if( evt->size().isNull() || evt->size().isEmpty() ) return;

//    if( evt->size().width() <= 0 || evt->size().height() <= 0 ) return;

    onUpdateScreen();
}

void QKeyDesignWidget::showEvent(QShowEvent *)
{
    if( m_eScreenMode == ScreenModeKeyDesign && m_pSoftKeyDesignTool && !m_pSoftKeyDesignTool->isVisible() )
        m_pSoftKeyDesignTool->show();
}

void QKeyDesignWidget::closeEvent(QCloseEvent *)
{
    updateKeys();

    emit closeWidget();

    if( m_pSoftKeyDesignTool )
        m_pSoftKeyDesignTool->close();
}

void QKeyDesignWidget::keyPressEvent(QKeyEvent *evt)
{
    if ( m_eScreenMode == ScreenModePreview || m_eScreenMode == ScreenModeTest )
        return;

    if ( evt->key() == Qt::Key_Escape )
    {
        if ( m_SelectKeys.count() > 0 )
        {
            m_SelectKeys.clear();

            QApplication::beep();
            m_KeyTracker.m_rect.setRect( 0,0,0,0 );
            m_rcDevTracker.setRect( 0,0,0,0 );
            update();
        }
        else
        {
            close();
            return;
        }
    }
    else
    {
        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        bool bIsCtrlDown = evt->modifiers() & Qt::ControlModifier;
        bool bIsShiftDown = evt->modifiers() & Qt::ShiftModifier;

        int nVk = evt->key();

        if ( nVk == Qt::Key_Left || nVk == Qt::Key_Right ||
            nVk == Qt::Key_Up || nVk == Qt::Key_Down )
        {
            int nOffsetXY = bIsShiftDown ? 1 : 10;

            QPoint ptScrnOffset(0, 0);
            switch ( nVk )
            {
            case Qt::Key_Left:
                ptScrnOffset.setX( ptScrnOffset.x() - nOffsetXY );
                break;
            case Qt::Key_Right:
                ptScrnOffset.setX( ptScrnOffset.x() + nOffsetXY );
                break;
            case Qt::Key_Up:
                ptScrnOffset.setY( ptScrnOffset.y() - nOffsetXY );
                break;
            case Qt::Key_Down:
                ptScrnOffset.setY( ptScrnOffset.y() + nOffsetXY );
                break;
            }

            if ( m_SelectKeys.count() > 0 )
            {
                pushHistory();

                QRect rcMergedTrack;

                for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
                {
                    CSoftkey* key = m_SelectKeys.at(nI);

                    QRect rcKey;
                    QPoint ptDev = screenToDevice(ptScrnOffset, false);

                    rcKey = key->getPosition();
                    rcKey.adjust( ptDev.x(), ptDev.y(), ptDev.x(), ptDev.y() );

                    rcMergedTrack = rcMergedTrack.united( rcKey );

                    key->setPosition( rcKey );
                }

                if ( !rcMergedTrack.isEmpty() )
                {
                    m_rcDevTracker = rcMergedTrack;
                    m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
                    m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
                }
                else
                {
                    m_rcDevTracker.setRect( 0,0,0,0 );
                    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
                }
                update();
            }
            else
            {
                m_rcScreen.adjust( ptScrnOffset.x(), ptScrnOffset.y(), ptScrnOffset.x(), ptScrnOffset.y() );
                update();
            }
        }
        else if ( nVk == Qt::Key_Home )
        {
            resizeScreen();
            update();
        }
        else if ( nVk == Qt::Key_Delete )
        {
            onRemoveSelectKeys();
        }
        else if ( bIsCtrlDown )
        {
            switch ( nVk )
            {
            case 'C':
                keyCopyToClipboard();
                break;
            case 'A':
                keySelectAll();
                break;
            case 'Z':
                undo();
                break;
            case 'Y':
                redo();
                break;
            case 'V':
                keyPaste();
                break;
            case 'G':
                onGroupSelectKeys();
                break;
            case 'U':
                onUngroupSelectKeys(true);
                break;
            }
        }
    }
}

void QKeyDesignWidget::mousePressEvent(QMouseEvent *evt)
{
    if ( m_eScreenMode == ScreenModePreview )
    {
        evt->ignore();
        return;
    }

    switch( evt->button() )
    {
    case Qt::LeftButton:
        mouseLButtonDown(evt);
        break;
    case Qt::MiddleButton:
        if ( !m_bCapture )
        {
            m_bCapture = true;
            m_bMoveScreen = true;
            m_ptMouseLast = evt->pos();
        }
        break;
    default:
        break;
    }
}

void QKeyDesignWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    if ( m_eScreenMode == ScreenModePreview )
    {
        evt->ignore();
        return;
    }

    switch( evt->button() )
    {
    case Qt::LeftButton:
    {
        if( m_bCapture )
        {
            m_bDownCloseButton = false;
            update( m_rcCloseButton );
            m_bCapture = false;
            if ( m_rcCloseButton.contains(evt->pos()) )
            {
                close();
            }
            return;
        }

        if ( m_eScreenMode != ScreenModeKeyDesign )
            return;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        QRect rcKey;
        for ( int i=0 ; i<Keys.getSize() ; i++ )
        {
            CSoftkey* key = Keys[i];
            rcKey = deviceToScreen(key->getPosition());

            if ( rcKey.contains( evt->pos() ) )
            {
                m_rcDevTracker = key->getPosition();
                m_KeyTracker.m_rect = rcKey;
                m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
                update();
                break;
            }
        }
    }
        break;
    case Qt::MidButton:
        if ( m_bCapture )
        {
            m_bCapture = false;
            m_bMoveScreen = false;
            update();
        }
        break;
    default:
        break;
    }
}

void QKeyDesignWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    switch( evt->type() )
    {
    case Qt::MidButton:
        updateKeys();
        resizeScreen();
        update();
        break;
    default:
        break;
    }
}

void QKeyDesignWidget::mouseMoveEvent(QMouseEvent *evt)
{
    if ( m_eScreenMode == ScreenModePreview || m_eScreenMode == ScreenModeTest )
        return;

    if ( m_bMoveScreen )
    {
        QPoint ptScrnOffset;
        ptScrnOffset = evt->pos() - m_ptMouseLast;

        m_rcScreen.adjust( ptScrnOffset.x(), ptScrnOffset.y(), ptScrnOffset.x(), ptScrnOffset.y() );

        if ( m_SelectKeys.count() > 0 )
        {
            QRect rcMergedTrack;
            for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
            {
                CSoftkey* key = m_SelectKeys.at(nI);
                QRect rcKey = key->getPosition();

                rcMergedTrack = rcMergedTrack.united( rcKey );
            }

            if ( !rcMergedTrack.isEmpty() )
            {
                m_rcDevTracker = rcMergedTrack;
                m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
                m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
            }
            else
            {
                m_rcDevTracker.setRect( 0,0,0,0 );
                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
            }
        }
        update();
    }

    switch( m_KeyTracker.hitTest( evt->pos() ) )
    {
    case QKeyTracker::hitNothing:
        setCursor( Qt::ArrowCursor );
        break;
    case QKeyTracker::hitTopLeft:
    case QKeyTracker::hitBottomRight:
        setCursor( Qt::SizeFDiagCursor );
        break;
    case QKeyTracker::hitTopRight:
    case QKeyTracker::hitBottomLeft:
        setCursor( Qt::SizeBDiagCursor );
        break;
    case QKeyTracker::hitTop:
    case QKeyTracker::hitBottom:
        setCursor( Qt::SizeVerCursor );
        break;
    case QKeyTracker::hitLeft:
    case QKeyTracker::hitRight:
        setCursor( Qt::SizeHorCursor );
        break;
    case QKeyTracker::hitMiddle:
        setCursor( Qt::SizeAllCursor );
        break;
    }

    m_ptMouseLast = evt->pos();
}

void QKeyDesignWidget::mouseLButtonDown(QMouseEvent* evt)
{
    if( m_rcCloseButton.contains( evt->pos() ) )
    {
        m_bDownCloseButton = true;
        m_bCapture = true;
        update( m_rcCloseButton );
        return;
    }

    if ( m_eScreenMode != ScreenModeKeyDesign )
        return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    QRect rcKey;
    QRect rcKeyScrn;

    m_bCtrlDown = evt->modifiers() & Qt::ControlModifier;
    m_bShiftDown = evt->modifiers() & Qt::ShiftModifier;
goto_ReCheck:
    if ( m_KeyTracker.m_rect.isEmpty() || m_KeyTracker.hitTest(evt->pos()) == QKeyTracker::hitNothing )
    {
        bool bOneClickMove = false;
        CSoftkey* keySelect = NULL;
        GroupKey* groupSelect = NULL;

        for ( int nI=GroupKeys.count()-1 ; nI>=0 ; nI-- )
        {
            GroupKey* pGroup = GroupKeys.at(nI);
            for ( int nG=pGroup->getCount()-1 ; nG>=0 ; nG-- )
            {
                CSoftkey* key = pGroup->getAt(nG);
                rcKeyScrn = deviceToScreen( key->getPosition() );
                rcKey = key->getPosition();
                if ( rcKeyScrn.contains( evt->pos() ) )
                {
                    groupSelect = pGroup;
                    bOneClickMove = true;
                    break;
                }
            }
            if ( bOneClickMove )
                break;
        }
        if ( !bOneClickMove )
        {
            for ( int i=Keys.getSize()-1 ; i>=0 ; i-- )
            {
                CSoftkey* key = Keys[i];
                rcKeyScrn = deviceToScreen( key->getPosition() );
                rcKey = key->getPosition();
                if ( rcKeyScrn.contains( evt->pos() ) )
                {
                    keySelect = key;
                    bOneClickMove = true;
                    break;
                }
            }
        }

        if ( bOneClickMove && !m_bCtrlDown )
        {
            if ( groupSelect )
            {
                m_SelectKeys.clear();

                QRect rcMergedTrack;

                for ( int nG=0 ; nG<groupSelect->getCount() ; nG++ )
                {
                    CSoftkey* key = groupSelect->getAt(nG);
                    m_SelectKeys.push_back( key );
                    rcKey = key->getPosition();

                    rcMergedTrack = rcMergedTrack.united( rcKey );
                }

                if ( !rcMergedTrack.isEmpty() )
                {
                    m_rcDevTracker = rcMergedTrack;
                    m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
                    m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
                }
                else
                {
                    QApplication::beep();
                    m_rcDevTracker.setRect( 0,0,0,0 );
                    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
                }
                update();

                goto goto_ReCheck;

            }
            if ( keySelect )
            {
                m_SelectKeys.clear();
                m_SelectKeys.push_back( keySelect );
                m_rcDevTracker = rcKey;
                m_KeyTracker.m_rect = deviceToScreen(rcKey);
                m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
                update();
                goto goto_ReCheck;
            }
        }

        // rubberband

        m_bCheckRubberBand = true;
        m_KeyTracker.m_rect.setRect( evt->pos().x(), evt->pos().y(), 0, 0 );

        m_KeyTracker.trackRubberBand( this, evt->pos(), true );
    }
    else
    {
        m_rcOldTracker = m_KeyTracker.m_rect;
        m_rcOldTracker = m_rcOldTracker.normalized();
        m_rcOldTracker.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

        m_rcOld = m_rcDevTracker;

        if ( m_KeyTracker.track( this, evt->pos(), true )  )
        {
            m_bCheckRubberBand = false;
        }
    }
}

void QKeyDesignWidget::timerEvent(QTimerEvent *evt)
{
    if ( evt->timerId() == m_nTimerBlink )
    {
        m_bBlinkOnOff = !m_bBlinkOnOff;
        if ( m_pBlinkGroup )
        {
            QRect rcMerged;
	
            for ( int nI=0 ; nI<m_pBlinkGroup->getCount() ; nI++ )
            {
                CSoftkey* key = m_pBlinkGroup->getAt(nI);
                QRect rcKey = deviceToScreen( key->getPosition() );
                rcMerged = rcMerged.united( rcKey );
            }
            rcMerged.adjust( -10, -10, 10, 10 );
            update( rcMerged );
        }
        if ( m_pBlinkKey )
        {
            QRect rcKey = deviceToScreen( m_pBlinkKey->getPosition() );
            rcKey.adjust( -10, -10, 10, 10 );
            update( rcKey );
        }
    }
}

void QKeyDesignWidget::onCustomContextMenuRequested(const QPoint& pos)
{
    if ( m_KeyTracker.m_rect.isEmpty() ) return;
    if ( !m_KeyTracker.m_rect.contains( pos ) ) return;

    QMenu myMenu;
    QString strCaption;

    switch ( checkGroupStatus() )
    {
    case EnableGroup:
        // Group
        strCaption = "Group";
        m_bMenuGroup = true;
        break;
    case EnableUngroup:
        // Ungroup
        strCaption = "Ungroup";
        m_bMenuGroup = false;
        break;
    case DisableGroup:
        // Disable
        strCaption = "Group";
        m_bMenuGroup = true;
        break;
    }

    myMenu.addAction( strCaption, this, SLOT(onContextMenuGrouping()) );
    myMenu.addSeparator();
    myMenu.addAction( "Remove", this, SLOT(onContextMenuRemove()) );

    myMenu.exec(pos);
}

void QKeyDesignWidget::onContextMenuGrouping()
{
    if ( m_SelectKeys.count() <= 0 )
        return;

    if ( m_bMenuGroup )
        onGroupSelectKeys();
    else
        onUngroupSelectKeys( true );
}

void QKeyDesignWidget::onContextMenuRemove()
{
    onRemoveSelectKeys();
}

int QKeyDesignWidget::onAddNewKey()
{
    pushHistory();

    QRect rcKey = getFirstSelectKey();
    if ( rcKey.isNull() || rcKey.isEmpty() )
    {
        int nOffset = DEV_COORD * 5 / 100 / 2;
        rcKey.setLeft( nOffset );
        rcKey.setTop( nOffset );
        nOffset = DEV_COORD * 10 / 100 / 2;
        rcKey.setRight( rcKey.left() + nOffset );
        rcKey.setBottom( rcKey.top() + nOffset );
    }
    else
    {
        int nOffset = DEV_COORD * 1 / 100;
        rcKey.adjust( nOffset, nOffset, nOffset, nOffset );
    }

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    int nIndex = Keys.addSoftkey( false, rcKey );

    if ( nIndex < 0 )
    {
        // Error
        return -1;
    }

    if( Keys.getSize() >= SOFT_KEY_MAX )
        emit keyStateCount( false );

    updateKeys();
    selectKey( nIndex );

    return nIndex;
}

void QKeyDesignWidget::onRemoveSelectKeys()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

    pushHistory();

    onUngroupSelectKeys( false );

    CSoftkey* key;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        key = m_SelectKeys.at(nI);
        Keys.remove( key, &Logics );
    }

    if( Keys.getSize() < SOFT_KEY_MAX )
        emit keyStateCount( true );

    updateKeys();
}

void QKeyDesignWidget::onGroupSelectKeys()
{
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    if ( GroupKeys.count() >= MAX_GROUP )
        return;

    pushHistory();

    onUngroupSelectKeys( false );

    GroupKey* pGroup = new GroupKey( &GroupKeys );
    GroupKeys.push_back( pGroup );
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);

        key->setGroup( pGroup );
        pGroup->add( key );
    }

    update();
}

void QKeyDesignWidget::onUngroupSelectKeys( bool bPushHistory )
{
    QVector<const GroupKey*> TempGroupArray;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);
        if ( key->getGroup() != NO_GROUP )
        {
            if ( !isContainGroup( TempGroupArray, key->getGroup() ) )
            {
                TempGroupArray.push_back(key->getGroup());
            }
        }
    }

    if ( bPushHistory && TempGroupArray.count() != 0 )
    {
        pushHistory();
    }

    for ( int nI=0; nI<TempGroupArray.count() ; nI++ )
    {
        ungroup( TempGroupArray.at(nI) );
    }

    update();
}

void QKeyDesignWidget::onReorderKeys()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

    updateKeys();

    Keys.reOrder( &Logics );

    update();
}

void QKeyDesignWidget::onInvalidateKey( CSoftkey* key )
{
    QRect rcKey = key->getPosition();
    rcKey = deviceToScreen( rcKey );
    rcKey.adjust( -3, -3, 3, 3 );
    update( rcKey );
}

void QKeyDesignWidget::onRecalcSelectionKeys( QRect rcOld, QRect rcNew )
{
    if ( rcOld == rcNew )
        return;

    pushHistory();

    double dX = (double)rcOld.width() / rcNew.width();
    double dY = (double)rcOld.height() / rcNew.height();

    QPoint ptT = rcNew.topLeft();
    ptT -= rcOld.topLeft();

    QRect rcMerge;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* pKey = m_SelectKeys.at(nI);

        QRect rcKey = pKey->getPosition();

        rcKey.setLeft( (int)( (rcKey.left() - rcOld.left()) / dX + rcNew.left() + 0.5 ) );
        rcKey.setRight( (int)( (rcKey.right() - rcOld.left()) / dX + rcNew.left() + 0.5 ) );
        rcKey.setTop( (int)( (rcKey.top() - rcOld.top()) / dY + rcNew.top() + 0.5 ) );
        rcKey.setBottom( (int)( (rcKey.bottom() - rcOld.top()) / dY + rcNew.top() + 0.5 ) );

        //CRect rcScKey = DeviceToScreen(rcKey);
        rcMerge = rcMerge.united( rcKey );
        pKey->setPosition( rcKey );
    }

    if ( !rcMerge.isEmpty() )
    {
        m_rcDevTracker = rcMerge;
        m_KeyTracker.m_rect = deviceToScreen(rcMerge);
        m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
        update();
    }

}

void QKeyDesignWidget::onUpdateScreen()
{
    int cx, cy;
    QRect rc( 0, 0, width(), height() );
    cx = rc.width();
    cy = rc.height();

    int nL, nT, nR, nB;
    nL = nT = 0;
    nR = cx;
    nB = cy;

    bool bRealScale = false;
    if ( m_pSoftKeyDesignTool && (m_pSoftKeyDesignTool->getUnit() == UnitMM) )
        bRealScale = true;

    if ( (m_eScreenMode == ScreenModeKeyDesign) && bRealScale )
    {
        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        double dScrnDimWidth, dScrnDimHeight;
        Keys.getScreenDimension( dScrnDimWidth, dScrnDimHeight );

        if ( dScrnDimWidth != 0.0 && dScrnDimHeight != 0.0 )
        {
            if ( dScrnDimHeight/dScrnDimWidth < 1.f )
            {
                nB = (int)(dScrnDimHeight/dScrnDimWidth * cx + 0.5);
            }
            else
            {
                nR = (int)(dScrnDimWidth/dScrnDimHeight * cy + 0.5);
            }
        }
    }

    m_rcScreen.setRect( nL, nT, nR-nL, nB-nT );
    m_rcScreenOrg = m_rcScreen;

    m_SelectKeys.clear();
    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
    m_rcDevTracker.setRect( 0,0,0,0 );

    resizeScreen();
}

void QKeyDesignWidget::onResetKeys()
{
    updateKeys();
}

void QKeyDesignWidget::onRubberBandFinish(bool bChanged)
{
    if( m_bCheckRubberBand )
    {
        QRect rcMergedTrack;
        QRect rcKey;
        QRect rcKeyScrn;

        GroupKeyArray& GroupKeys =T3kCommonData::instance()->getGroupKeys();
        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if( bChanged )
        {
            if ( !m_bShiftDown )
            {
                m_SelectKeys.clear();
            }

            for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
            {
                CSoftkey* key = m_SelectKeys.at(nI);
                rcKey = key->getPosition();
                rcMergedTrack = rcMergedTrack.united( rcKey );
            }

            QRect rcTrackerScrn;
            rcTrackerScrn = m_KeyTracker.m_rect;
            rcTrackerScrn = rcTrackerScrn.normalized();
            rcTrackerScrn.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

            // group check
            QVector<const GroupKey*> TempGroupArray;
            for ( int nI=GroupKeys.count()-1 ; nI>=0 ; nI-- )
            {
                GroupKey* pGroup = GroupKeys.at(nI);
                for ( int nG=pGroup->getCount()-1 ; nG>=0 ; nG-- )
                {
                    CSoftkey* key = pGroup->getAt(nG);
                    rcKeyScrn = deviceToScreen( key->getPosition() );
                    if ( rcKeyScrn.intersects( rcTrackerScrn ) )
                    {
                        if ( !isContainGroup( TempGroupArray, key->getGroup() ) )
                        {
                            TempGroupArray.push_back(key->getGroup());
                        }
                        break;
                    }
                }
            }

            for ( int nI=0; nI<TempGroupArray.count() ; nI++ )
            {
                const GroupKey* pGroup = TempGroupArray.at(nI);
                for ( int nG=0 ; nG<pGroup->getCount() ; nG++ )
                {
                    CSoftkey* key = pGroup->getAt(nG);
                    rcKey = key->getPosition();

                    rcMergedTrack = rcMergedTrack.united( rcKey );

                    if ( !isSelectKey(key) )
                        m_SelectKeys.push_back( key );
                }
            }

            for ( int i=0 ; i<Keys.getSize() ; i++ )
            {
                CSoftkey* key = Keys[i];
                rcKeyScrn = deviceToScreen(key->getPosition());
                rcKey = key->getPosition();

                if ( rcKeyScrn.intersects(rcTrackerScrn) )
                {
                    rcMergedTrack = rcMergedTrack.united( rcKey );

                    if ( !isSelectKey(key) )
                        m_SelectKeys.push_back( key );
                }
            }

            if ( !rcMergedTrack.isEmpty() )
            {
                m_rcDevTracker = rcMergedTrack;
                m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
                m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
            }
            else
            {
                QApplication::beep();
                m_rcDevTracker.setRect( 0,0,0,0 );
                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
            }
        }
        else
        {
            bool bCheckKey = false;

            if ( !m_bCtrlDown )
            {
                m_SelectKeys.clear();
            }

            for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
            {
                CSoftkey* key = m_SelectKeys.at(nI);
                rcKey = key->getPosition();
                rcMergedTrack = rcMergedTrack.united( rcKey );
            }

            for ( int i=0 ; i<Keys.getSize() ; i++ )
            {
                CSoftkey* key = Keys[i];
                rcKeyScrn = deviceToScreen(key->getPosition());
                rcKey = key->getPosition();

                if ( rcKeyScrn.contains(m_KeyTracker.m_ptStart) )
                {
                    bCheckKey = true;
                    rcMergedTrack = rcMergedTrack.united( rcKey );
                    if ( !isSelectKey(key) )
                        m_SelectKeys.push_back( key );
                    break;
                }
            }
            if ( bCheckKey )
            {
                if ( !rcMergedTrack.isEmpty() )
                {
                    m_rcDevTracker = rcMergedTrack;
                    m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
                    m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
                }
                else
                {
                    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
                    m_rcDevTracker.setRect( 0,0,0,0 );
                }
            }
            else
            {
                m_SelectKeys.clear();
                QApplication::beep();
                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
                m_rcDevTracker.setRect( 0,0,0,0 );
            }
        }

        m_bCtrlDown = false;
        m_bShiftDown = false;

        update();
    }
    else
    {
        QRect rcNewTracker;
        QRect rcNew;
        rcNewTracker = m_KeyTracker.m_rect;
        rcNewTracker = rcNewTracker.normalized();
        rcNewTracker.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

        double dX = (double)m_rcOldTracker.width() / rcNewTracker.width();
        double dY = (double)m_rcOldTracker.height() / rcNewTracker.height();
        rcNew = m_rcDevTracker;
        long lNewWidth = (long)(m_rcDevTracker.width() / dX + 0.5);
        long lNewHeight = (long)(m_rcDevTracker.height() / dY + 0.5);
        rcNew.setRight( rcNew.left() + lNewWidth );
        rcNew.setBottom( rcNew.top() + lNewHeight );
        if ( rcNewTracker.topLeft() != m_rcOldTracker.topLeft() )
        {
            QPoint ptOffset = rcNewTracker.topLeft() - m_rcOldTracker.topLeft();
            ptOffset = screenToDevice( ptOffset, false );
            rcNew.adjust( ptOffset.x(), ptOffset.y(), ptOffset.x(), ptOffset.y() );
        }

        onRecalcSelectionKeys( m_rcOld, rcNew );
    }
}

void QKeyDesignWidget::onGenerateKeys(KeyArrange eArrange, int nCount, int nW, int nH, int nInterval)
{
    if( m_SelectKeys.count() == 0 )
        makeArrangedKeys( eArrange, nCount, nW, nH, nInterval );
    else
        arrangeSelectKeys( eArrange, nW, nH, nInterval );
}

void QKeyDesignWidget::onAlignSelectedKeys(KeyAlign eAlign)
{
    alignSelectKeys( eAlign );
}

void QKeyDesignWidget::onAdjustSizeSelectedKeys(AdjustSize eSize)
{
    adjustSizeSelectKeys( eSize );
}

void QKeyDesignWidget::onDistribSelectKeys(Distrib eDistrib)
{
    distribSelectKeys( eDistrib );
}
