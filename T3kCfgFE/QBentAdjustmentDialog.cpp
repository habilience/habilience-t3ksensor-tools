#include "QBentAdjustmentDialog.h"
#include "ui_QBentAdjustmentDialog.h"
#include "dialog.h"
#include <QPainter>
#include <QCloseEvent>

#include "../common/QUtils.h"
#include "QT3kDevice.h"
#include "QShowMessageBox.h"
#include "QLogSystem.h"

#include "QBentCfgParam.h"
#include "QInitDataIni.h"
#include "QSensorInitDataCfg.h"
#include "QBorderStyleEdit.h"

#include <QScreen>
#include <math.h>
#include <QFont>
#include "AppData.h"
#include <QtNumeric>
#include "t3kcomdef.h"
#include "../common/T3kConstStr.h"

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

QBentAdjustmentDialog::QBentAdjustmentDialog(Dialog *parent) :
    QDialog(parent),
    m_pMainDlg(parent),
    ui(new Ui::QBentAdjustmentDialog),
    m_EventRedirect(this)
{
    ui->setupUi(this);
    QT3kDevice* pDevice = QT3kDevice::instance();

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;

    if (!pDevice->isVirtualDevice())
    {
#if defined(Q_OS_WIN)
        flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
        flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose);

    DISABLE_MSWINDOWS_TOUCH_PROPERTY;

    m_bIsModified = false;
    m_bEnterAdjustmentMode = false;
    m_nAdjustmentStep = 0;
    m_bIsValidTouch = 0;
    m_nValidTouchCount = 0;
    m_bBentAdjustmentPerformed = false;
    m_nBentProgress = 0;

    m_bCheckCamTouch[0] = m_bCheckCamTouch[1] = false;
    m_nTouchCount = 0;
    m_nMaxTouchCount = g_AppData.nCameraCount * 130;
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
    m_TimerShowButtons = 0;
    m_TimerHideButtons = 0;
    m_TimerDrawWaitTimeout = 0;

    LOG_I( "Enter [Bent Adjustment]" );

    onChangeLanguage();

    QBentCfgParam* bentParam = QBentCfgParam::instance();
    QInitDataIni* iniData = QInitDataIni::instance();
    if (g_AppData.nCameraCount == 2)
    {
        if (g_AppData.strFirmwareVersion.compare( "2.8" ) <= 0)
        {
            bentParam->setAlgorithm( iniData->getBentAlgorithm4() );
        }
        else
        {
            bentParam->setAlgorithm( iniData->getBentAlgorithm2() );
        }
    }
    else
    {
        bentParam->setAlgorithm( iniData->getBentAlgorithm4() );
    }

    int nLeft, nTop, nRight, nBottom, nDir;
    iniData->getBentMargin( nLeft, nTop, nRight, nBottom, nDir );
    bentParam->setMargin( nLeft/100.f, nTop/100.f, nRight/100.f, nBottom/100.f );
    bentParam->setDirection( nDir );

    loadDefaultSettingValues();

    updateData( false );

    QString strScreenInformation;
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        strScreenInformation =
                QString("%1x%2\n")
                        .arg(screen->size().width())
                        .arg(screen->size().height());
        Qt::ScreenOrientation orientation = screen->orientation();
        m_nMonitorOrientation = 0;
        switch (orientation)
        {
        default:
        case Qt::LandscapeOrientation:
            m_nMonitorOrientation = 0;
            strScreenInformation += "Landscape";
            break;
        case Qt::PortraitOrientation:
            m_nMonitorOrientation = 1;
            strScreenInformation += "Portrait";
            break;
        case Qt::InvertedLandscapeOrientation:
            m_nMonitorOrientation = 2;
            strScreenInformation += "Inverted Landscape";
            break;
        case Qt::InvertedPortraitOrientation:
            m_nMonitorOrientation = 3;
            strScreenInformation += "Inverted Portrait";
            break;
        }

        LOG_I( "Init Monitor Orientation: %d", m_nMonitorOrientation );
    }
    ui->lblScreenInformation->setText( strScreenInformation );

    installEventFilter(&m_EventRedirect);

    QBorderStyleEdit* edits[] = {
        ui->txtEdtMarginLeft,
        ui->txtEdtMarginUp,
        ui->txtEdtMarginRight,
        ui->txtEdtMarginDown
    };

    for (int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++)
    {
        edits[i]->setAlignment(Qt::AlignCenter);
        edits[i]->setMaxTextLength(2);
        edits[i]->setFloatStyle(false);

        connect( edits[i], SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    }

    ui->btnLeft->setDirection(QArrowButton::DirectionLeft);
    ui->btnUp->setDirection(QArrowButton::DirectionUp);
    ui->btnRight->setDirection(QArrowButton::DirectionRight);
    ui->btnDown->setDirection(QArrowButton::DirectionDown);

    m_pMainDlg->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_OBJECT );
    ui->cmdAsyncMngr->setT3kDevice(QT3kDevice::instance());

    // request information
    if (ui->cmdAsyncMngr->isStarted())
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    QString strCmd;
    strCmd = sCam1 + cstrFactorialCamPos + "?";
    ui->cmdAsyncMngr->insertCommand(strCmd);
    strCmd = sCam2 + cstrFactorialCamPos + "?";
    ui->cmdAsyncMngr->insertCommand(strCmd);
    if ( g_AppData.bIsSubCameraExist )
    {
        strCmd = sCam1_1 + cstrFactorialCamPos + "?";
        ui->cmdAsyncMngr->insertCommand(strCmd);
        strCmd = sCam2_1 + cstrFactorialCamPos + "?";
        ui->cmdAsyncMngr->insertCommand(strCmd);
    }
    strCmd = QString(cstrAreaC) + "?";
    ui->cmdAsyncMngr->insertCommand(strCmd);

    ui->cmdAsyncMngr->start( (unsigned int)-1 );
}

QBentAdjustmentDialog::~QBentAdjustmentDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;

    LOG_I( "Exit [Bent Adjustment]" );
}


bool QBentAdjustmentDialog::canClose()
{
    if (m_bIsModified)
    {
        QLangRes& res = QLangManager::getResource();
        QString strPrompt = res.getResString( MAIN_TAG, "TEXT_WARNING_SENSOR_DATA_IS_CHANGED" );
        QString strTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE" );
        int nRet = showMessageBox( this,
            strPrompt,
            strTitle,
            QMessageBox::Question, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes );
        if (nRet == QMessageBox::Yes)
        {
            on_btnSave_clicked();
            return true;
        }
        else if (nRet == QMessageBox::No)
        {
        }
        else
        {
            ui->btnClose->setEnabled(true);
            ui->btnClose->setFocus();
            return false;
        }
    }

    return true;
}

void QBentAdjustmentDialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    ui->btnUp->setText( res.getResString(RES_TAG, "BTN_CAPTION_UP") );
    ui->btnDown->setText( res.getResString(RES_TAG, "BTN_CAPTION_DOWN") );
    ui->btnLeft->setText( res.getResString(RES_TAG, "BTN_CAPTION_LEFT") );
    ui->btnRight->setText( res.getResString(RES_TAG, "BTN_CAPTION_RIGHT") );

    ui->btnReset->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_RESET") );
    ui->btnSave->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_SAVE") );
    ui->btnClose->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_CLOSE") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: !!!!
    }

    s_bIsR2L = bIsR2L;
}

bool QBentAdjustmentDialog::onKeyPress(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Escape)
    {
        if (m_bEnterAdjustmentMode)
        {
            leaveAdjustmentMode( false );
            return true;
        }
        else
        {
            LOG_I( "From Keyboard(ESC)" );
            on_btnClose_clicked();
            return true;
        }
    }
    return false;
}

bool QBentAdjustmentDialog::onKeyRelease(QKeyEvent *evt)
{
    QKeyEvent* keyEvt = (QKeyEvent*)evt;
    if ( (keyEvt->key() == Qt::Key_Enter) ||
         (keyEvt->key() == Qt::Key_Return) )
    {
        QWidget* pWidget = focusWidget();
        if (pWidget->objectName().indexOf("txtEdt") >= 0)
        {
            pWidget->clearFocus();
            return true;
        }
    }
    return false;
}

bool QBentAdjustmentDialog::loadDefaultSettingValues()
{
    QString strValue;
    if ( QSensorInitDataCfg::instance()->isLoaded() &&
         QSensorInitDataCfg::instance()->getValue( "{bent margin}", strValue ) )
    {
        if ( !strValue.isEmpty() )
        {
            do
            {
                int nMarginLeft, nMarginUp, nMarginRight, nMarginDown;
                int nDirection;
                int nCP;
                nCP = strValue.indexOf(',');
                if ( nCP <= 0 ) break;
                nMarginLeft = trim( strValue.left(nCP) ).toInt(0, 10);
                strValue.remove(0, nCP+1);
                nCP = strValue.indexOf(',');
                if ( nCP <= 0 ) break;
                nMarginUp = trim( strValue.left(nCP) ).toInt(0, 10);
                strValue.remove(0, nCP+1);
                nCP = strValue.indexOf(',');
                if ( nCP <= 0 ) break;
                nMarginRight = trim( strValue.left(nCP) ).toInt(0, 10);
                strValue.remove(0, nCP+1);
                nCP = strValue.indexOf(',');
                if ( nCP <= 0 ) break;
                nMarginDown = trim( strValue.left(nCP) ).toInt(0, 10);
                strValue.remove(0, nCP+1);
                if ( strValue.isEmpty() ) break;
                nDirection = trim( strValue.left(nCP) ).toInt(0, 10);

                QBentCfgParam* param = QBentCfgParam::instance();
                param->setMargin(nMarginLeft/100.f, nMarginUp/100.f, nMarginRight/100.f, nMarginDown/100.f);
                param->setDirection( nDirection );

                return true;
            } while ( false );
        }
    }

    return false;
}

void QBentAdjustmentDialog::updateData( bool bSaveAndValidate )
{
    QBentCfgParam* param = QBentCfgParam::instance();

    if (bSaveAndValidate)
    {
        const int s_nMinDist = 1;

        float fMarginLeft, fMarginUp, fMarginRight, fMarginDown;
        float fOldMarginUp, fOldMarginDown, fOldMarginLeft, fOldMarginRight;

        fOldMarginUp = param->marginTop();
        fOldMarginDown = param->marginBottom();
        fOldMarginLeft = param->marginLeft();
        fOldMarginRight = param->marginRight();

        fMarginLeft = ui->txtEdtMarginLeft->toPlainText().toInt() / 100.f;
        fMarginUp = ui->txtEdtMarginUp->toPlainText().toInt() / 100.f;
        fMarginRight = ui->txtEdtMarginRight->toPlainText().toInt() / 100.f;
        fMarginDown = ui->txtEdtMarginDown->toPlainText().toInt() / 100.f;

        if (fOldMarginLeft != fMarginLeft)
        {
            int nLeftMax = 100-(int)(fMarginRight * 100 + .5f) - s_nMinDist;
            int nLeft = (int)(fMarginLeft * 100 + .5f);
            if ( nLeft > nLeftMax ) nLeft = nLeftMax;
            fMarginLeft = nLeft / 100.f;
            param->setMarginLeft(fMarginLeft);
            ui->txtEdtMarginLeft->setText( QString::number((int)(fMarginLeft*100+0.5f)) );
        }
        if (fOldMarginUp != fMarginUp)
        {
            int nUpMax = 100-(int)(fMarginDown * 100 + .5f) - s_nMinDist;
            int nUp = (int)(fMarginUp * 100 + .5f);
            if ( nUp > nUpMax ) nUp = nUpMax;
            fMarginUp = nUp / 100.f;
            param->setMarginTop(fMarginUp);
            ui->txtEdtMarginUp->setText( QString::number((int)(fMarginUp*100+0.5f)) );
        }
        if (fOldMarginRight != fMarginRight)
        {
            int nRightMax = 100-(int)(fMarginLeft * 100 + .5f) - s_nMinDist;
            int nRight = (int)(fMarginRight * 100 + .5f);
            if ( nRight > nRightMax ) nRight = nRightMax;
            fMarginRight = nRight / 100.f;
            param->setMarginRight(fMarginRight);
            ui->txtEdtMarginRight->setText( QString::number((int)(fMarginRight*100+0.5f)) );
        }
        if (fOldMarginDown != fMarginDown)
        {
            int nDnMax = 100-(int)(fMarginUp * 100 + .5f) - s_nMinDist;
            int nDn = (int)(fMarginDown * 100 + .5f);
            if ( nDn > nDnMax ) nDn = nDnMax;
            fMarginDown = nDn / 100.f;
            param->setMarginBottom(fMarginDown);
            ui->txtEdtMarginDown->setText( QString::number((int)(fMarginDown*100+0.5f)) );
        }
    }
    else
    {
        int nMarginLeft = (int)(param->marginLeft() * 100 + 0.5f);
        int nMarginUp = (int)(param->marginTop() * 100 + 0.5f);
        int nMarginRight = (int)(param->marginRight() * 100 + 0.5f);
        int nMarginDown = (int)(param->marginBottom() * 100 + 0.5f);

        ui->txtEdtMarginLeft->setText( QString::number(nMarginLeft) );
        ui->txtEdtMarginUp->setText( QString::number(nMarginUp) );
        ui->txtEdtMarginRight->setText( QString::number(nMarginRight) );
        ui->txtEdtMarginDown->setText( QString::number(nMarginDown) );
    }
}

void QBentAdjustmentDialog::enterAdjustmentMode()
{
    setCursor(QCursor(Qt::BlankCursor));
}

void QBentAdjustmentDialog::leaveAdjustmentMode( bool bSuccess )
{
    setCursor(QCursor(Qt::ArrowCursor));
}

void QBentAdjustmentDialog::enableAllControls(bool bEnable)
{

}

void QBentAdjustmentDialog::on_btnClose_clicked()
{
    close();
}

void QBentAdjustmentDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.save();
    QRect rcBody(0, 0, width(), height());
    p.fillRect( rcBody, Qt::white );

    p.setRenderHint(QPainter::Antialiasing);

    QPoint ptCursor;

    if (ui->widgetBentDirMargin->isVisible())
    {
        drawCameraLocations( p, rcBody );
    }

    drawAdjustmentGrid( p, rcBody, &ptCursor );

    int nIn = (int)hypotf( (float)rcBody.width(), (float)rcBody.height() );

    int nC = nIn / 50;
    if( nC < 12 ) nC = 12;

    drawErrorText( p, 10, 10, nIn / 60 );
    drawCursor( p, ptCursor.x(), ptCursor.y(), nC );

    int nWXY = rcBody.height() / 12;
    QRect rcWait( rcBody.center().x()-nWXY, rcBody.center().y()-nWXY, nWXY*2, nWXY*2 );
    m_rcWaitTime = rcWait;
    if ( m_bDrawWaitTimeout )
        drawWaitTime( p, rcWait );

    p.restore();
}

QPoint QBentAdjustmentDialog::PosToDCC( float x, float y, const QRect rcClient )
{
    QPointF pt = PosToTPos(x, y);
    return TPosToDCC(pt.x(), pt.y(), rcClient);
}

QPointF QBentAdjustmentDialog::PosToTPos( float x, float y, float o/*=1.f*/ )
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

QPoint QBentAdjustmentDialog::TPosToDCC( float x, float y, const QRect rcClient )
{
    return QPoint((long)(x * rcClient.width()) + rcClient.left(), (long)(y * rcClient.height()) + rcClient.top());
}

inline int camIdxToIdx( int nCameraIndex )
{
    switch (nCameraIndex)
    {
    case IDX_CM1:
        return 0;
    case IDX_CM2:
        return 1;
    case IDX_CM1_1:
        return 2;
    case IDX_CM2_1:
        return 3;
    }
    return 0;
}

void QBentAdjustmentDialog::drawAdjustmentGrid(QPainter &p, QRect rcBody, QPoint* pPtCursor)
{
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
    QPolygon gridPolyline;
    for ( int i=0; i<ADJUSTMENT_STEP; i++ )
    {
        if ( qIsNaN(s_PosXY[nPosXYSel][i].x) )
            continue;

        pt = PosToDCC( s_PosXY[nPosXYSel][i].x, s_PosXY[nPosXYSel][i].y, rcBody );
        if ( m_bEnterAdjustmentMode )
        {
            if ( i > m_nAdjustmentStep )
                p.setPen( penGd );
        }
        gridPolyline.push_back( pt );
        if ( m_nAdjustmentStep == i ) *pPtCursor = pt;
    }

    p.drawPolyline( gridPolyline );

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

    if ( !m_bIsTouchLift )
    {
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
                QPoint pts[3];
                pts[0] = TPosToDCC(item.fCamE, item.fCamF, rcBody);
                pts[1] = TPosToDCC((item.fTouchPosS[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosS[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);
                pts[2] = TPosToDCC((item.fTouchPosE[j] * item.fCamA + item.fCamC) * 25.f + item.fCamE, (item.fTouchPosE[j] * item.fCamB + item.fCamD) * 25.f + item.fCamF, rcBody);

                p.drawPolygon(pts, 3);
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

                p.drawLine(pt0, ptV);
            }
        }
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
            if ( m_aryCTs[ni].pts[np].x == -1 && m_aryCTs[ni].pts[np].y == -1 )
                continue;
            polyline.push_back(m_aryCTs[ni].pts[np]);
        }
        p.drawPolyline(polyline);
    }
#endif //DEVELOP_CROSSTRACE
}

inline QString getCameraText( int nIndex )
{
    switch (nIndex)
    {
    case IDX_CM1:
        return "CM1";
    case IDX_CM2:
        return "CM2";
    case IDX_CM1_1:
        return "CM1-1";
    case IDX_CM2_1:
        return "CM2-1";
    }
    return "";
}

inline QString getCameraPrefix( int nIndex )
{
    switch (nIndex)
    {
    case IDX_CM1:
        return sCam1;
    case IDX_CM2:
        return sCam2;
    case IDX_CM1_1:
        return sCam1_1;
    case IDX_CM2_1:
        return sCam2_1;
    }
    return "";
}

void QBentAdjustmentDialog::drawCameraLocations( QPainter& p, QRect rcBody )
{
    int nt = rcBody.width() / 42;
    int nth = rcBody.height() / 24;
    int ntx = nt > nth ? nth : nt;
    QFont fnt("Arial", ntx, QFont::DemiBold);
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

void QBentAdjustmentDialog::drawCursor( QPainter& p, int nx, int ny, int nc )
{
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
                QPen penArc( QColor(237, 28, 36, 128), nc/4 );
                p.setPen(penArc);
                p.drawArc( nx-nc/2, ny-nc/2, nc, nc, -90, -m_nBentProgress*360/100 );
                p.drawArc( nx-nc/2, ny-nc/2, nc, nc, -90, m_nBentProgress*360/100 );
            }
        }
    }
    m_rcCursor.setLeft(nx-(nc*3/2+1));
    m_rcCursor.setTop(ny-(nc*3/2+1));
    m_rcCursor.setRight(nx+(nc*3/2+1));
    m_rcCursor.setBottom(ny+(nc*3/2+1));

    m_rcCursor.adjust( -nIR, -nIR, nIR, nIR );
}

void QBentAdjustmentDialog::drawErrorText( QPainter& p, int nx, int ny, int nc )
{
    if ( m_cError == 0 && m_cNG == 0 )
        return;

    QFont fntError("Arial", nc);
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

void QBentAdjustmentDialog::drawWaitTime( QPainter& p, QRect rcWait )
{
    int nTime = m_nWaitCountDown/WAIT_ANIMATION_FRAME;
    if ( nTime == 0 ) return;

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
    p.drawArc( rectWait, 0, 360*nStep/WAIT_ANIMATION_FRAME );

    QFont fntWait("Arial", rectWait.height()*2/3, QFont::Bold);
    p.setFont(fntWait);
    int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;

    rectWait.adjust( 0, rectWait.height()*5/100, 0, rectWait.height()*5/100 );

    QString str = QString::number(nTime);

    p.setPen(QColor(125,125,125));
    p.drawText( rectWait, flags, str );
}


void QBentAdjustmentDialog::closeEvent(QCloseEvent *evt)
{
    if (!canClose())
        evt->ignore();
}

void QBentAdjustmentDialog::reject()
{
    LOG_I( "From Keyboard(ESC)" );
    ui->btnClose->setEnabled(false);
    close();
}

void QBentAdjustmentDialog::accept()
{
    close();
}

void QBentAdjustmentDialog::calculateCameraPosition( float* /*fObcS*/, float* /*fObcE*/ )
{

}

bool QBentAdjustmentDialog::calculateCameraValues( BentItem& item )
{
    // TODO:
    return false;
}

void QBentAdjustmentDialog::showArrowButtons( bool bShow )
{
    ui->widgetBentDirMargin->setVisible(bShow);
}

void QBentAdjustmentDialog::showAllButtonsWithoutClose( bool bShow )
{
    showArrowButtons(bShow);
    ui->btnReset->setVisible(bShow);
    ui->btnSave->setVisible(bShow);
    ui->btnClose->setVisible(bShow);
    ui->cmdAsyncMngr->setVisible(bShow);
}

void QBentAdjustmentDialog::onAdjustmentFinish()
{
    m_cError = m_cNG = 0;

    // TODO:

    playBuzzer( BuzzerCalibrationSucces );

    m_bEnterAdjustmentMode = false;
    m_bShowCursor = false;

    m_bIsModified = true;
    showAllButtonsWithoutClose(true);

    m_bDrawWaitTimeout = false;
    if (m_TimerDrawWaitTimeout)
    {
        killTimer(m_TimerDrawWaitTimeout);
        m_TimerDrawWaitTimeout = 0;
    }
    update();

    m_bBentAdjustmentPerformed = true;
}

void QBentAdjustmentDialog::checkTouchPoints( bool bTouch )
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
        }
        m_nTouchCount ++;
        int nProgress = m_nTouchCount * 100 / m_nMaxTouchCount;
        if (m_nBentProgress != nProgress)
        {
            m_nBentProgress = nProgress;
            if ( (nProgress % 2) == 0 )
            {
                QPoint ptRemote = getRemoteCursorPos(m_nAdjustmentStep);
                if ( ((nProgress % 10) == 0) && (nProgress != 100) )
                    remoteCursor( true, false, ptRemote.x(), ptRemote.y(), nProgress );
                update( m_rcCursor );
            }
        }
        if (m_nTouchCount >= m_nMaxTouchCount)
        {
            if (!checkValidTouch())
            {
                m_bIsValidTouch = true;
                setInvalidTouch();
                QPoint ptRemote = getRemoteCursorPos(m_nAdjustmentStep);
                remoteCursor( true, false, ptRemote.x(), ptRemote.y(), 100 );
                return;
            }
            m_bIsTouchOK = true;

            LOG_I( "TOUCH OK" );
            qDebug( "TOUCH OK" );

            m_TimerReCheckPoint = startTimer(1500);
            playBuzzer( BuzzerClick );

            m_bIsValidTouch = true;
            m_nValidTouchCount ++;
            update(m_rcCursor);

            QPoint ptRemote = getRemoteCursorPos(m_nAdjustmentStep);
            remoteCursor(true, true, ptRemote.x(), ptRemote.y(), 100);

            for (int i=0; i<m_BentItemArray.size(); i++)
            {
                BentItem& item = m_BentItemArray[i];
                if (item.bDataValid)
                {
                    item.fObcCenter[m_nAdjustmentStep] = (item.fLastTouchPosS+item.fLastTouchPosE) / 2.f;
                    item.fObcS[s_PosXY[nPosXYSel][m_nAdjustmentStep].idx] = item.fLastTouchPosS;
                    item.fObcE[s_PosXY[nPosXYSel][m_nAdjustmentStep].idx] = item.fLastTouchPosE;

                    qDebug( "[%d] %.2f, %.2f", i, item.fLastTouchPosS, item.fLastTouchPosE );
                    LOG_I( "[%d] %.2f, %.2f", i, item.fLastTouchPosS, item.fLastTouchPosE );
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
            LOG_I( "TOUCH LIFT" );

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
                remoteCursor(false);
                onAdjustmentFinish();
                m_nAdjustmentStep = 0;
            }
            else
            {
                playBuzzer( BuzzerNextPoint );
                m_bIsValidTouch = false;
                m_nValidTouchCount = 0;
                update();

                QPoint ptRemote = getRemoteCursorPos(m_nAdjustmentStep);
                remoteCursor( true, false, ptRemote.x(), ptRemote.y(), 0 );

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

void QBentAdjustmentDialog::setInvalidTouch()
{
    m_bIsTouchOK = false;
    m_nTouchCount = 0;
    if (m_nBentProgress != 0)
    {
        m_nBentProgress = 0;
        QPoint ptRemote = getRemoteCursorPos(m_nAdjustmentStep);
        remoteCursor( true, false, ptRemote.x(), ptRemote.y(), 0 );
    }

    if (!m_bIsValidTouch)
        return;

    m_bIsValidTouch = false;
    update( m_rcCursor );

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

bool QBentAdjustmentDialog::checkValidTouch()
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

QPoint QBentAdjustmentDialog::getRemoteCursorPos( int nAdjustmentStep )
{
    int nPosXYSel = QBentCfgParam::instance()->algorithm();
    QRect rcBody(0, 0, width(), height());
    QPoint pt = PosToDCC( s_PosXY[nPosXYSel][nAdjustmentStep].x, s_PosXY[nPosXYSel][m_nAdjustmentStep].y, rcBody );
    pt.setX( pt.x() * 0x7fff / rcBody.width() );
    pt.setY( pt.y() * 0x7fff / rcBody.height() );
    return pt;
}

void QBentAdjustmentDialog::remoteCursor( bool bShow, bool bTouchOK/*=false*/, int nx/*=0*/, int ny/*=0*/, int nProgress/*=0*/ )
{
    QT3kDevice* pDevice = QT3kDevice::instance();

    if (pDevice->isVirtualDevice())
    {
        FeatureCursorPos feature;
        feature.ReportID = REPORTID_FEATURE_CURSOR_POS;
        feature.IsScreenCoordinate = true;
        feature.Show = bShow;
        feature.TouchOK = bTouchOK;
        feature.X = nx;
        feature.Y = ny;
        feature.Progress = nProgress;

        pDevice->setFeature( &feature, sizeof(FeatureCursorPos) );
    }
}

void QBentAdjustmentDialog::TPDP_OnOBJ(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, unsigned char */*layerid*/, float *start_pos, float *end_pos, int cnt)
{
    if ( Part == MM ) return;
    int nCamIndex = getIndexFromPart(Part);
    int nCam = camIdxToIdx( nCamIndex );

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

        QRect rcClient(0, 0, width(), height());
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
                                           rcClient);
                bNew = true;
            }
            if ( item3 )
            {
                f = (item3->fTouchPosS[0] + item3->fTouchPosE[0]) / 2.f;
                float fdx3 = f * item3->fCamA + item3->fCamC;
                float fdy3 = f * item3->fCamB + item3->fCamD;
                ct.pts[1] = CalcCrossPoint(item1->fCamE, fdx1, item1->fCamF, fdy1,
                                           item3->fCamE, fdx3, item3->fCamF, fdy3,
                                           rcClient);
                bNew = true;
            }
            if ( item4 )
            {
                f = (item4->fTouchPosS[0] + item4->fTouchPosE[0]) / 2.f;
                float fdx4 = f * item4->fCamA + item4->fCamC;
                float fdy4 = f * item4->fCamB + item4->fCamD;
                ct.pts[2] = CalcCrossPoint(item1->fCamE, fdx1, item1->fCamF, fdy1,
                                           item4->fCamE, fdx4, item4->fCamF, fdy4,
                                           rcClient);
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
                                           rcClient);
                bNew = true;
            }
            if ( item4 )
            {
                f = (item4->fTouchPosS[0] + item4->fTouchPosE[0]) / 2.f;
                float fdx4 = f * item4->fCamA + item4->fCamC;
                float fdy4 = f * item4->fCamB + item4->fCamD;
                ct.pts[4] = CalcCrossPoint(item2->fCamE, fdx2, item2->fCamF, fdy2,
                                           item4->fCamE, fdx4, item4->fCamF, fdy4,
                                           rcClient);
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
                                           rcClient);
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

    QString str;

    if ( m_bEnterAdjustmentMode )
    {
        if ( cnt == 2 )
        {
            m_nWaitCountDown = WAIT_TOUCH_TIME;
            if ( m_bDrawWaitTimeout )
            {
                update(m_rcWaitTime);
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
                    item.fLastTouchPosS = start_pos[0];
                    item.fLastTouchPosE = end_pos[0];
                    item.fLastTouchPos = (start_pos[0] + end_pos[0]) / 2;
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
                item.fLastTouchPosS = start_pos[0];
                item.fLastTouchPosE = end_pos[0];
                item.fLastTouchPos = (start_pos[0] + end_pos[0]) / 2;

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
                if (m_TimerShowButtons)
                {
                    killTimer(m_TimerShowButtons);
                    m_TimerShowButtons = 0;
                }
                if (m_TimerHideButtons)
                    killTimer(m_TimerHideButtons);
                m_TimerHideButtons = startTimer(200);
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
                        item.fTouchPosS[j] = start_pos[j];
                        item.fTouchPosE[j] = end_pos[j];
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
                    item.fTouchPosS[j] = start_pos[j];
                    item.fTouchPosE[j] = end_pos[j];
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

            update();
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
                    update();
                    if (m_TimerHideButtons)
                    {
                        killTimer(m_TimerHideButtons);
                        m_TimerHideButtons = 0;
                    }
                    if (m_TimerShowButtons)
                        killTimer(m_TimerShowButtons);
                    m_TimerShowButtons = startTimer(1500);
                }
            }
        }
    }
}

static unsigned long mm_hex2u32( const char * pstr )
{
    const char * str = pstr;
    unsigned long u32Ret = 0;

    if ( str == NULL )
        return 0;

    while ( str[0] == ' ' || str[0] == '\t' )
        str++;

    while ( 1 )
    {
        if ( str[0] >= '0' && str[0] <= '9' )
            u32Ret = u32Ret * 16 + (*str++ - '0');
        else if ( str[0] >= 'A' && str[0] <= 'F' )
            u32Ret = u32Ret * 16 + (*str++ - 'A' + 10);
        else if ( str[0] >= 'a' && str[0] <= 'f' )
            u32Ret = u32Ret * 16 + (*str++ - 'a' + 10);
        else
            return u32Ret;
    }
}

void QBentAdjustmentDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    int nCamIndex = getIndexFromPart(Part);

    const char * pCur;
    if ( strstr(szCmd, cstrFactorialCamPos) == szCmd )
    {
        unsigned long dwA, dwB, dwC, dwD, dwE, dwF;
        float fA, fB, fC, fD, fE, fF;
        int nMode;
        pCur = szCmd + sizeof(cstrFactorialCamPos) - 1;

        dwA = mm_hex2u32(pCur);
        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            dwB = mm_hex2u32(pCur);
        else
            dwB = 0;
        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            dwC = mm_hex2u32(pCur);
        else
            dwC = 0;
        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            dwD = mm_hex2u32(pCur);
        else
            dwD = 0;
        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            dwE = mm_hex2u32(pCur);
        else
            dwE = 0;
        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            dwF = mm_hex2u32(pCur);
        else
            dwF = 0;

        if ( (pCur = strchr(pCur, ',') + 1) != (char *)1 )
            nMode = strtol(pCur, 0, 0);
        else
            nMode = 0;

        fA = *(float *)&dwA;
        fB = *(float *)&dwB;
        fC = *(float *)&dwC;
        fD = *(float *)&dwD;
        fE = *(float *)&dwE;
        fF = *(float *)&dwF;

        bool bExist = false;
        int nFoundItemPos;
        for ( int nI=0 ; nI<m_BentItemArray.size() ; nI++ )
        {
            const BentItem& item = m_BentItemArray.at(nI);
            if( item.nCameraIndex == nCamIndex )
            {
                nFoundItemPos = nI;
                bExist = true;
                break;
            }
        }

        if ( !bExist )
        {
            BentItem item;
            memset( &item, 0, sizeof(BentItem) );
            item.nCameraIndex = nCamIndex;
            item.fCamA = fA;
            item.fCamB = fB;
            item.fCamC = fC;
            item.fCamD = fD;
            item.fCamE = fE;
            item.fCamF = fF;
            item.mode = (unsigned char)nMode;

            item.fLastTouchPosS = item.fLastTouchPosE = item.fLastTouchPos = NaN;
            for( int i=0 ; i<ADJUSTMENT_STEP; i++ )
            {
                item.fObcS[i] = item.fObcE[i] = NaN;
                item.fObcCenter[i] = NaN;
            }

            m_BentItemArray.push_back( item );
        }
        else
        {
            BentItem& item = m_BentItemArray[nFoundItemPos];

            memset( &item, 0, sizeof(BentItem) );
            item.nCameraIndex = nCamIndex;
            item.fCamA = fA;
            item.fCamB = fB;
            item.fCamC = fC;
            item.fCamD = fD;
            item.fCamE = fE;
            item.fCamF = fF;
            item.mode = (unsigned char)nMode;

            item.fLastTouchPosS = item.fLastTouchPosE = item.fLastTouchPos = NaN;
            for( int i=0 ; i<ADJUSTMENT_STEP; i++ )
            {
                item.fObcS[i] = item.fObcE[i] = NaN;
                item.fObcCenter[i] = NaN;
            }
        }
        update();
    }
    else if ( strstr(szCmd, cstrAreaC) == szCmd )
    {
        pCur = szCmd + sizeof(cstrAreaC) - 1;

        int nAreaC = atoi(pCur);
        m_lClickArea = nAreaC * 2;

        if ( m_lClickArea < 300 )
        {
            m_lClickArea = 300;
        }
    }
}

void QBentAdjustmentDialog::onEditModified(QBorderStyleEdit* /*pEdit*/, int /*nValue*/, double /*dValue*/)
{
    updateData(true);
    update();
}

void QBentAdjustmentDialog::onRangeChange(QBorderStyleEdit* pEdit, int nMin, int nMax, int nStep)
{
    int nV = pEdit->toPlainText().toInt();
    nV += nStep;
    if (nV < nMin) nV = nMin;
    if (nV > nMax) nV = nMax;
    pEdit->setText( QString::number(nV) );

    updateData(true);
    update();
}

void QBentAdjustmentDialog::on_btnSave_clicked()
{

}

void QBentAdjustmentDialog::on_btnReset_clicked()
{

}

void QBentAdjustmentDialog::on_btnMarginLeftDec_clicked()
{
    onRangeChange(ui->txtEdtMarginLeft, 0, 100, -1);
}

void QBentAdjustmentDialog::on_btnMarginLeftInc_clicked()
{
    onRangeChange(ui->txtEdtMarginLeft, 0, 100, 1);
}

void QBentAdjustmentDialog::on_btnMaringUpDec_clicked()
{
    onRangeChange(ui->txtEdtMarginUp, 0, 100, -1);
}

void QBentAdjustmentDialog::on_btnMaringUpInc_clicked()
{
    onRangeChange(ui->txtEdtMarginUp, 0, 100, 1);
}

void QBentAdjustmentDialog::on_btnMarginRightDec_clicked()
{
    onRangeChange(ui->txtEdtMarginRight, 0, 100, -1);
}

void QBentAdjustmentDialog::on_btnMarginRightInc_clicked()
{
    onRangeChange(ui->txtEdtMarginRight, 0, 100, 1);
}

void QBentAdjustmentDialog::on_btnMarginDownDec_clicked()
{
    onRangeChange(ui->txtEdtMarginDown, 0, 100, -1);
}

void QBentAdjustmentDialog::on_btnMarginDownInc_clicked()
{
    onRangeChange(ui->txtEdtMarginDown, 0, 100, 1);
}

void QBentAdjustmentDialog::on_btnLeft_clicked()
{

}

void QBentAdjustmentDialog::on_btnUp_clicked()
{

}

void QBentAdjustmentDialog::on_btnRight_clicked()
{

}

void QBentAdjustmentDialog::on_btnDown_clicked()
{

}
