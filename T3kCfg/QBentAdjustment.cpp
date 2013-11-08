#include "QBentAdjustment.h"

#include <QPainter>

//#include "QLogSystem.h"
#include "QUtils.h"
#include "QBentCfgParam.h"
#include "fe/QCalcCamValue.h"
#include "T3kConstStr.h"
#include "QBentProgressDialog.h"
#include "QT3kUserData.h"
#include "T3kCamNameDef.h"
#include "T3kBuzzerDef.h"
#include "QRequestHIDManager.h"

#include <QScreen>
#include <math.h>
#include <QFont>
#include <QtNumeric>
#include <QEvent>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>

#define NaN ((float)qQNaN())
#define WAIT_ANIMATION_FRAME	(10)
#define WAIT_TOUCH_TIME			(30*WAIT_ANIMATION_FRAME)
#define WAIT_TOUCH_DRAW_TIME	(16*WAIT_ANIMATION_FRAME)

struct PosXY {
    float x;
    float y;
    int idx;
};

const PosXY s_PosXY[3][ADJUSTMENT_STEP] = {
    { // old 13 points
        { -2.0f, -1.0f, 9  },
        { -2.0f,  0.0f, 5  },
        { -2.0f, +1.0f, 10 },
        { -1.5f,  0.0f, 7  },
        { -1.0f, +1.0f, 2  },
        { -1.0f, -1.0f, 1  },
        {  0.0f,  0.0f, 0  },
        { +1.0f, -1.0f, 4  },
        { +1.0f, +1.0f, 3  },
        { +1.5f,  0.0f, 8  },
        { +2.0f, +1.0f, 11 },
        { +2.0f,  0.0f, 6  },
        { +2.0f, -1.0f, 12 }
    },
    { // new 13 points
        { -2.0f, -1.0f, 9  },
        { -2.0f,  0.0f, 5  },
        { -2.0f, +1.0f, 10 },
        { -1.0f, +1.0f, 2  },
        { -1.0f,  0.0f, 7  },
        { -1.0f, -1.0f, 1  },
        {  0.0f,  0.0f, 0  },
        { +1.0f, -1.0f, 4  },
        { +1.0f,  0.0f, 8  },
        { +1.0f, +1.0f, 3  },
        { +2.0f, +1.0f, 11 },
        { +2.0f,  0.0f, 6  },
        { +2.0f, -1.0f, 12 }
    },
    { // new 9 points
        { -2.0f, -1.0f, 9  },
        {   NaN,   NaN, 5  },
        { -2.0f, +1.0f, 10 },
        { -1.0f, +1.0f, 2  },
        {   NaN,   NaN, 7  },
        { -1.0f, -1.0f, 1  },
        {  0.0f,  0.0f, 0  },
        { +1.0f, -1.0f, 4  },
        {   NaN,   NaN, 8  },
        { +1.0f, +1.0f, 3  },
        { +2.0f, +1.0f, 11 },
        {   NaN,   NaN, 6  },
        { +2.0f, -1.0f, 12 }
    }
};

#define MAIN_TAG "MAIN"
#define RES_TAG "BENT ADJUSTMENT"


QBentAdjustment::QBentAdjustment(QWidget* targetWidget, QObject *parent) :
    QObject(parent), m_pTargetWidget(targetWidget)
{
    Q_ASSERT( targetWidget && targetWidget->isWidgetType() );

    m_pT3kHandle = QT3kUserData::GetInstance()->getT3kHandle();

    m_bEnterAdjustmentMode = false;
    m_nAdjustmentStep = 0;
    m_bIsValidTouch = 0;
    m_nValidTouchCount = 0;
    m_bBentAdjustmentPerformed = false;
    m_nBentProgress = 0;

    m_bCheckCamTouch[0] = m_bCheckCamTouch[1] = false;
    m_nTouchCount = 0;
    m_nMaxTouchCount = QT3kUserData::GetInstance()->getCamCount() * 130;
    m_bIsTouchOK = false;
    m_bIsTouchLift = true;
    m_lClickArea = 0;

    m_bDrawWaitTimeout = false;
    m_nWaitCountDown = WAIT_TOUCH_TIME;
    m_bOldTouchState = false;

    m_cError = 0x00;
    m_cNG = 0x00;

    m_bShowCursor = false;

    m_TimerReCheckPoint = 0;
    m_TimerBlinkCursor = 0;
    m_TimerDrawWaitTimeout = 0;

    m_nTimerSaveCmd = 0;
    m_nSendCmdID = -1;

    m_nMonitorOrientation = 0;

    connect( &m_T3kRequestManager, &QRequestHIDManager::finish, this,&QBentAdjustment::onFinishRequestCommand );

//    LOG_I( "Enter [Bent Adjustment]" );

    onChangeLanguage();

    QBentCfgParam* bentParam = QBentCfgParam::instance();
    int nAlgorithm = 0;
    if( QT3kUserData::GetInstance()->isSubCameraExist() )
        nAlgorithm = 1; // get
    else
    {
        if( QT3kUserData::GetInstance()->getFirmwareVersionStr().compare( "2.8" ) <= 0 )
            nAlgorithm = 1; // get
        else
            nAlgorithm = 2; // get
    }
    bentParam->setAlgorithm( nAlgorithm );

    int nLeft, nTop, nRight, nBottom, nDir = 0;
    nLeft = 2; nTop = 2, nRight = 2, nBottom = 2;
//    iniData->getBentMargin( nLeft, nTop, nRight, nBottom, nDir );
    bentParam->setMargin( nLeft/100.f, nTop/100.f, nRight/100.f, nBottom/100.f );
    bentParam->setDirection( nDir );

    m_TimerBlinkCursor = startTimer(500);
}

QBentAdjustment::~QBentAdjustment()
{
    if (m_TimerBlinkCursor)
    {
        killTimer(m_TimerBlinkCursor);
        m_TimerBlinkCursor = 0;
    }
    if (m_TimerReCheckPoint)
    {
        killTimer(m_TimerReCheckPoint);
        m_TimerReCheckPoint = 0;
    }
    if (m_TimerDrawWaitTimeout)
    {
        killTimer(m_TimerDrawWaitTimeout);
        m_TimerDrawWaitTimeout = 0;
    }

    m_T3kRequestManager.Stop();

//    LOG_I( "Exit [Bent Adjustment]" );
}

void QBentAdjustment::onChangeLanguage()
{
//    QLangRes& res = QLangManager::getResource();

}

void QBentAdjustment::enterAdjustmentMode()
{
//    LOG_I( "enter adjustment mode" );

    QString strCmdList[] = {
        QString(cstrAreaC) + "?",
        "cam_enable=**",
        QString(sCam1+cstrCamPosTrc) + "?",
        QString(cstrFactoryCalibration) + "?"
    };

    for( int i=0; i<4; i++ )
    {
        int nRetry = 3;
        bool bOK = false;
        QString strCmd( strCmdList[i] );
        do
        {
            if (m_pT3kHandle->SendCommand(strCmd.toUtf8().data(), false))
            {
                bOK = true;
                break;
            }
        } while (--nRetry > 0);

        if (!bOK)
        {
            QMessageBox::critical( m_pTargetWidget, "Error", "Command sending fail. \"" + strCmd.left(strCmd.indexOf('=')) + "\"", QMessageBox::Ok );
            emit finishBentAdjustment();
            return;
        }
    }

    m_pT3kHandle->SetReportObject( true );

//    setCursor(QCursor(Qt::BlankCursor));

    // reset camera enabler



    m_nBentProgress = 0;

    m_BentItemArray.clear();
    m_bEnterAdjustmentMode = true;
    m_bIsValidTouch = false;
    m_nValidTouchCount = 0;
    m_cError = m_cNG = 0;
    m_nAdjustmentStep = 0;

    playBuzzer( m_pT3kHandle, BuzzerEnterCalibration );

    emit updateWidget();

    m_nWaitCountDown = WAIT_TOUCH_TIME;
    if (m_TimerDrawWaitTimeout)
    {
        killTimer(m_TimerDrawWaitTimeout);
    }
    m_TimerDrawWaitTimeout = startTimer( 1000 / WAIT_ANIMATION_FRAME );
}

void QBentAdjustment::leaveAdjustmentMode( bool bSuccess )
{
//    LOG_I( "leave adjustment mode" );

    m_pT3kHandle->SetReportObject( false );

    m_T3kRequestManager.Stop();

    m_nAdjustmentStep = 0;
    m_bIsValidTouch = false;
    m_nValidTouchCount = 0;

    m_bIsTouchOK = false;
    m_nTouchCount = 0;

    m_bEnterAdjustmentMode = false;
    m_bShowCursor = false;
    emit updateWidgetRect( m_rcCursor );

    playBuzzer( m_pT3kHandle, bSuccess ? BuzzerCalibrationSucces : BuzzerCancelCalibration );

    if (bSuccess)
    {
        m_bBentAdjustmentPerformed = true;
    }
    else
    {
        m_BentItemArray.clear();
    }

//    setCursor(QCursor(Qt::ArrowCursor));

    emit updateWidget();

    m_bDrawWaitTimeout = false;
    if (m_TimerDrawWaitTimeout)
    {
        killTimer(m_TimerDrawWaitTimeout);
        m_TimerDrawWaitTimeout = 0;
    }
}

void QBentAdjustment::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerReCheckPoint)
    {
        m_bIsValidTouch = true;
        setInvalidTouch();

        for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
        {
            BentItem& item = m_BentItemArray[i];
            item.fLastTouchPos = item.fLastTouchPosS = item.fLastTouchPosE = NaN;
        }
        killTimer(m_TimerReCheckPoint);
        m_TimerReCheckPoint = 0;
    }
    else if (evt->timerId() == m_TimerBlinkCursor)
    {
        if (m_bEnterAdjustmentMode)
        {
            m_bShowCursor = !m_bShowCursor;
            emit updateWidgetRect( m_rcCursor );
        }
    }
    else if (m_TimerDrawWaitTimeout)
    {
        m_nWaitCountDown--;
        if (m_nWaitCountDown <= WAIT_TOUCH_DRAW_TIME-1)
        {
            int nTime = m_nWaitCountDown / WAIT_ANIMATION_FRAME;
            if (nTime != 0)
            {
                m_bDrawWaitTimeout = true;
            }
        }
        if (m_bDrawWaitTimeout)
            emit updateWidgetRect( m_rcWaitTime );
        if (m_nWaitCountDown/WAIT_ANIMATION_FRAME <= 0)
        {
            if (m_bEnterAdjustmentMode)
            {
                leaveAdjustmentMode(false);
                emit finishBentAdjustment();
            }
        }
    }
    else if(m_nTimerSaveCmd)
    {
        if( m_vSendCmd.size() <= 0 )
        {
            killTimer( m_nTimerSaveCmd );
            m_nTimerSaveCmd = 0;

            emit finishBentAdjustment();
        }
        else
            sendSaveCommand();
    }
}

void QBentAdjustment::draw(QPainter &p, QRect rcBody)
{
    p.save();

    m_rcBody = rcBody;

    p.fillRect( rcBody, Qt::white );

    QPoint ptCursor;

//    if (ui->widgetBentDirMargin->isVisible())
//    {
//        drawCameraLocations( p, rcBody );
//    }

    drawAdjustmentGrid( p, rcBody, &ptCursor );

    if ( !m_bIsTouchLift )
    {
        drawTouchLines( p, rcBody );
    }

#ifdef DEVELOP_CROSSTRACE
    p.setBrush(Qt::NoBrush);
    for ( int ni = m_aryCTs.size() - 1; ni >= 0; ni-- )
    {
        QPen pen(QColor(255,ni*2,ni*2), 1);
        p.setPen(pen);
        QPolygon polyline;
        int np = 0;
        for ( np = 0; np < 6; np++ )
        {
            if ( m_aryCTs[ni].pts[np].x() == -1 && m_aryCTs[ni].pts[np].y() == -1 )
                continue;
            p.drawRect(m_aryCTs[ni].pts[np].x() - 2, m_aryCTs[ni].pts[np].y() - 2, 4, 4);
            polyline.push_back(m_aryCTs[ni].pts[np]);
            break;
        }
        for ( ; np < 6; np++ )
        {
            if ( m_aryCTs[ni].pts[np].x() == -1 && m_aryCTs[ni].pts[np].y() == -1 )
                continue;
            polyline.push_back(m_aryCTs[ni].pts[np]);
        }
        p.drawPolyline(polyline);
    }
#endif //DEVELOP_CROSSTRACE

    int nIn = (int)hypotf( (float)rcBody.width(), (float)rcBody.height() );

    int nC = nIn / 50;
    if( nC < 12 ) nC = 12;

    drawErrorText( p, 10, 80, nIn / 60 );
    drawCursor( p, ptCursor.x(), ptCursor.y(), nC );

    int nWXY = rcBody.height() / 12;
    QRect rcWait( rcBody.center().x()-nWXY, rcBody.center().y()-nWXY, nWXY*2, nWXY*2 );
    m_rcWaitTime = rcWait;
    if ( m_bDrawWaitTimeout )
        drawWaitTime( p, rcWait );

    p.restore();
}

QPoint QBentAdjustment::PosToDCC( float x, float y, const QRect rcClient )
{
    QPointF pt = PosToTPos(x, y);
    return TPosToDCC(pt.x(), pt.y(), rcClient);
}

QPointF QBentAdjustment::PosToTPos( float x, float y, float o/*=1.f*/ )
{
    QBentCfgParam* param = QBentCfgParam::instance();
    float fWidth = 1.f - param->marginLeft() - param->marginRight();
    float fHeight = 1.f - param->marginTop() - param->marginBottom();
    float fCx = (param->marginLeft() + (1.f - param->marginRight())) / 2.f;
    float fCy = (param->marginTop() + (1.f - param->marginBottom())) / 2.f;

    float fScaleW, fScaleH;
    QPointF ptRet;

    int nDirection = param->direction();
    switch ( nDirection )
    {
    default:
    case BENT_DIR_UP:
        fScaleW = fWidth / 4.f;
        fScaleH = fHeight / -2.f;
        ptRet.setX(x * fScaleW + fCx * o);
        ptRet.setY(y * fScaleH + fCy * o);
        break;

    case BENT_DIR_DOWN:
        fScaleW = fWidth / -4.f;
        fScaleH = fHeight / 2.f;
        ptRet.setX(x * fScaleW + fCx * o);
        ptRet.setY(y * fScaleH + fCy * o);
        break;

    case BENT_DIR_LEFT:
        fScaleW = fWidth / -2.f;
        fScaleH = fHeight / -4.f;
        ptRet.setX(y * fScaleW + fCx * o);
        ptRet.setY(x * fScaleH + fCy * o);
        break;

    case BENT_DIR_RIGHT:
        fScaleW = fWidth / 2.f;
        fScaleH = fHeight / 4.f;
        ptRet.setX(y * fScaleW + fCx * o);
        ptRet.setY(x * fScaleH + fCy * o);
        break;
    }

    return ptRet;
}

QPoint QBentAdjustment::TPosToDCC( float x, float y, const QRect rcClient )
{
    return QPoint((long)(x * rcClient.width()) + rcClient.left(), (long)(y * rcClient.height()) + rcClient.top());
}

void QBentAdjustment::drawAdjustmentGrid(QPainter &p, QRect rcBody, QPoint* pPtCursor)
{
    p.setRenderHint(QPainter::Antialiasing);

    //float fx, fy;
    QPoint pt;

    QPen penGd(QColor(200,200,200), 1);
    QPen penGd2(QColor(190,190,190), 5);
    QBrush brushGd(QColor(190,190,190));

    p.setPen(penGd);
    p.setBrush(brushGd);

    int nCW = 3;

    if ( m_bEnterAdjustmentMode )
    {
        p.setPen(penGd2);
    }

    int nPosXYSel = QBentCfgParam::instance()->algorithm();

    //QPainterPath gridPath;
    QPolygon gridPolyline1;
    QPolygon gridPolyline2;
    for ( int i=0; i<ADJUSTMENT_STEP; i++ )
    {
        if ( qIsNaN(s_PosXY[nPosXYSel][i].x) )
            continue;

        pt = PosToDCC( s_PosXY[nPosXYSel][i].x, s_PosXY[nPosXYSel][i].y, rcBody );
        if ( m_bEnterAdjustmentMode )
        {
            if ( i <= m_nAdjustmentStep )
            {
                gridPolyline2.push_back( pt );
            }
        }
        gridPolyline1.push_back( pt );
        if ( m_nAdjustmentStep == i ) *pPtCursor = pt;
    }

    p.setPen( penGd );
    p.drawPolyline( gridPolyline1 );

    if (m_bEnterAdjustmentMode)
    {
        if (gridPolyline2.size() > 1)
        {
            p.setPen( penGd2 );
            p.drawPolyline( gridPolyline2 );
        }
    }

    for ( int i=0; i<ADJUSTMENT_STEP; i++ )
    {
        if ( qIsNaN(s_PosXY[nPosXYSel][i].x) )
            continue;

        pt = PosToDCC( s_PosXY[nPosXYSel][i].x, s_PosXY[nPosXYSel][i].y, rcBody );

        p.drawEllipse( QRect(pt.x()-nCW, pt.y()-nCW, nCW*2+1, nCW*2+1) );
    }

    nCW = 15;

    QPen penOL(QColor(120,120,120), 1);
    QPen penB1(QColor(140,90,0), 2);
    QPen penB2(QColor(200,60,0), 3);
    QPen penB3(QColor(255,0,0), 5);
    for ( int i=0; i<ADJUSTMENT_STEP; i++ )
    {
        pt = PosToDCC( s_PosXY[nPosXYSel][i].x, s_PosXY[nPosXYSel][i].y, rcBody );

        float fPos[4] = { NaN, NaN, NaN, NaN };
        float fDist[4] = { NaN, NaN, NaN, NaN };
        for ( int j=0 ; j<m_BentItemArray.size() ; j++ )
        {
            const BentItem& item = m_BentItemArray.at(j);
            fPos[camIdxToIdx(item.nCameraIndex)] = item.fObcCenter[i];
            fDist[camIdxToIdx(item.nCameraIndex)] = item.fDistortion[s_PosXY[nPosXYSel][i].idx];
        }

        if ( qIsNaN(fPos[0]) && qIsNaN(fPos[1]) && qIsNaN(fPos[2]) && qIsNaN(fPos[3]) )
        {
        }
        else
        {
            p.setPen(penOL);
            QRect rcC = QRect(pt.x()-nCW, pt.y()-nCW, nCW*2+1, nCW*2+1);
            p.drawEllipse( rcC );
            if ( !qIsNaN(fPos[0]) && !qIsNaN(fPos[1]) )
            {
                if ( qIsNaN(fDist[0]) || fDist[0] > 0.01f ||
                     qIsNaN(fDist[1]) || fDist[1] > 0.01f )
                    p.setPen(penB3);
                else if ( fDist[0] > 0.0005f || fDist[1] > 0.0005f )
                    p.setPen(penB2);
                else if ( fDist[0] > 2.e-5f || fDist[1] > 2.e-5f )
                    p.setPen(penB1);
                else
                    p.setPen(penOL);

                p.drawLine( rcC.center().x(), rcC.top(), rcC.center().x(), rcC.bottom() );
            }
            if ( !qIsNaN(fPos[2]) )
            {
                if ( qIsNaN(fDist[2]) || fDist[2] > 0.01f )
                    p.setPen(penB3);
                else if ( fDist[2] > 0.0005f )
                    p.setPen(penB2);
                else if ( fDist[2] > 2.e-5f )
                    p.setPen(penB1);
                else
                    p.setPen(penOL);

                p.drawLine( rcC.left(), rcC.center().y(), rcC.center().x(), rcC.center().y() );
            }
            if ( !qIsNaN(fPos[3]) )
            {
                if ( qIsNaN(fDist[3]) || fDist[3] > 0.01f )
                    p.setPen(penB3);
                else if ( fDist[3] > 0.0005f )
                    p.setPen(penB2);
                else if ( fDist[3] > 2.e-5f )
                    p.setPen(penB1);
                else
                    p.setPen(penOL);

                p.drawLine( rcC.center().x(), rcC.center().y(), rcC.right(), rcC.center().y() );
            }
        }
    }
}

void QBentAdjustment::drawTouchLines( QPainter& p, QRect rcBody )
{
    p.setRenderHint(QPainter::Antialiasing, false);

    QPen pen(QColor(255,0,0), 1);
#if 1
    QBrush brush(QColor(220,220,220));

    p.setBrush(brush);
    p.setPen(Qt::NoPen);

    for ( int i=0 ; i<m_BentItemArray.size(); i++ )
    {
        const BentItem& item = m_BentItemArray.at(i);

        for ( int j=1 ; j<item.nTouchCnt && j<T3K_MAX_DTC_COUNT; j++ )
        {
            //QPoint pts[3];
            //pts[0] = TPosToDCC(item.fCamE, item.fCamF, rcBody);
            //pts[1] = TPosToDCC((item.fTouchPosS[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosS[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);
            //pts[2] = TPosToDCC((item.fTouchPosE[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosE[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);

            m_PointClipper.reset();
            QPoint pt;
            pt = TPosToDCC(item.fCamE, item.fCamF, rcBody);
            m_PointClipper.addPoint(pt);
            pt = TPosToDCC((item.fTouchPosS[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosS[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);
            m_PointClipper.addPoint(pt);
            pt = TPosToDCC((item.fTouchPosE[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosE[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);
            m_PointClipper.addPoint(pt);
            m_PointClipper.closePolygon();

            m_PointClipper.clip(rcBody);

            p.drawPolygon( m_PointClipper.getClippedPoints(), m_PointClipper.getClippedPointCount() );
        }
    }
#endif

    p.setPen(pen);
    for ( int i=0 ; i<m_BentItemArray.size(); i++ )
    {
        const BentItem& item = m_BentItemArray.at(i);

        for ( int j=1 ; j<item.nTouchCnt && j<T3K_MAX_DTC_COUNT; j++ )
        {
            QPoint pt0 = TPosToDCC(item.fCamE, item.fCamF, rcBody);
            float fCam = (item.fTouchPosS[j] + item.fTouchPosE[j]) / 2.f;
            QPoint ptV = TPosToDCC((fCam * item.fCamA + item.fCamC) * 25.f + item.fCamE, (fCam * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);
            m_PointClipper.reset();
            m_PointClipper.addPoint(pt0);
            m_PointClipper.addPoint(ptV);
            m_PointClipper.closePolygon();
            m_PointClipper.clip(rcBody);

            if (m_PointClipper.getClippedPointCount() >= 2)
            {
                p.drawLine(m_PointClipper.getClippedPoint(0), m_PointClipper.getClippedPoint(1));
            }
        }
    }
}

void QBentAdjustment::drawCameraLocations( QPainter& p, QRect rcBody )
{
    p.setRenderHint(QPainter::Antialiasing);

    int nt = rcBody.width() / 42;
    int nth = rcBody.height() / 24;
    int ntx = nt > nth ? nth : nt;
    QFont fnt = qApp->font();
    fnt.setPixelSize(ntx);
    fnt.setWeight(QFont::DemiBold);
    p.setFont(fnt);
    p.setPen(Qt::white);
    p.setBrush(QColor(220,220,120));

    nth = nt;

    for ( int i=0 ; i < m_BentItemArray.size(); i++ )
    {
        const BentItem& item = m_BentItemArray.at(i);
        if ( item.fCamA == 0.f )
            continue;

        int ntw = nt * 9 / 5;

        QString strCM = getCameraText(item.nCameraIndex);
        if (item.nCameraIndex == IDX_CM1_1 || item.nCameraIndex == IDX_CM2_1)
        {
            ntw = nt * 11 / 5;
        }

        QPoint ptCm;
        ptCm.setX(rcBody.left() + (int)(rcBody.width() * item.fCamE));
        ptCm.setY(rcBody.top() + (int)(rcBody.height() * item.fCamF));
        QPoint ptTxt = ptCm;
        if ( ptTxt.x() < rcBody.left() + ntw ) ptTxt.setX(rcBody.left() + ntw);
        if ( ptTxt.x() > rcBody.right() - ntw ) ptTxt.setX(rcBody.right() - ntw);
        if ( ptTxt.y() < rcBody.top() + nth ) ptTxt.setY(rcBody.top() + nth);
        if ( ptTxt.y() > rcBody.bottom() - nth ) ptTxt.setY(rcBody.bottom() - nth);
        QPoint ptD = ptCm - ptTxt;
        int nC;
        if ( ptCm.x() < rcBody.left() )
        {
            nC = ptCm.y() + (rcBody.left() - ptCm.x()) * ptD.y() / ptD.x();
            if ( nC >= rcBody.top() && nC <= rcBody.bottom() )
            {
                ptCm.setX(rcBody.left());
                ptCm.setY(nC);
            }
        }
        else if ( ptCm.x() > rcBody.right() )
        {
            nC = ptCm.y() + (rcBody.right() - ptCm.x()) * ptD.y() / ptD.x();
            if ( nC >= rcBody.top() && nC <= rcBody.bottom() )
            {
                ptCm.setX(rcBody.right());
                ptCm.setY(nC);
            }
        }
        if ( ptCm.y() < rcBody.top() )
        {
            nC = ptCm.x() + (rcBody.top() - ptCm.y()) * ptD.x() / ptD.y();
            if ( nC >= rcBody.left() && nC <= rcBody.right() )
            {
                ptCm.setX(nC);
                ptCm.setY(rcBody.top());
            }
        }
        else if ( ptCm.y() > rcBody.bottom() )
        {
            nC = ptCm.x() + (rcBody.bottom() - ptCm.y()) * ptD.x() / ptD.y();
            if ( nC >= rcBody.left() && nC <= rcBody.right() )
            {
                ptCm.setX(nC);
                ptCm.setY(rcBody.bottom());
            }
        }

        ptD = ptCm - ptTxt;
        float fd = sqrtf((float)(ptD.x() * ptD.x() + ptD.y() * ptD.y())) / ntx * 12.f / 5.f;
        ptD.setX((int)(ptD.x() / fd));
        ptD.setY((int)(ptD.y() / fd));

        QPoint pts[3];
        pts[0] = ptCm;
        pts[1] = ptTxt;
        pts[1].setX(pts[1].x()+ptD.y()); pts[1].setY(pts[1].y()-ptD.x());
        pts[2] = ptTxt;
        pts[2].setX(pts[2].x()-ptD.y()); pts[2].setY(pts[2].y()+ptD.x());
        p.drawPolygon( pts, 3 );

        QRect rcText(pts[1], pts[2]);
        int flags = Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextDontClip|Qt::TextSingleLine;
        //p.drawRect(rcText);
        p.setBackground(QColor(220,220,120));
        p.setBackgroundMode(Qt::OpaqueMode);
        p.drawText(rcText, flags, strCM);
    }
}

void QBentAdjustment::drawCursor( QPainter& p, int nx, int ny, int nc )
{
    p.setRenderHint(QPainter::Antialiasing, false);

    int nPW1 = nc/4;
    int nPW2 = nc/8;
    int nIR = nc/4;

    nPW1 < 2 ? nPW1 = 2 : nPW1 = nPW1;
    nPW1 += nPW1 % 2;
    nPW2 < 1 ? nPW2 = 1 : nPW2 = nPW2;
    nIR < 2 ? nIR = 2 : nIR = nIR;

    p.setBrush(Qt::NoBrush);

    if ( m_bEnterAdjustmentMode )
    {
        if ( m_bIsValidTouch )
        {
            QPen penCross( QColor(255,0,0), nc/8 );
            penCross.setCapStyle(Qt::FlatCap);

            p.setPen( penCross );
            p.drawLine( nx - (nc*3/2+1), ny, nx + (nc*3/2+2), ny );
            p.drawLine( nx, ny - (nc*3/2+1), nx, ny + (nc*3/2+2) );
        }
        else
        {
            QPen penCross( QColor(100,100,100), 1 );
            if ( m_bShowCursor && (m_nBentProgress == 0) )
                penCross.setColor( QColor(255,0,0) );
            penCross.setCapStyle(Qt::FlatCap);

            p.setPen( penCross );
            if ( m_bShowCursor && (m_nBentProgress == 0) )
                p.drawRect( nx-nIR, ny-nIR, nIR*2, nIR*2 );

            p.drawLine( nx - nc, ny, nx + (nc+1), ny );
            p.drawLine( nx, ny - nc, nx, ny + (nc+1) );

            if ( m_nBentProgress != 0 )
            {
                p.setRenderHint(QPainter::Antialiasing);
                QPen penArc( QColor(237, 28, 36, 128), nc/4 );
                penArc.setCapStyle(Qt::FlatCap);
                p.setPen(penArc);
                p.drawArc( nx-nc/2, ny-nc/2, nc, nc, 90*16, -m_nBentProgress*360/100 * 16 ); // Qt 1/16th of a degree
                p.drawArc( nx-nc/2, ny-nc/2, nc, nc, 90*16, m_nBentProgress*360/100 * 16 );
                p.setRenderHint(QPainter::Antialiasing, false);
            }
        }
    }
    m_rcCursor.setLeft(nx-(nc*3/2+1));
    m_rcCursor.setTop(ny-(nc*3/2+1));
    m_rcCursor.setRight(nx+(nc*3/2+1));
    m_rcCursor.setBottom(ny+(nc*3/2+1));

    m_rcCursor.adjust( -nIR, -nIR, nIR, nIR );
}

void QBentAdjustment::drawErrorText( QPainter& p, int nx, int ny, int nc )
{
    if ( m_cError == 0 && m_cNG == 0 )
        return;

    p.setRenderHint(QPainter::Antialiasing);

    QFont fntError = qApp->font();
    fntError.setPixelSize(nc);
    p.setFont(fntError);
    p.setPen( Qt::red );

    QString str;
    if ( (m_cError & FLAG_CAM1) != 0x0 )
        str += " CM1";
    if ( (m_cError & FLAG_CAM2) != 0x0 )
        str += str.isEmpty() ? " CM2" : ", CM2";
    if ( !str.isEmpty() )
    {
        str = "Error:" + str;
        p.drawText(nx, ny, str);
        ny += 40;
    }

    str = "";
    if ( (m_cError & FLAG_CAM1_1) != 0x0 )
        str += " CM1_1";
    if ( (m_cError & FLAG_CAM2_1) != 0x0 )
        str += str.isEmpty() ? " CM2-1" : ", CM2-1";
    if ( !str.isEmpty() )
    {
        str = "Isolated:" + str;
        p.drawText(nx, ny, str);
        ny += 40;
    }

    str = "";
    if ( (m_cNG & FLAG_CAM1) != 0x0 )
        str += " CM1";
    if ( (m_cNG & FLAG_CAM2) != 0x0 )
        str += str.isEmpty() ? " CM2" : ", CM2";
    if ( (m_cNG & FLAG_CAM1_1) != 0x0 )
        str += str.isEmpty() ? " CM1-1" : ", CM1-1";
    if ( (m_cNG & FLAG_CAM2_1) != 0x0 )
        str += str.isEmpty() ? " CM2-1" : ", CM2-1";
    if ( !str.isEmpty() )
    {
        str = "NG:" + str;
        p.drawText(nx, ny, str);
    }
}

void QBentAdjustment::drawWaitTime( QPainter& p, QRect rcWait )
{
    int nTime = m_nWaitCountDown/WAIT_ANIMATION_FRAME;
    if ( nTime == 0 ) return;

    p.setRenderHint(QPainter::Antialiasing);

    int nStep = WAIT_ANIMATION_FRAME - (m_nWaitCountDown-nTime*WAIT_ANIMATION_FRAME);

    int nLineW = rcWait.width()/10;

    int deflate = nLineW/2+1;
    rcWait.adjust( deflate, deflate, -deflate, -deflate );

    QRect rectWait;
    rectWait.setX(rcWait.left()), rectWait.setY(rcWait.top());
    rectWait.setWidth(rcWait.width()), rectWait.setHeight(rcWait.height());

    QPen penCircle(QColor(230, 230, 230), nLineW);
    p.setPen(penCircle);
    p.drawEllipse( rectWait );

    QPen penMovingCircle(QColor(200, 200, 200, 150), nLineW);
    p.setPen(penMovingCircle);
    p.drawArc( rectWait, 90*16 + (-360*nStep/WAIT_ANIMATION_FRAME * 16), 30*16 );  // Qt 1/16th of a degree

    QFont fntWait = qApp->font();
    fntWait.setPixelSize(rectWait.height()*2/3);
    fntWait.setWeight(QFont::Bold);
    p.setFont(fntWait);
    int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;

    rectWait.adjust( 0, rectWait.height()*5/100, 0, rectWait.height()*5/100 );

    QString str = QString::number(nTime);

    p.setPen(QColor(125,125,125));
    p.drawText( rectWait, flags, str );
}

#define PROGSIZE_scanCamFactor  900

bool scanCamFactor_cb( float fProg, void* lpUser )
{
    int & nCalcPosProg = *(int *)lpUser;

    nCalcPosProg = (int)(fProg * PROGSIZE_scanCamFactor);

    //::Sleep(1);

    return true;
}

bool QBentAdjustment::calculateCameraValues( BentItem& item )
{
    float * fObcS = item.fObcS;
    float * fObcE = item.fObcE;

    item.nCalcPosProg = 0;

    if ( qIsNaN(fObcS[0]) || qIsNaN(fObcE[0]) )
    {
        Q_ASSERT(0);
        return false;
    }
    float fsObc0 = (fObcS[0] + fObcE[0]) / 2.f;

    float fInitCamS = 0.f;
    float fInitCamR = 0.f;
    QCalcCamValue ccv;
    QBentCfgParam* param = QBentCfgParam::instance();
    ccv.setNewMethod(param->algorithm() > 0);
    ccv.scanCamFactor(fObcS, fObcE, &fInitCamS, &fInitCamR, scanCamFactor_cb, &item.nCalcPosProg);

    item.nCalcPosProg = PROGSIZE_scanCamFactor;

    float fFinalCamS = fInitCamS;
    float fFinalCamR = fInitCamR;
#if 0 //!!
    QPointF ptsCalc[CALC_PNTCNT_MAX];
    QPointF pt = ccv.findCamSR(fObcS, fObcE, &fFinalCamS, &fFinalCamR, ptsCalc);
    int nCalcPosCnt = 0;
    for ( int ni = 0; ni < CALC_PNTCNT_MAX; ni++ )
    {
        if ( qIsNaN(ptsCalc[ni].x) )
            continue;

        nCalcPosCnt++;
    }
#else
    item.fDistortion[0] = 0.f;
    QPointF pt = ccv.find2CamSR(fObcS, fObcE, &fFinalCamS, &fFinalCamR, item.fDistortion + 1);
    for ( int ni = 0; ni < CALC_PNTCNT_MAX; ni++ )
    {
        if ( item.fDistortionMax < item.fDistortion[ni + 1] )
            item.fDistortionMax = item.fDistortion[ni + 1];
    }
#endif

    float fCamX = item.fCamX = pt.x();
    float fCamY = item.fCamY = pt.y();
    float fCamS = item.fCamS = fFinalCamS;
    float fCamR = item.fCamR = fFinalCamR;

    float fObx0 = fCamS * fsObc0 + 0.5f;
    float fOby0 = (1.f - fObx0) * fCamR;
    float fCmdx = -fCamX;
    float fCmdy = -fCamY;
    float fCamAngle = atan2f(fOby0, fObx0);
    fCamAngle -= atan2f(fCmdy, fCmdx);

    float fCamCos = cosf(fCamAngle);
    float fCamSin = sinf(fCamAngle);
    float fa = fCamS * fCamCos - fCamS * fCamR * fCamSin;
    float fb = -fCamS * fCamSin - fCamS * fCamR * fCamCos;
    float fc = (fCamCos + fCamR * fCamSin) / 2.f;
    float fd = (-fCamSin + fCamR * fCamCos) / 2.f;
    QPointF cp = PosToTPos(fa, fb, 0.f);
    item.fCamA = cp.x();
    item.fCamB = cp.y();
    cp = PosToTPos(fc, fd, 0.f);
    item.fCamC = cp.x();
    item.fCamD = cp.y();
    cp = PosToTPos(fCamX, fCamY, 1.f);
    item.fCamE = cp.x();
    item.fCamF = cp.y();

    item.nCalcPosProg = 1000;

    return (item.fDistortionMax <= 0.01f);
}

QCalcCamPosThread::QCalcCamPosThread(BentItem &item) : m_item(item)
{

}

void QCalcCamPosThread::run()
{
    m_item.bCalcPos = QBentAdjustment::calculateCameraValues(m_item) && !qIsNaN(m_item.fCamX) && !qIsNaN(m_item.fCamY);

    qDebug( "exit thread: %p", m_item.pThread );
}

bool QKeyWatcher::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress)
    {
        m_bStopGUI = !m_bStopGUI;
    }
    return QObject::eventFilter(obj, evt);
}

void QBentAdjustment::onAdjustmentFinish()
{
    m_cError = m_cNG = 0;

    QRect rcBody( m_rcBody );

    emit updateWidget();

#define PROG_SIZE  150
    for ( int i = 0; i < m_BentItemArray.size(); i++ )
    {
        BentItem & item = m_BentItemArray[i];

        int ni;
        float fObcD[ADJUSTMENT_STEP];
        for ( ni = 0; ni < ADJUSTMENT_STEP; ni++ )
        {
            if ( qIsNaN(item.fObcS[ni]) || qIsNaN(item.fObcE[ni]) )
            {
                fObcD[ni] = NaN;
                continue;
            }

            fObcD[ni] = qAbs(item.fObcS[ni] - item.fObcE[ni]);
        }
        float fDn[ADJUSTMENT_STEP];
        float fDnTot = 0.f;
        float nDnTot = 0;
        for ( ni = 0; ni < ADJUSTMENT_STEP; ni++ )
        {
            if ( qIsNaN(fObcD[ni]) )
            {
                fDn[ni] = NaN;
                continue;
            }


            float fTot = 0.f;
            int nTot = 0;
            for ( int nj = 0; nj < ADJUSTMENT_STEP; nj++ )
            {
                if ( nj == ni )
                    continue;
                if ( qIsNaN(fObcD[nj]) )
                    continue;

                fTot += qAbs(fObcD[nj] - fObcD[ni]);
                nTot++;
            }
            fDnTot += (fDn[ni] = fTot / nTot);
            nDnTot++;
        }
        //float fDnAvr = fDnTot / nDnTot;
        for ( ni = 0; ni < ADJUSTMENT_STEP; ni++ )
        {
            if ( qIsNaN(fObcD[ni]) )
            {
                fObcD[ni] = NaN;
                continue;
            }
        }

        item.mode = 0;
        item.nCalcPosDataCnt = 0;
        for ( ni = 0; ni < ADJUSTMENT_STEP; ni++ )
        {
            if ( qIsNaN(item.fObcS[ni]) || qIsNaN(item.fObcE[ni]) )
                continue;

            item.nCalcPosDataCnt++;
        }
        memset(item.fDistortion, 0, sizeof(item.fDistortion));
        item.fDistortionMax = 0.f;
        item.nCalcPosProg = 0;
        item.pCalcPosWnd = new QBentProgressDialog(item, m_pTargetWidget);
        QRect rcProg;
        rcProg.setTop(rcBody.top() + (rcBody.height() - PROG_SIZE) / 2);
        rcProg.setBottom(rcProg.top()+ PROG_SIZE);
        rcProg.setLeft(rcBody.left() + (rcBody.width() - PROG_SIZE * m_BentItemArray.size()) / 2 + PROG_SIZE * camIdxToIdx(item.nCameraIndex));
        rcProg.setRight(rcProg.left() + PROG_SIZE);
        item.pCalcPosWnd->move(rcProg.topLeft());
        item.pCalcPosWnd->resize(rcProg.width(), rcProg.height());
        item.pCalcPosWnd->show();
        item.pThread = new QCalcCamPosThread(item);
        item.pThread->start();
    }

    bool bStopGUI = false;
    int cnt = 0;

    QKeyWatcher keyWatcher(bStopGUI);
    installEventFilter( &keyWatcher );
    do
    {
        cnt = 0;
        for ( int ni = 0; ni < m_BentItemArray.size(); ni++ )
        {
            const BentItem & item = m_BentItemArray.at(ni);
            item.pCalcPosWnd->update();
            if ( item.pThread->isRunning() )
                cnt++;
        }

        QCoreApplication::processEvents();
    }
    while ( cnt != 0 );

    while ( bStopGUI )
    {
        QCoreApplication::processEvents();
    }

    removeEventFilter( &keyWatcher );

    const char* szNaN = "-1.#IND00";
    QString strBaLog;
    for ( int i = 0; i < m_BentItemArray.size(); i++ )
    {
        BentItem & item = m_BentItemArray[i];

        Q_ASSERT( item.pCalcPosWnd );
        delete item.pCalcPosWnd;
        item.pCalcPosWnd = NULL;
        delete item.pThread;
        item.pThread = NULL;

        int nCamNumber = camIdxToIdx(item.nCameraIndex);
        if ( item.bCalcPos )
        {
            if ( item.fDistortionMax > 0.01f )
            {
                if ( nCamNumber > 1 )
                    m_cError |= (FLAG_CAM1_1 << (nCamNumber - 2));
                else
                    m_cError |= (FLAG_CAM1 << nCamNumber);
            }
            else if ( item.fDistortionMax > 0.0005f )
            {
                if ( nCamNumber > 1 )
                    m_cNG |= (FLAG_CAM1_1 << (nCamNumber - 2));
                else
                    m_cNG |= (FLAG_CAM1 << nCamNumber);
            }
        }
        else
        {
            item.fCamA = 0.f;
            item.fCamB = 0.f;
            item.fCamC = 0.f;
            item.fCamD = 0.f;
            item.fCamE = 0.f;
            item.fCamF = 0.f;

            item.fCamX = 0.f;
            item.fCamY = 0.f;
            item.fCamS = 1.f;
            item.fCamR = 1.f;

            if ( nCamNumber > 1 )
                m_cError |= (FLAG_CAM1_1 << (nCamNumber - 2));
            else
                m_cError |= (FLAG_CAM1 << nCamNumber);
        }

        char buf[2048] = {0};
        char * bufC = buf;
        bufC += sprintf( bufC, "@Cam%d:", nCamNumber+1 );
        for ( int ni = 0; ni < ADJUSTMENT_STEP; ni++ )
        {
            if (qIsNaN(item.fObcS[ni]))
                bufC += sprintf(bufC, "%s,", szNaN);
            else
                bufC += sprintf(bufC, "%f,", item.fObcS[ni]);
            if (qIsNaN(item.fObcE[ni]))
                bufC += sprintf(bufC, "%s|", szNaN);
            else
                bufC += sprintf(bufC, "%f|", item.fObcE[ni]);
            //bufC += sprintf( bufC, "%f,%f|", item.fObcS[ni], item.fObcE[ni]);
        }
        if (qIsNaN(item.fCamX))
            bufC += sprintf(bufC, "%s,", szNaN);
        else
            bufC += sprintf(bufC, "%f,", item.fCamX);
        if (qIsNaN(item.fCamY))
            bufC += sprintf(bufC, "%s,", szNaN);
        else
            bufC += sprintf(bufC, "%f,", item.fCamY);
        if (qIsNaN(item.fCamS))
            bufC += sprintf(bufC, "%s,", szNaN);
        else
            bufC += sprintf(bufC, "%f,", item.fCamS);
        if (qIsNaN(item.fCamR))
            bufC += sprintf(bufC, "%s|", szNaN);
        else
            bufC += sprintf(bufC, "%f|", item.fCamR);

        //bufC += sprintf(bufC, "%f,%f,%f,%f|", item.fCamX, item.fCamY, item.fCamS, item.fCamR);
//        LOG_I(buf);

        QDateTime curDate = QDateTime::currentDateTime();
        if ( !strBaLog.isEmpty() )
            strBaLog += "\r\n";
        strBaLog += curDate.toString("yyyy-MM-dd hh-mm-ss ") + buf;
    }

//#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
//    QString strPath = QCoreApplication::applicationDirPath();
//    strPath = rstrip(strPath, "/\\");
//    strPath += QDir::separator();
//    strPath += "bent_adjustment.txt";
//#else
//    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//    strDocuments = rstrip( strDocuments, "/\\" );
//    QString strPath = strDocuments + QDir::separator() + "T3kCfgFE" + QDir::separator();
//    makeDirectory(strPath);
//    strPath += "bent_adjustment.txt";
//#endif

//    QFile file(strPath);
//    if (file.open(QIODevice::WriteOnly))
//    {
//        file.write( strBaLog.toLatin1() );
//        file.close();
//    }

    leaveAdjustmentMode(true);

    if( m_bBentAdjustmentPerformed )
    {
        if( m_nTimerSaveCmd )
        {
            killTimer( m_nTimerSaveCmd );
            m_nTimerSaveCmd = 0;
        }
        QBentCfgParam* param = QBentCfgParam::instance();
        const int nPosXYSel = param->algorithm();
        char szTemp[256];

        snprintf( szTemp, 256, "0x%02x", m_nMonitorOrientation );
        m_vSendCmd.push_back( QString("%1%2").arg(cstrDisplayOrientation).arg(szTemp) );
        m_vSendCmd.push_back( sCam1 + cstrFactorialCamPos + "**" );
        m_vSendCmd.push_back( sCam2 + cstrFactorialCamPos + "**" );
        if( QT3kUserData::GetInstance()->isSubCameraExist() )
        {
            m_vSendCmd.push_back( sCam1_1 + cstrFactorialCamPos + "**" );
            m_vSendCmd.push_back( sCam2_1 + cstrFactorialCamPos + "**" );
        }

        QString strCamPos;
        for ( int nI=0 ; nI<m_BentItemArray.size() ; nI++ )
        {
            const BentItem& item = m_BentItemArray.at(nI);
            unsigned long dwA, dwB, dwC, dwD, dwE, dwF;

            memcpy( &dwA, &item.fCamA, sizeof(float) );
            memcpy( &dwB, &item.fCamB, sizeof(float) );
            memcpy( &dwC, &item.fCamC, sizeof(float) );
            memcpy( &dwD, &item.fCamD, sizeof(float) );
            memcpy( &dwE, &item.fCamE, sizeof(float) );
            memcpy( &dwF, &item.fCamF, sizeof(float) );

            snprintf( szTemp, 256, "%08lx,%08lx,%08lx,%08lx,%08lx,%08lx,0x%02x", dwA, dwB, dwC, dwD, dwE, dwF, item.mode );
            m_vSendCmd.push_back( getCameraPrefix(item.nCameraIndex) + cstrFactorialCamPos + szTemp );

            strCamPos.clear();
            unsigned char ch = param->direction() << 6; // direction
            if ( item.nCameraIndex > 1 ) // error
            {
                if ( (m_cError & (FLAG_CAM1_1 << (item.nCameraIndex - 2))) != 0x0 )
                    ch |= 0x20;
                if ( (m_cNG & (FLAG_CAM1_1 << (item.nCameraIndex - 2))) != 0x0 )
                    ch |= 0x10;
            }
            else
            {
                if ( (m_cError & (FLAG_CAM1 << item.nCameraIndex)) != 0x0 )
                    ch |= 0x20;
                if ( (m_cNG & (FLAG_CAM1 << item.nCameraIndex)) != 0x0 )
                    ch |= 0x10;
            }
            ch |= nPosXYSel; // type
            strCamPos += QString("%1").arg(ch, 2, 16, QChar('0')); // direction:2 error:2 type:4
            for ( int ni = 0; ni < ADJUSTMENT_STEP; ni++ ) // bent_adjustment trc
            {
                ulong dwOs = *(uint*)(&item.fObcS[s_PosXY[nPosXYSel][ni].idx]);
                ulong dwOe = *(uint*)(&item.fObcE[s_PosXY[nPosXYSel][ni].idx]);
                strCamPos += QString("%1%2").arg(dwOs, 8, 16, QChar('0')).arg(dwOe, 8, 16, QChar('0'));
            }
            m_vSendCmd.push_back( getCameraPrefix(item.nCameraIndex) + cstrCamPosUserTrc +
                                  QString("%1%2").arg(strCamPos.length()/2, 2, 16, QChar('0')).arg(strCamPos) +
                                  QString("%1%2%3%4%5%6%7")  // f31 backup
                                  .arg(dwA, 8, 16, QChar('0')).arg(dwB, 8, 16, QChar('0')).arg(dwC, 8, 16, QChar('0')).arg(dwD, 8, 16, QChar('0'))
                                  .arg(dwE, 8, 16, QChar('0')).arg(dwF, 8, 16, QChar('0')).arg(item.mode, 2, 16, QChar('0')) );
        }

        // reset calibration
        m_vSendCmd.push_back( QString("%1**").arg(cstrCalibration) );

        // f42!
        QString strMargin;
        snprintf( szTemp, 256, "%02x%02x%02x%02x",
                  (unsigned char)(param->marginLeft() * 100 + .5f),
                  (unsigned char)(param->marginTop() * 100 + .5f),
                  (unsigned char)(param->marginRight() * 100 + .5f),
                  (unsigned char)(param->marginBottom() * 100 + .5f) );
        strMargin = szTemp;

        int nCam1Idx = -1, nCam2Idx = -1;
        for ( int nI=0 ; nI<m_BentItemArray.size() ; nI++ )
        {
            const BentItem& item = m_BentItemArray.at(nI);
            if ( item.nCameraIndex == IDX_CM1 )
            {
                nCam1Idx = nI;
            }
            else if ( item.nCameraIndex == IDX_CM2 )
            {
                nCam2Idx = nI;
            }
        }

        QString strCalPos;
        if ( nCam1Idx >= 0 && nCam2Idx >= 0 )
        {
            const BentItem& item1 = m_BentItemArray.at(nCam1Idx);
            const BentItem& item2 = m_BentItemArray.at(nCam2Idx);
            for ( int i=0 ; i<ADJUSTMENT_STEP ; i++ )
            {
                float fV = item1.fObcCenter[i];
                if ( (nPosXYSel == 1 || nPosXYSel == 2) &&
                     (i == 1 || i == 4 || i == 8 || i == 11) )
                {
                    fV = NaN;
                }
                int *x = (int*)&fV;
                snprintf( szTemp, 256, "%08x", (unsigned int)*x );
                strCalPos += szTemp;
            }

            for ( int i=0 ; i<ADJUSTMENT_STEP ; i++ )
            {
                float fV = item2.fObcCenter[i];
                if ( (nPosXYSel == 1 || nPosXYSel == 2) &&
                     (i == 1 || i == 4 || i == 8 || i == 11) )
                {
                    fV = NaN;
                }
                int *x = (int*)&fV;
                snprintf( szTemp, 256, "%08x", (unsigned int)*x );
                strCalPos += szTemp;
            }
        }

        if ( !strMargin.isEmpty() && !strCalPos.isEmpty() )
            m_vSendCmd.push_back( QString("%1%2").arg(cstrFactoryCalibration).arg(strMargin+strCalPos) );


        sendSaveCommand();
        m_nTimerSaveCmd = startTimer( 1000 );
    }
}

void QBentAdjustment::checkTouchPoints( bool bTouch )
{
    if (!m_bEnterAdjustmentMode)
        return;

    if (bTouch && !m_bOldTouchState)
    {
        // reset old data
        qDebug( "Reset!" );
        setInvalidTouch();
        m_bOldTouchState = bTouch;
        return;
    }
    m_bOldTouchState = bTouch;

    int nPosXYSel = QBentCfgParam::instance()->algorithm();

    if (bTouch && !m_bIsTouchOK)
    {
        for (int i=0; i<m_BentItemArray.size(); i++)
        {
            BentItem& item = m_BentItemArray[i];
            if (item.nAveCount == 0)
            {
                if (qIsNaN(item.fLastTouchPos))
                    continue;
                item.lPrevTouch = (long)( (item.fLastTouchPosS+item.fLastTouchPosE) / 2.f * 0x7fff );
            }
            else
            {
                long lCurTouch = (long)( (item.fLastTouchPosS+item.fLastTouchPosE) / 2.f * 0x7fff );
                long lDiff = qAbs( lCurTouch - item.lPrevTouch );
                item.lAveDiff += lDiff;
            }
            item.nAveCount ++;
            //qDebug() << QString("AveCoutn %1 : %2").arg(i).arg(item.nAveCount);
        }
        m_nTouchCount ++;
        int nProgress = m_nTouchCount * 100 / m_nMaxTouchCount;
        if (m_nBentProgress != nProgress)
        {
            m_nBentProgress = nProgress;
            if ( (nProgress % 2) == 0 )
                emit updateWidgetRect( m_rcCursor );
        }
        if (m_nTouchCount >= m_nMaxTouchCount)
        {
            if (!checkValidTouch())
            {
                m_bIsValidTouch = true;
                setInvalidTouch();
                return;
            }
            m_bIsTouchOK = true;

//            LOG_I( "TOUCH OK" );
            qDebug( "TOUCH OK" );

            m_TimerReCheckPoint = startTimer(1500);
            playBuzzer( m_pT3kHandle, BuzzerClick );

            m_bIsValidTouch = true;
            m_nValidTouchCount ++;
            emit updateWidgetRect(m_rcCursor);

            for (int i=0; i<m_BentItemArray.size(); i++)
            {
                BentItem& item = m_BentItemArray[i];
                if (item.bDataValid)
                {
                    item.fObcCenter[m_nAdjustmentStep] = (item.fLastTouchPosS+item.fLastTouchPosE) / 2.f;
                    item.fObcS[s_PosXY[nPosXYSel][m_nAdjustmentStep].idx] = item.fLastTouchPosS;
                    item.fObcE[s_PosXY[nPosXYSel][m_nAdjustmentStep].idx] = item.fLastTouchPosE;

                    qDebug( "[%d] %.2f, %.2f", i, item.fLastTouchPosS, item.fLastTouchPosE );
//                    LOG_I( "[%d] %.2f, %.2f", i, item.fLastTouchPosS, item.fLastTouchPosE );
                }

                item.nAveCount = 0;
                item.lAveDiff = 0;
                item.lPrevTouch = 0;
                item.bDataValid = false;
            }
        }
    }

    if (!bTouch)
    {
        if (m_bIsTouchOK)
        {
            qDebug( "TOUCH LIFT" );
//            LOG_I( "TOUCH LIFT" );

            if (m_TimerReCheckPoint)
            {
                killTimer(m_TimerReCheckPoint);
                m_TimerReCheckPoint = 0;
            }

            do
            {
                m_nAdjustmentStep ++;
                if (!qIsNaN(s_PosXY[nPosXYSel][m_nAdjustmentStep].x) &&
                    !qIsNaN(s_PosXY[nPosXYSel][m_nAdjustmentStep].y) )
                    break;
            } while (m_nAdjustmentStep < ADJUSTMENT_STEP);

            m_bIsValidTouch = true;
            setInvalidTouch();

            if (m_nAdjustmentStep == ADJUSTMENT_STEP)
            {
                onAdjustmentFinish();
                m_nAdjustmentStep = 0;
            }
            else
            {
                playBuzzer( m_pT3kHandle, BuzzerNextPoint );
                m_bIsValidTouch = false;
                m_nValidTouchCount = 0;
                emit updateWidget();

                for (int i=0; i<m_BentItemArray.size(); i++)
                {
                    BentItem& item = m_BentItemArray[i];
                    item.fLastTouchPos = item.fLastTouchPosS = item.fLastTouchPosE = NaN;
                }
            }
        }
        else
        {
            setInvalidTouch();
        }
    }
}

void QBentAdjustment::setInvalidTouch()
{
    m_bIsTouchOK = false;
    m_nTouchCount = 0;
    if (m_nBentProgress != 0)
        m_nBentProgress = 0;

    if (!m_bIsValidTouch)
        return;

    m_bIsValidTouch = false;
    emit updateWidgetRect( m_rcCursor );

    for (int i=0; i<m_BentItemArray.size(); i++)
    {
        BentItem& item = m_BentItemArray[i];
        item.nAveCount = 0;
        item.lAveDiff = 0;
        item.lPrevTouch = 0;
        item.bDataValid = false;

        if (m_nAdjustmentStep < ADJUSTMENT_STEP)
        {
            item.fObcCenter[m_nAdjustmentStep] = NaN;
        }
    }
}

bool QBentAdjustment::checkValidTouch()
{
    bool bInvalidTouch = false;
    int nCamCheck = 0;
    for (int i=0; i<m_BentItemArray.size(); i++)
    {
        BentItem& item = m_BentItemArray[i];

        if ( item.bDataValid &&
             ((item.nCameraIndex == IDX_CM1) || (item.nCameraIndex == IDX_CM2)) )
            nCamCheck ++;
        if (item.nAveCount != 0)
        {
            item.lAveDiff /= item.nAveCount;
            item.nAveCount = 0;
        }
        if ((item.lAveDiff < 0) || (item.lAveDiff > m_lClickArea))
        {
            qDebug( "Invalid Area: %ld / %ld", item.lAveDiff, m_lClickArea );
            bInvalidTouch = true;
            break;
        }
    }
    if (nCamCheck != 2)
        qDebug( "Invalid Main Camera Data" );
    if (bInvalidTouch)
        qDebug( "Invalid Touch Position" );
    if ((nCamCheck != 2) || bInvalidTouch)
        return false;
    return true;
}

void QBentAdjustment::sendSaveCommand()
{
    QMutexLocker Lock( &m_Mutex );
    m_vSendCmdID.clear();
    for( int i=0; i<m_vSendCmd.size(); i++ )
    {
        QString strCmd( m_vSendCmd.at(i) );
        int nSendCmdID = m_pT3kHandle->SendCommand( strCmd.toUtf8().data(), true );
        m_vSendCmdID.push_back( nSendCmdID );
    }
}

void QBentAdjustment::OnOBJ(ResponsePart Part, ushort, const char *, int, T3kRangeF *pOBJ, unsigned short cnt)
{
    if ( Part == MM ) return;
    int nCamIndex = (int)Part;
    int nCam = ((int)Part)-1;

#ifdef DEVELOP_CROSSTRACE
    switch ( nCam )
    {
    case 2:
    case 3:
        if ( (m_ucObj & 0x03) == 0x03 )
            m_ucObj = 0x0;
        break;
    case 0:
        m_ucObj |= 0x01;
        break;
    case 1:
        m_ucObj |= 0x02;
        break;
    }
    if ( (m_ucObj & 0x03) == 0x03 )
    {
        BentItem * item1 = NULL;
        BentItem * item2 = NULL;
        BentItem * item3 = NULL;
        BentItem * item4 = NULL;
        for ( int i=0 ; i<m_BentItemArray.size(); i++ )
        {
            BentItem & item = m_BentItemArray[i];
            switch ( item.nCamNumber )
            {
            case 0: if ( item.nTouchCnt > 0 ) item1 = &item; break;
            case 1: if ( item.nTouchCnt > 0 ) item2 = &item; break;
            case 2: if ( item.nTouchCnt > 0 ) item3 = &item; break;
            case 3: if ( item.nTouchCnt > 0 ) item4 = &item; break;
            }
        }

        bool bNew = false;

        QRect rcBody(0, 0, width(), height());
        CT ct;
        memset(&ct, -1, sizeof(CT));
        float f;
        if ( item1 )
        {
            f = (item1->fTouchPosS[0] + item1->fTouchPosE[0]) / 2.f;
            float fdx1 = f * item1->fCamA + item1->fCamC;
            float fdy1 = f * item1->fCamB + item1->fCamD;
            if ( item2 )
            {
                f = (item2->fTouchPosS[0] + item2->fTouchPosE[0]) / 2.f;
                float fdx2 = f * item2->fCamA + item2->fCamC;
                float fdy2 = f * item2->fCamB + item2->fCamD;
                ct.pts[0] = CalcCrossPoint(item1->fCamE, fdx1, item1->fCamF, fdy1,
                                           item2->fCamE, fdx2, item2->fCamF, fdy2,
                                           rcBody);
                bNew = true;
            }
            if ( item3 )
            {
                f = (item3->fTouchPosS[0] + item3->fTouchPosE[0]) / 2.f;
                float fdx3 = f * item3->fCamA + item3->fCamC;
                float fdy3 = f * item3->fCamB + item3->fCamD;
                ct.pts[1] = CalcCrossPoint(item1->fCamE, fdx1, item1->fCamF, fdy1,
                                           item3->fCamE, fdx3, item3->fCamF, fdy3,
                                           rcBody);
                bNew = true;
            }
            if ( item4 )
            {
                f = (item4->fTouchPosS[0] + item4->fTouchPosE[0]) / 2.f;
                float fdx4 = f * item4->fCamA + item4->fCamC;
                float fdy4 = f * item4->fCamB + item4->fCamD;
                ct.pts[2] = CalcCrossPoint(item1->fCamE, fdx1, item1->fCamF, fdy1,
                                           item4->fCamE, fdx4, item4->fCamF, fdy4,
                                           rcBody);
                bNew = true;
            }
        }
        if ( item2 )
        {
            f = (item2->fTouchPosS[0] + item2->fTouchPosE[0]) / 2.f;
            float fdx2 = f * item2->fCamA + item2->fCamC;
            float fdy2 = f * item2->fCamB + item2->fCamD;
            if ( item3 )
            {
                f = (item3->fTouchPosS[0] + item3->fTouchPosE[0]) / 2.f;
                float fdx3 = f * item3->fCamA + item3->fCamC;
                float fdy3 = f * item3->fCamB + item3->fCamD;
                ct.pts[3] = CalcCrossPoint(item2->fCamE, fdx2, item2->fCamF, fdy2,
                                           item3->fCamE, fdx3, item3->fCamF, fdy3,
                                           rcBody);
                bNew = true;
            }
            if ( item4 )
            {
                f = (item4->fTouchPosS[0] + item4->fTouchPosE[0]) / 2.f;
                float fdx4 = f * item4->fCamA + item4->fCamC;
                float fdy4 = f * item4->fCamB + item4->fCamD;
                ct.pts[4] = CalcCrossPoint(item2->fCamE, fdx2, item2->fCamF, fdy2,
                                           item4->fCamE, fdx4, item4->fCamF, fdy4,
                                           rcBody);
                bNew = true;
            }
        }
        if ( item3 )
        {
            f = (item3->fTouchPosS[0] + item3->fTouchPosE[0]) / 2.f;
            float fdx3 = f * item3->fCamA + item3->fCamC;
            float fdy3 = f * item3->fCamB + item3->fCamD;
            if ( item4 )
            {
                f = (item4->fTouchPosS[0] + item4->fTouchPosE[0]) / 2.f;
                float fdx4 = f * item4->fCamA + item4->fCamC;
                float fdy4 = f * item4->fCamB + item4->fCamD;
                ct.pts[5] = CalcCrossPoint(item3->fCamE, fdx3, item3->fCamF, fdy3,
                                           item4->fCamE, fdx4, item4->fCamF, fdy4,
                                           rcBody);
                bNew = true;
            }
        }

        if ( bNew )
            m_aryCTs.InsertAt(0, ct);
        else if ( m_aryCTs.GetSize() > 0 )
        {
//!!			m_aryCTs.RemoveAt(m_aryCTs.GetSize() - 1);
        }
#define CT_TAILSIZE  120
        if ( m_aryCTs.size() > CT_TAILSIZE )
        {
            m_aryCTs.RemoveAt(CT_TAILSIZE, m_aryCTs.GetSize() - CT_TAILSIZE);
        }
    }
#endif //DEVELOP_CROSSTRACE

    if ( m_bEnterAdjustmentMode )
    {
        if ( cnt == 2 )
        {
            m_nWaitCountDown = WAIT_TOUCH_TIME;
            if ( m_bDrawWaitTimeout )
            {
                emit updateWidgetRect(m_rcWaitTime);
                m_bDrawWaitTimeout = false;
            }

            bool bNewOBC = true;
            for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
            {
                BentItem& item = m_BentItemArray[i];
                if( item.nCameraIndex == nCamIndex )
                {
                    bNewOBC = false;

                    item.bDataValid = true;
                    item.nTouchCnt = cnt;
                    item.fLastTouchPosS = pOBJ[0].Start;
                    item.fLastTouchPosE = pOBJ[0].End;
                    item.fLastTouchPos = (pOBJ[0].Start + pOBJ[0].End) / 2;
                    break;
                }
            }

            if ( bNewOBC )
            {
                BentItem item;
                memset( &item, 0, sizeof(BentItem) );
                item.nCameraIndex = nCamIndex;
                item.bDataValid = false;
                item.nTouchCnt = cnt;
                item.fLastTouchPosS = pOBJ[0].Start;
                item.fLastTouchPosE = pOBJ[0].End;
                item.fLastTouchPos = (pOBJ[0].Start + pOBJ[0].Start) / 2;

                for ( int i=0 ; i<ADJUSTMENT_STEP; i++ )
                {
                    item.fObcS[i] = item.fObcE[i] = NaN;
                    item.fObcCenter[i] = NaN;
                }

                m_BentItemArray.push_back(item);
            }
        }

        bool bLift = false;
        if ( cnt == 0 )
        {
            if ( m_BentItemArray.size() > 0 )
            {
                for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
                {
                    BentItem& item = m_BentItemArray[i];
                    if( item.nCameraIndex == nCamIndex )
                    {
                        item.nTouchCnt = 0;
                        break;
                    }
                }

                int nLiftCount = 0;
                for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
                {
                    const BentItem& item = m_BentItemArray.at(i);
                    if ( item.nTouchCnt == 0 )
                        nLiftCount ++;
                }

                if ( nLiftCount >= m_BentItemArray.size() )
                {
                    bLift = true;
                }
            }
        }

        if ( nCam == 0 || nCam == 1 )
            m_bCheckCamTouch[nCam] = ((cnt == 2) ? true : false);

        if ( m_bIsTouchOK )
            checkTouchPoints( !bLift );
        else
        {
            checkTouchPoints( m_bCheckCamTouch[0] && m_bCheckCamTouch[1] );
        }
    }
    else
    {
        if ( cnt > 0 )
        {
            if ( m_bIsTouchLift )
            {
                m_bIsTouchLift = false;
            }

            bool bNewOBC = true;
            for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
            {
                BentItem& item = m_BentItemArray[i];
                if( item.nCameraIndex == nCamIndex )
                {
                    bNewOBC = false;

                    for ( int j=0 ; j<cnt ; j++ )
                    {
                        item.fTouchPosS[j] = pOBJ[j].Start;
                        item.fTouchPosE[j] = pOBJ[j].End;
                    }
                    item.nTouchCnt = cnt;
                    break;
                }
            }

            if ( bNewOBC && nCamIndex >= IDX_CM1 && nCamIndex <= IDX_CM2_1 )
            {
                BentItem item;
                memset( &item, 0, sizeof(BentItem) );
                item.nCameraIndex = nCamIndex;

                for ( int j=0 ; j<cnt ; j++ )
                {
                    item.fTouchPosS[j] = pOBJ[j].Start;
                    item.fTouchPosE[j] = pOBJ[j].End;
                }
                item.nTouchCnt = cnt;

                item.fLastTouchPosS = item.fLastTouchPosE = item.fLastTouchPos = NaN;
                for( int i=0 ; i<ADJUSTMENT_STEP; i++ )
                {
                    item.fObcS[i] = item.fObcE[i] = NaN;
                    item.fObcCenter[i] = NaN;
                }

                m_BentItemArray.push_back(item);
            }

            emit updateWidget();
        }
        else
        {
            if ( !m_bIsTouchLift && m_BentItemArray.size() > 0 )
            {
                for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
                {
                    BentItem& item = m_BentItemArray[i];
                    if( item.nCameraIndex == nCamIndex )
                    {
                        item.nTouchCnt = 0;
                        break;
                    }
                }

                int nLiftCount = 0;
                for ( int i=0 ; i<m_BentItemArray.size() ; i++ )
                {
                    const BentItem& item = m_BentItemArray.at(i);
                    if ( item.nTouchCnt == 0 )
                        nLiftCount ++;
                }

                if ( nLiftCount >= m_BentItemArray.size() )
                {
                    m_bIsTouchLift = true;
                    emit updateWidget();
                }
            }
        }
    }
}

void QBentAdjustment::OnRSP(ResponsePart, ushort, const char *, long lID, bool, const char *szCmd)
{
    if( !m_pTargetWidget->isVisible() ) return;

    if ( strstr(szCmd, cstrAreaC) == szCmd )
    {
        m_T3kRequestManager.RemoveItem( cstrAreaC );
        const char * pCur = szCmd + sizeof(cstrAreaC) - 1;

        int nAreaC = atoi(pCur);
        m_lClickArea = nAreaC * 2;

        if ( m_lClickArea < 300 )
        {
            m_lClickArea = 300;
        }
    }
    else if( strstr(szCmd, cstrCamPosTrc) == szCmd )
    {
        QString str(szCmd);
        str = str.mid( str.indexOf('=')+1 );
        int nV = str.mid( 2, 2 ).toInt(0, 16);
        int nDir = nV >> 6;
        int nAlgorithm = nV & 0x0F;
        QBentCfgParam* bentParam = QBentCfgParam::instance();

//        if( QT3kUserData::GetInstance()->isSubCameraExist() )
//            nAlgorithm = 1; // get
//        else
//        {
//            if( QT3kUserData::GetInstance()->getFirmwareVersionStr().compare( "2.8" ) <= 0 )
//                nAlgorithm = 1; // get
//            else
//                nAlgorithm = 2; // get
//        }
        bentParam->setAlgorithm( nAlgorithm );
        bentParam->setDirection( nDir );
    }
    else if( m_nTimerSaveCmd == 0 && strstr(szCmd, cstrFactoryCalibration) == szCmd )
    {
        QString str(szCmd);
        str = str.left( str.indexOf(' ') ).mid( str.indexOf('=')+1 );

        QBentCfgParam* bentParam = QBentCfgParam::instance();
        int nLeft, nTop, nRight, nBottom;
        nLeft = str.left(2).toInt(0, 16); str = str.mid( 2 );
        nTop = str.left(2).toInt(0, 16); str = str.mid( 2 );
        nRight = str.left(2).toInt(0, 16); str = str.mid( 2 );
        nBottom = str.left(2).toInt(0, 16); str = str.mid( 2 );
        bentParam->setMargin( nLeft/100.f, nTop/100.f, nRight/100.f, nBottom/100.f );
    }
    else
    {
        QMutexLocker Lock( &m_Mutex );
        for( int i=0; i<m_vSendCmdID.size(); i++ )
        {
            if( m_vSendCmdID.at(i) == lID )
            {
                m_vSendCmd.remove(i);
                m_vSendCmdID.remove(i);
                break;
            }
        }
    }
}

void QBentAdjustment::OnRSE(ResponsePart, ushort, const char *, long lID, bool, const char *)
{
    QMutexLocker Lock( &m_Mutex );
    for( int i=0; i<m_vSendCmdID.size(); i++ )
    {
        if( m_vSendCmdID.at(i) == lID )
        {
            m_vSendCmd.remove(i);
            m_vSendCmdID.remove(i);
            break;
        }
    }
}

void QBentAdjustment::onFinishRequestCommand()
{
    enterAdjustmentMode();
}
