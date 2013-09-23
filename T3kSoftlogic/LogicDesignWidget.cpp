#include "LogicDesignWidget.h"

#include "QKeyMapStr.h"
#include "T3kCommonData.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QIcon>
#include <QDebug>

#define LOGIC_HEIGHT                4
#define LOGIC_SIZE                  15


QLogicDesignWidget::QLogicDesignWidget(QWidget *parent) :
    QWidget(parent)
{
	m_SelPart = epqNill;
    m_bShowNumber = false;
    m_pImageCanvas = NULL;
	m_pSoftlogicForRemove = NULL;
    m_eScreenMode = ScreenModePreview;

    if( parent == NULL )
    {
        setWindowFlags( Qt::FramelessWindowHint );
        setMouseTracking( true );
        m_eScreenMode = ScreenModeLogicDesign;
        setAttribute( Qt::WA_AcceptTouchEvents );

        connect( &m_wndLogicGallery, &QLogicGalleryWidget::addLogicFromGallery, this, &QLogicDesignWidget::onAddLogicFromGallery );
    }

    m_bDownCloseButton = false;

    m_pImageConsumerButton = NULL;

    m_ptSoftkeyPositionBackup.setX( -1.f );
    m_ptSoftkeyPositionBackup.setY( -1.f );

	m_pToBindKey = NULL;
	m_pCheckKey = NULL;

    m_bCpaute = false;

    m_penFocus.setStyle( Qt::DotLine );
    m_ftDrawText.setFamily( "Arial" );
    m_brshHatchGate.setStyle( Qt::BDiagPattern );
    m_brshHatchGate.setColor( qRgb(127,127,127) );

    setInvertDrawing( false );

    // crate
    m_pImageConsumerButton = new QPixmap( ":/T3kSoftlogicRes/resources/PNG_ICON_CONSUMER_BTN.png" );

    connect( &m_wndEditGate, &QEditGateWidget::updateLogic, this, &QLogicDesignWidget::onUpdate );
}

QLogicDesignWidget::~QLogicDesignWidget()
{
    if( m_pImageCanvas )
        delete m_pImageCanvas;
    m_pImageCanvas = NULL;

    if( m_pImageConsumerButton )
        delete m_pImageConsumerButton;
}

void QLogicDesignWidget::setInvertDrawing( bool bInvert )
{
	if ( bInvert )
	{
        m_clrBackground = qRgb(255, 255, 255);
        m_clrCloseBtnBg = qRgb(230, 230, 230);
        m_clrCloseBtnFg = qRgb(100, 100, 100);
        m_clrGrid       = qRgb(200, 200, 200);
        m_clrBindNormal = qRgb(149, 179, 215);
        m_clrBindSelect = qRgb(108, 150, 200);

        m_clrKeyFg              = qRgb(50, 50, 50);
        m_clrKeyBg              = qRgb(255, 255, 255);
        m_clrKeyBorderActive    = qRgb(0, 180, 0);
        m_clrKeyBorderNormal    = qRgb(56, 93, 138);
        m_clrKeyBorderBind      = qRgb(180, 0, 0);
        m_clrConnector          = qRgb(56, 93, 138);

        m_clrLogicBorderNormal  = qRgb(192, 80, 77);
        m_clrLogicBorderActive  = qRgb(0, 180, 0);
        m_clrLogicFg            = qRgb(50, 50, 50);
	}
	else
	{
        m_clrBackground = qRgb(0, 0, 0);
        m_clrCloseBtnBg = qRgb(100, 100, 100);
        m_clrCloseBtnFg = qRgb(0, 0, 0);
        m_clrGrid       = qRgb(50, 50, 50);
        m_clrBindNormal = qRgb(40, 40, 40);
        m_clrBindSelect = qRgb(70, 70, 70);

        m_clrKeyFg              = qRgb(200, 200, 200);
        m_clrKeyBg              = qRgb(0, 0, 0);
        m_clrKeyBorderActive    = qRgb(50, 200, 50);
        m_clrKeyBorderNormal    = qRgb(50, 50, 200);
        m_clrKeyBorderBind      = qRgb(200, 50, 50);
        m_clrConnector          = qRgb(200, 200, 200);

        m_clrLogicBorderNormal  = qRgb(200, 50, 50);
        m_clrLogicBorderActive  = qRgb(50, 200, 50);
        m_clrLogicFg            = qRgb(200, 200, 200);
	}

    if( isVisible() )
        update();
}

void QLogicDesignWidget::closeEvent(QCloseEvent *)
{
    if( m_wndEditGate.isVisible() )
        m_wndEditGate.close();

    emit closeWidget();
}

void QLogicDesignWidget::paintEvent(QPaintEvent *)
{
    QRect rcClient( 0, 0, width(), height() );

    QPainter painter;

    painter.begin( this );

    painter.save();

    painter.fillRect( rcClient, m_clrBackground );

    int nStep = rcClient.width() / 50;

    QPen penDots( Qt::CustomDashLine );
    penDots.setColor( m_clrGrid );
    QVector<qreal> vPattern;
    vPattern << 1 << nStep-1;
    penDots.setDashPattern( vPattern );
    painter.setPen( penDots );
    for ( int ny = rcClient.top() + nStep; ny < rcClient.bottom(); ny += nStep )
        painter.drawLine( 0, ny, rcClient.width(), ny );

    for ( int nx = rcClient.left() + nStep; nx < rcClient.right(); nx += nStep )
        painter.drawLine( nx, 0, nx, rcClient.height() );

    painter.restore();

    draw( &painter );

    if( m_eScreenMode == ScreenModePreview )
    {
        painter.setPen( Qt::black );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect( rcClient.adjusted(1,1,-1,-1) );
    }

    painter.end();
}

void QLogicDesignWidget::drawCloseButton( QPainter* painter, QRect rcClose )
{
	if( m_eScreenMode == ScreenModePreview )
		return;

    painter->save();

    QPen penClose( Qt::SolidLine );
    penClose.setWidth( 1 );
    penClose.setColor( qRgb(200, 200, 200) );
    QBrush brushClose( Qt::SolidPattern );
    brushClose.setColor( m_bDownCloseButton ? qRgb(237, 28, 36) : m_clrCloseBtnBg );
    QBrush brushX( m_clrCloseBtnFg );

    painter->setPen( penClose );
    painter->setBrush( brushClose );

    painter->drawRoundedRect( rcClose, 25, 25, Qt::RelativeSize );

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

    painter->restore();
}

void QLogicDesignWidget::draw( QPainter* painter )
{
    painter->save();

    QRect rcClient( 0, 0, width(), height() );
    int logic_size = rcClient.width() / LOGIC_SIZE;

    CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();

    m_bShowText = false;
    m_bShowText = (logic_size * 2 / 5) > 20 ? true : false;

	int ni;

    for ( ni = 0; ni < Keys.getSize(); ni++ )
	{
		if ( m_pCheckKey == Keys[ni] ) continue;
        drawKeyBind(painter, rcClient, Keys[ni], ni, logic_size);
	}
	if ( m_pCheckKey )
	{
        drawKeyBind(painter, rcClient, m_pCheckKey, ni, logic_size);
	}
	int nbid = -1;
    for ( ni = 0; ni < Keys.getSize(); ni++ )
	{
		if ( Keys[ni]->getBind() )
            nbid = Keys.indexFromSoftkey( Keys[ni]->getBind() );
		else
			nbid = -1;
        drawKey(painter, rcClient, Keys[ni], ni, nbid, logic_size,
            (ni == m_port_hit - MM_SOFTLOGIC_PORT_SOFTKEY_START));
	}

    for ( ni = 0; ni < Logics.getSize(); ni++ )
    {
        drawLogic(painter, rcClient, Logics[ni], ni < Logics.getShowSize(), logic_size,
            (ni == m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START),
            (m_bShowNumber ? ni + 1 : -1));
    }

    int nCloseWH = rcClient.width() / 36;
    nCloseWH -= nCloseWH%10;

    QRect rcClose( rcClient.right() - nCloseWH, rcClient.top(), nCloseWH, nCloseWH );

    rcClose.adjust( -20, 20, -20, 20 );

	m_rcCloseButton = rcClose;
    drawCloseButton( painter, rcClose );

    painter->restore();
}

void QLogicDesignWidget::drawKeyBind( QPainter * painter, QRect rcClient, CSoftkey * key, int /*id*/, int size )
{
    Q_ASSERT( painter );
	if ( key == NULL )
		return;
	CSoftkey * keyTo = key->getBind();
	if ( keyTo == NULL )
		return;

    QPoint ptPos((int)(rcClient.width() * key->m_ptPosition.x()) - size / 2, (int)(rcClient.height() * key->m_ptPosition.y()));
    QPoint ptPosTo((int)(rcClient.width() * keyTo->m_ptPosition.x()) - size / 2, (int)(rcClient.height() * keyTo->m_ptPosition.y()));

    painter->save();

    painter->setRenderHint( QPainter::HighQualityAntialiasing );
    QPen LinePen( m_pCheckKey == key ? m_clrBindSelect : m_clrBindNormal );
    LinePen.setWidthF( size/8.0f );
    LinePen.setCapStyle( Qt::RoundCap );
    //LinePen.SetEndCap( LineCapArrowAnchor );
    LinePen.setJoinStyle( Qt::RoundJoin );
    _drawLine2( painter, LinePen, ptPos, ptPosTo, 0.5f/*, size/2 - size/8*/ );

    painter->restore();
}

void QLogicDesignWidget::drawKey( QPainter * painter, QRect rcClient, CSoftkey * key, int id, int nbid, int size, bool bBound )
{
    Q_ASSERT( painter );
	if ( key == NULL )
		return;

    painter->save();

    QPoint ptPos((int)(rcClient.width() * key->m_ptPosition.x()), (int)(rcClient.height() * key->m_ptPosition.y()));
    QPen penKey( Qt::SolidLine );
    QBrush brushKey( m_clrKeyBg );

	if ( m_pToBindKey == key || m_pCheckKey == key )
	{
		if ( m_pToBindKey )
        {
            penKey.setWidth( 3 );
            penKey.setColor( m_clrKeyBorderBind );
        }
		else
		{
			if ( key->getBind() )
			{
                penKey.setWidth( 2 );
                penKey.setColor( m_clrKeyBorderNormal );
			}
			else
			{
                penKey.setWidth( 1 );
                penKey.setColor( m_clrKeyBorderNormal );
			}
		}
        painter->setBrush( Qt::NoBrush );
	}
	else
	{
		if ( key->getBind() )
		{
            penKey.setWidth( 2 );
            penKey.setColor( m_clrKeyBorderNormal );
            painter->setBrush( brushKey );
		}
		else
		{
            penKey.setWidth( 1 );
			if ( m_pCheckKey && (m_pCheckKey->getBind() == key) )
			{
                penKey.setColor( m_clrKeyBorderActive );
                painter->setBrush( Qt::NoBrush );
			}
			else
			{
                penKey.setColor( m_clrKeyBorderNormal );
                painter->setBrush( brushKey );
			}
		}
	}

    painter->setPen( penKey );
    QRect rcKey(ptPos.x() - size, ptPos.y() - size / 5, size, size / 5 * 2);
    painter->drawRect( rcKey );
	if ( !key->getShow() )
	{
        painter->save();

        QBrush brsh( Qt::BDiagPattern );
        brsh.setColor( qRgb(127,127,127) );
        painter->setBrush( brsh );
        painter->drawRect( rcKey );

        painter->restore();
	}

	if( m_bShowText )
	{
        painter->save();

        int nHeightLine = rcKey.height() / 3;
        rcKey.setBottom( rcKey.bottom() - nHeightLine );

        m_ftDrawText.setPixelSize( size/6 - 2 );
        painter->setFont( m_ftDrawText );

        painter->setPen( m_clrKeyFg );

        painter->drawText( rcKey, QString::number(id+1), Qt::AlignVCenter|Qt::AlignHCenter );

        rcKey.adjust( 0, nHeightLine, 0, nHeightLine );

        painter->drawText( rcKey, key->getName(), Qt::AlignVCenter|Qt::AlignHCenter );

		if ( nbid >= 0 )
		{
            m_ftDrawText.setPixelSize( size/8 );
            painter->setFont( m_ftDrawText );

            rcKey.adjust( rcKey.width(), nHeightLine, rcKey.width(), nHeightLine );
            painter->drawText( rcKey, QString::number( nbid+1 ), Qt::AlignLeft | Qt::AlignVCenter );
		}

        painter->restore();
	}

	if ( bBound )
	{
        painter->save();

        painter->setPen( m_penFocus );
        painter->setBrush( Qt::NoBrush );
        painter->setCompositionMode( QPainter::RasterOp_NotDestination );

        painter->drawRect( QRect(ptPos.x() - size, ptPos.y() - size / LOGIC_HEIGHT,
                                 size, (size / LOGIC_HEIGHT)*2) );

        painter->restore();
	}

    painter->restore();
}

bool QLogicDesignWidget::isAssociate( CSoftlogic* pLogicCheck, CSoftlogic* pLogic )
{
	if ( !pLogicCheck || !pLogic )
        return false;

    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

	CSoftlogic::Port epIn1 = pLogicCheck->getIn1Port();
	if ( epIn1 != CSoftlogic::epNoGate )
	{
		if ( epIn1 >= CSoftlogic::epSoftLogic0 )
		{
			CSoftlogic* pAssociateLogic = Logics[epIn1 - CSoftlogic::epSoftLogic0];

			if ( pAssociateLogic == pLogic )
                return true;
		}
	}

	CSoftlogic::Port epIn2 = pLogicCheck->getIn2Port();
	if ( epIn2 != CSoftlogic::epNoGate )
	{
		if ( epIn2 >= CSoftlogic::epSoftLogic0 )
		{
			CSoftlogic* pAssociateLogic = Logics[epIn2 - CSoftlogic::epSoftLogic0];

			if ( pAssociateLogic == pLogic )
                return true;
		}
	}

    return false;
}

bool QLogicDesignWidget::isAssociate( CSoftkey* pSoftkey, CSoftlogic* pLogic )
{
	if ( !pSoftkey || !pLogic )
        return false;

	// Check Input port 1
	CSoftlogic::Port port1 = pLogic->getIn1Port();

	if ( port1 >= CSoftlogic::epSoftkey0 && port1 < CSoftlogic::epSoftLogic0 )
	{
		// is key
		int idx = port1 - CSoftlogic::epSoftkey0;
        CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
		CSoftkey* pInSoftkey = Keys[idx];
		if( pInSoftkey == pSoftkey )
            return true;
	}
	else if ( port1 >= CSoftlogic::epSoftLogic0 )
	{
		int idx = port1 - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pLogic == pSoftlogic )
				goto goto_CheckPort2;
			if ( pSoftlogic )
			{
                if ( isAssociate(pSoftkey, pSoftlogic) )
                    return true;
			}
		}
	}

goto_CheckPort2:

	// Check Input port 2
	CSoftlogic::Port port2 = pLogic->getIn2Port();

	if ( port2 >= CSoftlogic::epSoftkey0 && port2 < CSoftlogic::epSoftkeyAll )
	{
		// is key
		int idx = port2 - CSoftlogic::epSoftkey0;
        CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
		CSoftkey* pInSoftkey = Keys[idx];
		if( pInSoftkey == pSoftkey )
            return true;
	}
	else if ( port2 >= CSoftlogic::epSoftLogic0 )
	{
		int idx = port2 - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pSoftlogic )
			{
				if ( pLogic == pSoftlogic )
                    return false;
                if ( isAssociate(pSoftkey, pSoftlogic) )
                    return true;
			}
		}
	}

    return false;
}

void QLogicDesignWidget::drawLogic( QPainter * painter, QRect rcClient, CSoftlogic * logic, bool bShow, int size, bool bBound, int ID )
{
    Q_ASSERT( painter );
	if ( logic == NULL )
		return;

	if ( logic->getLogicType() == CSoftlogic::eltNILL )
		return;

    painter->save();

    QPoint ptPos((int)(rcClient.width() * logic->m_ptPosition.x()), (int)(rcClient.height() * logic->m_ptPosition.y()));

    painter->setBrush( Qt::NoBrush );
    QPen penLine( Qt::SolidLine );
    penLine.setWidth( 1 );
    QPen penLogic( penLine );

    penLine.setColor( m_clrConnector );

    if( (m_wndEditGate.isVisible() && (m_wndEditGate.getLogic() == logic))
        || (m_pSoftlogicForRemove == logic) ||
        isAssociate(m_pCheckKey, logic) )
    {
        penLogic.setColor( m_clrLogicBorderActive );
    }
    else
    {
        penLogic.setColor( m_clrLogicBorderNormal );
    }

    QPoint ptOutPort;

	switch ( logic->getLogicType() )
	{
	default:
	case CSoftlogic::eltNILL:
		return;

	case CSoftlogic::eltAND:
        painter->setPen( penLogic );
        _drawAndGate(painter, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
        default:
        case CSoftlogic::eptKey:
            _drawOutKey(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
            break;
        case CSoftlogic::eptMouse:
            _drawOutMouse(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutMouse());
            break;
        case CSoftlogic::eptState:
            _drawOutState(painter, ptPos, size, logic->getOutPort());
            break;
        case CSoftlogic::eptGPIO:
            _drawOutGPIO(painter, ptPos, size, logic->getOutPort());
            break;
        case CSoftlogic::eptReport:
            _drawOutReport(painter, ptPos, size, logic->getReportId());
            break;
		}

        painter->setPen( penLine );
        _drawTrigger(painter, ptPos, size, logic->getLogicTrigger());

        if ( getOutPortPosI(logic->getIn1Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn1Port(), QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), size, true, true, true);
            painter->setPen( penLine );
		}
        if ( getOutPortPosI(logic->getIn2Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn2Port(), QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), size, false, true, true);
		}
		break;

	case CSoftlogic::eltOR:
        painter->setPen( penLogic );
//		painter->SetTextColor(m_clrLogicFg);
        _drawOrGate(painter, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
            _drawOutKey(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			break;
		case CSoftlogic::eptMouse:
			break;
		case CSoftlogic::eptState:
            _drawOutState(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptGPIO:
            _drawOutGPIO(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptReport:
            _drawOutReport(painter, ptPos, size, logic->getReportId());
			break;
		}
        painter->setPen( penLine );
        _drawTrigger(painter, ptPos, size, logic->getLogicTrigger());

        if ( getOutPortPosI(logic->getIn1Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn1Port(), QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), size, true, false);
            painter->setPen( penLine );
		}
        if ( getOutPortPosI(logic->getIn2Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn2Port(), QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), size, false, false);
		}
		break;

	case CSoftlogic::eltXOR:
        painter->setPen( penLogic );
//        painter->SetTextColor(m_clrLogicFg);
        _drawXorGate(painter, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
            _drawOutKey(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			break;
		case CSoftlogic::eptMouse:
			break;
		case CSoftlogic::eptState:
            _drawOutState(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptGPIO:
            _drawOutGPIO(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptReport:
            _drawOutReport(painter, ptPos, size, logic->getReportId());
			break;
		}
        painter->setPen( penLine );
        _drawTrigger(painter, ptPos, size, logic->getLogicTrigger());

        if ( getOutPortPosI(logic->getIn1Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn1Port(), QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), size, true, false);
            painter->setPen( penLine );
		}
        if ( getOutPortPosI(logic->getIn2Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn2Port(), QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), size, false, false, true);
		}
		break;

	case CSoftlogic::eltAND_RISINGEDGE:
        painter->setPen( penLogic );
//		painter->SetTextColor(m_clrLogicFg);
        _drawAndRisingedgeGate(painter, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
            _drawOutKey(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			break;
		case CSoftlogic::eptMouse:
            _drawOutMouse(painter, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutMouse());
			break;
		case CSoftlogic::eptState:
            _drawOutState(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptGPIO:
            _drawOutGPIO(painter, ptPos, size, logic->getOutPort());
			break;
		case CSoftlogic::eptReport:
            _drawOutReport(painter, ptPos, size, logic->getReportId());
			break;
		}
        painter->setPen( penLine );
        _drawTrigger(painter, ptPos, size, logic->getLogicTrigger());

        if ( getOutPortPosI(logic->getIn1Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn1Port(), QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), size, true, true, true);
            painter->setPen( penLine );
		}
        if ( getOutPortPosI(logic->getIn2Port(), &ptOutPort, rcClient) )
            _drawLine(painter, ptOutPort, QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
            painter->setPen( penLogic );
            _drawInPort(painter, logic->getIn2Port(), QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), size, false, true, true);
		}
		break;
	}

	if ( bBound )
	{
		if ( m_SelPart == epgIn1 )
		{
            if ( m_pt_dst.x() >= 0 )
			{
                painter->setPen( penLine );
                _drawLine(painter, m_pt_dst, QPoint(ptPos.x() - size, (ptPos.y() - size / 4) + size / 8), 0.5f);
			}
		}
		else if ( m_SelPart == epgIn2 )
		{
            if ( m_pt_dst.x() >= 0 )
			{
                painter->setPen( penLine );
                _drawLine(painter, m_pt_dst, QPoint(ptPos.x() - size, (ptPos.y() + size / 4) - size / 8), 0.5f);
			}
		}

		const int xsize = size / 20;
		const int ysize = size / 20;

        painter->save();

        painter->setPen( m_penFocus );
        painter->setBrush( Qt::NoBrush );
        painter->setCompositionMode( QPainter::RasterOp_NotDestination );

        switch ( m_SelPart )
        {
        default:
            break;
        case egpBody:
            painter->drawRect( QRect(ptPos.x() - size, ptPos.y() - size / LOGIC_HEIGHT,
                                     size - 1, size / LOGIC_HEIGHT * 2 - 1) );
            break;
        case epgIn1:
            painter->drawRect( QRect(ptPos.x() - size - xsize * 2, ptPos.y() - size / LOGIC_HEIGHT + ysize,
                                     xsize * 6, size / LOGIC_HEIGHT - ysize * 2) );
            break;
        case epgIn2:
            painter->drawRect( QRect(ptPos.x() - size - xsize * 2, ptPos.y() + ysize,
                                     xsize * 6, size / LOGIC_HEIGHT - ysize * 2) );
            break;
        }

        painter->restore();
	}

	if ( ID >= 0 )
	{
		if( m_bShowText )
		{
            painter->save();

            m_ftDrawText.setPixelSize( size / LOGIC_HEIGHT * 2 );
            painter->setFont( m_ftDrawText );
            painter->setPen( m_clrLogicFg );

            QString str( QString::number(ID) );
            painter->drawText( QRect(ptPos.x() - size, ptPos.y() - size / LOGIC_HEIGHT, size, (size / LOGIC_HEIGHT)*2),
                               str, Qt::AlignVCenter|Qt::AlignHCenter );

            painter->restore();
		}
	}

    painter->restore();
}

bool QLogicDesignWidget::getOutPortPosI( CSoftlogic::Port port, QPoint * ptPos, QRect rcClient )
{
	if ( port == CSoftlogic::epNoGate )
        return false;

    Q_ASSERT( ptPos );

	int idx;

	if ( port < CSoftlogic::epGpio0 )
	{	// state
        return false;
	}
	else if ( port < CSoftlogic::epSoftkey0 )
	{	// GPIO
		//ASSERT(0); //!!
        return false;
	}
	else if ( port < CSoftlogic::epSoftkeyAll )
	{	// softkey
		idx = port - MM_SOFTLOGIC_PORT_SOFTKEY_START;
		if ( idx < SOFT_KEY_MAX )
		{
            CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
			CSoftkey * pSoftkey = Keys[idx];
			if ( pSoftkey )
			{
                ptPos->setX( (int)(pSoftkey->m_ptPosition.x() * rcClient.width()) + rcClient.left() );
                ptPos->setY( (int)(pSoftkey->m_ptPosition.y() * rcClient.height()) + rcClient.top() );
                return true;
			}
		}
	}
	else
	{ // softlogic
		idx = port - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pSoftlogic )
			{
                ptPos->setX( (int)(pSoftlogic->m_ptPosition.x() * rcClient.width()) + rcClient.left() );
                ptPos->setY( (int)(pSoftlogic->m_ptPosition.y() * rcClient.height()) + rcClient.top() );
                return true;
			}
		}
	}

    return false;
}

void QLogicDesignWidget::_drawEnable( QPainter * painter, CSoftlogic::Port enable_port, bool enable_not,
								 int xe, int ye, int xsize, int ysize )
{
	if ( enable_port == CSoftlogic::epNoGate )
		return;

    painter->save();

	if ( enable_not )
	{
        painter->drawEllipse( xe - xsize, ye - xsize * 2, xsize * 2, xsize * 2);
		ye -= xsize * 2;
	}

    QRect rc(xe - xsize * 3, ye - ysize * 3 - ysize, xsize * 6, ysize * 3);

    painter->drawLine( xe, ye, xe, ye - ysize );
    painter->drawEllipse( rc );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        QString strE;
		switch ( enable_port )
		{
		default:
			break;
		case CSoftlogic::epStateWorking:
            strE = "W"; break;
		case CSoftlogic::epStateTouch:
            strE = "Tch"; break;
		case CSoftlogic::epStateCalibration:
            strE = "Cal"; break;
		case CSoftlogic::epStateTouchScreen:
            strE = "TS"; break;
		case CSoftlogic::epStateMultiTouchDevice:
            strE = "MTD"; break;
		case CSoftlogic::epStateInvertDetection:
            strE = "IVT"; break;
		}

        painter->setPen( m_clrLogicFg );
        rc.adjust( -2,-2,2,2 );
        painter->drawText( rc, strE, Qt::AlignVCenter|Qt::AlignHCenter );
	}

    painter->restore();
}

void QLogicDesignWidget::_drawBufferGate( QPainter * painter, QPoint pt, int size,
                                     CSoftlogic::Port enable_port, bool enable_not, bool in_not, bool bShow )
{
    QRect rcLogic( pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                   size * 3 / 5, (size / LOGIC_HEIGHT * 2) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    QPainterPath path;
    path.moveTo( rcLogic.left(), rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.bottom() );
    path.lineTo( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2 );
    path.lineTo( rcLogic.left(), rcLogic.top() );
    path.closeSubpath();

    painter->save();
    if ( !bShow )
        painter->setBrush( m_brshHatchGate );
    else
        painter->setBrush( Qt::NoBrush );
    painter->drawPath( path );
    painter->restore();

    painter->drawLine( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2,
                       pt.x(), rcLogic.top() + rcLogic.height() / 2 );

    int inx = rcLogic.left();
	if ( in_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() / 2 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.top() + rcLogic.height() / 2,
                       pt.x() - size, rcLogic.top() + rcLogic.height() / 2);

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top() + ysize * 2;
    _drawEnable(painter, enable_port, enable_not, xe, ye, xsize, ysize);

    painter->restore();
}

void QLogicDesignWidget::_drawAndGate( QPainter * painter, QPoint pt, int size,
                                  CSoftlogic::Port enable_port, bool enable_not,
                                  bool in1_not, bool in2_not, bool bShow )
{
    QRect rcLogic(pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                  size * 3 / 5, (size / LOGIC_HEIGHT * 2) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    QPainterPath path;
    path.moveTo( rcLogic.left() + rcLogic.width() / 2, rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.bottom() );
    path.lineTo( rcLogic.left() + rcLogic.width() / 2, rcLogic.bottom() );
    path.arcTo( rcLogic.left(), rcLogic.top(), rcLogic.width(), rcLogic.height(),
                -90, 180 );
//		rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom, rcLogic.left + rcLogic.Width() / 2, rcLogic.top);
    path.closeSubpath();

    painter->save();
    if ( !bShow )
        painter->setBrush( m_brshHatchGate );
    else
        painter->setBrush( Qt::NoBrush );
    painter->drawPath( path );
    painter->restore();

    painter->drawLine( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2,
                       pt.x(), rcLogic.top() + rcLogic.height() / 2 );

    int inx = rcLogic.left();
	if ( in1_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.top() + rcLogic.height() / 4,
                       pt.x() - size, rcLogic.top() + rcLogic.height() / 4 );

    inx = rcLogic.left();
	if ( in2_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() * 3 / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.bottom() - rcLogic.height() / 4,
                       pt.x() - size, rcLogic.bottom() - rcLogic.height() / 4 );

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top();
    _drawEnable(painter, enable_port, enable_not, xe, ye, xsize, ysize);

    painter->restore();
}

void QLogicDesignWidget::_drawOrGate( QPainter * painter, QPoint pt, int size,
                                 CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow )
{
    QRect rcLogic(pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                  size * 3 / 5, (size / LOGIC_HEIGHT * 2) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    QPainterPath path;
    path.moveTo( rcLogic.left(), rcLogic.top() );
    path.cubicTo( rcLogic.left() + rcLogic.width() / 2,
                  rcLogic.top(), rcLogic.right() - rcLogic.width() / 5,
                  rcLogic.center().y() - rcLogic.height() / 4, rcLogic.right(), rcLogic.center().y() );
    path.cubicTo( rcLogic.right() - rcLogic.width() / 5, rcLogic.center().y() + rcLogic.height() / 4,
                  rcLogic.left() + rcLogic.width() / 2, rcLogic.bottom(),
                  rcLogic.left(), rcLogic.bottom() );
    path.cubicTo( rcLogic.left() + rcLogic.width() / 10, rcLogic.center().y(),
                  rcLogic.left(), rcLogic.top(),
                  rcLogic.left(), rcLogic.top() );
    path.closeSubpath();

    painter->save();
    if ( !bShow )
        painter->setBrush( m_brshHatchGate );
    else
        painter->setBrush( Qt::NoBrush );
    painter->drawPath( path );
    painter->restore();

    painter->drawLine( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2,
                       pt.x(), rcLogic.top() + rcLogic.height() / 2 );

    int inx = rcLogic.left();
	if ( in1_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.top() + rcLogic.height() / 4,
                       pt.x() - size, rcLogic.top() + rcLogic.height() / 4 );

    inx = rcLogic.left();
	if ( in2_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.bottom() - rcLogic.height() / 4 - ysize,
                             xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.bottom() - rcLogic.height() / 4,
                       pt.x() - size, rcLogic.bottom() - rcLogic.height() / 4);

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top() + (int)(ysize / 1.5f);
    _drawEnable(painter, enable_port, enable_not, xe, ye, xsize, ysize);

    painter->restore();
}

void QLogicDesignWidget::_drawXorGate( QPainter * painter, QPoint pt, int size,
                                  CSoftlogic::Port enable_port, bool enable_not, bool in1_not, bool in2_not, bool bShow )
{
    QRect rcLogic( pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                   size * 3 / 5, (size / LOGIC_HEIGHT * 2 ) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    QPainterPath path;
    path.moveTo( rcLogic.left() + xsize, rcLogic.top() );
    path.cubicTo( rcLogic.left() + xsize + rcLogic.width() / 2, rcLogic.top(),
                  rcLogic.right() - rcLogic.width() / 5, rcLogic.center().y() - rcLogic.height() / 4, rcLogic.right(), rcLogic.center().y() );
    path.cubicTo( rcLogic.right() - rcLogic.width() / 5, rcLogic.center().y() + rcLogic.height() / 4,
                  rcLogic.left() + xsize + rcLogic.width() / 2, rcLogic.bottom(), rcLogic.left() + xsize, rcLogic.bottom() );
    path.cubicTo( rcLogic.left() + xsize + rcLogic.width() / 10, rcLogic.center().y(),
                  rcLogic.left() + xsize, rcLogic.top(), rcLogic.left() + xsize, rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.top() );
    path.cubicTo( rcLogic.left() + rcLogic.width() / 10, rcLogic.center().y(), rcLogic.left(), rcLogic.bottom(), rcLogic.left(), rcLogic.bottom() );
    path.lineTo( rcLogic.left() + xsize, rcLogic.bottom() );
    path.cubicTo( rcLogic.left() + xsize + rcLogic.width() / 10, rcLogic.center().y(), rcLogic.left() + xsize, rcLogic.top(), rcLogic.left() + xsize, rcLogic.top() );
    path.closeSubpath();

    painter->save();
    if ( !bShow )
        painter->setBrush( m_brshHatchGate );
    else
        painter->setBrush( Qt::NoBrush );
    painter->drawPath( path );
    painter->restore();

    painter->drawLine( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2,
                       pt.x(), rcLogic.top() + rcLogic.height() / 2 );

    int inx = rcLogic.left();
	if ( in1_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.top() + rcLogic.height() / 4,
                       pt.x() - size, rcLogic.top() + rcLogic.height() / 4 );

    inx = rcLogic.left();
	if ( in2_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.bottom() - rcLogic.height() / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.bottom() - rcLogic.height() / 4,
                       pt.x() - size, rcLogic.bottom() - rcLogic.height() / 4 );

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top() + (int)(ysize / 1.5f);
    _drawEnable(painter, enable_port, enable_not, xe, ye, xsize, ysize);

    painter->restore();
}

void QLogicDesignWidget::_drawAndRisingedgeGate( QPainter * painter, QPoint pt, int size,
                                            CSoftlogic::Port enable_port, bool enable_not,
                                            bool in1_not, bool in2_not, bool bShow )
{
    QRect rcLogic( pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                  size * 3 / 5, (size / LOGIC_HEIGHT * 2) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    QPainterPath path;
    path.moveTo( rcLogic.left() + rcLogic.width() / 2, rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.top() );
    path.lineTo( rcLogic.left(), rcLogic.bottom() );
    path.lineTo( rcLogic.left() + rcLogic.width() / 2, rcLogic.bottom() );
    path.arcTo( rcLogic.left(), rcLogic.top(), rcLogic.width() - xsize , rcLogic.height(),
                -90, 180 );
    path.closeSubpath();

    painter->save();
	if ( !bShow )
        painter->setBrush( m_brshHatchGate );
	else
        painter->setBrush( Qt::NoBrush );
    painter->drawPath( path );
    painter->restore();

    painter->drawLine( rcLogic.right() - xsize, rcLogic.top(),
                       rcLogic.right() - xsize, rcLogic.bottom() );
    painter->drawLine( rcLogic.right(), rcLogic.top(),
                       rcLogic.right(), rcLogic.bottom() );

    painter->drawLine( rcLogic.right(), rcLogic.top() + rcLogic.height() / 2,
                       pt.x(), rcLogic.top() + rcLogic.height() / 2 );

    int inx = rcLogic.left();
	if ( in1_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.top() + rcLogic.height() / 4,
                       pt.x() - size, rcLogic.top() + rcLogic.height() / 4 );

    inx = rcLogic.left();
	if ( in2_not )
	{
        painter->drawEllipse( inx - xsize * 2, rcLogic.top() + rcLogic.height() * 3 / 4 - ysize,
                              xsize * 2, ysize * 2 );
		inx -= xsize * 2;
	}
    painter->drawLine( inx, rcLogic.bottom() - rcLogic.height() / 4,
                       pt.x() - size, rcLogic.bottom() - rcLogic.height() / 4 );

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top();
    _drawEnable(painter, enable_port, enable_not, xe, ye, xsize, ysize);

    painter->restore();
}

void QLogicDesignWidget::_drawTrigger( QPainter * painter, QPoint pt, int size, CSoftlogic::Trigger trigger )
{
	if ( trigger == CSoftlogic::etNoTrigger )
		return;

    QRect rcLogic( pt.x() - size * 4 / 5, pt.y() - size / LOGIC_HEIGHT,
                   size * 3 / 5, (size / LOGIC_HEIGHT * 2) );
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    painter->drawLine( rcLogic.left() + rcLogic.width() / 2 - xsize * 2, rcLogic.bottom() - ysize * 3,
                       rcLogic.left() + rcLogic.width() / 2 - xsize * 2, rcLogic.top() + ysize * 3 );
    painter->drawLine( rcLogic.left() + rcLogic.width() / 2 - xsize * 2, rcLogic.top() + ysize * 3,
                       rcLogic.left() + rcLogic.width() / 2 - xsize * 3, rcLogic.top() + ysize * 4 );

    painter->drawLine( rcLogic.left() + rcLogic.width() / 2 - xsize, rcLogic.top() + ysize * 3,
                       rcLogic.left() + rcLogic.width() / 2 - xsize, rcLogic.bottom() - ysize * 3 );
    painter->drawLine( rcLogic.left() + rcLogic.width() / 2 - xsize, rcLogic.bottom() - ysize * 3,
                       rcLogic.left() + rcLogic.width() / 2, rcLogic.bottom() - ysize * 4 );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QRect rcText1( rcLogic.left() - size, rcLogic.top(),
                      size + (rcLogic.width() / 2 - xsize * 5 / 2), rcLogic.height() );
        QRect rcText2( rcLogic.left() + rcLogic.width() / 2 - xsize / 2, rcLogic.top(),
                       rcLogic.width() / 2 - xsize / 2 + size, rcLogic.height() );
		switch ( trigger )
		{
		default:
		case CSoftlogic::etNoTrigger:
			break;

        case CSoftlogic::etTrigger_U128_D128:
            painter->drawText( rcText1, "128", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "128", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U256_D256:
            painter->drawText( rcText1, "256", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "256", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U512_D512:
            painter->drawText( rcText1, "512", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "512", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U1024_D1024:
            painter->drawText( rcText1, "1024", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "1024", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U2048_D2048:
            painter->drawText( rcText1, "2048", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "2048", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U256_D0:
            painter->drawText( rcText1, "256", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "0", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U512_D0:
            painter->drawText( rcText1, "512", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "0", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U1024_D0:
            painter->drawText( rcText1, "1024", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "0", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U2048_D0:
            painter->drawText( rcText1, "2048", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "0", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U3072_D0:
            painter->drawText( rcText1, "3072", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "0", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U192_D64:
            painter->drawText( rcText1, "192", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "64", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U384_D128:
            painter->drawText( rcText1, "384", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "128", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U768_D256:
            painter->drawText( rcText1, "768", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "256", Qt::AlignLeft | Qt::AlignVCenter );
		break;

        case CSoftlogic::etTrigger_U1536_D512:
            painter->drawText( rcText1, "1536", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "512", Qt::AlignLeft | Qt::AlignVCenter );
			break;

        case CSoftlogic::etTrigger_U2048_D1024:
            painter->drawText( rcText1, "2048", Qt::AlignRight | Qt::AlignVCenter );
            painter->drawText( rcText2, "1024", Qt::AlignLeft | Qt::AlignVCenter );
			break;
		}
	}

    painter->restore();
}

void QLogicDesignWidget::_drawLine2( QPainter* painter, QPen& LinePen, QPoint pt1, QPoint pt2, float fD )
{
    QPoint pts[4];

    if ( pt1.x() < pt2.x() )
	{
        int xD = pt1.x() + (int)((pt2.x() - pt1.x()) * fD + .5f);
        pts[0].setX( pt1.x() );	pts[0].setY( pt1.y() );
        pts[1].setX( xD );		pts[1].setY( pt1.y() );
        pts[2].setX( xD );		pts[2].setY( pt2.y() );
        pts[3].setX( pt2.x() );	pts[3].setY( pt2.y() );
	}
	else
	{
        int yD = pt1.y() + (int)((pt2.y() - pt1.y()) * fD + .5f);
        pts[0].setX( pt1.x() );	pts[0].setY( pt1.y() );
        pts[1].setX( pt1.x() );	pts[1].setY( yD );
        pts[2].setX( pt2.x() );	pts[2].setY( yD );
        pts[3].setX( pt2.x() );	pts[3].setY( pt2.y() );
	}

    painter->save();

    painter->setPen( LinePen );
    for( int i=0; i<3; i++ )
        painter->drawLine( pts[i], pts[i+1] );

    painter->restore();
}

void QLogicDesignWidget::_drawLine( QPainter * painter, QPoint pt1, QPoint pt2, float fD )
{
    if ( pt1.x() < pt2.x() )
	{
        int xD = pt1.x() + (int)((pt2.x() - pt1.x()) * fD);
        painter->drawLine( pt1.x(), pt1.y(), xD, pt1.y() );
        painter->drawLine( xD, pt1.y(), xD, pt2.y() );
        painter->drawLine( xD, pt2.y(), pt2.x(), pt2.y() );
	}
	else
	{
        int yD = pt1.y() + (int)((pt2.y() - pt1.y()) * fD);
        painter->drawLine( pt1.x(), pt1.y(), pt1.x(), yD );
        painter->drawLine( pt1.x(), yD, pt2.x(), yD );
        painter->drawLine( pt2.x(), yD, pt2.x(), pt2.y() );
	}
}

void QLogicDesignWidget::_drawInPort( QPainter * painter, CSoftlogic::Port port, QPoint pt, int size, bool bUp, bool bDefaultTrue, bool bNoGateOK )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    m_ftDrawText.setPixelSize( ysize * 3 );
    painter->setFont( m_ftDrawText );

    QRect rc;
	if ( port == CSoftlogic::epNoGate )
	{
        rc = QRect( pt.x() - xsize * 4, pt.y() - ysize * 2, xsize * 4, ysize * 4 );
        painter->drawEllipse( rc );
		if ( bNoGateOK )
		{
			if( m_bShowText )
            {
                painter->setPen( m_clrLogicFg );
                painter->drawText( rc, bDefaultTrue ? "1" : "0", Qt::AlignVCenter|Qt::AlignHCenter );
			}
		}
		else
		{
            painter->drawLine( pt.x() - xsize * 4, pt.y() - ysize * 2, pt.x(), pt.y() + ysize * 2 );
            painter->drawLine( pt.x(), pt.y() - ysize * 2, pt.x() - xsize * 4, pt.y() + ysize * 2 );
		}
	}
	else if ( port < CSoftlogic::epGpio0 )
	{	// state
        QString strE;
		switch ( port )
		{
		default:
			goto jmp_x;
		case CSoftlogic::epStateWorking:
            strE = "W"; break;
		case CSoftlogic::epStateTouch:
            strE = "Tch"; break;
		case CSoftlogic::epStateCalibration:
            strE = "Cal"; break;
		case CSoftlogic::epStateTouchScreen:
            strE = "TS"; break;
		case CSoftlogic::epStateMultiTouchDevice:
            strE = "MTD"; break;
		case CSoftlogic::epStateInvertDetection:
            strE = "IVT"; break;
		}

		if ( bUp )
		{
            painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() - ysize );
            rc = QRect( pt.x() - xsize * 3, pt.y() - ysize - ysize * 4, xsize * 6, ysize * 4 );
		}
		else
		{
            painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() + ysize );
            rc = QRect( pt.x() - xsize * 3, pt.y() + ysize, xsize * 6, ysize * 4 );
		}
        painter->drawEllipse( rc );
		if( m_bShowText )
        {
            painter->save();

            painter->setPen( m_clrLogicFg );
            rc.adjust( -2,-2,2,2 );
            painter->drawText( rc, strE, Qt::AlignVCenter|Qt::AlignHCenter );

            painter->restore();
        }
	}
	else if ( port < CSoftlogic::epSoftkey0 )
	{	// GPIO
		//ASSERT(0); //!!
        rc = QRect( pt.x() - xsize * 4, pt.y() - ysize * 2, xsize * 4, ysize * 4 );
        painter->setPen( m_clrKeyBorderNormal );
        painter->drawRect( rc );
		if( m_bShowText )
        {
            painter->save();

            painter->setPen( m_clrLogicFg );
            painter->drawText( rc, QString("IO%1").arg((port - CSoftlogic::epGpio0) + 1), Qt::AlignVCenter|Qt::AlignHCenter );

            painter->restore();
        }
	}
	else if ( port == CSoftlogic::epSoftkeyAll )
	{
        rc = QRect( pt.x() - xsize * 4, pt.y() - ysize * 2, xsize * 4, ysize * 4 );
        painter->setPen( m_clrKeyBorderNormal );
        painter->drawRect( rc );
		if( m_bShowText )
        {
            painter->save();

            painter->setPen( m_clrLogicFg );
            painter->drawText( rc, "All", Qt::AlignVCenter|Qt::AlignHCenter );

            painter->restore();
        }
	}
	else
	{	// softkey, softlogic
jmp_x:
        rc = QRect( pt.x() - xsize * 4, pt.y() - ysize * 2, xsize * 4, ysize * 4 );
        painter->drawEllipse( rc );
		if( m_bShowText )
        {
            painter->save();

            painter->setPen( m_clrLogicFg );
            painter->drawText( rc, "?", Qt::AlignVCenter|Qt::AlignHCenter );

            painter->restore();
        }
	}

    painter->restore();
}

void QLogicDesignWidget::_drawOutKey( QPainter * painter, QPoint pt, int size, uchar ckey, uchar key )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    painter->drawLine( pt.x(), pt.y() ,pt.x(), pt.y() - ysize );
	if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
        painter->drawRect( pt.x() - xsize * 4, pt.y() - ysize * 6 - ysize, xsize * 8, ysize * 6 );
	else
        painter->drawRect( pt.x() - xsize * 4, pt.y() - ysize * 4 - ysize, xsize * 8, ysize * 4 );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 - ysize / 4 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QString str;
		if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		{
            str = "[";
			if ( (ckey & MM_SOFTLOGIC_CKEY_CTRL) != 0x0 ) str += 'C';
			if ( (ckey & MM_SOFTLOGIC_CKEY_ALT) != 0x0 ) str += 'A';
			if ( (ckey & MM_SOFTLOGIC_CKEY_SHIFT) != 0x0 ) str += 'S';
			if ( (ckey & MM_SOFTLOGIC_CKEY_WINDOW) != 0x0 ) str += 'W';
            str += "]";
            painter->drawText( QRect(pt.x() - xsize * 5, pt.y() - ysize * 7 - ysize - ysize/4, xsize * 10, ysize * 5 ), str, Qt::AlignVCenter|Qt::AlignHCenter );
		}
		int vkey = 0;
#define ICON_WIDTH (16)
#define ICON_HEIGHT (16)
        int nIX = pt.x() - ICON_WIDTH / 2;
        int nIY = pt.y() - ysize - ysize * 2 - ICON_HEIGHT / 2;
        if( key-V_KEY_VOLUME_INC_VALUE < 0 || key-V_KEY_AL_WEB_BROWSER > 0 )
        {
            findScanCode(key, vkey);
            vkey = scanCodeToVirtualKey( vkey );
            str = virtualKeyToString(vkey);
            QRect rc( pt.x() - xsize * 4, pt.y() - ysize * 4 - ysize, xsize * 8, ysize * 4 );
            rc.adjust( -4,-4,4,4 );
            painter->drawText( rc, str, Qt::AlignVCenter|Qt::AlignHCenter );
        }
        else
        {
            if( key > V_KEY_TRANSPORT_PLAYPAUSE )
            {
                painter->fillRect( QRect(pt.x() - xsize * 4+1, pt.y() - ysize * 4 - ysize+1,
                                         xsize * 8 - 2, ysize * 4 - 2), qRgb(180, 180, 220) );

                painter->drawPixmap( QRect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), *m_pImageConsumerButton,
                                    QRect((key-V_KEY_AL_EMAIL_VIEWER+12)*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT) );
            }
            else
            {
                painter->fillRect( QRect(pt.x() - xsize * 4+1, pt.y() - ysize * 4 - ysize+1,
                                         xsize * 8 - 2, ysize * 4 - 2), qRgb(220, 180, 180) );

                if( key == V_KEY_TRANSPORT_PLAYPAUSE )
                {
                    painter->drawPixmap( QRect(nIX-ICON_WIDTH/3, nIY, ICON_WIDTH, ICON_HEIGHT), *m_pImageConsumerButton,
                                         QRect(3*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT) );
                    painter->drawPixmap( QRect(nIX+ICON_WIDTH/3, nIY, ICON_WIDTH, ICON_HEIGHT), *m_pImageConsumerButton,
                                         QRect(4*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT) );
                }

                else
                {
                    painter->drawPixmap( QRect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), *m_pImageConsumerButton,
                                        QRect((key-V_KEY_VOLUME_INC_VALUE)*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT) );
                }
            }
        }
	}

    painter->restore();
}

void QLogicDesignWidget::_drawOutMouse( QPainter * painter, QPoint pt, int size, uchar ckey, uchar mkey )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() - ysize );
	if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
        painter->drawRect( pt.x() - xsize * 4, pt.y() - ysize * 6 - ysize, xsize * 8, ysize * 6 );
	else
        painter->drawRect( pt.x() - xsize * 4, pt.y() - ysize * 4 - ysize, xsize * 8, ysize * 4 );
    painter->drawLine( pt.x() - xsize * 4, pt.y() - (int)(ysize * 0.9f),
                       pt.x() + xsize * 4, pt.y() - (int)(ysize * 0.9f) );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QString str;
		if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		{
            str = "[";
			if ( (ckey & MM_SOFTLOGIC_CKEY_CTRL) != 0x0 ) str += 'C';
			if ( (ckey & MM_SOFTLOGIC_CKEY_ALT) != 0x0 ) str += 'A';
			if ( (ckey & MM_SOFTLOGIC_CKEY_SHIFT) != 0x0 ) str += 'S';
			if ( (ckey & MM_SOFTLOGIC_CKEY_WINDOW) != 0x0 ) str += 'W';
            str += "]";
            painter->drawText( QRect(pt.x() - xsize * 5, pt.y() - ysize * 7 - ysize, xsize * 10, ysize * 5),
                               str, Qt::AlignVCenter|Qt::AlignHCenter );
		}
        str = "(";
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN) != 0x0 ) str += 'L';
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN) != 0x0 ) str += 'R';
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN) != 0x0 ) str += 'M';
        str += ")";
        painter->drawText( QRect(pt.x() - xsize * 4, pt.y() - ysize * 4 - ysize, xsize * 8, ysize * 4),
                           str, Qt::AlignVCenter|Qt::AlignHCenter );
	}

    painter->restore();
}

void QLogicDesignWidget::_drawOutState( QPainter * painter, QPoint pt, int size, CSoftlogic::Port out_port )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	if ( out_port == CSoftlogic::epNoGate )
		return;

    painter->save();

    painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() - ysize );
    painter->drawEllipse( pt.x() - xsize * 4, pt.y() - ysize * 4 - ysize, xsize * 8, ysize * 4 );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QString strE;
		switch ( out_port )
		{
		default:
			break;
		case CSoftlogic::epStateWorking:
            strE = "Beep1"; break;
		case CSoftlogic::epStateTouch:
            strE = "Beep2"; break;
		case CSoftlogic::epStateCalibration:
            strE = "Cal"; break;
		case CSoftlogic::epStateTouchScreen:
            strE = "TS"; break;
		case CSoftlogic::epStateMultiTouchDevice:
            strE = "MTD"; break;
		case CSoftlogic::epStateInvertDetection:
            strE = "IVT"; break;
		}
        QRect rc(pt.x() - xsize * 5, pt.y() - ysize * 4 - ysize, xsize * 10, ysize * 4);
        rc.adjust( -2,-2,2,2 );
        painter->drawText( rc,
                           strE, Qt::AlignVCenter|Qt::AlignHCenter );
	}

    painter->restore();
}

void QLogicDesignWidget::_drawOutGPIO( QPainter * painter, QPoint pt, int size, CSoftlogic::Port out_port )
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    GPIOInfo* pInfo = Keys.getGPIOInfo(out_port-CSoftlogic::epGpio0);
	if ( !pInfo ) return;

	if ( !pInfo->bEnable || !pInfo->bOutput ) return;

	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() - ysize );
    painter->drawEllipse( pt.x() - xsize * 4, pt.y() - ysize * 6 - ysize, xsize * 8, ysize * 6 );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QString str;
		if ( out_port >= CSoftlogic::epGpio0 && out_port < CSoftlogic::epSoftkey0 )
            str = QString::number( (out_port - CSoftlogic::epGpio0)+1 );

        painter->drawText( QRect(pt.x() - xsize * 4, pt.y() - ysize * 5 - ysize, xsize * 8, ysize * 3),
                           "GPIO", Qt::AlignVCenter|Qt::AlignHCenter );
        painter->drawText( QRect(pt.x() - xsize * 4, pt.y() - ysize * 3 - ysize, xsize * 8, ysize * 3),
                           str, Qt::AlignVCenter|Qt::AlignHCenter );
	}

    painter->restore();
}

void QLogicDesignWidget::_drawOutReport( QPainter * painter, QPoint pt, int size, int id )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

    painter->save();

    painter->drawLine( pt.x(), pt.y(), pt.x(), pt.y() - ysize );
    painter->drawEllipse( pt.x() - (int)(xsize * 4.5f), pt.y() - ysize * 6 - ysize,
                          (int)((xsize * 4.5f)*2.f), ysize * 6 );

	if( m_bShowText )
	{
        m_ftDrawText.setPixelSize( ysize * 3 );
        painter->setFont( m_ftDrawText );
        painter->setPen( m_clrLogicFg );

        QString str( QString::number(id) );
        painter->drawText( QRect(pt.x() - xsize * 4, pt.y() - ysize * 5 - ysize - ysize, xsize * 8, ysize * 3),
                           "Repo", Qt::AlignVCenter|Qt::AlignHCenter );
        painter->drawText( QRect(pt.x() - xsize * 4, pt.y() - ysize * 3 - ysize, xsize * 8, ysize * 3),
                           str, Qt::AlignVCenter|Qt::AlignHCenter );
	}

    painter->restore();
}

CSoftlogic::Port QLogicDesignWidget::hitTest( QPoint pt, QPoint * ptPos, GatePart * part, CSoftkey* pDontCheckKey/*=NULL*/ )
{
    QRect rcClient( 0, 0, width(), height() );

    int logic_size = rcClient.width() / LOGIC_SIZE;
    QRect rcLogic( 0 - logic_size, 0 - logic_size / LOGIC_HEIGHT,
                  logic_size, logic_size / LOGIC_HEIGHT * 2 );
	const int xsize = logic_size / 20;
	const int ysize = logic_size / 20;

    QRect rcIn1( 0 - logic_size - xsize * 2, 0 - logic_size / LOGIC_HEIGHT + ysize,
                 xsize * 6, logic_size / LOGIC_HEIGHT - ysize * 2 );
    QRect rcIn2( 0 - logic_size - xsize * 2, 0 + ysize,
                 xsize * 6, logic_size / LOGIC_HEIGHT - ysize * 2 );

	int ni;
    QRect rc;

    CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
    for ( ni = Logics.getSize() - 1; ni >= 0; ni-- )
	{
		CSoftlogic * pSoftlogic = Logics[ni];
		if ( pSoftlogic == NULL )
			continue;

        int x = (int)(pSoftlogic->m_ptPosition.x() * rcClient.width()) + rcClient.left();
        int y = (int)(pSoftlogic->m_ptPosition.y() * rcClient.height()) + rcClient.top();

        rc = rcIn1.adjusted( x, y, x, y );
        if ( rc.contains(pt) )
		{
			if ( ptPos )
                *ptPos = QPoint(x, y);
			if ( part )
				*part = epgIn1;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}

        rc = rcIn2.adjusted( x, y, x, y );
        if ( rc.contains(pt) )
		{
			if ( ptPos )
                *ptPos = QPoint(x, y);
			if ( part )
				*part = epgIn2;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}

        rc = rcLogic.adjusted( x, y, x, y );
        if ( rc.contains(pt) )
		{
			if ( ptPos )
                *ptPos = QPoint(x, y);
			if ( part )
				*part = egpBody;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}
	}

    CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
    for ( ni = Keys.getSize() - 1; ni >= 0; ni-- )
	{
		CSoftkey * pSoftkey = Keys[ni];
		if ( pSoftkey == NULL )
			continue;
		if ( pSoftkey == pDontCheckKey )
			continue;

        int x = (int)(pSoftkey->m_ptPosition.x() * rcClient.width()) + rcClient.left();
        int y = (int)(pSoftkey->m_ptPosition.y() * rcClient.height()) + rcClient.top();
        rc = rcLogic.adjusted( x, y, x, y );
        if ( rc.contains(pt) )
		{
			if ( ptPos )
                *ptPos = QPoint(x, y);
			if ( part )
				*part = egpBody;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTKEY_START);
		}
	}

	return CSoftlogic::epNoGate;
}

QRect QLogicDesignWidget::getKeyBound( CSoftkey* pSoftkey, bool bFocusRect )
{
    QRect rcClient( 0, 0, width(), height() );
    int logic_size = rcClient.width() / LOGIC_SIZE;
    QPoint ptX((int)(rcClient.width() * pSoftkey->m_ptPosition.x()), (int)(rcClient.height() * pSoftkey->m_ptPosition.y()));
    QRect rcKey;
	if ( bFocusRect )
	{
        rcKey = QRect( ptX.x() - logic_size, ptX.y() - logic_size / LOGIC_HEIGHT,
                       logic_size, logic_size / LOGIC_HEIGHT * 2 );
        rcKey.adjust( -12, -1, 12, 1 );
		return rcKey;
	}
    rcKey = QRect( ptX.x() - logic_size, ptX.y() - logic_size / 5,
                   logic_size, logic_size / 5 * 2);
    rcKey.adjust( -1, -1, 1, 1 );
	return rcKey;
}

QRect QLogicDesignWidget::getLogicBound( CSoftlogic* pLogic )
{
	if ( pLogic == NULL )
        return QRect();

    QRect rcClient( 0, 0, width(), height() );
    int logic_size = rcClient.width() / LOGIC_SIZE;

    QPoint pt((int)(rcClient.width() * pLogic->m_ptPosition.x()), (int)(rcClient.height() * pLogic->m_ptPosition.y()));

    QRect rcLogic( pt.x() - logic_size * 4 / 5, pt.y() - logic_size / LOGIC_HEIGHT,
                   logic_size * 3 / 5, logic_size / LOGIC_HEIGHT * 2 );

	const int xsize = logic_size / 20;
	const int ysize = logic_size / 20;

    QRect rcOutPort( pt.x() - xsize * 4, pt.y() - ysize * 6 - ysize,
                     xsize * 8, ysize * 6 );

    int xe = rcLogic.left() + rcLogic.width() / 2 - xsize;
    int ye = rcLogic.top();
	ye -= xsize * 2;	// not
    QRect rcEnablePort( xe - xsize * 3, ye - ysize * 4 - ysize, xsize * 6, ysize * 4 );


    QPoint ptIn1( pt.x() - logic_size, (pt.y() - logic_size / 4) + logic_size / 8 );
    QPoint ptIn2( pt.x() - logic_size, (pt.y() + logic_size / 4) - logic_size / 8 );
    QRect rcInPort1( ptIn1.x() - xsize * 4, ptIn1.y() - ysize * 2, xsize * 4, ysize * 4 );
    QRect rcInPort2( ptIn2.x() - xsize * 4, ptIn2.y() - ysize * 2, xsize * 4, ysize * 4 );

    rcOutPort.adjust( -2, -2, 2, 2 );

    rcLogic = rcLogic.united( rcOutPort );
    rcLogic = rcLogic.united( rcEnablePort );
    rcLogic = rcLogic.united( rcInPort1 );
    rcLogic = rcLogic.united( rcInPort2 );

    rcLogic.adjust( -1, -1, 1, 1 );

	return rcLogic;
}


// event

void QLogicDesignWidget::mousePressEvent(QMouseEvent *evt)
{
    if( m_eScreenMode == ScreenModePreview ) return;

    switch( evt->button() )
    {
    case Qt::LeftButton:
        mouseLBtnDown(evt->pos());
        break;
    case Qt::RightButton:
//        mouseRBtnDown(evt);
        break;
    case Qt::MidButton:
        break;
    default:
        QWidget::mousePressEvent(evt);
        break;
    }
}

void QLogicDesignWidget::mouseLBtnDown(QPoint pos)
{
    m_ptSoftkeyPositionBackup.setX( -1.f );
    m_ptSoftkeyPositionBackup.setY( -1.f );

    if( m_rcCloseButton.contains(pos) )
    {
        m_bDownCloseButton = true;
        m_bCpaute = true;
        update( m_rcCloseButton );
        return;
    }

    QPoint pt;
    CSoftlogic::Port port_hit = hitTest(pos, &pt, &m_SelPart);
    if ( port_hit != CSoftlogic::epNoGate )
    {
        m_SelOffset = pt - pos;
        m_port_dst = CSoftlogic::epNoGate;
        m_bCpaute = true;

        if ( m_SelPart == egpBody )
        {
            if ( m_port_hit >= CSoftlogic::epSoftkey0 && m_port_hit < CSoftlogic::epSoftkeyAll )
            {
                CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
                CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
                if ( pSoftkey != NULL )
                {
                    m_pCheckKey = pSoftkey;
                    m_ptSoftkeyPositionBackup = pSoftkey->m_ptPosition;
                    QRect rcUpdate = getKeyBound( m_pCheckKey, true );

                    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();
                    for ( int i=0 ; i<Logics.getSize() ; i++ )
                    {
                        CSoftlogic* pSoftlogic = Logics[i];

                        if( isAssociate(m_pCheckKey, pSoftlogic) )
                        {
                            rcUpdate = rcUpdate.united( getLogicBound(pSoftlogic) );
                        }
                    }

                    if ( pSoftkey->getBind() )
                    {
                        rcUpdate = rcUpdate.united( getKeyBound(pSoftkey->getBind(), false) );
                    }

                    update( rcUpdate );
                }
            }
        }
    }

    if ( m_wndEditGate.isVisible() )
        m_wndEditGate.hide();
    if ( m_wndLogicGallery.isVisible() )
        m_wndLogicGallery.close();
}

void QLogicDesignWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    if( m_eScreenMode == ScreenModePreview ) return;

    switch( evt->button() )
    {
    case Qt::LeftButton:
        mouseLBtnUp(evt->pos());
        break;
    case Qt::RightButton:
        mouseRBtnUp(evt->pos());
        break;
    default:
        QWidget::mouseReleaseEvent(evt);
        break;
    }
}

void QLogicDesignWidget::mouseLBtnUp(QPoint pos)
{
    QRect rcClient( 0, 0, width(), height() );
    int logic_size = rcClient.width() / LOGIC_SIZE;

	if( m_bDownCloseButton )
	{
        if( m_bCpaute )
		{
            m_bDownCloseButton = false;
            update( m_rcCloseButton );
            m_bCpaute = false;
            if( m_rcCloseButton.contains(pos) )
			{
                close();
			}
			return;
		}
	}

    if ( m_bCpaute )
	{
		if ( m_SelPart == epgIn1 )
		{
			int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
			CSoftlogic * pSoftlogic = Logics[idx_logic];
			if ( pSoftlogic )
			{
				pSoftlogic->setIn1Port(m_port_dst);
                update();
				goto goto_return;
			}
		}
		else if ( m_SelPart == epgIn2 )
		{
			int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
			CSoftlogic * pSoftlogic = Logics[idx_logic];
			if ( pSoftlogic )
			{
				pSoftlogic->setIn2Port(m_port_dst);
                update();
				goto goto_return;
			}
		}

		if ( m_pToBindKey )
		{
			m_pCheckKey->setBind( m_pToBindKey );
			if ( m_pCheckKey )
			{
                float fdx = m_pCheckKey->m_ptPosition.x() - m_ptSoftkeyPositionBackup.x();
                float fdy = m_pCheckKey->m_ptPosition.y() - m_ptSoftkeyPositionBackup.y();
				m_pCheckKey->m_ptPosition = m_ptSoftkeyPositionBackup;

				// restore logic position
                CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
                for ( int i=0 ; i<Logics.getSize() ; i++ )
				{
					CSoftlogic* pSoftlogic = Logics[i];

                    if( isAssociate(m_pCheckKey, pSoftlogic) )
					{
                        pSoftlogic->m_ptPosition.setX( pSoftlogic->m_ptPosition.x() - fdx );
                        pSoftlogic->m_ptPosition.setY( pSoftlogic->m_ptPosition.y() - fdy );
					}
				}
			}
			m_pToBindKey = NULL;
            update();
		}
		else
		{
			if ( m_pCheckKey )
			{
                CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
                for ( int i=0 ; i<Logics.getSize() ; i++ )
				{
					CSoftlogic* pSoftlogic = Logics[i];

                    if( isAssociate(m_pCheckKey, pSoftlogic) )
					{
						// adjustment position
                        QPoint ptX((int)(rcClient.width() * pSoftlogic->m_ptPosition.x()), (int)(rcClient.height() * pSoftlogic->m_ptPosition.y()));
                        QRect rcLogic(ptX.x() - logic_size * 4 / 5, ptX.y() - logic_size / LOGIC_HEIGHT,
                                      logic_size * 3 / 5, logic_size / LOGIC_HEIGHT * 2);

                        QPoint ptN( ptX );
                        if ( rcLogic.left() < rcClient.left() ) ptN.setX( ptN.x() + (rcClient.left()-rcLogic.left()) );
                        if ( rcLogic.top() < rcClient.top() ) ptN.setY( ptN.y() + (rcClient.top()-rcLogic.top()) );
                        if ( rcLogic.right() > rcClient.right() ) ptN.setX( ptN.x() + (rcClient.right()-rcLogic.right()) );
                        if ( rcLogic.bottom() > rcClient.bottom() ) ptN.setY( ptN.y() + (rcClient.bottom()-rcLogic.bottom()) );

						if( ptN != ptX )
						{
                            float fX = (float)(ptX.x() - ptN.x()) / rcClient.width();
                            float fY = (float)(ptX.y() - ptN.y()) / rcClient.height();
                            pSoftlogic->m_ptPosition.setX( pSoftlogic->m_ptPosition.x() - fX );
                            pSoftlogic->m_ptPosition.setY( pSoftlogic->m_ptPosition.y() - fY );
						}
					}
				}
			}
		}

		if ( m_pCheckKey )
		{
			m_pCheckKey = NULL;
            update();
		}

goto_return:
        m_bCpaute = false;
	}
}

void QLogicDesignWidget::mouseRBtnUp(QPoint pos)
{
    if( m_wndEditGate.isVisible() )
        m_wndEditGate.hide();

    if ( m_port_hit <= CSoftlogic::epNoGate )
    {
        m_wndLogicGallery.setInfo( CSoftlogic::epNoGate, pos );
        m_wndLogicGallery.exec();
        return;
    }

    if ( m_port_hit >= CSoftlogic::epSoftkey0 && m_port_hit < CSoftlogic::epSoftkeyAll )
    {
        // unbind key

        CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
        CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
        if ( pSoftkey != NULL )
        {
            if ( pSoftkey->getBind() )
            {
                pSoftkey->setBind( NULL );
                update();
            }
        }
        return;
    }

    int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;

    if( idx_logic < 0 ) return;

    CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
    CSoftlogic * pSoftlogic = Logics[idx_logic];


    m_pSoftlogicForRemove = pSoftlogic;

    update();

    if( QMessageBox::question( this, "Warning", "Would you like to delete the logic gate?", QMessageBox::Yes|QMessageBox::No ) == QMessageBox::Yes )
    {
        // delete!
        Logics.remove( pSoftlogic );
    }

    m_pSoftlogicForRemove = NULL;
    update();
}

void QLogicDesignWidget::mouseMoveEvent(QMouseEvent *evt)
{
    if( m_eScreenMode == ScreenModePreview ) return;

    if ( m_bCpaute )
    {
        QRect rcClient( 0, 0, width(), height() );

        if ( m_SelPart == egpBody )
        {
            CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
            CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

            if ( m_port_hit <= CSoftlogic::epNoGate )
            {
            }
            else if ( m_port_hit < CSoftlogic::epGpio0 )
            {
            }
            else if ( m_port_hit < CSoftlogic::epSoftkey0 )
            {
            }
            else if ( m_port_hit < CSoftlogic::epSoftkeyAll )
            {
                if ( m_pCheckKey != NULL )
                {
                    QRect rcUpdate;

                    GatePart gp;
                    QRect rcKey( getKeyBound( m_pCheckKey, true ) );
                    rcUpdate = rcKey;

                    if ( m_pToBindKey )
                    {
                        rcUpdate = rcUpdate.united( getKeyBound( m_pToBindKey, true ) );
                    }

                    m_pToBindKey = NULL;

                    bool bPressCtrlkey = evt->modifiers() & Qt::ControlModifier;
                    if( !bPressCtrlkey )
                    {
                        QPoint ptCheck( rcKey.center() );
                        CSoftlogic::Port over_port_hit = hitTest( ptCheck, NULL, &gp, m_pCheckKey );
                        if ( over_port_hit >= CSoftlogic::epSoftkey0 && over_port_hit < CSoftlogic::epSoftkeyAll )
                        {
                            CSoftkey * pOverKey = Keys[over_port_hit - CSoftlogic::epSoftkey0];
                            if ( pOverKey )
                            {
                                m_pToBindKey = pOverKey;
                            }
                        }
                    }

                    QPointF ptOld( m_pCheckKey->m_ptPosition );

                    QPoint point( evt->pos() + m_SelOffset );
                    m_pCheckKey->m_ptPosition.setX( (float)point.x() / rcClient.width() );
                    m_pCheckKey->m_ptPosition.setX( (float)(int)(m_pCheckKey->m_ptPosition.x() * 200 + 0.5f) / 200 );
                    m_pCheckKey->m_ptPosition.setY( (float)point.y() / rcClient.height() );
                    m_pCheckKey->m_ptPosition.setY( (float)(int)(m_pCheckKey->m_ptPosition.y() * 200 + 0.5f) / 200 );

                    // adjustment position
                    {
                        QRect rcKey( getKeyBound( m_pCheckKey, false ) );

                        QPoint ptN( point );
                        if ( rcKey.left() < rcClient.left() ) ptN.setX( ptN.x() + (rcClient.left()-rcKey.left()) );
                        if ( rcKey.top() < rcClient.top() ) ptN.setY( ptN.y() + (rcClient.top()-rcKey.top()) );
                        if ( rcKey.right() > rcClient.right() ) ptN.setX( ptN.x() + (rcClient.right()-rcKey.right()) );
                        if ( rcKey.bottom() > rcClient.bottom() ) ptN.setY( ptN.y() + (rcClient.bottom()-rcKey.bottom()) );

                        if( ptN != point )
                        {
                            m_pCheckKey->m_ptPosition.setX( (float)ptN.x() / rcClient.width() );
                            m_pCheckKey->m_ptPosition.setX( (float)(int)(m_pCheckKey->m_ptPosition.x() * 200 + 0.5f) / 200 );
                            m_pCheckKey->m_ptPosition.setY( (float)ptN.y() / rcClient.height() );
                            m_pCheckKey->m_ptPosition.setY( (float)(int)(m_pCheckKey->m_ptPosition.y() * 200 + 0.5f) / 200 );
                        }
                    }

                    rcUpdate = rcUpdate.united( getKeyBound(m_pCheckKey, true) );

                    float fdx = m_pCheckKey->m_ptPosition.x() - ptOld.x();
                    float fdy = m_pCheckKey->m_ptPosition.y() - ptOld.y();

                    for ( int i=0 ; i<Logics.getSize() ; i++ )
                    {
                        CSoftlogic* pSoftlogic = Logics[i];

                        if( isAssociate(m_pCheckKey, pSoftlogic) )
                        {
                            rcUpdate = rcUpdate.united( getLogicBound(pSoftlogic) );
                            pSoftlogic->m_ptPosition.setX( pSoftlogic->m_ptPosition.x() + fdx );
                            pSoftlogic->m_ptPosition.setY( pSoftlogic->m_ptPosition.y() + fdy );
                            rcUpdate = rcUpdate.united( getLogicBound(pSoftlogic) );
                        }
                    }

                    if ( m_pToBindKey )
                    {
                        rcUpdate = rcUpdate.united( getKeyBound(m_pToBindKey, true) );
                    }

                    if ( m_pCheckKey->getBind() )
                    {
                        rcUpdate = rcUpdate.united( getKeyBound(m_pCheckKey->getBind(), false) );
                        rcUpdate.adjust( 0, 0, 10, 0 );
                    }

                    for ( int i=0 ; i<Keys.getSize() ; i++ )
                    {
                        CSoftkey* pKey = Keys[i];
                        if ( pKey->getBind() == m_pCheckKey )
                        {
                            rcUpdate = rcUpdate.united( getKeyBound( pKey, false) );
                        }
                    }

                    update( rcUpdate );

                }
            }
            else
            {	// softlogic
                CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
                CSoftlogic * pSoftlogic = Logics[m_port_hit - CSoftlogic::epSoftLogic0];
                if ( pSoftlogic != NULL )
                {
                    QRect rcUpdate( getLogicBound( pSoftlogic ) );

                    QPoint point( evt->pos() + m_SelOffset );
                    pSoftlogic->m_ptPosition.setX( ((float)point.x() / (float)rcClient.width() * 200.f + 0.5f) / 200.f );
                    pSoftlogic->m_ptPosition.setY( ((float)point.y() / (float)rcClient.height() * 200.f + 0.5f) / 200.f );

                    rcUpdate = rcUpdate.united( getLogicBound(pSoftlogic) );

                    CSoftlogic::Port epIn1 = pSoftlogic->getIn1Port();
                    if ( epIn1 != CSoftlogic::epNoGate )
                    {
                        if ( epIn1 >= CSoftlogic::epSoftkey0 && epIn1 < CSoftlogic::epSoftkeyAll )
                        {
                            CSoftkey* pAssociateKey = Keys[epIn1 - CSoftlogic::epSoftkey0];
                            rcUpdate = rcUpdate.united( getKeyBound(pAssociateKey, false) );
                        }
                        else if ( epIn1 >= CSoftlogic::epSoftLogic0 )
                        {
                            CSoftlogic* pAssociateLogic = Logics[epIn1 - CSoftlogic::epSoftLogic0];
                            rcUpdate = rcUpdate.united( getLogicBound(pAssociateLogic) );
                        }
                    }

                    CSoftlogic::Port epIn2 = pSoftlogic->getIn2Port();
                    if ( epIn2 != CSoftlogic::epNoGate )
                    {
                        if ( epIn2 >= CSoftlogic::epSoftkey0 && epIn2 < CSoftlogic::epSoftkeyAll )
                        {
                            CSoftkey* pAssociateKey = Keys[epIn2 - CSoftlogic::epSoftkey0];
                            rcUpdate = rcUpdate.united( getKeyBound(pAssociateKey, false) );
                        }
                        else if ( epIn2 >= CSoftlogic::epSoftLogic0 )
                        {
                            CSoftlogic* pAssociateLogic = Logics[epIn2 - CSoftlogic::epSoftLogic0];
                            rcUpdate = rcUpdate.united( getLogicBound(pAssociateLogic) );
                        }
                    }

                    CSoftlogic::PortType eptOut = pSoftlogic->getOutPortType();
                    if ( eptOut == CSoftlogic::eptState && pSoftlogic->getOutPort() == CSoftlogic::epNoGate )
                    {
                        for ( int i=0 ; i<Logics.getSize() ; i++ )
                        {
                            CSoftlogic* pSoftlogicCheck = Logics[i];

                            if( isAssociate(pSoftlogicCheck, pSoftlogic) )
                            {
                                rcUpdate = rcUpdate.united( getLogicBound(pSoftlogicCheck) );
                            }
                        }
                    }

                    update( rcUpdate );
                }
            }
        } // body
        else // in port
        {
            CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
            CSoftlogic * logic = Logics[m_port_hit - CSoftlogic::epSoftLogic0];

            if ( logic )
            {
                QPoint ptPos( (int)(rcClient.width() * logic->m_ptPosition.x()),
                              (int)(rcClient.height() * logic->m_ptPosition.y()) );
                int logic_size = rcClient.width() / LOGIC_SIZE;

                QRect rcUpdate;
                if ( m_SelPart == epgIn1 )
                {
                    if ( m_pt_dst.x() >= 0 )
                    {
                        QPoint ptX( ptPos.x() - logic_size, (ptPos.y() - logic_size / 4) + logic_size / 8 );
                        rcUpdate.setRect( m_pt_dst.x(), m_pt_dst.y(), ptX.x()-m_pt_dst.x(), ptX.y()-m_pt_dst.y() );
                    }
                }
                else if ( m_SelPart == epgIn2 )
                {
                    if ( m_pt_dst.x() >= 0 )
                    {
                        QPoint ptX( ptPos.x() - logic_size, (ptPos.y() + logic_size / 4) - logic_size / 8 );
                        rcUpdate.setRect( m_pt_dst.x(), m_pt_dst.y(), ptX.x()-m_pt_dst.x(), ptX.y()-m_pt_dst.y() );
                    }
                }

                rcUpdate = rcUpdate.normalized();
                rcUpdate.adjust( -1, -1, 1, 1 );

                update( rcUpdate );
            }

            QPoint pt;
            GatePart gp;
            CSoftlogic::Port port_hit = hitTest(evt->pos(), &pt, &gp);
            if ( port_hit != CSoftlogic::epNoGate )
                m_pt_dst = pt;
            else
                m_pt_dst = evt->pos();
            m_port_dst = port_hit;

            if ( logic )
            {
                QPoint ptPos( (int)(rcClient.width() * logic->m_ptPosition.x()),
                              (int)(rcClient.height() * logic->m_ptPosition.y()) );
                int logic_size = rcClient.width() / LOGIC_SIZE;

                QRect rcUpdate;
                if ( m_SelPart == epgIn1 )
                {
                    if ( m_pt_dst.x() >= 0 )
                    {
                        QPoint ptX( ptPos.x() - logic_size, (ptPos.y() - logic_size / 4) + logic_size / 8 );
                        rcUpdate.setRect( m_pt_dst.x(), m_pt_dst.y(), ptX.x()-m_pt_dst.x(), ptX.y()-m_pt_dst.y() );
                    }
                }
                else if ( m_SelPart == epgIn2 )
                {
                    if ( m_pt_dst.x() >= 0 )
                    {
                        QPoint ptX( ptPos.x() - logic_size, (ptPos.y() + logic_size / 4) - logic_size / 8 );
                        rcUpdate.setRect( m_pt_dst.x(), m_pt_dst.y(), ptX.x()-m_pt_dst.x(), ptX.y()-m_pt_dst.y() );
                    }
                }

                rcUpdate = rcUpdate.normalized();
                rcUpdate.adjust( -1, -1, 1, 1 );

                update( rcUpdate );
            }
        }
    }
    else	// not capture
    {
        GatePart gp;
        CSoftlogic::Port port_hit = hitTest(evt->pos(), NULL, &gp);
        if ( m_port_hit != port_hit || m_SelPart != gp )
        {
            m_port_hit = port_hit;
            m_SelPart = gp;
            m_pt_dst.setX( -1 );
            m_pt_dst.setY( -1 );
            update();
        }

        if( m_port_hit != CSoftlogic::epNoGate )
            setCursor( Qt::SizeAllCursor );
        else
            setCursor( Qt::ArrowCursor );
    }
}

void QLogicDesignWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    if( m_eScreenMode == ScreenModePreview ) return;

    switch( evt->button() )
    {
    case Qt::LeftButton:
        mouseLBtnDoubleClick(evt->pos());
        break;
    default:
        QWidget::mouseDoubleClickEvent(evt);
        break;
    }
}

void QLogicDesignWidget::mouseLBtnDoubleClick(QPoint pos)
{
    CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();

	if ( m_port_hit <= CSoftlogic::epNoGate )
	{
		// new logic!

        int idx_logic = Logics.addSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE, false, CSoftlogic::epNoGate,
                                    false, CSoftlogic::epNoGate,
                                    false, CSoftlogic::epNoGate,
                                    CSoftlogic::epNoGate, CSoftlogic::etNoTrigger, true );

		CSoftlogic * pSoftlogic = Logics[idx_logic];
		if ( pSoftlogic )
		{
            QRect rcClient( 0, 0, width(), height() );
            pSoftlogic->m_ptPosition.setX( (float)pos.x() / (float)rcClient.width() );
            pSoftlogic->m_ptPosition.setY( (float)pos.y() / (float)rcClient.height() );

            m_wndEditGate.setWindowTitle( "New Gate" );
            m_wndEditGate.setLogic( pSoftlogic, (idx_logic) < Logics.getShowSize() );
            m_wndEditGate.show();
            m_wndEditGate.activateWindow();
            update();
		}
	}
	else if ( m_port_hit >= CSoftlogic::epSoftLogic0 )
	{
		int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		CSoftlogic * pSoftlogic = Logics[idx_logic];

		if ( pSoftlogic )
		{
			// Active!
            m_wndEditGate.setWindowTitle( "Edit Gate" );
            m_wndEditGate.setLogic( pSoftlogic, (idx_logic) < Logics.getShowSize() );
            m_wndEditGate.show();
            m_wndEditGate.activateWindow();
            update();
		}
	}
	else if ( (m_port_hit >= CSoftlogic::epSoftkey0) && (m_port_hit <= CSoftlogic::epSoftkey29) )
	{
//        CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
//        CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
//        if( pSoftkey )
//        {
//            pSoftkey->
//        }
        m_wndLogicGallery.setInfo( m_port_hit, pos );
        m_wndLogicGallery.exec();
        m_bCpaute = false;
	}
}

void QLogicDesignWidget::keyPressEvent(QKeyEvent *evt)
{
    switch( evt->key() )
	{
    case Qt::Key_Alt:
		if ( !m_bShowNumber )
            update();
        m_bShowNumber = true;
        return;
        break;
    case Qt::Key_Escape:
        close();
        return;
        break;
    default:
        break;
	}

    QWidget::keyPressEvent(evt);
}

void QLogicDesignWidget::keyReleaseEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Alt )
	{
		if ( m_bShowNumber )
            update();
        m_bShowNumber = false;

        return;
	}

    QWidget::keyReleaseEvent(evt);
}

void QLogicDesignWidget::resizeEvent(QResizeEvent *evt)
{
    if ( evt->size().isEmpty() ) return;

    if ( m_pImageCanvas != NULL ) delete m_pImageCanvas;
    m_pImageCanvas = NULL;

    QWidget::resizeEvent(evt);
}

void QLogicDesignWidget::onAddLogicFromGallery( int nGalleryItemType, CSoftlogic::Port portKey, QPoint ptMouse )
{
    QRect rcClient( 0, 0, width(), height() );
    int logic_size = rcClient.width() / LOGIC_SIZE;
    float fLogicSize = (float)logic_size / rcClient.width();

    CSoftlogicArray & Logics = T3kCommonData::instance()->getLogics();
    int nLogicIndex;
    CSoftlogic* pLogic;

    if ( portKey >= CSoftlogic::epGpio0 && portKey < CSoftlogic::epSoftkey0 )		// GPIO type
    {
        switch ( nGalleryItemType )
        {
        case 3:
            // Calibration with GPIO
            nLogicIndex = Logics.addSoftlogic_State(
                CSoftlogic::eltAND_RISINGEDGE,
                true, CSoftlogic::epStateCalibration,
                true, portKey,
                false, CSoftlogic::epNoGate,
                CSoftlogic::epStateCalibration,
                CSoftlogic::etTrigger_U1024_D0, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() - fLogicSize*0.45f );

            nLogicIndex = Logics.addSoftlogic_State(
                CSoftlogic::eltAND_RISINGEDGE,
                false, CSoftlogic::epStateCalibration,
                true, portKey,
                false, CSoftlogic::epNoGate,
                CSoftlogic::epStateCalibration,
                CSoftlogic::etTrigger_U128_D128, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() + fLogicSize*0.8f );
            break;
        case 4:
            // Touch ON/OFF with GPIO
            nLogicIndex = Logics.addSoftlogic_State(
                CSoftlogic::eltAND_RISINGEDGE,
                false, CSoftlogic::epNoGate,
                true, portKey,
                false, CSoftlogic::epStateTouchScreen,
                CSoftlogic::epStateTouchScreen,
                CSoftlogic::etTrigger_U128_D128, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() - fLogicSize*0.45f );

            nLogicIndex = Logics.addSoftlogic_State(
                CSoftlogic::eltAND_RISINGEDGE,
                false, CSoftlogic::epNoGate,
                false, portKey,
                true, CSoftlogic::epStateTouchScreen,
                CSoftlogic::epStateTouchScreen,
                CSoftlogic::etTrigger_U128_D128, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() + fLogicSize*0.8f );
            break;
        case 5:
            // IR Pen Mode with GPIO
            {
                CSoftlogic::Port port1, port2;
                nLogicIndex = Logics.addSoftlogic_State(
                    CSoftlogic::eltAND_RISINGEDGE,
                    false, CSoftlogic::epNoGate,
                    true, portKey,
                    false, CSoftlogic::epStateInvertDetection,
                    CSoftlogic::epStateInvertDetection,
                    CSoftlogic::etTrigger_U128_D128, true );

                port1 = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
                pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() - fLogicSize*0.45f );

                nLogicIndex = Logics.addSoftlogic_State(
                    CSoftlogic::eltAND_RISINGEDGE,
                    false, CSoftlogic::epNoGate,
                    false, portKey,
                    true, CSoftlogic::epStateInvertDetection,
                    CSoftlogic::epStateInvertDetection,
                    CSoftlogic::etTrigger_U128_D128, true );

                port2 = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*1.5f );
                pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() + fLogicSize*0.8f );

                nLogicIndex = Logics.addSoftlogic_State(
                    CSoftlogic::eltOR,
                    false, CSoftlogic::epNoGate,
                    false, port1,
                    false, port2,
                    CSoftlogic::epStateWorking,
                    CSoftlogic::etNoTrigger, true );

                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)ptMouse.x() / rcClient.width() + fLogicSize*3.0f );
                pLogic->m_ptPosition.setY( (float)ptMouse.y() / rcClient.height() + fLogicSize*0.1f );
            }
            break;
        }
    }
    else
    {
        Q_ASSERT( (portKey >= CSoftlogic::epSoftkey0) && (portKey <= CSoftlogic::epSoftkey29) );

        CSoftkeyArray & Keys = T3kCommonData::instance()->getKeys();
        CSoftkey* pSoftkey = Keys[(int)(portKey-CSoftlogic::epSoftkey0)];
        QRect rcKey = getKeyBound( pSoftkey, false );

        switch ( nGalleryItemType )
        {
        case 0:
            // 1sec delay touch ON/OFF
            nLogicIndex = Logics.addSoftlogic_State(
                CSoftlogic::eltAND_RISINGEDGE,
                true, CSoftlogic::epStateCalibration,
                false, portKey,
                false, CSoftlogic::epNoGate,
                CSoftlogic::epStateTouchScreen,
                CSoftlogic::etTrigger_U1024_D0, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() );
            break;
        case 1:
            // 3sec delay enter coord calibration
            nLogicIndex = Logics.addSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE,
                true, CSoftlogic::epStateCalibration,
                false, portKey,
                false, CSoftlogic::epNoGate,
                CSoftlogic::epStateCalibration,
                CSoftlogic::etTrigger_U3072_D0, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() - fLogicSize*0.45f );

            nLogicIndex = Logics.addSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE,
                false, CSoftlogic::epStateCalibration,
                false, portKey,
                false, CSoftlogic::epNoGate,
                CSoftlogic::epStateCalibration,
                CSoftlogic::etTrigger_U2048_D0, true );

            pLogic = Logics[nLogicIndex];
            pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*1.5f );
            pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() + fLogicSize*0.8f );
            break;
        case 2:
            // TaskSwitch
            {
                // MTD Gate
                nLogicIndex = Logics.addSoftlogic_State( CSoftlogic::eltAND,
                    false, CSoftlogic::epStateMultiTouchDevice,
                    false, CSoftlogic::epNoGate,
                    false, portKey,
                    CSoftlogic::epNoGate,
                    CSoftlogic::etNoTrigger, true );

                CSoftlogic::Port portMTD = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*1.5f );
                pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() + fLogicSize*1.5f );

                // OR Gate
                nLogicIndex = Logics.addSoftlogic_State( CSoftlogic::eltOR,
                    false, CSoftlogic::epNoGate,
                    true, portKey,
                    false, portMTD,
                    CSoftlogic::epNoGate,
                    CSoftlogic::etTrigger_U1024_D0, true );

                CSoftlogic::Port portORGate = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*2.3f );
                pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() + fLogicSize*0.5f );


                // Tab
                nLogicIndex = Logics.addSoftlogic_Key( CSoftlogic::eltAND_RISINGEDGE,
                    false, portKey,
                    true, portMTD,
                    0x00, 0x2b,			//0x2b = tab
                    CSoftlogic::etNoTrigger, true );
                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*3.8f );
                pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() - fLogicSize*0.5f );

                // Alt
                nLogicIndex = Logics.addSoftlogic_Key( CSoftlogic::eltAND,
                    true, portORGate,
                    false, CSoftlogic::epNoGate,
                    0x04, 0x00,			//0x04 = Alt
                    CSoftlogic::etNoTrigger, true );
                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*3.8f );
                pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() + fLogicSize*0.5f );


                // Ctrl+Win+Tab
                nLogicIndex = Logics.addSoftlogic_Key( CSoftlogic::eltAND_RISINGEDGE,
                    false, portMTD,
                    false, CSoftlogic::epNoGate,
                    0x01|0x08, 0x2b,			//0x01 = Ctrl, 0x08 = Win, 0x2b = tab
                    CSoftlogic::etNoTrigger, true );
                pLogic = Logics[nLogicIndex];
                pLogic->m_ptPosition.setX( (float)rcKey.right() / rcClient.width() + fLogicSize*3.8f );
                pLogic->m_ptPosition.setY( (float)rcKey.center().y() / rcClient.height() + fLogicSize*1.5f );
            }
            break;
        } // end of switch ( nGalleryItemType )
    }

    update();
}

void QLogicDesignWidget::onUpdate()
{
    update();
}
