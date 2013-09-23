#include "QRemoteTouchMarkDialog.h"
#include "ui_QRemoteTouchMarkDialog.h"

#include <QPainter>
#include <math.h>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QSettings>
#include "QT3kDevice.h"
#include "T3kConstStr.h"
#include "t3kcomdef.h"
#include "dialog.h"

#define STATUS_HEIGHT		(16)
#define DEVICE_COORD		(0x7fff)

QRemoteTouchMarkDialog::QRemoteTouchMarkDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QRemoteTouchMarkDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
    flags |= Qt::WindowMinMaxButtonsHint;

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose);

    memset( m_StatusDatas, 0, sizeof(m_StatusDatas) );
    m_bShowMark = false;

    QT3kDevice* pDevice = QT3kDevice::instance();
    pDevice->setInstantMode( pDevice->getInstantMode() | T3K_HID_MODE_TOUCHPNT, 5000 );
}

QRemoteTouchMarkDialog::~QRemoteTouchMarkDialog()
{
    // save windows position
    QSettings winPosSettings( "Habilience", "T3kCfgFE" );
    winPosSettings.beginGroup("TouchMarkWindow");
    winPosSettings.setValue( "x", QVariant::fromValue(x()) );
    winPosSettings.setValue( "y", QVariant::fromValue(y()) );
    winPosSettings.setValue( "w", QVariant::fromValue(width()) );
    winPosSettings.setValue( "h", QVariant::fromValue(height()) );
    winPosSettings.endGroup();

    QT3kDevice* pDevice = QT3kDevice::instance();
    pDevice->setInstantMode( pDevice->getInstantMode() & (~T3K_HID_MODE_TOUCHPNT), 5000 );

    m_pMainDlg->onCloseTouchMarkDialog();
    delete ui;
}

void QRemoteTouchMarkDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rectBody( 0, STATUS_HEIGHT, width(), height()-STATUS_HEIGHT );

    p.save();

    p.setRenderHint(QPainter::Antialiasing);
    drawBackground(p);

    if (m_bShowMark)
    {
        QPoint point( m_ptMark.x() * rectBody.width() / DEVICE_COORD
                      , m_ptMark.y() * rectBody.height() / DEVICE_COORD + rectBody.y() );

        const int nMarkWH = rectBody.height()/30;
        m_rcMark = QRect( point.x()-nMarkWH, point.y()-nMarkWH, nMarkWH*2, nMarkWH*2 );
        QRect rectMark = m_rcMark;

        rectMark.adjust( 2, 2, -2, -2 );
        drawMark( p, rectMark );
    }
    drawTouchPoints( p );

    p.restore();
}

void QRemoteTouchMarkDialog::resizeEvent(QResizeEvent *evt)
{
    QDialog::resizeEvent(evt);
}

void QRemoteTouchMarkDialog::drawBackground(QPainter& p)
{
    QRect rc(0, 0, width(), height());

    p.fillRect( 0, 0, width(), height(), Qt::black );

    QRectF rectStatusF((float)rc.left(), (float)rc.top(), (float)rc.width(), (float)STATUS_HEIGHT);
    m_rcStatus = QRect( rc.left(), rc.top(), rc.width(), STATUS_HEIGHT );

    {
        p.setPen( QColor(180, 180, 180) );
        p.drawLine( rectStatusF.bottomLeft(), rectStatusF.bottomRight() );
        float fSW = rectStatusF.width() / 14.f;
        const QString strTitle[] = {"MX", "MY", "TX", "TY", "dX", "dY", "Count" };
        int nCnt = 0;
        float fX;
        int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;
        QFont fnt = font();
        fnt.setPixelSize( 8 );
        QString strValue;
        p.setFont(fnt);
        for ( fX=rectStatusF.left()+fSW ; fX<rectStatusF.right() ; fX+=fSW, nCnt++ )
        {
            p.drawLine( (int)fX, (int)rectStatusF.top(), (int)fX, (int)rectStatusF.bottom() );
            if ( nCnt % 2 == 0 )
                p.drawText( QRectF(fX-fSW, rectStatusF.top(), fSW, rectStatusF.height()), flags, strTitle[nCnt/2] );
            else
            {
                strValue = QString::number(m_StatusDatas[nCnt/2]);
                p.drawText( QRectF(fX-fSW, rectStatusF.top(), fSW, rectStatusF.height()), flags, strValue );
            }
        }
        strValue = QString::number(m_StatusDatas[nCnt/2]);
        p.drawText( QRectF(fX-fSW, rectStatusF.top(), fSW, rectStatusF.height()), flags, strValue );
    }

    QRectF rectBodyF((float)rc.left(), (float)rc.top()+STATUS_HEIGHT, (float)rc.width(), (float)rc.height()-STATUS_HEIGHT);

    QPen penCenterLine( QColor(255,255,200), 2.f );
    penCenterLine.setStyle(Qt::DashDotLine);

    p.setPen( penCenterLine );
    p.drawLine( rectBodyF.left(), rectBodyF.top()+rectBodyF.height()/2.f, rectBodyF.right(), rectBodyF.top()+rectBodyF.height()/2.f );
    p.drawLine( rectBodyF.left()+rectBodyF.width()/2.f, rectBodyF.top(), rectBodyF.left()+rectBodyF.width()/2.f, rectBodyF.bottom() );

    float fW16 = rectBodyF.width()/16.f;
    float fH16 = rectBodyF.height()/16.f;

    QPen penInter1( QColor(125, 125, 125), 0.5f );
    penInter1.setStyle( Qt::DashLine );
    QPen penInter2( QColor(135, 135, 135), 0.5f );

    int nCnt = 0;
    for ( float fX = rectBodyF.left()+fW16 ; fX<rectBodyF.right() ; fX += fW16, nCnt++ )
    {
        if ( nCnt == 7 ) continue;
        if ( ((nCnt+1) % 4) == 0 )
        {
            p.setPen( penInter2 );
            p.drawLine( fX, rectBodyF.top(), fX, rectBodyF.bottom() );
        }
        else
        {
            p.setPen( penInter1 );
            p.drawLine( fX, rectBodyF.top(), fX, rectBodyF.bottom() );
        }
    }
    nCnt=0;
    for ( float fY = rectBodyF.top()+fH16 ; fY<rectBodyF.bottom() ; fY += fH16, nCnt++ )
    {
        if ( nCnt == 7 ) continue;
        if ( ((nCnt+1) % 4) == 0 )
        {
            p.setPen( penInter2 );
            p.drawLine( rectBodyF.left(), fY, rectBodyF.right(), fY );
        }
        else
        {
            p.setPen( penInter1 );
            p.drawLine( rectBodyF.left(), fY, rectBodyF.right(), fY );
        }
    }

    QRectF rectTextF(rectBodyF.right()-rectBodyF.width()/4, rectBodyF.bottom()-rectBodyF.height()/20, rectBodyF.width()/4, rectBodyF.height()/20);

    int flags = Qt::AlignRight|Qt::AlignBottom;
    QFont fnt = font();
    fnt.setPixelSize(rectBodyF.height()/50);
    fnt.setBold(true);
    p.setFont(fnt);
    p.setPen( QColor(240,240,240,80) );
    p.drawText( rectTextF, flags, "Remote Screen" );
}

void QRemoteTouchMarkDialog::mouseDoubleClickEvent(QMouseEvent *evt)
{
    if (evt->button() != Qt::LeftButton)
        return;
    QRect rc( 0, 0, width(), height() );
    rc.adjust( 0, STATUS_HEIGHT, 0, 0 );

    QPoint point = evt->pos();
    if ( !rc.contains(point) ) return;

    if ( m_bShowMark )
        update( m_rcMark );

    m_StatusDatas[StatusMarkX] = (unsigned short)((point.x()) * DEVICE_COORD / rc.width());
    m_StatusDatas[StatusMarkY] = (unsigned short)((point.y()-rc.top()) * DEVICE_COORD / rc.height());
    m_ptMark.setX(m_StatusDatas[StatusMarkX]);
    m_ptMark.setY(m_StatusDatas[StatusMarkY]);
    m_StatusDatas[StatusDeltaX] = 0;
    m_StatusDatas[StatusDeltaY] = 0;
    update( m_rcStatus );

    const int nMarkWH = rc.height()/25;
    m_rcMark = QRect( point.x()-nMarkWH, point.y()-nMarkWH, nMarkWH*2, nMarkWH*2 );
    m_bShowMark = true;

    QT3kDevice* pDevice = QT3kDevice::instance();
    if ( pDevice->isVirtualDevice() )
    {
        FeatureCursorPos feature;
        feature.ReportID = REPORTID_FEATURE_CURSOR_POS;
        feature.IsScreenCoordinate = true;
        feature.Show = true;
        feature.TouchOK = false;
        feature.X = (unsigned short)m_ptMark.x();
        feature.Y = (unsigned short)m_ptMark.y();
        feature.Progress = 0;

        pDevice->setFeature( &feature, sizeof(FeatureCursorPos) );
    }

    update( m_rcMark );

}

void QRemoteTouchMarkDialog::mouseReleaseEvent(QMouseEvent *evt)
{
    if (evt->button() != Qt::RightButton)
        return;

    if ( m_bShowMark )
    {
        m_bShowMark = false;
        m_ptMark = QPoint(0, 0);
        m_StatusDatas[StatusMarkX] = 0;
        m_StatusDatas[StatusMarkY] = 0;
        m_StatusDatas[StatusDeltaX] = 0;
        m_StatusDatas[StatusDeltaY] = 0;
        update( m_rcStatus );
        update( m_rcMark );

        QT3kDevice* pDevice = QT3kDevice::instance();
        if ( pDevice->isVirtualDevice() )
        {
            FeatureCursorPos feature;
            feature.ReportID = REPORTID_FEATURE_CURSOR_POS;
            feature.IsScreenCoordinate = true;
            feature.Show = false;
            feature.TouchOK = false;
            feature.X = (unsigned short)m_ptMark.x();
            feature.Y = (unsigned short)m_ptMark.y();
            feature.Progress = 0;

            pDevice->setFeature( &feature, sizeof(FeatureCursorPos) );
        }
    }
}

void QRemoteTouchMarkDialog::drawTouchPoints( QPainter& p )
{
    p.setBrush( QColor(100, 255, 100, 120) );
    p.setPen( QColor(100, 255, 100, 240) );

    for ( int nI=0 ; nI<m_aryTouchPoints.size() ; nI++ )
    {
        const QPoint& pt = m_aryTouchPoints.at(nI);
        const QSize& size = m_aryTouchSizes.at(nI);

        p.drawEllipse( pt.x()-size.width()/2, pt.y()-size.height()/2, size.width(), size.height() );
    }
}

void QRemoteTouchMarkDialog::drawMark( QPainter& p, QRect& rectMark )
{
    p.setPen( QColor(255, 0, 0) );
    p.setBrush( Qt::NoBrush );

    float fCWH = rectMark.width() / 4.f;
    p.drawLine( rectMark.left()+rectMark.width()/2, rectMark.top(), rectMark.left()+rectMark.width()/2, rectMark.bottom() );
    p.drawLine( rectMark.left(), rectMark.top()+rectMark.height()/2, rectMark.right(), rectMark.top()+rectMark.height()/2 );

    QRectF rectInnerF( rectMark.left()+rectMark.width()/2.f-fCWH/2.f, rectMark.top()+rectMark.height()/2.f-fCWH/2.f, fCWH, fCWH );
    p.drawRect( rectInnerF );
}

void QRemoteTouchMarkDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    if ( strstr(szCmd, cstrInstantMode) == szCmd )
    {
        const char* szMode = szCmd + sizeof(cstrInstantMode) - 1;
        const char* pszC = strchr( szMode, ',' );
        bool bViewMode = false;
        for ( const char* pCur = szMode ; pCur < pszC ; pCur++ )
        {
            if( *pCur == 'V' )
            {
                bViewMode = true;
                break;
            }
        }

        QT3kDevice* pDevice = QT3kDevice::instance();
        if ( !bViewMode )
        {
            if ( (pDevice->getInstantMode() & T3K_HID_MODE_TOUCHPNT) == 0 )
                pDevice->setInstantMode( pDevice->getInstantMode() | T3K_HID_MODE_TOUCHPNT, 5000, 0 );
        }
        else
        {
            if ( (pDevice->getInstantMode() & T3K_HID_MODE_TOUCHPNT) != 0 )
                pDevice->setInstantMode( pDevice->getInstantMode() & (~T3K_HID_MODE_TOUCHPNT), 5000, 0 );
        }
    }
}

void QRemoteTouchMarkDialog::TPDP_OnTPT(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int count, int /*guess_cnt*/, t3ktouchpoint *points)
{
    QRect rc( 0, 0, width(), height());
    rc.adjust( 0, STATUS_HEIGHT, 0, 0 );

    QVector<QRect> aryInvalidRect;
    for ( int nI=0 ; nI<m_aryTouchPoints.size() ; nI++ )
    {
        const QPoint& pt = m_aryTouchPoints.at(nI);
        const QSize& size = m_aryTouchSizes.at(nI);

        QRect rectPoint(pt.x()-size.width()/2, pt.y()-size.height()/2, size.width(), size.height());
        rectPoint.adjust( -2, -2, 2, 2 );
        aryInvalidRect.push_back( rectPoint );
    }

    m_aryTouchPoints.clear();
    m_aryTouchSizes.clear();

    for ( int nI=0 ; nI<aryInvalidRect.size() ; nI++ )
    {
        update( aryInvalidRect.at(nI) );
    }

    for ( int i=1 ; i<count ; i++ )
    {
        if ( points[i].s != 0 )
        {
            QPoint pt;
            pt.setX(points[i].x * rc.width() / DEVICE_COORD);
            pt.setY(points[i].y * rc.height() / DEVICE_COORD + rc.top());
            m_aryTouchPoints.push_back( pt );
            QSize size;
            size.setWidth(points[i].w * rc.width() / DEVICE_COORD);
            size.setHeight(points[i].h * rc.height() / DEVICE_COORD);
            if ( size.width() <= 5 ) size.setWidth(5);
            if ( size.height() <= 5 ) size.setHeight(5);
            int nWH = (int)(hypot((double)size.width(), (double)size.height()) + 0.5);
            size = QSize(nWH, nWH);
            m_aryTouchSizes.push_back( size );
            QRect rectPoint(pt.x()-size.width()/2, pt.y()-size.height()/2, size.width(), size.height());
            rectPoint.adjust( -2, -2, 2, 2 );
            update( rectPoint );
        }
    }

    bool bRefreshStatus = false;
    if ( m_aryTouchPoints.size() != m_StatusDatas[StatusCount] )
    {
        m_StatusDatas[StatusCount] = m_aryTouchPoints.size();
        bRefreshStatus = true;
    }
    if ( m_aryTouchPoints.size() >= 1 )
    {
        QPoint ptTouch = m_aryTouchPoints.at(0);
        ptTouch.setX(ptTouch.x() * DEVICE_COORD / rc.width());
        ptTouch.setY((ptTouch.y()-rc.top()) * DEVICE_COORD / rc.height());
        if ( m_StatusDatas[StatusTouchX] != ptTouch.x() )
        {
            m_StatusDatas[StatusTouchX] = (unsigned short)ptTouch.x();
            bRefreshStatus = true;
        }
        if ( m_StatusDatas[StatusTouchY] != ptTouch.y() )
        {
            m_StatusDatas[StatusTouchY] = (unsigned short)ptTouch.y();
            bRefreshStatus = true;
        }
        if ( m_StatusDatas[StatusMarkX] != 0 )
        {
            unsigned short dX = qAbs(m_StatusDatas[StatusMarkX] - m_StatusDatas[StatusTouchX]);
            if ( m_StatusDatas[StatusDeltaX] != dX )
            {
                m_StatusDatas[StatusDeltaX] = dX;
                bRefreshStatus = true;
            }
        }
        if ( m_StatusDatas[StatusMarkY] != 0 )
        {
            unsigned short dY = qAbs(m_StatusDatas[StatusMarkY] - m_StatusDatas[StatusTouchY]);
            if ( m_StatusDatas[StatusDeltaY] != dY )
            {
                m_StatusDatas[StatusDeltaY] = dY;
                bRefreshStatus = true;
            }
        }
    }

    if ( bRefreshStatus )
    {
        update( m_rcStatus );
    }
}
