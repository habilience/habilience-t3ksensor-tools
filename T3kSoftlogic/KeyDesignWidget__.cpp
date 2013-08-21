#include "KeyDesignWidget.h"

#include "T3kSoftlogicDlg.h"
#include "T3kCommonData.h"
#include "GraphicsKeyItem.h"

#include <QDebug>
#include <QPainter>
#include <QtEvents>
#include <QApplication>
#include <QStackedLayout>

#define TRACK_OFFSETXY		(1)
#define MAX_GROUP			(13)
#define ID_TIMER_BLINK		(100)

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
    QGraphicsView(parent)
{
    setScene( &m_GraphicScens );

    m_pSelectionItem = NULL;

    connect( &m_GraphicScens, &QGraphicsScene::selectionChanged, this, &QKeyDesignWidget::onSelectionChanged );

    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_pGIBtnClose = NULL;

    m_eScreenMode = ScreenModePreview;

    m_pSoftKeyDesignTool = NULL;

    m_bMousePress = false;
    m_bMouseDrag = false;

    if( parent == NULL )
    {
        setWindowFlags( Qt::FramelessWindowHint );
        setDragMode( QGraphicsView::RubberBandDrag );

        m_eScreenMode = ScreenModeKeyDesign;

        m_pGIBtnClose = new QGraphicsButtonItem();
        m_pGIBtnClose->setObjectName( "BtnClose" );
        connect( m_pGIBtnClose, &QGraphicsButtonItem::Clicked, this, &QKeyDesignWidget::onBtnClose );
        connect( this, &QKeyDesignWidget::InvertDrawing, m_pGIBtnClose, &QGraphicsButtonItem::onUpdateInvert );

        QRect rcClient( 0 , 0, width(), height() );

        QRect rcClose;
        int nCloseWH = rcClient.width() / 36;
        rcClose.setLeft( rcClient.right() - nCloseWH );
        rcClose.setRight( rcClient.right() );
        rcClose.setTop( rcClient.top() );
        rcClose.setBottom( rcClient.top() + nCloseWH );

        rcClose.adjust( -20, 20, -20, 20 );

        m_pGIBtnClose->onUpdateSize( rcClose );
        m_pGIBtnClose->setZValue( 0 );

        m_pSoftKeyDesignTool = new QSoftKeyDesignToolWidget( this );

        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::closeWidget, this, &QKeyDesignWidget::close );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::addNewKey, this, &QKeyDesignWidget::onAddNewKey );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::removeSelectedKeys, this, &QKeyDesignWidget::onRemoveSelectedKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::groupSelectedKeys, this, &QKeyDesignWidget::onGroupSelectedKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::ungroupSelectedKeys, this, &QKeyDesignWidget::onUngroupSelectedKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::reorderKeys, this, &QKeyDesignWidget::onReorderKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::invalidateKey, this, &QKeyDesignWidget::onInvalidateKey );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::recalcSelectionKeys, this, &QKeyDesignWidget::onRecalcSelectionKeys );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::updateScreen, this, &QKeyDesignWidget::onUpdateScreen );
        connect( m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::resetKeys, this, &QKeyDesignWidget::onResetKeys );

        connect( this, &QKeyDesignWidget::keyStateCount, m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::onKeyStateCount );
        connect( this, &QKeyDesignWidget::SelectedKeys, m_pSoftKeyDesignTool, &QSoftKeyDesignToolWidget::onSelectedKeys );
    }

    m_rcScreen.setRect( 0, 0, width(), height() );
    m_rcScreenOrg = m_rcScreen;
//	m_pBitmap = NULL;
//    m_bMoveScreen = false;
    m_eScrnSize = ScreenSizeFit;

//    m_bBlink = false;
//    m_bBlinkOnOff = false;
//	m_pBlinkGroup = NULL;
//	m_pBlinkKey = NULL;
//	m_pFocusKey = NULL;
//    m_bFocusKeyOn = false;

//    m_bViewTouchPoint = false;
//    m_bDownCloseButton = false;

//    m_bBlinkKeySet = false;

//	m_nCalPos = 0;

    m_nOldTouchCount = 0;

    setMouseTracking( true );

    setInvertDrawing( false );

    init();
}

QKeyDesignWidget::~QKeyDesignWidget()
{
}

QObject* QKeyDesignWidget::findWantToParent(QObject *target, const char* strObjectName)
{
    Q_ASSERT( target );
    qDebug("QKeyDesignWidget : %s", target->objectName().toUtf8().data() );
    QObject* p = target;
    while( p )
    {
        if( p->inherits( strObjectName ) )
            break;

        p = p->parent();
    }

    Q_ASSERT( p );

    return p;
}

void QKeyDesignWidget::init()
{
    if ( m_eScreenMode == ScreenModeKeyDesign )
    {
        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        double dScrnDimWidth, dScrnDimHeight;
        Keys.getScreenDimension( dScrnDimWidth, dScrnDimHeight );

        if ( dScrnDimWidth != 0.0 && dScrnDimHeight != 0.0 )
        {
            double dScaleDimWidth = dScrnDimWidth / DEV_COORD;		// mm/dev
            double dScaleDimHeight = dScrnDimHeight / DEV_COORD;

            if( m_pSoftKeyDesignTool )
                m_pSoftKeyDesignTool->setScaleFactor( dScaleDimWidth, dScaleDimHeight );
        }
    }

    m_rcScreen.setRect( 0, 0, width(), height() );

    m_GraphicScens.setSceneRect( 0, 0, width(), height() );
    m_GraphicScens.clear();
}

//QGraphicsItemGroup* QKeyDesignWidget::getSelectKeys()
//{
//    return m_GraphicScens.createItemGroup( m_GraphicScens.selectedItems() );
//}

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
        m_clrGridMajor = qRgb(200, 200, 200);
        m_clrGridMinor = qRgb(230, 230, 230);

        s_dwGroupColor = s_dwGroupColor2;
    }
    else
    {
        m_clrBackground = qRgb(0, 0, 0);
        m_clrGridMajor = qRgb(80, 80, 80);
        m_clrGridMinor = qRgb(40, 40, 40);

        s_dwGroupColor = s_dwGroupColor1;
    }

    emit InvertDrawing(bInvert);
}

void QKeyDesignWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->save();

    painter->fillRect( rect, m_clrBackground );
    if ( m_eScreenMode != ScreenModePreview )
        drawGrid( painter );

    QPen penScreen;
    penScreen.setColor( qRgb(77, 109, 243) );
    if ( m_eScreenMode != ScreenModePreview )
    {
        penScreen.setWidth( 4 );
        penScreen.setStyle( Qt::SolidLine );
    }
    else
    {
        penScreen.setWidth( 1 );
        penScreen.setStyle( Qt::DotLine );
    }

    painter->setPen( penScreen );
    painter->setBrush( Qt::NoBrush );

    QRect rcScreen = m_rcScreen;
    rcScreen.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

    painter->drawRect( rcScreen );

    painter->drawLine( rcScreen.center().x(), rcScreen.top(), rcScreen.center().x(), rcScreen.top() );
    painter->drawLine( rcScreen.center().x(), rcScreen.bottom(), rcScreen.center().x(), rcScreen.bottom()-5 );
    painter->drawLine( rcScreen.left(), rcScreen.center().y(), rcScreen.left()+5, rcScreen.center().y() );
    painter->drawLine( rcScreen.right(), rcScreen.center().y(), rcScreen.right()-5, rcScreen.center().y() );

    painter->restore();
}

void QKeyDesignWidget::selectKey( int nIndex )
{
    m_SelectKeys.clear();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    if ( nIndex < 0 || nIndex >= Keys.getSize() ) return;

    CSoftkey* key = Keys[nIndex];

    if ( !isSelectKey(key) )
        m_SelectKeys.push_back( key );
}

//void QKeyDesignWidget::viewTouchPoint( long lX, long lY, bool bDown )
//{
//	if ( !m_bViewTouchPoint && bDown )
//	{
//        m_ptTouchPoint = deviceToScreen( QPoint(lX, lY), true );
//	}

//	m_bViewTouchPoint = bDown;

//    update( QRect( m_ptTouchPoint.x()-10, m_ptTouchPoint.y()-10, m_ptTouchPoint.x()+10, m_ptTouchPoint.y()+10 ) );

//    m_ptTouchPoint = deviceToScreen( QPoint(lX, lY), true );

//    update( QRect( m_ptTouchPoint.x()-10, m_ptTouchPoint.y()-10, m_ptTouchPoint.x()+10, m_ptTouchPoint.y()+10 ) );
//}

void QKeyDesignWidget::updateKeys()
{
    m_SelectKeys.clear();
//    m_ClipboardKeys.clear();

//    m_bBlink = false;
//    m_bBlinkOnOff = false;
//    m_pBlinkGroup = NULL;
//    m_pBlinkKey = NULL;
//    m_pFocusKey = NULL;
//    m_bFocusKeyOn = false;

    if ( winId() )
    {
        resizeScreen();

        //KillTimer( ID_TIMER_BLINK );

        m_GraphicScens.clear();

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        int nFntHeight = 14;
        if ( Keys.getSize() > 0 )
        {
            CSoftkey* key = Keys[0];
            QRect rcKey = deviceToScreen( key->getPosition() );

            int nKH = ( (rcKey.height() < rcKey.width()) ? rcKey.height() : rcKey.width() ) * 3 / 7;
            nFntHeight = nKH;

            if ( nFntHeight > 15 ) nFntHeight = 15;
        }

        for( int i=0; i<Keys.getSize(); i++ )
        {
            QRect rc( deviceToScreen( Keys[i]->getPosition() ) );

            QGraphicsKeyItem* pItem = new QGraphicsKeyItem( this, Keys[i]->getShow(), rc.width(), rc.height(), nFntHeight );
            pItem->setID( i );
            pItem->setName( Keys[i]->getName() );
            pItem->onUpdateColor( Keys[i]->getGroup() == NO_GROUP ? s_dwNormalColor : s_dwGroupColor[getGroupIndex(Keys[i]->getGroup())] );
            if( parent() == NULL )
                pItem->setEnableMoving( true );
            connect( this, &QKeyDesignWidget::UpdateEnable, pItem, &QGraphicsKeyItem::onUpdateEnable, Qt::QueuedConnection );
            //connect( this, &QKeyDesignWidget::UpdateResize, pItem, &QGraphicsKeyItem::onUpdaterResize, Qt::QueuedConnection );
            connect( this, &QKeyDesignWidget::InvertDrawing, pItem, &QGraphicsKeyItem::onInvertDrawing, Qt::QueuedConnection );

            m_GraphicScens.addItem( pItem );
        }

        if( parent() == NULL )
        {
            QGraphicsButtonItem* pGIBtnClose = new QGraphicsButtonItem();
            m_pGIBtnClose = pGIBtnClose;
            pGIBtnClose->setObjectName( "BtnClose" );
            connect( pGIBtnClose, &QGraphicsButtonItem::Clicked, this, &QKeyDesignWidget::onBtnClose );
            connect( this, &QKeyDesignWidget::InvertDrawing, pGIBtnClose, &QGraphicsButtonItem::onUpdateInvert );

            QRect rcClient( 0 , 0, width(), height() );

            QRect rcClose;
            int nCloseWH = rcClient.width() / 36;
            rcClose.setLeft( rcClient.right() - nCloseWH );
            rcClose.setRight( rcClient.right() );
            rcClose.setTop( rcClient.top() );
            rcClose.setBottom( rcClient.top() + nCloseWH );

            rcClose.adjust( -20, 20, -20, 20 );

            pGIBtnClose->onUpdateSize( rcClose );

            pGIBtnClose->setZValue( 0 );

            m_GraphicScens.addItem( pGIBtnClose );
        }

        m_GraphicScens.update();
    }
}

QRect QKeyDesignWidget::getFirstSelectKey()
{
    QRect rc;

    if ( m_SelectKeys.count() != 0 )
        rc = m_SelectKeys.at(0)->getPosition();

    return rc;
}

//void QKeyDesignWidget::arrangeSelectKeys( KeyArrange eArrange, int nKeyWidth, int nKeyHeight, int nKeyInterval )
//{
//	if ( m_SelectKeys.GetCount() <= 1 ) return;

//	// sort
//    QVector<CSoftkey*> SortKeys( m_SelectKeys );

//	CSoftkey *key, *keyNext;
//    QRect rcKey, rcKeyNext;

//    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
//	{
//        for ( int nJ=nI+1 ; nJ<SortKeys.count() ; nJ++ )
//		{
//            key = SortKeys.at(nI);
//            keyNext = SortKeys.at(nJ);
//			rcKey = key->getPosition();
//			rcKeyNext = keyNext->getPosition();
//            qDebug( "key Name: %s\r\n", key->getName() );

//            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
//			{
//                // swap
//                CSoftkey* pI = SortKeys.at(nI);
//                CSoftkey* pJ = SortKeys.at(nJ);
//                SortKeys.replace( nJ, pI );
//                SortKeys.replace( nI, pJ );
//			}
//		}
//	}

//    CSoftkey* pFirstKey = SortKeys.at(0);

//    QRect rcFirst = pFirstKey->getPosition();
//    //rcFirst = DeviceToScreen( rcFirst, false );
//    QPoint ptFirst = rcFirst.topLeft();

//    QRect rcMergedTrack;
//    for ( int nI=0 ; nI<SortKeys.count() ; nI++ )
//	{
//        CSoftkey* key = SortKeys.at(nI);
//        rcKey.setLeft( ptFirst.x() );
//        rcKey.setTop( ptFirst.y() );
//        rcKey.setRight( rcKey.left() + nKeyWidth );
//        rcKey.setBottom( rcKey.top() + nKeyHeight );

//		switch ( eArrange )
//		{
//		case KeyArrangeHorizontal:
//            rcKey.setLeft( rcKey.left() + (nKeyWidth + nKeyInterval) * (int)nI );
//            rcKey.setRight( rcKey.right() + (nKeyWidth + nKeyInterval) * (int)nI );
//			break;
//		case KeyArrangeVertical:
//            rcKey.setTop( rcKey.top() + (nKeyHeight + nKeyInterval) * (int)nI );
//            rcKey.setBottom( rcKey.bottom() + (nKeyHeight + nKeyInterval) * (int)nI );
//			break;
//		}

//		key->setPosition( rcKey );

//        rcMergedTrack = rcMergedTrack.united( rcKey );
//	}

//    if ( !rcMerageTrack.isNull() && !rcMergedTrack.isEmpty() )
//	{
//		m_rcDevTracker = rcMergedTrack;
////		m_KeyTracker.m_rect = deviceToScreen( rcMergedTrack );
////		m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//	}
//	else
//	{
//        m_rcDevTracker.setRect( 0,0,0,0 );
//        //m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//	}
//    update();
//}

//void QKeyDesignWidget::makeArrangedKeys( KeyArrange eArrange, int nKeyCount, int nKeyWidth, int nKeyHeight, int nKeyInterval )
//{
//    QRect rcKey;
//    int nV = DEV_COORD * 5 / 100 / 2;
//    rcKey.setLeft( nV );
//    rcKey.setTop( nV );
//    nV = DEV_COORD * 10 / 100 / 2;
//    rcKey.setRight( rcKey.left + nV );
//    rcKey.setBottom( rcKey.top + nV );

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    updateKeys();

//    nV = DEV_COORD * 1 / 100;
//	for ( int i=0 ; i<nKeyCount ; i++ )
//	{
//        int nIndex = Keys.addSoftkey( false, rcKey );

//		if ( nIndex < 0 ) break;

//        m_SelectKeys.push_back( Keys[nIndex] );


//        rcKey.adjust( nV, nV, nV, nV );
//	}

//    arrangeSelectKeys( eArrange, nKeyWidth, nKeyHeight, nKeyInterval );
//}


//void QKeyDesignWidget::alignSelectKeys( KeyAlign eAlign )
//{
//    if ( m_SelectKeys.count() <= 1 ) return;

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    QRect rcBase;
//	CSoftkey *key, *keyNext;
//    QRect rcKey, rcKeyNext;

//	// sort
//    QVector<CSoftkey*> SortKeys( m_SelectKeys );

//    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
//	{
//        for ( int nJ=nI ; nJ<SortKeys.count() ; nJ++ )
//		{
//            key = SortKeys.at(nI);
//            keyNext = SortKeys.at(nJ);
//			rcKey = key->getPosition();
//			rcKeyNext = keyNext->getPosition();
//            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
//			{
//				// swap
//                CSoftkey* pI = SortKeys.at(nI);
//                CSoftkey* pJ = SortKeys.at(nJ);
//                SortKeys.replace( nJ, pI );
//                SortKeys.replace( nI, pJ );
//			}
//		}
//	}

//	switch ( eAlign )
//	{
//	case KeyAlignLeft:
//        rcBase = SortKeys.at(0)->getPosition();
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDx = rcKey.left() - rcBase.left();
//            rcKey.setLeft( rcKey.left() - nDx );
//            rcKey.setRight( rcKey.right() - nDx );
//			key->setPosition( rcKey );
//		}
//		break;
//	case KeyAlignCenter:
//        rcBase.setRect( 0,0,0,0 );
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.ct(nI);
//			rcKey = key->getPosition();
//            rcBase = rcBase.united( rcKey );
//		}
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDx = rcKey.center().x() - rcBase.center().x();
//            rcKey.setLeft( rcKey.left() - nDx );
//            rcKey.setRight( rcKey.right() - nDx );
//			key->setPosition( rcKey );
//		}
//		break;
//	case KeyAlignRight:
//        rcBase = SortKeys.at(SortKeys.count()-1)->getPosition();
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDx = rcKey.right() - rcBase.right();
//            rcKey.setLeft( rcKey.left() - nDx );
//            rcKey.setRight( rcKey.right() - nDx );
//			key->setPosition( rcKey );
//		}
//		break;
//	case KeyAlignTop:
//        rcBase = SortKeys.at(0)->getPosition();
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDy = rcKey.top() - rcBase.top();
//            rcKey.setTop( rcKey.top() - nDy );
//            rcKey.setBottom( rcKey.bottom() - nDy );
//			key->setPosition( rcKey );
//		}
//		break;
//	case KeyAlignMiddle:
//        rcBase.setRect( 0,0,0,0 );
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            rcBase = rcBase.united( rcKey );
//		}
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDy = rcKey.center().y() - rcBase.center().y();
//            rcKey.setTop( rcKey.top() - nDy );
//            rcKey.setBottom( rcKey.bottom() - nDy );
//			key->setPosition( rcKey );
//		}
//		break;
//	case KeyAlignBottom:
//        rcBase = SortKeys.at(SortKeys.count()-1)->getPosition();
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            int nDy = rcKey.bottom() - rcBase.bottom();
//            rcKey.setTop( rcKey.top() - nDy );
//            rcKey.setBottom( rcKey.bottom() - nDy );
//			key->setPosition( rcKey );
//		}
//		break;
//	}

//    QRect rcMergedTrack;
//    rcMergedTrack.setRect( 0,0,0,0 );
	
//    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//	{
//        CSoftkey* key = m_SelectKeys.at(nI);
//        QRect rcKey = key->getPosition();
//        rcMergedTrack = rcMergedTrack.united( rcKey );
//	}

//    if ( !rcMerageTrack.isNull() && !rcMergedTrack.isEmpty() )
//	{
//		m_rcDevTracker = rcMergedTrack;
////		m_KeyTracker.m_rect = deviceToScreen( rcMergedTrack );
////		m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//	}
//	else
//	{
//        m_rcDevTracker.setRect( 0,0,0,0 );
//        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//	}

//    update();
//}

//void QKeyDesignWidget::distribSelectKeys( Distrib eDistrib )
//{
//    if ( m_SelectKeys.count() <= 1 ) return;

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//	CSoftkey* key;
//    QRect rcKey;

//	CSoftkey* keyNext;
//    QRect rcKeyNext;

//    QVector<CSoftkey*> SortKeys( m_SelectKeys );

//    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
//	{
//        for ( int nJ=nI ; nJ<SortKeys.count() ; nJ++ )
//		{
//            key = SortKeys.at(nI);
//            keyNext = SortKeys.at(nJ);
//			rcKey = key->getPosition();
//			rcKeyNext = keyNext->getPosition();
//            if ( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
//			{
//				// swap
//                CSoftkey* pI = SortKeys.at(nI);
//                CSoftkey* pJ = SortKeys.at(nJ);
//                SortKeys.replace( nJ, pI );
//                SortKeys.replace( nI, pJ );
//			}
//		}
//	}

//	float fAveInterval = 0.0f;
//	int nInterCount = 0;
//    for ( int nI=0 ; nI<SortKeys.count()-1 ; nI++ )
//	{
//        key = SortKeys.at(nI);
//        keyNext = SortKeys.at(nI+1);

//		rcKey = key->getPosition();
//		rcKeyNext = keyNext->getPosition();

//		switch ( eDistrib )
//		{
//		case DistribHorzEqualGap:
//            fAveInterval += (rcKeyNext.left() - rcKey.right());
//			break;
//		case DistribVertEqualGap:
//            fAveInterval += (rcKeyNext.top() - rcKey.bottom());
//			break;
//		}
//		nInterCount ++;
//	}
//	if ( nInterCount != 0 )
//		fAveInterval /= nInterCount;

//	if ( fAveInterval != 0.0f )
//	{
//        key = SortKeys.at(0);
//        QRect rcBase = key->getPosition();

//		float fBaseXY;
//		switch ( eDistrib )
//		{
//		case DistribHorzEqualGap:
//            fBaseXY = (float)rcBase.right();
//			break;
//		case DistribVertEqualGap:
//            fBaseXY = (float)rcBase.bottom();
//			break;
//		}

//        for ( int nI=1 ; nI<SortKeys.count() ; nI++ )
//		{
//            key = SortKeys.at(nI);

//			rcKey = key->getPosition();

//			int nT;
//			switch ( eDistrib )
//			{
//			case DistribHorzEqualGap:
//                nT = rcKey.width();
//                rcKey.setLeft( (int)(fBaseXY + fAveInterval + 0.5f) );
//                rcKey.setRight( rcKey.left() + nT );

//				fBaseXY += fAveInterval + nT;
//				break;
//			case DistribVertEqualGap:
//                nT = rcKey.height();
//                rcKey.setTop( (int)(fBaseXY + fAveInterval + 0.5f) );
//                rcKey.setBottom( rcKey.top() + nT );

//				fBaseXY += fAveInterval + nT;
//				break;
//			}

//			key->setPosition( rcKey );
//		}
//	}

//    QRect rcMergedTrack;
//    rcMergedTrack.setRect( 0,0,0,0 );
	
//    for ( int nI=0 ; nI<SortKeys.count() ; nI++ )
//	{
//        CSoftkey* key = SortKeys.at(nI);
//        QRect rcKey = key->getPosition();
//        rcMergedTrack = rcMergedTrack.united( rcKey );
//	}

//    if ( !rcMerageTrack.isNull() && !rcMergedTrack.isEmpty() )
//	{
//		m_rcDevTracker = rcMergedTrack;
////		m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////		m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//	}
//	else
//	{
//        m_rcDevTracker.setRect( 0,0,0,0 );
//        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//	}

//    update();
//}

//void QKeyDesignWidget::adjustSizeSelectKeys( AdjustSize eAdjust )
//{
//    if ( m_SelectKeys.count() <= 1 ) return;

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    CSoftkey* key = m_SelectKeys.at(0);
//    QRect rcBase = key->getPosition();

//    QRect rcKey;

//	switch ( eAdjust )
//	{
//	case AdjustSizeSameWidth:
//        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            rcKey.setRight( rcKey.left() + rcBase.width() );
//			key->setPosition( rcKey );
//		}
//		break;
//	case AdjustSizeSameHeight:
//        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            rcKey.setBottom( rcKey.top() + rcBase.height() );
//			key->setPosition( rcKey );
//		}
//		break;
//	case AdjustSizeSameBoth:
//        for ( int nI=1 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            key = m_SelectKeys.at(nI);
//			rcKey = key->getPosition();
//            rcKey.setRight( rcKey.left() + rcBase.width() );
//            rcKey.setBottom( rcKey.top() + rcBase.height() );
//			key->setPosition( rcKey );
//		}
//		break;
//	}

//    QRect rcMergedTrack;
//    rcMergedTrack.setRect( 0,0,0,0 );
	
//    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//	{
//        CSoftkey* key = m_SelectKeys.at(nI);
//        QRect rcKey = key->getPosition();
//        rcMergedTrack = rcMergedTrack.united( rcKey );
//	}

//    if ( !rcMerageTrack.isNull() && !rcMergedTrack.isEmpty() )
//	{
//		m_rcDevTracker = rcMergedTrack;
////		m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////		m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//	}
//	else
//	{
//        m_rcDevTracker.setRect( 0,0,0,0 );
//        m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//	}

//    update();
//}

//void QKeyDesignWidget::onDraw( QPainter* pDC )
//{
//    pDC->save();

//    QPen ScreenPen;
//    ScreenPen.setColor( qRgb(77, 109, 243) );

//	if ( m_eScreenMode != ScreenModePreview )
//    {
//        ScreenPen.setStyle( Qt::SolidLine );
//        ScreenPen.setWidth( 4 );
//    }
//	else
//    {
//        ScreenPen.setStyle( Qt::DotLine );
//        ScreenPen.setWidth( 1 );
//    }

//    pDC->setPen( ScreenPen );
//    pDC->setBrush( Qt::NoBrush );

//    QRect rcScreen = m_rcScreen;
//    rcScreen.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

//    pDC->drawRect( rcScreen );

//    pDC->drawLine( rcScreen.center().x(), rcScreen.top(), rcScreen.center().x(), rcScreen.top()+5 );
//    pDC->drawLine( rcScreen.center().x(), rcScreen.bottom(), rcScreen.center().x(), rcScreen.bottom()-5 );
//    pDC->drawLine( rcScreen.left(), rcScreen.center().y(), rcScreen.left()+5, rcScreen.center().y() );
//    pDC->drawLine( rcScreen.right(), rcScreen.center().y(), rcScreen.right()-5, rcScreen.center().y() );

//	if ( !m_bMoveScreen )
//	{
//        drawKeys( pDC );
//	}

//    drawCalPos( pDC );

//	if ( m_bViewTouchPoint )
//	{
//        drawTouchPoint( pDC );
//	}

//    pDC->restore();

//	if ( !m_bMoveScreen )
//	{
////        if ( !m_KeyTracker.m_rect.isEmpty() )
////			m_KeyTracker.draw( pDC );
//	}

//    QRect rcClient( 0 , 0, width(), height() );

//    QRect rcClose;
//    int nCloseWH = rcClient.width() / 36;
//    rcClose.setLeft( rcClient.right() - nCloseWH );
//    rcClose.setRight( rcClient.right() );
//    rcClose.setTop( rcClient.top() );
//    rcClose.setBottom( rcClient.top() + nCloseWH );

//    rcClose.adjust( -20, 20, -20, 20 );

//	m_rcCloseButton = rcClose;
//    drawCloseButton( pDC, rcClose );

//    QRect rcTouchCount = rcClient;
//    rcTouchCount.setLeft( rcClient.center().x() - 40 );
//    rcTouchCount.setRight( rcClient.center().x() + 40 );
//    rcTouchCount.setTop( rcClient.center().x() - 40 );
//    rcTouchCount.setBottom( rcClient.center().y() + 40 );
//	m_rcTouchCount = rcTouchCount;
//    drawTouchCount( pDC, m_rcTouchCount );
//}

//void QKeyDesignWidget::drawTouchPoint( QPainter* pDC )
//{
//    pDC->save();

//    QBrush brushTouch( qRgb(0, 255, 0) );
//    QPen penTouch;
//    penTouch.setStyle( Qt::SolidLine );
//    penTouch.setWidth( 1 );
//    penTouch.setColor( qRgb(0, 0, 0) );

//    pDC->setPen( penTouch );
//    pDC->setBrush( brushTouch );

//    pDC->drawEllipse( m_ptTouchPoint.x(), m_ptTouchPoint.y(), 5, 5 );
//}

QRect QKeyDesignWidget::deviceToScreen( const QRect rcDevice, bool bTranslate/*=true*/ )
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

    //if ( bTranslate )
    //	rcScreen.OffsetRect( m_rcScreen.left, m_rcScreen.top );

    return rcScreen;
}

QRect QKeyDesignWidget::screenToDevice( const QRect rcScreen, bool bTranslate/*=true*/ )
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

QPoint QKeyDesignWidget::deviceToScreen( const QPoint ptDevice, bool bTranslate/*=true*/ )
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

QPoint QKeyDesignWidget::screenToDevice( const QPoint ptScreen, bool bTranslate/*=true*/ )
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

//void QKeyDesignWidget::drawTouchCount( QPainter* pDC, QRect rcTouchCount )
//{
//	if ( m_eScreenMode != ScreenModePreview || !m_bBlink )
//		return;

//    pDC->save();

//    QPen penTouchCount( Qt::SolidLine );
//    penTouchCount.setWidth( 1 );
//    penTouchCount.setColor( qRgb(200, 200, 200) );
//    QBrush brushTouchCount ( m_bBlinkKeySet ? qRgb(237, 28, 28) : m_clrCloseBtnBg );//qRgb(28, 28, 237) );
//    pDC->setPen( penTouchCount );
//    pDC->setBrush( brushTouchCount );

//    pDC->drawRoundRect( rcTouchCount, rcTouchCount.width()/10, rcTouchCount.height()/10 );

//	// draw text
//    int nTextHeight = rcTouchCount.height() * 2 / 3;
//    QFont fntText( "Arial" ); // size ?
//    fntText.setPixelSize( nTextHeight );
//    pDC->setFont( fntText );

//    QString strText = QString("%d").arg(m_nOldTouchCount);
//    pDC->pen().setColor( m_bBlinkKeySet ? qRgb(255, 255, 255) : m_clrCloseBtnFg );//qRgb(128, 128, 128) );
//    pDC->drawText( rcTouchCount, strText, Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextSingleLine );

//    pDC->restore();
//}


//void QKeyDesignWidget::drawCalPos( QPainter* pDC )
//{
//	if ( !m_bBlink ) return;

//	m_nCalPos;
//    QRect rcCalKey;

//	if ( m_pBlinkKey )
//	{
//        rcCalKey = deviceToScreen( m_pBlinkKey->getPosition() );
//	}
//	if ( m_pBlinkGroup )
//	{
//        for ( int nG = 0 ; nG < m_pBlinkGroup->getCount() ; nG++ )
//		{
//            CSoftkey* key = m_pBlinkGroup->getAt(nG);

//            QRect rcKey = deviceToScreen( key->getPosition() );
//            rcCalKey = rcCalKey.united( rcKey );
//		}
//	}

//    if ( !rcCalKey.isEmpty() )
//	{
//        QPoint ptCal;
//		switch ( m_nCalPos )
//		{
//		case 0:	// left-top
//            ptCal.setX( rcCalKey.left() ); ptCal.setY( rcCalKey.top() );
//			break;
//		case 1: // right-top
//            ptCal.setX( rcCalKey.right() ); ptCal.setY( rcCalKey.top() );
//			break;
//		case 2: // right-bottom
//            ptCal.setX( rcCalKey.right() ); ptCal.setY( rcCalKey.bottom() );
//			break;
//		case 3: // left-bottom
//            ptCal.setX( rcCalKey.left() ); ptCal.setY( rcCalKey.bottom() );
//			break;
//		}

//        QRect rcFocus( ptCal.x()-2, ptCal.y()-2, ptCal.x()+3, ptCal.y()+3 );
//        QPen penCal( Qt::SolidLine );//( PS_SOLID, 1, qRgb(0, 255, 0) );
//        penCal.setColor( qRgb(0, 255, 0) );
//		if ( m_bBlinkKeySet )
//            penCal.setWidth( 3 );
//		else
//            penCal.setWidth( 1 );

//        pDC->setBrush( Qt::NoBrush );
//        pDC->setPen( penCal );

//		if ( m_bBlinkKeySet )
//		{
//            pDC->drawLine( ptCal.x() - 15, ptCal.y(), ptCal.x() + 16, ptCal.y() );
//            pDC->drawLine( ptCal.x(), ptCal.y() - 15, ptCal.x(), ptCal.y() + 16 );
//		}
//		else
//		{
//            pDC->drawLine( ptCal.x() - 9, ptCal.y(), ptCal.x() + 10, ptCal.y() );
//            pDC->drawLine( ptCal.x(), ptCal.y() - 9, ptCal.x(), ptCal.y() + 10 );
//		}
//        pDC->drawRect( rcFocus );
//	}
//}

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

void QKeyDesignWidget::resizeEvent(QResizeEvent *evt)
{
    if( evt->size().isNull() || evt->size().isEmpty() ) return;

    m_GraphicScens.setSceneRect( 0, 0, evt->size().width(), evt->size().height() );

    QRect rcClient( 0 , 0, width(), height() );

    QRect rcClose;
    int nCloseWH = rcClient.width() / 36;
    rcClose.setLeft( rcClient.right() - nCloseWH );
    rcClose.setRight( rcClient.right() );
    rcClose.setTop( rcClient.top() );
    rcClose.setBottom( rcClient.top() + nCloseWH );

    rcClose.adjust( -20, 20, -20, 20 );

    if( parent() == NULL )
    {
        foreach( QGraphicsItem* pItem, m_GraphicScens.items() )
        {
            QGraphicsButtonItem* pBtn = (QGraphicsButtonItem*)pItem;
            if( !pBtn->inherits( "BtnClose" ) ) continue;

            pBtn->onUpdateSize( rcClose );
        }
    }

    onUpdateScreen();
    updateKeys();
}

void QKeyDesignWidget::showEvent(QShowEvent *event)
{
    if( m_pSoftKeyDesignTool && !m_pSoftKeyDesignTool->isVisible() )
        m_pSoftKeyDesignTool->show();
}

void QKeyDesignWidget::closeEvent(QCloseEvent *)
{
//    updateKeys();

    //T3kSoftlogicDlg* pDlg = (T3kSoftlogicDlg*)findWantToParent( parent(), "T3kSoftlogicDlg" );
    //pDlg->onCloseCanvasWnd();

    if( m_pSoftKeyDesignTool )
        m_pSoftKeyDesignTool->close();

//	if ( m_pBitmap )
//		delete m_pBitmap;
//	m_pBitmap = NULL;
}

void QKeyDesignWidget::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Escape )
        close();
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

//void QKeyDesignWidget::undo()
//{
////	UpdateKeys();
//    popHistory();
//    updateKeys();
//    update();
//}

//void QKeyDesignWidget::redo()
//{
//    int nRedoSize = m_aryRedoHistoryKey.count();
//	if ( nRedoSize <= 0 )
//		return;

//    updateKeys();

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    QString strExtra;
//    QString strKey( Keys.save( strExtra, NULL ) );

//    m_aryUndoHistoryKey.push_back( strKey );
//    m_aryUndoHistoryExtra.push_back( strExtra );

//    if ( m_aryUndoHistoryKey.count() >= 30 )
//	{
//        m_aryUndoHistoryKey.remove( 0 );
//        m_aryUndoHistoryExtra.remove( 0 );
//	}

//    strKey = m_aryRedoHistoryKey.at( nRedoSize - 1 );
//    m_aryRedoHistoryKey.remove( nRedoSize - 1 );
//    strExtra = m_aryRedoHistoryExtra.at( nRedoSize - 1 );
//    m_aryRedoHistoryExtra.remove( nRedoSize - 1 );

//    Keys.load( strKey, strExtra, NULL );


//    update();
//}

//void QKeyDesignWidget::keySelectAll()
//{
//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    m_SelectKeys.clear();
//    QRect rcMergedTrack;

//	for ( int i=0 ; i<Keys.GetSize() ; i++ )
//	{
//		CSoftkey* key = Keys[i];
//        m_SelectKeys.push_back( key );
//        QRect rcKey = key->getPosition();
//        rcMergedTrack = rcMergedTrack.united( rcKey );
//	}

//    if ( !rcMergedTrack.isEmpty() )
//	{
//		m_rcDevTracker = rcMergedTrack;
////		m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////		m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//	}
//	else
//	{
//        m_rcDevTracker.setRect( 0,0,0,0 );
//        //m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//	}

//    update();
//}

//void QKeyDesignWidget::keyCopyToClipboard()
//{
//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//	m_nPasteCount = 0;
//    qDebug( "Copy" );
//    m_ClipboardKeys.clear();
//    if ( m_SelectKeys.count() > 0 )
//	{
//        for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//		{
//            m_ClipboardKeys.push_back( m_SelectKeys.at(nI) );
//		}
//	}
//	else
//	{
//        QApplication::beep();
//	}
//}

//void QKeyDesignWidget::keyPaste()
//{
//	CSoftkeyArray& Keys = pDlg->GetKeys();

//    if ( m_ClipboardKeys.count() > 0 )
//	{
//        if ( Keys.getSize() >= SOFT_KEY_MAX )
//		{
//            QApplication::beep();
//			return;
//		}

//        pushHistory();
//        m_SelectKeys.clear();

//        QRect rcMergedTrack;
		
//		m_nPasteCount++;
//        QPoint ptOffsetXY( 20, 20 );
//        ptOffsetXY = screenToDevice( ptOffsetXY, false );
//        for ( int nI=0 ; nI<m_ClipboardKeys.count() ; nI++ )
//		{
//            CSoftkey* key = m_ClipboardKeys.at(nI);
//            QRect rcPos = key->getPosition();
//            int nX = ptOffsetXY.x() * m_nPasteCount;
//            int nY = ptOffsetXY.y() * m_nPasteCount;
//            rcPos.adjust( nX, nY, nX, nY );
//            int nIndex = Keys.addSoftkey( key->getShow(), rcPos );
//            if ( nIndex < 0 ) break;

//			Keys[nIndex]->setName( key->getName() );
//			Keys[nIndex]->setShow( key->getShow() );
//            m_SelectKeys.push_back( Keys[nIndex] );
//            QRect rcKey( Keys[nIndex]->getPosition() );
//            rcMergedTrack = rcMergedTrack.united( rcKey );
//		}

//        if ( !rcMergedTrack.isEmpty() )
//		{
//			m_rcDevTracker = rcMergedTrack;
////			m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////			m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//		}
//		else
//		{
//            m_rcDevTracker.setRect( 0,0,0,0 );
//            //m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//		}

//        update();
//	}
//	else
//	{
//        QApplication::beep();
//	}
//}

//void QKeyDesignWidget::mousePressEvent(QMouseEvent *evt)
//{
//    switch( evt->button() )
//    {
//    case Qt::LeftButton:
//        onLButtonDown(evt->pos());
//        break;
//    case Qt::RightButton:
//    case Qt::MiddleButton:
//        evt->ignore();
//        break;
//    default:
//        break;
//    }
//}

//void QKeyDesignWidget::mouseReleaseEvent(QMouseEvent *evt)
//{
//    if ( m_eScreenMode == ScreenModePreview || m_eScreenMode == ScreenModeTest )
//    {
//        switch( evt->button() )
//        {
//        case Qt::RightButton:
//        case Qt::MidButton:
//            evt->ignore();
//            break;
//        default:
//            break;
//        }
//    }
//}

//void QKeyDesignWidget::mouseMoveEvent(QMouseEvent *evt)
//{
//    if ( m_eScreenMode == ScreenModePreview || m_eScreenMode == ScreenModeTest )
//        evt->ignore();
//}

//void QKeyDesignWidget::keyPressEvent(QKeyEvent *evt)
//{
//    if ( m_eScreenMode == ScreenModePreview || m_eScreenMode == ScreenModeTest )
//        return;

//    if ( evt->key() == Qt::Key_Escape )
//    {
//        if ( m_SelectKeys.count() > 0 )
//        {
//            m_SelectKeys.clear();

//            QApplication::beep();
//            //m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//            m_rcDevTracker.setRect( 0,0,0,0 );
//            update();
//        }
//        else
//        {
//            close();
//            return true;
//        }
//    }
//    else
//    {
//        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//        bool bIsCtrlDown = evt->modifiers() & Qt::ControlModifier;
//        bool bIsShiftDown = evt->modifiers() & Qt::ShiftModifier;

//        int nVk = evt->key();

//        if ( nVk == Qt::Key_Left || nVk == Qt::Key_Right ||
//            nVk == Qt::Key_Up || nVk == Qt::Key_Down )
//        {
//            int nOffsetXY = bIsShiftDown ? 1 : 10;

//            QPoint ptScrnOffset(0, 0);
//            switch ( nVk )
//            {
//            case Qt::Key_Left:
//                ptScrnOffset.setX( ptScrnOffset.x() - nOffsetXY );
//                break;
//            case Qt::Key_Right:
//                ptScrnOffset.setX( ptScrnOffset.x() + nOffsetXY );
//                break;
//            case Qt::Key_Up:
//                ptScrnOffset.setY( ptScrnOffset.y() - nOffsetXY );
//                break;
//            case Qt::Key_Down:
//                ptScrnOffset.setY( ptScrnOffset.y() + nOffsetXY );
//                break;
//            }

//            if ( m_SelectKeys.count() > 0 )
//            {
//                pushHistory();

//                QRect rcMergedTrack;

//                for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//                {
//                    CSoftkey* key = m_SelectKeys.at(nI);

//                    //CRect rcKey = DeviceToScreen(key->getPosition());
//                    //rcKey.OffsetRect( ptScrnOffset );
//                    QRect rcKey;
//                    QPoint ptDev = screenToDevice(ptScrnOffset, false);

//                    rcKey = key->getPosition();
//                    rcKey.adjust( ptDev.x(), ptDev.y(), ptDev.x(), ptDev.y() );

//                    rcMergedTrack = rcMergedTrack.united( rcKey );

//                    key->setPosition( rcKey );
//                }

//                if ( !rcMergedTrack.isEmpty() )
//                {
//                    m_rcDevTracker = rcMergedTrack;
////                    m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////                    m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//                }
//                else
//                {
//                    m_rcDevTracker.setRect( 0,0,0,0 );
//                    //m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//                }
//                update();
//            }
//            else
//            {
//                m_rcScreen.adjust( ptScrnOffset.x(), ptScrnOffset.y(), ptScrnOffset.x(), ptScrnOffset.y() );
//                //UpdateCanvas( ptScrnOffset );
//                update();
//            }
//        }
//        else if ( nVk == Qt::Key_Home )
//        {
//            resizeScreen();
//            update();
//        }
//        else if ( nVk == Qt::Key_Delete )
//        {
//            removeSelectKeys();
//        }
//        else if ( bIsCtrlDown )
//        {
//            switch ( nVk )
//            {
//            case 'C':
//                keyCopyToClipboard();
//                break;
//            case 'A':
//                keySelectAll();
//                break;
//            case 'Z':
//                undo();
//                break;
//            case 'Y':
//                redo();
//                break;
//            case 'V':
//                keyPaste();
//                break;
//            case 'G':
//                groupSelectKeys();
//                break;
//            case 'U':
//                ungroupSelectKeys(true);
//                break;
//            }
//        }
//    }
//}

//void QKeyDesignWidget::updateArea( QRect& rc, QPoint& ptScrnOffset, bool bUpdateAll/*=false*/ )
//{
//	const int nOffset = 5;

//	if ( bUpdateAll )
//	{
//        QRect rcUpdate = rc;
//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}
//		rcUpdate = rc;
//        rcUpdate.adjust( -ptScrnOffset.x, -ptScrnOffset.y, -ptScrnOffset.x, -ptScrnOffset.y );
//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}
//	}
//	else
//	{
//        QRect rcUpdate;

//        rcUpdate.setLeft( rc.left() - ptScrnOffset.x() );
//        rcUpdate.setTop( rc.top() - ptScrnOffset.y() );
//        rcUpdate.setRight( rc.left() + m_rcScreen.width() );
//        rcUpdate.setBottom( rc.top() + ptScrnOffset.y() );

//        rcUpdate.normalized();

//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}

//        rcUpdate.setLeft( rc.left() - ptScrnOffset.x() );
//        rcUpdate.setTop( rc.top() );
//        rcUpdate.setRight( rc.left() );
//        rcUpdate.setBottom( rc.bottom() - ptScrnOffset.y() );

//        rcUpdate.normalized();

//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}

//        rcUpdate.setLeft( rc.left() );
//        rcUpdate.setTop( rc.bottom() - ptScrnOffset.y() );
//        rcUpdate.setRight( rcUpdate.left() + rc.width() );
//        rcUpdate.setBottom( rcUpdate.top() + ptScrnOffset.y() );

//        rcUpdate.normalized();

//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}

//        rcUpdate.setLeft( rc.right() - ptScrnOffset.x() );
//        rcUpdate.setTop( rc.top() );
//        rcUpdate.setRight( rc.right() );
//        rcUpdate.setBottom( rc.bottom() - ptScrnOffset.y() );

//        rcUpdate.normalized();

//        if ( !rcUpdate.isEmpty() )
//		{
//            rcUpdate.adjust( -nOffset, -nOffset, nOffset, nOffset );
//            update( rcUpdate );
//		}
//	}
//}

//void QKeyDesignWidget::updateCanvas( QPoint& ptScrnOffset )
//{
//    updateArea( m_rcScreen, ptScrnOffset );

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    QRect rcKey;
//    for ( int i=0 ; i<Keys.getSize() ; i++ )
//	{
//		CSoftkey* key = Keys[i];
//        rcKey = deviceToScreen(key->getPosition());

//        updateArea( rcKey, ptScrnOffset, true );
//	}

////    if ( !m_KeyTracker.m_rect.isEmpty() )
////	{
////        QPoint ptDevScrnOffset = screenToDevice(ptScrnOffset, false );
////		m_rcDevTracker.adjust( ptDevScrnOffset.x(), ptDevScrnOffset.y(), ptDevScrnOffset.x(), ptDevScrnOffset.y() );
////		m_KeyTracker.m_rect.adjust( ptScrnOffset.x(), ptScrnOffset.y(), ptScrnOffset.x(), ptScrnOffset.y() );
////		updateArea( m_KeyTracker.m_rect, ptScrnOffset );
////	}
//}

void QKeyDesignWidget::drawGrid( QPainter* pDC )
{
    pDC->save();

    QRect rc( 0, 0, width()-1, height()-1 );
    QPen GridMajorPen( Qt::SolidLine );
    GridMajorPen.setWidth( 1 );
    GridMajorPen.setColor( m_clrGridMajor );
    QPen GridMinorPen( Qt::SolidLine );
    GridMinorPen.setWidth( 1 );
    GridMinorPen.setColor( m_clrGridMinor );
    pDC->setPen( GridMinorPen );
    //CPen* pOldPen = (CPen*)m_MemDC.SelectObject( &GridMinorPen );

    float fGridStep = m_rcScreen.width() / 100.0f;

    QPoint ptOffset = m_rcScreen.topLeft();

    float fX = ptOffset.x() - ((int)(ptOffset.x()  / fGridStep)) * fGridStep;
    for ( float i=fX ; i<(float)rc.width() ; i+=fGridStep )
    {
        pDC->drawLine( (int)(i+0.5f), 0, (int)(i+0.5f), rc.height() );
    }

    float fY = ptOffset.y() - ((int)(ptOffset.y()  / fGridStep)) * fGridStep;
    for ( float i=fY ; i<rc.height() ; i+=fGridStep )
    {
        pDC->drawLine( 0, (int)(i+0.5f), rc.width(), (int)(i+0.5f) );
    }

    // draw major grid
    {
        pDC->setPen( GridMajorPen );
        //CPen* pOldPen = (CPen*)m_MemDC.SelectObject( &GridMajorPen );

        float fMajorGridStep = fGridStep * 5.0f;
        float fX = ptOffset.x() - ((int)(ptOffset.x()  / fMajorGridStep)) * fMajorGridStep;
        for ( float i=fX ; i<(float)rc.width() ; i+=fMajorGridStep )
        {
            pDC->drawLine( (int)(i+0.5f), 0, (int)(i+0.5f), rc.height() );
        }

        float fY = ptOffset.y() - ((int)(ptOffset.y()  / fMajorGridStep)) * fMajorGridStep;
        for ( float i=fY ; i<(float)rc.height() ; i+=fMajorGridStep )
        {
            pDC->drawLine( 0, (int)(i+0.5f), rc.width(), (int)(i+0.5f) );
        }
    }

    pDC->restore();
}

bool QKeyDesignWidget::isSelectKey( CSoftkey* key )
{
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        if ( m_SelectKeys.at(nI) == key ) return true;
    }
    return false;
}

//void QKeyDesignWidget::onLButtonDown(QMouseEvent* evt)
//{
//    if ( (m_eScreenMode != ScreenModePreview) && m_rcCloseButton.contains( point ) )
//	{
//        m_bDownCloseButton = true;
////		if ( GetCapture() != this )
////			SetCapture();
//        update( m_rcCloseButton );
//		return;
//	}

//	if ( m_eScreenMode != ScreenModeKeyDesign )
//		return;

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
//    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

//    QRect rcKey;
//    QRect rcKeyScrn;

//    bool bIsCtrlDown = evt->modifiers() & Qt::ControlModifier;
//    bool bIsShiftDown = evt->modifiers() & Qt::ShiftModifier;
//goto_ReCheck:
//    if ( m_KeyTracker.m_rect.isEmpty() || m_KeyTracker.HitTest(point) == CRectTracker::hitNothing )
//	{
//        bool bOneClickMove = false;
//		CSoftkey* keySelect = NULL;
//		GroupKey* groupSelect = NULL;

//        for ( int nI=GroupKeys.count()-1 ; nI>=0 ; nI-- )
//		{
//            GroupKey* pGroup = GroupKeys.at(nI);
//            for ( int nG=pGroup->getCount()-1 ; nG>=0 ; nG-- )
//			{
//                CSoftkey* key = pGroup->getAt(nG);
//                rcKeyScrn = deviceToScreen( key->getPosition() );
//				rcKey = key->getPosition();
//                if ( rcKeyScrn.contains( evt->pos() ) )
//				{
//					groupSelect = pGroup;
//                    bOneClickMove = true;
//					break;
//				}
//			}
//			if ( bOneClickMove )
//				break;
//		}
//		if ( !bOneClickMove )
//		{
//            for ( int i=Keys.getSize()-1 ; i>=0 ; i-- )
//			{
//				CSoftkey* key = Keys[i];
//                rcKeyScrn = deviceToScreen( key->getPosition() );
//				rcKey = key->getPosition();
//                if ( rcKeyScrn.contains( evt->pos() ) )
//				{
//					keySelect = key;
//                    bOneClickMove = true;
//					break;
//				}
//			}
//		}

//		if ( bOneClickMove && !bIsCtrlDown )
//		{
//			if ( groupSelect )
//			{
//                m_SelectKeys.clear();

//                QRect rcMergedTrack;

//                for ( int nG=0 ; nG<groupSelect->getCount() ; nG++ )
//				{
//                    CSoftkey* key = groupSelect->getAt(nG);
//                    m_SelectKeys.push_back( key );
//					rcKey = key->getPosition();

//                    rcMergedTrack = rcMergedTrack.united( rcKey );
//				}

//                if ( !rcMergedTrack.isEmpty() )
//				{
//					m_rcDevTracker = rcMergedTrack;
////					m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
////					m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//				}
//				else
//				{
//                    QApplication::beep();
//                    m_rcDevTracker.setRect( 0,0,0,0 );
////                    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//				}
//                update();

//				goto goto_ReCheck;

//			}
//			if ( keySelect )
//			{
//                m_SelectKeys.clear();
//                m_SelectKeys.push_back( keySelect );
//				m_rcDevTracker = rcKey;
////				m_KeyTracker.m_rect = deviceToScreen(rcKey);
////				m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//                update();
//				goto goto_ReCheck;
//			}
//		}

//        if ( m_KeyTracker.TrackRubberBand( this, point, true ) )
//		{
//			if ( !bIsShiftDown )
//                m_SelectKeys.clear();

//            QRect rcMergedTrack;
//            QRect rcInter;

//            for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//			{
//                CSoftkey* key = m_SelectKeys.at(nI);
//				rcKey = key->getPosition();
//                rcMergedTrack = rcMergedTrack.united( rcKey );
//			}

//            QRect rcTrackerScrn;
//            //rcTrackerScrn = m_KeyTracker.m_rect;
//            rcTrackerScrn.normalized();
//            rcTrackerScrn.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

//			// group check
//            QVector<const GroupKey*> TempGroupArray;
//            for ( int nI=GroupKeys.count()-1 ; nI>=0 ; nI-- )
//			{
//                GroupKey* pGroup = GroupKeys.at(nI);
//                for ( int nG=pGroup->getCount()-1 ; nG>=0 ; nG-- )
//				{
//                    CSoftkey* key = pGroup->getAt(nG);
//                    rcKeyScrn = deviceToScreen( key->getPosition() );
//                    rcInter = rcKeyScrn;
//                    if ( rcInter.intersects( rcTrackerScrn ) )
//					{
//                        if ( !isContainGroup( TempGroupArray, key->getGroup() ) )
//						{
//                            TempGroupArray.push_back(key->getGroup());
//						}
//						break;
//					}
//				}
//			}

//            for ( int nI=0; nI<TempGroupArray.count() ; nI++ )
//			{
//                const GroupKey* pGroup = TempGroupArray.at(nI);
//				//for ( INT_PTR nG=pGroup->GetCount()-1 ; nG>=0 ; nG-- )
//                for ( int nG=0 ; nG<pGroup->getCount() ; nG++ )
//				{
//                    CSoftkey* key = pGroup->getAt(nG);
//					rcKey = key->getPosition();

//                    rcMergedTrack = rcMergedTrack.united( rcKey );
						
//                    if ( !isSelectKey(key) )
//                        m_SelectKeys.push_back( key );
//				}
//			}


//			//for ( int i=Keys.GetSize()-1 ; i>=0 ; i-- )
//            for ( int i=0 ; i<Keys.getSize() ; i++ )
//			{
//				CSoftkey* key = Keys[i];
//                rcKeyScrn = deviceToScreen(key->getPosition());
//				rcKey = key->getPosition();

//                rcInter = rcKeyScrn;
//                if ( rcInter.intersects( rcTrackerScrn ) )
//				{
//                    rcMergedTrack = rcMergedTrack.united( rcKey );
					
//                    if ( !isSelectKey(key) )
//                        m_SelectKeys.push_back( key );
//				}
//			}

//            if ( !rcMergedTrack.isEmpty() )
//			{
//				m_rcDevTracker = rcMergedTrack;
//                m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
//                m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//			}
//			else
//			{
//                QApplication::beep();
//                m_rcDevTracker.setRect( 0,0,0,0 );
////                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//			}
//            update();
//		}
//		else
//		{
//            bool bCheckKey = false;

//			if ( !bIsCtrlDown )
//                m_SelectKeys.clear();

//            QRect rcMergedTrack;
//            QRect rcInter;

//            for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
//			{
//                CSoftkey* key = m_SelectKeys.at(nI);
//				rcKey = key->getPosition();
//                rcMergedTrack = rcMergedTrack.united( rcKey );
//			}

//            for ( int i=0 ; i<Keys.getSize() ; i++ )
//			{
//				CSoftkey* key = Keys[i];
//                rcKeyScrn = deviceToScreen(key->getPosition());
//				rcKey = key->getPosition();

//                if ( rcKeyScrn.contains( evt->pos() ) )
//				{
//                    bCheckKey = true;
//                    rcMergedTrack = rcMergedTrack.united( rcKey );
//                    if ( !isSelectKey(key) )
//                        m_SelectKeys.push_back( key );
//					break;
//				}
//			}
//			if ( bCheckKey )
//			{
//                if ( !rcMergedTrack.isEmpty() )
//				{
//					m_rcDevTracker = rcMergedTrack;
//                    m_KeyTracker.m_rect = deviceToScreen(rcMergedTrack);
//                    m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//				}
//				else
//				{
////                    m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//                    m_rcDevTracker.setRect( 0,0,0,0 );
//				}
//			}
//			else
//			{
//                m_SelectKeys.clear();
//                QApplication::beep();
////                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//                m_rcDevTracker.setRect( 0,0,0,0 );
//			}
//            update();
//		}
//	}
//	else
//	{
//        QRect rcOldTracker( /*m_KeyTracker.m_rect*/ );
//        QRect rcOld;
//        rcOldTracker.normalized();
//        rcOldTracker.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

//        rcOld = m_rcDevTracker;//ScreenToDevice( rcOld, true );

//		if ( m_KeyTracker.Track( this, point ) )
//		{
//            QRect rcNewTracker;
//            QRect rcNew;
//            //rcNewTracker = m_KeyTracker.m_rect;
//            rcNewTracker.normalized();
//            rcNewTracker.adjust( TRACK_OFFSETXY, TRACK_OFFSETXY, -TRACK_OFFSETXY, -TRACK_OFFSETXY );

//            double dX = (double)rcOldTracker.width() / rcNewTracker.width();
//            double dY = (double)rcOldTracker.height() / rcNewTracker.height();
//			rcNew = m_rcDevTracker;
//            long lNewWidth = (long)(m_rcDevTracker.width() / dX + 0.5);
//            long lNewHeight = (long)(m_rcDevTracker.height() / dY + 0.5);
//            rcNew.setRight( rcNew.left() + lNewWidth );
//            rcNew.setBottom( rcNew.top() + lNewHeight );
//            if ( rcNewTracker.topLeft() != rcOldTracker.topLeft() )
//			{
//                QPoint ptOffset = rcNewTracker.topLeft() - rcOldTracker.topLeft();
//                ptOffset = screenToDevice( ptOffset, false );
//                rcNew.adjust( ptOffset.x(), ptOffset.y(), ptOffset.x(), ptOffset.y() );
//			}

//            recalcSelectionKeys( rcOld, rcNew );
//		}
//	}
//}

//void QKeyDesignWidget::onLButtonUp(QMouseEvent* evt)
//{
//	if ( (m_eScreenMode != ScreenModePreview) && m_bDownCloseButton )
//	{
//		if ( GetCapture() == this )
//		{
//            m_bDownCloseButton = false;
//            update( m_rcCloseButton );
//			ReleaseCapture();
//            if ( m_rcCloseButton.contains(point) )
//			{
//                close();
//			}
//			return;
//		}
//	}

//	if ( m_eScreenMode != ScreenModeKeyDesign )
//		return;

//    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

//    QRect rcKey;
//	for ( int i=0 ; i<Keys.GetSize() ; i++ )
//	{
//		CSoftkey* key = Keys[i];
//        rcKey = deviceToScreen(key->getPosition());

//        if ( rcKey.contains( evt->pos() ) )
//		{
//			m_rcDevTracker = key->getPosition();
//			m_KeyTracker.m_rect = rcKey;
//            m_KeyTracker.m_rect.adjust( -TRACK_OFFSETXY, -TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//            update();
//			break;
//		}
//	}
//}

//void QKeyDesignWidget::OnMButtonDown(UINT nFlags, CPoint point)
//{
//	if ( GetCapture() != this )
//	{
//		SetCapture();

//        m_bMoveScreen = true;

//		m_ptMouseLast = point;
//	}

//	CFullScrnWnd::OnMButtonDown(nFlags, point);
//}

//void QKeyDesignWidget::OnMButtonDblClk(UINT nFlags, CPoint point)
//{
//	UpdateKeys();

//	ResizeScreen();

//    update();

//	CFullScrnWnd::OnMButtonDblClk(nFlags, point);
//}

//void QKeyDesignWidget::OnMButtonUp(UINT nFlags, CPoint point)
//{
//	if ( GetCapture() == this )
//	{
//		ReleaseCapture();

//        m_bMoveScreen = false;
//        update();
//	}

//	CFullScrnWnd::OnMButtonUp(nFlags, point);
//}

//void QKeyDesignWidget::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if ( m_bMoveScreen )
//	{
//		CPoint ptScrnOffset;
//		ptScrnOffset = point - m_ptMouseLast;

//		m_rcScreen.OffsetRect( ptScrnOffset );

//		if ( m_SelectKeys.GetCount() > 0 )
//		{
//			CRect rcMergedTrack;
//            rcMergedTrack.setRect( 0,0,0,0 );

//			CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
//			CSoftkeyArray& Keys = pDlg->GetKeys();

//			for ( INT_PTR nI=0 ; nI<m_SelectKeys.GetCount() ; nI++ )
//			{
//				CSoftkey* key = m_SelectKeys.GetAt(nI);
//				CRect rcKey = key->getPosition();

//				rcMergedTrack.UnionRect( rcMergedTrack, rcKey );
//			}

//            if ( !rcMergedTrack.isEmpty() )
//			{
//				m_rcDevTracker = rcMergedTrack;
//				m_KeyTracker.m_rect = DeviceToScreen(rcMergedTrack);
//				m_KeyTracker.m_rect.InflateRect( TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY, TRACK_OFFSETXY );
//			}
//			else
//			{
//                m_rcDevTracker.setRect( 0,0,0,0 );
//                m_KeyTracker.m_rect.setRect( 0,0,0,0 );
//			}
//		}
//        update();
//	}
//	else
//	{
//		if ( nFlags & MK_LBUTTON )
//		{

//		}
//	}

//	m_ptMouseLast = point;

//	CFullScrnWnd::OnMouseMove(nFlags, point);
//}

//bool QKeyDesignWidget::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//	if ( pWnd == this && m_KeyTracker.SetCursor(this, nHitTest) )
//        return true;

//	SetCursor( AfxGetApp()->LoadStandardCursor(IDC_ARROW) );


//	return CFullScrnWnd::OnSetCursor(pWnd, nHitTest, message);
//}

//GroupStatus QKeyDesignWidget::CheckGroupStatus()
//{
//    bool bIsExistNoGroup = false;
//	CArray<const GroupKey*> TempGroupArray;
//	for ( INT_PTR nI=0 ; nI<m_SelectKeys.GetCount() ; nI++ )
//	{
//		CSoftkey* key = m_SelectKeys.GetAt(nI);
//		if ( key->getGroup() != NO_GROUP )
//		{
//			if ( !IsContainGroup( TempGroupArray, key->getGroup() ) )
//			{
//				TempGroupArray.Add(key->getGroup());
//			}
//		}
//		else
//		{
//            bIsExistNoGroup = true;
//		}
//	}

//	CString strCaption;
//	MENUITEMINFO	MenuItemInfo;
//	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
//	MenuItemInfo.fMask = MIIM_STATE | MIIM_STRING;

//	switch ( TempGroupArray.GetCount() )
//	{
//	case 1:
//		if ( bIsExistNoGroup )
//		{
//			return EnableGroup;
//		}
//		else
//		{
//			return EnableUngroup;
//		}
//		break;
//	case 0:
//		if ( bIsExistNoGroup )
//		{
//			return EnableGroup;
//		}
//	default:
//		return DisableGroup;
//	}
//}

//void QKeyDesignWidget::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
//{
//    if ( m_KeyTracker.m_rect.isEmpty() )
//		return;

//	CPoint pt = point;
//	ScreenToClient( &pt );
//	if ( !m_KeyTracker.m_rect.PtInRect(pt) )
//	{
//		return;
//	}

//	CMenu menu;
//	menu.LoadMenu( IDR_TRACKER_POPUP );

//	CMenu* pMenu = menu.GetSubMenu( 0 );

//	CString strCaption;
//	MENUITEMINFO	MenuItemInfo;
//	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
//	MenuItemInfo.fMask = MIIM_STATE | MIIM_STRING;

//	switch ( CheckGroupStatus() )
//	{
//	case EnableGroup:
//		// Group
//		strCaption = _T("Group");
//        m_bMenuGroup = true;
//		MenuItemInfo.fState = MFS_ENABLED;
//		break;
//	case EnableUngroup:
//			// Ungroup
//			strCaption = _T("Ungroup");
//            m_bMenuGroup = false;
//			MenuItemInfo.fState = MFS_ENABLED;
//		break;
//	case DisableGroup:
//		// Disable
//		strCaption = _T("Group");
//        m_bMenuGroup = true;
//		MenuItemInfo.fState = MFS_GRAYED;
//		break;
//	}

//	MenuItemInfo.wID = ID_TRACKER_GROUP;
//	MenuItemInfo.cch = strCaption.GetLength();
//	MenuItemInfo.dwTypeData = (LPTSTR)(LPCTSTR)strCaption;
//	pMenu->SetMenuItemInfo(ID_TRACKER_GROUP, &MenuItemInfo);



//	pMenu->TrackPopupMenu( TPM_LEFTALIGN|TPM_TOPALIGN, point.x, point.y, this );
//}

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

//void QKeyDesignWidget::OnTrackerGroupUngroup()
//{
//	if ( m_SelectKeys.GetCount() <= 0 )
//		return;

//	if ( m_bMenuGroup )
//	{
//		GroupSelectKeys();
//	}
//	else
//	{
//        UngroupSelectKeys( true );
//	}
//}

//void QKeyDesignWidget::OnTrackerRemove()
//{
//	RemoveSelectKeys();
//}

//void QKeyDesignWidget::SetBlinkKey( GroupKey* group, int nCalPos, bool bSet )
//{
//    if ( m_bBlink == true )
//	{
//		CancelBlink();
//	}

//	m_pBlinkGroup = group;
//    m_bBlink = true;
//	m_nCalPos = nCalPos;
//	m_bBlinkKeySet = bSet;

//	SetTimer( ID_TIMER_BLINK, 200, NULL );
//}

//void QKeyDesignWidget::SetBlinkKey( CSoftkey* key, int nCalPos, bool bSet )
//{
//    if ( m_bBlink == true )
//	{
//		CancelBlink();
//	}

//	m_pBlinkKey = key;
//    m_bBlink = true;
//	m_nCalPos = nCalPos;
//	m_bBlinkKeySet = bSet;

//	SetTimer( ID_TIMER_BLINK, 500, NULL );

//	//if ( bSet )
//	{
//        update( m_rcTouchCount );
//	}
//}

//void QKeyDesignWidget::SetOnOff( int nSKNum, bool bIsOn )
//{
//	CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();

//	CSoftkeyArray& Keys = pDlg->GetKeys();

//	if ( Keys.GetSize() <= nSKNum || nSKNum < 0 )
//		return;

//	CSoftkey* key = Keys[nSKNum];

//	m_pFocusKey = key;
//	m_bFocusKeyOn = bIsOn;

//	InvalidateKey( key );
//}

//void QKeyDesignWidget::CancelBlink()
//{
//	KillTimer( ID_TIMER_BLINK );

//    m_bBlink = false;
//    m_bBlinkOnOff = false;
//	m_pBlinkGroup = NULL;
//	m_pBlinkKey = NULL;

//    update();
//}

//void QKeyDesignWidget::OnTimer(UINT nIDEvent)
//{
//	if ( nIDEvent == ID_TIMER_BLINK )
//	{
//		m_bBlinkOnOff = !m_bBlinkOnOff;
//		if ( m_pBlinkGroup )
//		{
//			CRect rcMerged;
//            rcMerged.setRect( 0,0,0,0 );
	
//			for ( INT_PTR nI=0 ; nI<m_pBlinkGroup->GetCount() ; nI++ )
//			{
//				CSoftkey* key = m_pBlinkGroup->GetAt(nI);
//				CRect rcKey = DeviceToScreen( key->getPosition() );
//				rcMerged.UnionRect( rcMerged, rcKey );
//			}
//			rcMerged.InflateRect( 10, 10, 10, 10 );
//            update( rcMerged );
//		}
//		if ( m_pBlinkKey )
//		{
//			CRect rcKey = DeviceToScreen( m_pBlinkKey->getPosition() );
//			rcKey.InflateRect( 10, 10, 10, 10 );
//            update( rcKey );
//		}
//	}

//	CFullScrnWnd::OnTimer(nIDEvent);
//}

void QKeyDesignWidget::onBtnClose()
{
    close();
}

void QKeyDesignWidget::onSelectionChanged()
{
    QRectF rcSelction;

    //QGraphicsItemGroup* pGroup = m_GraphicScens.createItemGroup( m_GraphicScens.selectedItems() );

    foreach( QGraphicsItem* pItem, m_GraphicScens.selectedItems() )
    {
        rcSelction = rcSelction.united(  pItem->sceneBoundingRect() );
    }

    if( rcSelction.isNull() || rcSelction.isEmpty() )
    {
        m_pSelectionItem->onSelected( false );
        m_pSelectionItem->setVisible( false );

        emit SelectedKeys( false, 0 );
    }
    else
    {
        rcSelction.adjust( -5, -5, 5, 5 );

        if( !m_pSelectionItem )
        {
            m_pSelectionItem = new QResizingGraphicsItem( rcSelction.width(), rcSelction.height() );
            m_pSelectionItem->setPos( rcSelction.x(), rcSelction.y() );

            m_GraphicScens.addItem( m_pSelectionItem );

            m_pSelectionItem->setZValue( m_GraphicScens.items().size()-1 );

            connect( m_pSelectionItem, &QResizingGraphicsItem::UpdateSize, this, &QKeyDesignWidget::onRecalcSelectionKeys );
        }
        else
        {
            m_pSelectionItem->setVisible( true );
            m_pSelectionItem->onUpdateSize( rcSelction.width(), rcSelction.height() );
            m_pSelectionItem->setPos( rcSelction.x(), rcSelction.y() );

            m_pSelectionItem->setZValue( m_GraphicScens.items().size()-1 );
        }

        m_pSelectionItem->onSelected( true );

        emit SelectedKeys( false, m_GraphicScens.selectedItems().count() );
    }
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

    //updateKeys();
    selectKey( nIndex );

    int nFntHeight = 14;
    if ( Keys.getSize() > 0 )
    {
        CSoftkey* key = Keys[0];
        QRect rcKey = deviceToScreen( key->getPosition() );

        int nKH = ( (rcKey.height() < rcKey.width()) ? rcKey.height() : rcKey.width() ) * 3 / 7;
        nFntHeight = nKH;

        if ( nFntHeight > 15 ) nFntHeight = 15;
    }

    QRect rc = deviceToScreen( rcKey );
    QGraphicsKeyItem* pItem = new QGraphicsKeyItem( this, Keys[nIndex]->getShow(), rc.width(), rc.height(), nFntHeight );
    pItem->setZValue( 1 );
    pItem->setID( nIndex );
    if( parent() == NULL )
        pItem->setEnableMoving( true );

    connect( this, &QKeyDesignWidget::UpdateEnable, pItem, &QGraphicsKeyItem::onUpdateEnable, Qt::QueuedConnection );
    //connect( this, &QKeyDesignWidget::UpdateResize, pItem, &QGraphicsKeyItem::onUpdaterResize, Qt::QueuedConnection );
    connect( this, &QKeyDesignWidget::InvertDrawing, pItem, &QGraphicsKeyItem::onInvertDrawing, Qt::QueuedConnection );

    m_GraphicScens.addItem( pItem );

    pItem->setPos( rc.left(), rc.top() );

    m_GraphicScens.clearSelection();
    pItem->setSelected( true );

    m_pGIBtnClose->setZValue( m_GraphicScens.items().size()-1 );

    return nIndex;
}

void QKeyDesignWidget::onRemoveSelectedKeys()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

    pushHistory();

    onUngroupSelectedKeys( false );

    CSoftkey* key;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        key = m_SelectKeys.at(nI);
        Keys.remove( key, &Logics );
    }

    foreach( QGraphicsItem* item, m_GraphicScens.selectedItems() )
        m_GraphicScens.removeItem( item );

    if( Keys.getSize() < SOFT_KEY_MAX )
        emit keyStateCount( true );
}

void QKeyDesignWidget::onScreenSize(ScreenSize eScrnSize)
{
    m_eScrnSize = eScrnSize;

    m_SelectKeys.clear();

    resizeScreen();

    updateKeys();
}

void QKeyDesignWidget::onGroupSelectedKeys()
{
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    if ( GroupKeys.count() >= MAX_GROUP )
        return;

    pushHistory();

    onUngroupSelectedKeys( false );

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

void QKeyDesignWidget::onUngroupSelectedKeys(bool bPushHistory)
{
    QVector<const GroupKey*> TempGroupArray;
    for ( int nI=0 ; nI<m_SelectKeys.count() ; nI++ )
    {
        CSoftkey* key = m_SelectKeys.at(nI);
        if ( key->getGroup() != NO_GROUP )
        {
            if ( !isContainGroup( TempGroupArray, key->getGroup() ) )
            {
                TempGroupArray.push_back( key->getGroup() );
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

void QKeyDesignWidget::onInvalidateKey(QGraphicsKeyItem *key)
{
    key->update();
}

void QKeyDesignWidget::onRecalcSelectionKeys( QRectF rcOld, QRectF rcNew )
{
    if ( rcOld == rcNew )
        return;

    qDebug() << QString("old : %1,%2,%3,%4").arg(rcOld.x()).arg(rcOld.y()).arg(rcOld.width()).arg(rcOld.height());
    qDebug() << QString("new : %1,%2,%3,%4").arg(rcNew.x()).arg(rcNew.y()).arg(rcNew.width()).arg(rcNew.height());
    pushHistory();

    double dX = (double)rcOld.width() / rcNew.width();
    double dY = (double)rcOld.height() / rcNew.height();

    QRectF rcMerge;
    for ( int nI=0 ; nI<m_GraphicScens.selectedItems().size() ; nI++ )
    {
        QGraphicsKeyItem* pKey = (QGraphicsKeyItem*)m_GraphicScens.selectedItems().at(nI);

        QRectF rcKey = pKey->boundingRect();
        qreal dW = rcKey.width();
        qreal dH = rcKey.height();
        rcKey.setTopLeft( pKey->pos() );
        rcKey.setWidth( dW );
        rcKey.setHeight( dH );

        qDebug() << QString("b resize %1 : %2,%3,%4,%5").arg(pKey->getID()).arg(rcKey.x()).arg(rcKey.y()).arg(rcKey.width()).arg(rcKey.height());
        rcKey.setLeft(  (rcKey.left() - rcOld.left()) / dX + rcNew.left() + 0.5 );
        rcKey.setRight( (rcKey.right() - rcOld.left()) / dX + rcNew.left() + 0.5 );
        rcKey.setTop(   (rcKey.top() - rcOld.top()) / dY + rcNew.top() + 0.5 );
        rcKey.setBottom((rcKey.bottom() - rcOld.top()) / dY + rcNew.top() + 0.5 );

        rcMerge = rcMerge.united( rcKey );

        qDebug() << QString("resize %1 : %2,%3,%4,%5").arg(pKey->getID()).arg(rcKey.x()).arg(rcKey.y()).arg(rcKey.width()).arg(rcKey.height());
        qDebug() << QString("pos : %1,%2").arg(pKey->scenePos().x()).arg(pKey->scenePos().y());
        pKey->onUpdaterGeometry( rcKey.width(), rcKey.height() );

        pKey->setPos( rcKey.left(), rcKey.top() );
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
    if ( m_pSoftKeyDesignTool && m_pSoftKeyDesignTool->getUnit() == UnitMM )
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

    resizeScreen();
    //update();
}

void QKeyDesignWidget::onResetKeys()
{
    updateKeys();
}

void QKeyDesignWidget::onHideSelection(QPointF pos)
{
    m_pSelectionItem->setPos( m_pSelectionItem->pos()+pos );
}
