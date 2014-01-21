#include "QAutoDetectionRange.h"

//#include "QShowMessageBox.h"
//#include "QLogSystem.h"

#include "QUtils.h"
#include "T3kConstStr.h"

//#include "QSensorInitDataCfg.h"

//#include "QMyApplication.h"
#include "QCustomDefaultSensor.h"
#include "QT3kUserData.h"
#include "T3kBuzzerDef.h"
#include "CfgCustomCmdDef.h"

#include <QCoreApplication>
#include <QPainter>
#include <QTimerEvent>
#include <QDir>

#define MAX_TICK_COUNT      (400)


QAutoDetectionRange::QAutoDetectionRange(QObject *parent) :
    QObject(parent)
{
    m_pT3kHandle = QT3kUserData::GetInstance()->getT3kHandle();
    m_bEnterAutoRangeSetting = false;
    m_nCamTouchCount[0] = m_nCamTouchCount[1] = 0;
    m_nCamTouchMax[0] = m_nCamTouchMax[1] = 0;
    m_bTouchOK = false;
    m_bCamTouch = false;
    m_dwTickTouch = 0;

    m_bToggleArrow = false;
    m_nTouchProgress = 0;

    m_TimerBlinkArrow = 0;

    m_bChekcFinish = false;

    connect( &m_RequestHIDManager, &QRequestHIDManager::finish, this, &QAutoDetectionRange::onRequestFinish );

    QString strConfigFolderPath( QCoreApplication::applicationDirPath() + "/config/" );
    if( QDir(strConfigFolderPath).exists() &&
            QFile::exists( strConfigFolderPath + "topleft.png" ) &&
            QFile::exists( strConfigFolderPath + "topright.png" ) &&
            QFile::exists( strConfigFolderPath + "bottomright.png" ) &&
            QFile::exists( strConfigFolderPath + "bottomleft.png" ) )
    {
        m_pmDirection[0] = QPixmap( strConfigFolderPath + "topleft.png" );
        m_pmDirection[1] = QPixmap( strConfigFolderPath + "topright.png" );
        m_pmDirection[2] = QPixmap( strConfigFolderPath + "bottomright.png" );
        m_pmDirection[3] = QPixmap( strConfigFolderPath + "bottomleft.png" );
    }
    else
    {
        m_pmDirection[0] = QPixmap( ":/T3kCfgRes/resources/PNG_DETECTION_LT.png" );
        m_pmDirection[1] = QPixmap( ":/T3kCfgRes/resources/PNG_DETECTION_RT.png" );
        m_pmDirection[2] = QPixmap( ":/T3kCfgRes/resources/PNG_DETECTION_RB.png" );
        m_pmDirection[3] = QPixmap( ":/T3kCfgRes/resources/PNG_DETECTION_LB.png" );
    }
//    onChangeLanguage();
}

QAutoDetectionRange::~QAutoDetectionRange()
{
}

void QAutoDetectionRange::onChangeLanguage()
{
//    QLangRes& res = QLangManager::getResource();


}

#define ARROW_OFFSETXY  (4)
void QAutoDetectionRange::draw(QPainter &p, QRect rcBody)
{
    p.fillRect( rcBody, Qt::white );

    const QRect rcArrow(0, 0, 55, 55);
    // LT, RT, RB, LB
    m_rcArrow[0] = rcArrow;
    m_rcArrow[1] = QRect(rcBody.right()-rcArrow.width(), rcBody.top(), rcArrow.width(), rcArrow.height());
    m_rcArrow[2] = QRect(rcBody.right()-rcArrow.width(), rcBody.bottom()-rcArrow.height(), rcArrow.width(), rcArrow.height());
    m_rcArrow[3] = QRect(rcBody.left(), rcBody.bottom()-rcArrow.height(), rcArrow.width(), rcArrow.height());

    m_rcProgress[0] = QRect(m_rcArrow[0].left() + ARROW_OFFSETXY, m_rcArrow[0].bottom() + 2, m_rcArrow[0].width()-ARROW_OFFSETXY*2, 2);
    m_rcProgress[1] = QRect(m_rcArrow[1].left() + ARROW_OFFSETXY, m_rcArrow[1].bottom() + 2, m_rcArrow[1].width()-ARROW_OFFSETXY*2, 2);
    m_rcProgress[2] = QRect(m_rcArrow[2].left() + ARROW_OFFSETXY, m_rcArrow[2].top() - 2 - 2, m_rcArrow[2].width()-ARROW_OFFSETXY*2, 2);
    m_rcProgress[3] = QRect(m_rcArrow[3].left() + ARROW_OFFSETXY, m_rcArrow[3].top() - 2 - 2, m_rcArrow[3].width()-ARROW_OFFSETXY*2, 2);

    drawMonitor(p, rcBody);

    drawArrow(p);
}

void QAutoDetectionRange::drawMonitor(QPainter &p, QRect rcBody)
{
    if (!m_bEnterAutoRangeSetting)
        return;

    p.save();

    QRect rcTouchArea( rcBody.center().x()-m_pmDirection[m_nAutoRangeStep].width()/2, rcBody.center().y()-m_pmDirection[m_nAutoRangeStep].height()/2, m_pmDirection[m_nAutoRangeStep].width(), m_pmDirection[m_nAutoRangeStep].height() );

    p.drawPixmap( rcTouchArea, m_pmDirection[m_nAutoRangeStep] );

//    rcTouchArea.adjust( (int)(rcTouchArea.width()*0.0611), (int)(rcTouchArea.height()*0.0611), -(int)(rcTouchArea.width()*0.0611), -(int)(rcTouchArea.height()*32.72) );

    p.restore();

//    return rcTouchArea;
}

void QAutoDetectionRange::drawArrow(QPainter& p)
{
    if (!m_bEnterAutoRangeSetting)
        return;

    Q_ASSERT( m_nAutoRangeStep <= 3 );

    p.save();
    p.setRenderHint(QPainter::Antialiasing);

    QRect rcArea = m_rcArrow[m_nAutoRangeStep];
    const int nOffset = 1;
    const int nAW = rcArea.width() - nOffset - 1;
    const int nAH = rcArea.height() - nOffset - 1;
    QPointF ptArrowLT[] =
    {
        QPointF(nOffset, nOffset),
        QPointF(nAW+nOffset, nOffset),
        QPointF(nAW*4.f/5.f+nOffset, nAH/5.f+nOffset),
        QPointF(nAW+nOffset, nAH*2.f/5.f+nOffset),
        QPointF(nAW*2.f/5.f+nOffset, nAH+nOffset),
        QPointF(nAW/5.f+nOffset, nAH*4.f/5.f+nOffset),
        QPointF(nOffset, nAH+nOffset),
        QPointF(nOffset, nOffset)
    };

    int nPtCnt = (int)(sizeof(ptArrowLT) / sizeof(QPointF));

    QPointF* pDrawArrow = NULL;
    QPointF* pArrowMirror = NULL;
    switch ( m_nAutoRangeStep )
    {
    case 0:		// LT
        pDrawArrow = ptArrowLT;
        break;
    case 1:		// RT
        pArrowMirror = new QPointF[ nPtCnt ];
        pDrawArrow = pArrowMirror;
        for ( int i=0 ; i<nPtCnt ; i++ )
        {
            pArrowMirror[i].setX(rcArea.x() + rcArea.width() - ptArrowLT[i].x());
            pArrowMirror[i].setY(rcArea.y() + ptArrowLT[i].y());
        }
        break;
    case 2:		// RB
        pArrowMirror = new QPointF[ nPtCnt ];
        pDrawArrow = pArrowMirror;
        for ( int i=0 ; i<nPtCnt ; i++ )
        {
            pArrowMirror[i].setX(rcArea.x() + rcArea.width() - ptArrowLT[i].x());
            pArrowMirror[i].setY(rcArea.y() + rcArea.height() - ptArrowLT[i].y());
        }
        break;
    case 3:		// LB
        pArrowMirror = new QPointF[ nPtCnt ];
        pDrawArrow = pArrowMirror;
        for ( int i=0 ; i<nPtCnt ; i++ )
        {
            pArrowMirror[i].setX(rcArea.x() + ptArrowLT[i].x());
            pArrowMirror[i].setY(rcArea.y() + rcArea.height() - ptArrowLT[i].y());
        }
        break;
    }

    if (!m_bCamTouch)
    {
        if (!m_bToggleArrow)
        {
            p.setPen( QColor(195, 195, 195) );
            p.setBrush( QColor(232, 232, 232) );
        }
        else
        {
            if (!m_bTouchOK)
            {
                p.setPen( QColor(237, 28, 36, 100) );
                p.setBrush( QColor(243, 109, 116, 100) );
            }
            else
            {
                p.setPen( QColor(237, 28, 36) );
                p.setBrush( QColor(243, 109, 116) );
            }
        }
    }
    else
    {
        p.setPen( QColor(237, 28, 36) );
        p.setBrush( QColor(243, 109, 116) );
    }

    p.drawPolygon( pDrawArrow, nPtCnt );

    if (m_bCamTouch)
    {
        if (!m_bTouchOK)
        {
            p.setPen( Qt::NoPen );
            p.setBrush( QColor(243, 109, 116) );
            int progress = m_nTouchProgress * m_rcProgress[m_nAutoRangeStep].width() / 100;
            p.drawRect( m_rcProgress[m_nAutoRangeStep].left(), m_rcProgress[m_nAutoRangeStep].top(), progress, m_rcProgress[m_nAutoRangeStep].height() );
        }
    }

    if (pArrowMirror)
        delete[] pArrowMirror;

    p.restore();
}

void QAutoDetectionRange::enterAutoRangeSetting()
{
//    LOG_I( "enter autorange setting" );

    m_pT3kHandle->setReportView( true );

    m_RequestHIDManager.Stop();

    m_RequestHIDManager.AddItem( "mode=", "detection", QRequestHIDManager::CM1 );
    m_RequestHIDManager.AddItem( "mode=", "detection", QRequestHIDManager::CM2 );

    m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(0) + "," + QString::number(0xffff), QRequestHIDManager::CM1 );
    m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(0) + "," + QString::number(0xffff), QRequestHIDManager::CM2 );

    if ( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(0) + "," + QString::number(0xffff), QRequestHIDManager::CM1_1 );
        m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(0) + "," + QString::number(0xffff), QRequestHIDManager::CM2_1 );
    }

    m_RequestHIDManager.Start( m_pT3kHandle );

    m_lCam1Left = 0xFFFF;
    m_lCam1Right = 0x0000;
    m_lCam2Left = 0xFFFF;
    m_lCam2Right = 0x0000;

    m_bTouchOK = false;
    m_bEnterAutoRangeSetting = true;
    m_bCamTouch = false;
    m_nAutoRangeStep = 0;
    showArrow();

    playBuzzer( m_pT3kHandle, BuzzerEnterCalibration );
}

void QAutoDetectionRange::leaveAutoRangeSetting()
{
//    LOG_I( "leave autorange setting" );

    m_pT3kHandle->setReportView( false );

    m_bTouchOK = false;
    m_bCamTouch = false;
    m_bEnterAutoRangeSetting = false;
    m_nAutoRangeStep = 0;
    hideArrow();

    playBuzzer( m_pT3kHandle, BuzzerCancelCalibration );

    m_RequestHIDManager.Stop();

    m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM1 );
    m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM2 );

    if ( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM1_1 );
        m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM2_1 );
    }

    m_RequestHIDManager.Start( m_pT3kHandle );
}

void QAutoDetectionRange::onFinishAutoRange()
{
    analysisTouchObj();

    m_bCamTouch = false;
    m_bEnterAutoRangeSetting = false;

    hideArrow();

    m_nAutoRangeStep = 0;

    m_pT3kHandle->setReportView( false );

    bool bRet = (m_lCam1Left < m_lCam1Right && m_lCam2Left < m_lCam2Right);

    if( bRet )       // success
    {
        playBuzzer( m_pT3kHandle, BuzzerCalibrationSucces );

        emit showProgressDialog();

        m_RequestHIDManager.Stop();

        m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(m_lCam1Left) + "," + QString::number(m_lCam1Right), QRequestHIDManager::CM1 );
        m_RequestHIDManager.AddItem( cstrDetectionRange, QString::number(m_lCam2Left) + "," + QString::number(m_lCam2Right), QRequestHIDManager::CM2 );

        if ( QT3kUserData::GetInstance()->isSubCameraExist() )
        {
            QString strV( QCustomDefaultSensor::Instance()->GetDefaultData( QString("cam1/sub/%1").arg(cstrDetectionRange).toUtf8().data(), "" ) );
            if( strV.isEmpty() )
                m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM1_1 );
            else
                m_RequestHIDManager.AddItem( cstrDetectionRange, strV, QRequestHIDManager::CM1_1 );
            strV = QCustomDefaultSensor::Instance()->GetDefaultData( QString("cam2/sub/%1").arg(cstrDetectionRange).toUtf8().data(), "" );
            if( strV.isEmpty() )
                m_RequestHIDManager.AddItem( cstrDetectionRange, "*", QRequestHIDManager::CM2_1 );
            else
                m_RequestHIDManager.AddItem( cstrDetectionRange, strV, QRequestHIDManager::CM2_1 );
        }

        m_bChekcFinish = true;
        m_RequestHIDManager.Start( m_pT3kHandle );
    }
    else    // fail
    {
        playBuzzer( m_pT3kHandle, BuzzerCancelCalibration );

        emit finishDetectionRange( false );
    }
}

void QAutoDetectionRange::TPDP_OnDTC(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, unsigned char */*layerid*/, unsigned long *start_pos, unsigned long *end_pos, int cnt)
{
    int nCIdx = (Part == CM1) || (Part == CM1_1) ? 0 : 1;

    m_nCamTouchCount[nCIdx] = cnt;

    if (!m_bEnterAutoRangeSetting)
        return;

    bool bTouch = false;
    if (m_nCamTouchCount[0] > 0 && m_nCamTouchCount[1] > 0)
    {
        bTouch = true;
    }
    else if (m_nCamTouchCount[0] == 0 && m_nCamTouchCount[1] == 0)
    {
        bTouch = false;
    }
    else
    {
        return;
    }

    if (bTouch)
    {
        if (m_bTouchOK)
            return;

        if (!m_bCamTouch)   // 1st touch
        {
            m_bCamTouch = true;
            m_dwTickTouch = 0;
            if (m_nTouchProgress != 0)
            {
                m_nTouchProgress = 0;
                updateRect(m_rcProgress[m_nAutoRangeStep]);
            }
            else
            {
                updateRect(m_rcArrow[m_nAutoRangeStep]);
            }
        }

        if (m_bCamTouch)
        {
            if (end_pos[cnt-1] - start_pos[0] < 0xffff/4)
            {
                if (m_dwTickTouch < MAX_TICK_COUNT / 2)
                {
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setX( start_pos[0] );
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setY( end_pos[cnt-1] );
                    m_dwTickTouch ++;
                    int progress = m_dwTickTouch * 100 / MAX_TICK_COUNT;
                    if (m_nTouchProgress != progress)
                    {
                        m_nTouchProgress = progress;
                        updateRect(m_rcProgress[m_nAutoRangeStep]);
                    }
                }
                else
                {
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setX( start_pos[0] < (unsigned long)m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].x() ? start_pos[0] : m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].x() );
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setY( end_pos[cnt-1] > (unsigned long)m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].y() ? end_pos[cnt-1] : m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].y() );
                    m_dwTickTouch ++;
                    int progress = m_dwTickTouch * 100 / MAX_TICK_COUNT;
                    if (m_nTouchProgress != progress)
                    {
                        m_nTouchProgress = progress;
                        updateRect(m_rcProgress[m_nAutoRangeStep]);
                    }
                }
            }

            if (m_dwTickTouch >= MAX_TICK_COUNT)
            {
                if (m_nAutoRangeStep < 3)
                {
                    m_bTouchOK = true;
                    m_nAutoRangeStep ++;
                    emit updateWidget();
                    playBuzzer( m_pT3kHandle, BuzzerNextPoint );
                }
                else
                {
                    onFinishAutoRange();
                }
            }
        }
    }
    else
    {
        if (m_bTouchOK)
            m_bTouchOK = false;
        m_bCamTouch = false;
        m_dwTickTouch = 0;
        if (m_nTouchProgress != 0)
        {
            m_nTouchProgress = 0;
            updateRect(m_rcArrow[m_nAutoRangeStep]);
            updateRect(m_rcProgress[m_nAutoRangeStep]);
        }
    }
}

void QAutoDetectionRange::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( strstr(cmd, cstrDetectionRange) == cmd )
    {
        switch( Part )
        {
        case MM:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange );
            break;
        case CM1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM1 );
            break;
        case CM2:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM2 );
            break;
        case CM1_1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM1_1 );
            break;
        case CM2_1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM2_1 );
            break;
        default:
            break;
        }
    }
}

void QAutoDetectionRange::TPDP_OnRSE(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( strstr(cmd, cstrNoCam) == cmd )
    {
        switch( Part )
        {
        case MM:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange );
            break;
        case CM1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM1 );
            break;
        case CM2:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM2 );
            break;
        case CM1_1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM1_1 );
            break;
        case CM2_1:
            m_RequestHIDManager.RemoveItem( cstrDetectionRange, QRequestHIDManager::CM2_1 );
            break;
        default:
            break;
        }
    }
}

void QAutoDetectionRange::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerBlinkArrow)
    {
        m_bToggleArrow = !m_bToggleArrow;
        if (m_nAutoRangeStep < 4)
        {
            updateRect( m_rcArrow[m_nAutoRangeStep] );
        }
    }
}

void QAutoDetectionRange::showArrow()
{
    if (m_TimerBlinkArrow)
        killTimer(m_TimerBlinkArrow);
    m_bToggleArrow = true;
    m_TimerBlinkArrow = startTimer(500);

    emit updateWidgetRect(m_rcArrow[m_nAutoRangeStep]);
}

void QAutoDetectionRange::hideArrow()
{
    if (m_TimerBlinkArrow)
        killTimer(m_TimerBlinkArrow);
    m_TimerBlinkArrow = 0;
    m_bToggleArrow = false;
}

void QAutoDetectionRange::updateRect(QRect rc)
{
    rc.adjust( -2, -2, 2, 2 );
    emit updateWidgetRect( rc );
}

void QAutoDetectionRange::analysisTouchObj()
{
    const int nMaxW = 100;
    // left !!
    m_lCam1Left = 0xFFFF;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[0][i].x() - m_ptCamTouchObj[0][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lLeft = m_ptCamTouchObj[0][i].y() - lW;
        if ( lLeft < m_lCam1Left )
        {
            m_lCam1Left = lLeft;
        }
    }
    m_lCam2Left = 0xFFFF;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[1][i].x() - m_ptCamTouchObj[1][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lLeft = m_ptCamTouchObj[1][i].y() - lW;
        if ( lLeft < m_lCam2Left )
        {
            m_lCam2Left = lLeft;
        }
    }

    m_lCam1Right = 0x0000;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[0][i].x() - m_ptCamTouchObj[0][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lRight = m_ptCamTouchObj[0][i].x() + lW;
        if ( lRight > m_lCam1Right )
        {
            m_lCam1Right = lRight;
        }
    }
    m_lCam2Right = 0x0000;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[1][i].x() - m_ptCamTouchObj[1][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lRight = m_ptCamTouchObj[1][i].x() + lW;
        if ( lRight > m_lCam2Right )
        {
            m_lCam2Right = lRight;
        }
    }
}

void QAutoDetectionRange::onRequestFinish()
{
    if( m_bChekcFinish )
        emit finishDetectionRange( true );
}
