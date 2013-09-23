#include "QAreaSettingForm.h"
#include "ui_QAreaSettingForm.h"

#include "QBorderStyleEdit.h"
#include "QUtils.h"
#include <QPainter>
#include <QResizeEvent>
#include "T3kConstStr.h"
#include <QScreen>
#include <QtNumeric>
#include <math.h>
#include "QT3kDevice.h"
#include "QLogSystem.h"

const int s_nTitleHeight = 45;

#define USER_AREA_RANGE_START	(0x0000)
#define USER_AREA_RANGE_END		(0x7fff)
#define	USER_AREA_RANGE_MAX		(USER_AREA_RANGE_END)

#define TOUCH_RESOLUTION		(0x7fff)

#define RoundP1(A)          (float)((long int) (((A)+.05) * 10))/10;

#define RES_TAG     "TOUCH SETTING"

QAreaSettingForm::QAreaSettingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QAreaSettingForm)
{
    ui->setupUi(this);

    m_bIsModified = false;

    DISABLE_MSWINDOWS_TOUCH_PROPERTY;

    QBorderStyleEdit* edits[] = {
        ui->txtEdtAreaClick, ui->txtEdtAreaDblClick, ui->txtEdtAreaPalm, ui->txtEdtAreaPnT
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrNormalBorderColor, s_clrNormalBgColor);
        edits[i]->setText("");
        edits[i]->setAlignment(Qt::AlignRight);
        //edits[i]->installEventFilter(&m_EventRedirect);
        connect( edits[i], SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    }

    m_dPercentSingleClick = 0.0;
    m_dPercentDoubleClick = 0.0;
    m_dPercentPalm = 0.0;
    m_dPercentPutNTap = 0.0;
}

QAreaSettingForm::~QAreaSettingForm()
{
    delete ui;
}

void QAreaSettingForm::setModified(bool bModified)
{
    m_bIsModified = bModified;
    if (m_bIsModified)
    {
        setModifyAllEditColors();
    }
    else
    {
        resetEditColors();
    }
}

void QAreaSettingForm::resetEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtAreaClick, ui->txtEdtAreaDblClick, ui->txtEdtAreaPalm, ui->txtEdtAreaPnT
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrNormalBorderColor, s_clrNormalBgColor);
        edits[i]->update();
    }

    m_bIsModified = false;
}

void QAreaSettingForm::setModifyAllEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtAreaClick, ui->txtEdtAreaDblClick, ui->txtEdtAreaPalm, ui->txtEdtAreaPnT
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
        edits[i]->update();
    }
}

void QAreaSettingForm::setModifyEditColor(QBorderStyleEdit* pEdit)
{
    pEdit->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
    pEdit->update();
}

void QAreaSettingForm::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.save();

    p.setRenderHint(QPainter::Antialiasing);

    QLangRes& res = QLangManager::getResource();
    QString strSingleClick = res.getResString( RES_TAG, "TEXT_SINGLE_CLICK" );
    QString strDoubleClick = res.getResString( RES_TAG, "TEXT_DOUBLE_CLICK" );
    QString strPalm = res.getResString( RES_TAG, "TEXT_PALM" );
    QString strPutNTap = res.getResString( RES_TAG, "TEXT_PUTNTAP" );

    drawSectionTitle( p, m_rcSingleClick, strSingleClick, ui->txtEdtAreaClick->isEnabled() );
    drawSectionTitle( p, m_rcDoubleClick, strDoubleClick, ui->txtEdtAreaDblClick->isEnabled() );
    drawSectionTitle( p, m_rcPalm, strPalm, ui->txtEdtAreaPalm->isEnabled() );
    drawSectionTitle( p, m_rcPutNTap, strPutNTap, ui->txtEdtAreaPnT->isEnabled() );

    drawCircleArea( p, m_rcSingleClick, ui->txtEdtAreaClick );
    drawCircleArea( p, m_rcDoubleClick, ui->txtEdtAreaDblClick );
    drawCircleArea( p, m_rcPalm, ui->txtEdtAreaPalm );
    drawLinearArea( p, m_rcPutNTap, ui->txtEdtAreaPnT );

    drawTouchCircle( p );

    p.restore();
}

void QAreaSettingForm::drawSectionTitle( QPainter& p, QRect& rcSection, const QString& strTitle, bool bEnabled )
{
    QRect rcBody = rcSection;
    QRect rcTitle = rcBody;
    rcTitle.setHeight(s_nTitleHeight);

    QLinearGradient grad( rcBody.topLeft(), rcBody.bottomLeft() );
    grad.setColorAt( 0.f, QColor(0x99, 0xBB, 0xE8, 30) );
    grad.setColorAt( 1.f, QColor(0x99, 0xBB, 0xE8, 0) );
    QBrush titleBrush(grad);
    QPen bodyShadowPen1( QBrush(QColor(160,160,160,50)), 2.f );
    QPen bodyShadowPen2( QBrush(QColor(160,160,160,50)), 4.f );

    p.setBrush( Qt::NoBrush );
    p.setPen( bodyShadowPen2 );
    p.drawRect( rcBody );
    p.setPen( bodyShadowPen1 );
    p.drawRect( rcBody );

    p.setPen( QColor(0x99, 0xBB, 0xE8) );
    p.setBrush( titleBrush );
    p.drawRect( rcBody );

    QRectF rcText = rcTitle;
    rcText.adjust( 10.f, 3.f, -10.f, -3.f );

    QPen horzPen1( QColor(200,200,200) );
    QVector<qreal> dashPattern;
    qreal space = 3.f;
    dashPattern << 3.f << space;
    horzPen1.setStyle(Qt::CustomDashLine);
    horzPen1.setDashPattern(dashPattern);
    QPen horzPen2( QColor(255,255,255) );
    horzPen2.setStyle(Qt::CustomDashLine);
    horzPen2.setDashPattern(dashPattern);

    p.setPen( horzPen2 );
    p.drawLine( rcText.x()+1, rcText.bottom()+1, rcText.right()-1, rcText.bottom()+1 );
    p.setPen( horzPen1 );
    p.drawLine( rcText.x(), rcText.bottom(), rcText.right(), rcText.bottom() );

    int flags = Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine;
    QFont fnt = font();
    fnt.setPointSize(10);
    fnt.setBold(true);
    p.setFont( fnt );
    if (bEnabled)
        p.setPen(QColor(55, 100, 160));
    else
        p.setPen(QColor(100, 100, 100));
    p.drawText( rcText, flags, strTitle );
}

void QAreaSettingForm::drawCircleArea( QPainter& p, QRect& rcSection, QBorderStyleEdit* edit )
{
    float fV = edit->toPlainText().toFloat();

    QRect rcWin = parentWidget()->geometry();

    int nAreaWH = int( rcWin.width() * fV / 100.f );

    QRect rectC = rcSection;
    rectC.setY(rectC.y() + s_nTitleHeight);
    rectC.setHeight(rectC.height() - s_nTitleHeight);

    int nCX = rectC.x() + rectC.width() / 2;
    int nCY = rectC.y() + rectC.height() / 2;

    p.setClipRect( rectC );

    QRect rectDraw;
    rectDraw.setX(nCX - nAreaWH/2);
    rectDraw.setY(nCY - nAreaWH/2);
    rectDraw.setWidth(nAreaWH);
    rectDraw.setHeight(nAreaWH);

    QPen borderPen( QColor(100, 100, 100, 180) );
    QPen innerShadowPen( QBrush(QColor(255, 255, 255, 100)), 5.f );
    QPen outerShadowPen( QBrush(QColor(100, 100, 100, 100)), 5.f );
    p.setPen(outerShadowPen);
    p.setBrush( QColor(200,200,200,200) );
    p.drawEllipse( rectDraw );
    p.setBrush(Qt::NoBrush);
    p.setPen( innerShadowPen );
    p.drawEllipse( rectDraw );
    p.setPen( borderPen );
    p.drawEllipse( rectDraw );

    p.setClipRect( rectC, Qt::NoClip );
}

void QAreaSettingForm::drawLinearArea( QPainter& p, QRect& rcSection, QBorderStyleEdit* edit )
{
    float fV = edit->toPlainText().toFloat();
    QRect rcWin = parentWidget()->geometry();

    int nAreaWH = int( rcWin.width() * fV / 100.f );

    QRect rectC = rcSection;
    rectC.setY(rectC.y() + s_nTitleHeight);
    rectC.setHeight(rectC.height() - s_nTitleHeight);

    int nCX = rectC.x() + rectC.width() / 2;
    int nCY = rectC.y() + rectC.height() / 2;

    p.setClipRect( rectC );

    QRect rectDraw1;
    rectDraw1.setX(nCX - nAreaWH/2 - 5);
    rectDraw1.setY(nCY - 5);
    rectDraw1.setWidth(10);
    rectDraw1.setHeight(10);

    QRect rectDraw2;
    rectDraw2.setX(nCX + nAreaWH/2 - 5);
    rectDraw2.setY(nCY - 5);
    rectDraw2.setWidth(10);
    rectDraw2.setHeight(10);

    QPen borderPen( QColor(100, 100, 100, 180) );
    QPen innerShadowPen( QColor(255, 255, 255, 100), 5.f );
    QPen outerShadowPen( QColor(100, 100, 100, 100), 5.f );

    QVector<qreal> dashPattern;
    dashPattern << 0.5f << 2.0f;
    QPainterPathStroker stroker;
    stroker.setWidth( 3.f );
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setDashPattern(dashPattern);
    stroker.setDashOffset(1.f);

    QPainterPath path;
    path.moveTo(nCX - nAreaWH/2, nCY);
    path.lineTo(nCX + nAreaWH/2, nCY);

    QPainterPath stroke = stroker.createStroke(path);
    p.fillPath( stroke, QColor(100, 100, 100, 100) );

    drawArrowLine( p, QPointF(nCX - nAreaWH/2, nCY), QPointF(nCX - nAreaWH*3/4, nCY), QColor(100,100,100,100), 8.f );
    drawArrowLine( p, QPointF(nCX + nAreaWH/2, nCY), QPointF(nCX + nAreaWH*3/4, nCY), QColor(100,100,100,100), 8.f );

    p.setPen( outerShadowPen );
    p.setBrush( QColor(200,200,200,200) );
    p.drawEllipse( rectDraw1 );
    p.drawEllipse( rectDraw2 );

    p.setBrush( Qt::NoBrush );
    p.setPen( innerShadowPen );
    p.drawEllipse( rectDraw1 );
    p.drawEllipse( rectDraw2 );
    p.setPen( borderPen );
    p.drawEllipse( rectDraw1 );
    p.drawEllipse( rectDraw2 );

    p.setClipRect( rectC, Qt::NoClip );
}

void QAreaSettingForm::drawArrowLine( QPainter& p, const QPointF& pt1, const QPointF& pt2, const QColor& clrLine, float fLineWidth )
{
    QBrush arrowBrush( clrLine );
    QPen linePen( QBrush(clrLine), fLineWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin );

    p.setPen( linePen );
    p.drawLine( pt1, pt2 );

    const float fHeadSize = fLineWidth*1.8f;

    //QPen arrowPen( QBrush(clrLine), 1.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
    p.setPen( Qt::NoPen );
    p.setBrush( arrowBrush );

    const float fRatioY = 0.55f;
    // Draw the arrow head
    QPainterPath path;
    if (pt1.x() < pt2.x())
    {
        path.moveTo( pt2.x()+fHeadSize, pt2.y() );
        path.lineTo( pt2.x(), pt2.y()+fHeadSize*fRatioY );
        path.lineTo( pt2.x(), pt2.y()-fHeadSize*fRatioY );
        path.lineTo( pt2.x()+fHeadSize, pt2.y() );
    }
    else
    {
        path.moveTo( pt2.x()-fHeadSize, pt2.y() );
        path.lineTo( pt2.x(), pt2.y()+fHeadSize*fRatioY );
        path.lineTo( pt2.x(), pt2.y()-fHeadSize*fRatioY );
        path.lineTo( pt2.x()-fHeadSize, pt2.y() );
    }
    p.drawPath( path );
}
void QAreaSettingForm::drawDashedRoundAnchorLine( QPainter& p, const QPointF &pt1, const QPointF &pt2 , const QColor &clrLine, float fLineWidth)
{
    const float fRatio = 0.9f;

    QVector<qreal> dashPattern;
    dashPattern << 0.5f << 2.0f;

    QPainterPathStroker stroker;
    stroker.setWidth( fLineWidth );
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setDashPattern(dashPattern);
    stroker.setDashOffset(2.f);

    QPainterPath path;
    path.moveTo(pt1.x()+fLineWidth*1.5f, pt1.y());
    path.lineTo(pt2.x()-fLineWidth*1.5f, pt1.y());

    QPainterPath stroke = stroker.createStroke(path);
    p.fillPath( stroke, clrLine );

    p.setPen(Qt::NoPen);
    QBrush brush( clrLine );
    p.setBrush(brush);

    p.drawEllipse( pt1, fLineWidth*fRatio, fLineWidth*fRatio );
    p.drawEllipse( pt2, fLineWidth*fRatio, fLineWidth*fRatio );
}

void QAreaSettingForm::drawTouchCircle( QPainter& p )
{
    QLangRes& res = QLangManager::getResource();
    QString strAreaText = res.getResString(RES_TAG, "TEXT_AREA");
    QFont fntText = font();
    fntText.setPointSize(10);
    fntText.setBold(true);
    int flags = Qt::AlignRight|Qt::AlignVCenter|Qt::TextSingleLine;

    p.setFont(fntText);
    char szTemp[64];
    const int nTextMarginRight = 5;

    if (ui->txtEdtAreaClick->isEnabled())
    {
        if ( !m_rcDrawSingleClick.isEmpty() )
        {
            QRect rcArea( m_rcSingleClick );
            rcArea.setY( rcArea.y()+s_nTitleHeight );
            rcArea.setHeight( rcArea.height()-s_nTitleHeight );

            p.setClipRect( rcArea );

            int nCX = rcArea.x() + rcArea.width()/2;
            int nCY = rcArea.y() + rcArea.height()/2;

            rcArea.setX(nCX - m_rcDrawSingleClick.width()/2);
            rcArea.setY(nCY - m_rcDrawSingleClick.height()/2);
            rcArea.setWidth(m_rcDrawSingleClick.width());
            rcArea.setHeight(m_rcDrawSingleClick.height());

            p.setBrush( QColor(255,255,255,128) );
            p.setPen( Qt::NoPen );
            p.drawEllipse( rcArea );

            QPen pen( QColor(255,0,0,128), 2.f );
            pen.setStyle(Qt::DashLine);
            p.setPen(pen);
            p.setBrush( Qt::NoBrush );
            p.drawEllipse( rcArea );

            p.setClipRect( rcArea, Qt::NoClip );
        }

        if (m_dPercentSingleClick != 0.0)
        {
            snprintf( szTemp, 64, ": %.2lf", m_dPercentSingleClick );
            QString str = strAreaText + szTemp;

            QRectF rcText = m_rcSingleClick;
            rcText.setY( rcText.y() + s_nTitleHeight );
            rcText.setHeight( s_nTitleHeight );
            rcText.adjust(0, 0, -nTextMarginRight, 0);

            p.setPen( QColor(55, 100, 160) );
            p.drawText( rcText, flags, str );
        }
    }

    if (ui->txtEdtAreaDblClick->isEnabled())
    {
        if ( !m_rcDrawDoubleClick.isEmpty() )
        {
            QRect rcArea( m_rcDoubleClick );
            rcArea.setY( rcArea.y()+s_nTitleHeight );
            rcArea.setHeight( rcArea.height()-s_nTitleHeight );

            p.setClipRect( rcArea );

            int nCX = rcArea.x() + rcArea.width()/2;
            int nCY = rcArea.y() + rcArea.height()/2;

            rcArea.setX(nCX - m_rcDrawDoubleClick.width()/2);
            rcArea.setY(nCY - m_rcDrawDoubleClick.height()/2);
            rcArea.setWidth(m_rcDrawDoubleClick.width());
            rcArea.setHeight(m_rcDrawDoubleClick.height());

            p.setBrush( QColor(255,255,255,128) );
            p.setPen( Qt::NoPen );
            p.drawEllipse( rcArea );

            QPen pen( QColor(255,0,0,128), 2.f );
            pen.setStyle(Qt::DashLine);
            p.setPen(pen);
            p.setBrush( Qt::NoBrush );
            p.drawEllipse( rcArea );

            p.setClipRect( rcArea, Qt::NoClip );
        }

        if (m_dPercentDoubleClick != 0.0)
        {
            snprintf( szTemp, 64, ": %.2lf", m_dPercentDoubleClick );
            QString str = strAreaText + szTemp;

            QRectF rcText = m_rcDoubleClick;
            rcText.setY( rcText.y() + s_nTitleHeight );
            rcText.setHeight( s_nTitleHeight );
            rcText.adjust(0, 0, -nTextMarginRight, 0);

            p.setPen( QColor(55, 100, 160) );
            p.drawText( rcText, flags, str );
        }
    }

    if (ui->txtEdtAreaPalm->isEnabled())
    {
        if ( !m_rcDrawPalm.isEmpty() )
        {
            QRect rcArea( m_rcPalm );
            rcArea.setY( rcArea.y()+s_nTitleHeight );
            rcArea.setHeight( rcArea.height()-s_nTitleHeight );

            p.setClipRect( rcArea );

            int nCX = rcArea.x() + rcArea.width()/2;
            int nCY = rcArea.y() + rcArea.height()/2;

            rcArea.setX(nCX - m_rcDrawPalm.width()/2);
            rcArea.setY(nCY - m_rcDrawPalm.height()/2);
            rcArea.setWidth(m_rcDrawPalm.width());
            rcArea.setHeight(m_rcDrawPalm.height());

            p.setBrush( QColor(255,255,255,128) );
            p.setPen( Qt::NoPen );
            p.drawEllipse( rcArea );

            QPen pen( QColor(255,0,0,128), 2.f );
            pen.setStyle(Qt::DashLine);
            p.setPen(pen);
            p.setBrush( Qt::NoBrush );
            p.drawEllipse( rcArea );

            p.setClipRect( rcArea, Qt::NoClip );
        }

        if (m_dPercentPalm != 0.0)
        {
            snprintf( szTemp, 64, ": %.2lf", m_dPercentPalm );
            QString str = strAreaText + szTemp;

            QRectF rcText = m_rcPalm;
            rcText.setY( rcText.y() + s_nTitleHeight );
            rcText.setHeight( s_nTitleHeight );
            rcText.adjust(0, 0, -nTextMarginRight, 0);

            p.setPen( QColor(55, 100, 160) );
            p.drawText( rcText, flags, str );
        }
    }

    if (ui->txtEdtAreaPnT->isEnabled())
    {
        if ( !m_rcDrawPutNTap.isEmpty() )
        {
            QRect rcArea( m_rcPutNTap );
            rcArea.setY( rcArea.y()+s_nTitleHeight );
            rcArea.setHeight( rcArea.height()-s_nTitleHeight );

            p.setClipRect( rcArea );

            int nCX = rcArea.x() + rcArea.width()/2;
            int nCY = rcArea.y() + rcArea.height()/2;

            rcArea.setX(nCX - m_rcDrawPalm.width()/2);
            rcArea.setY(nCY - m_rcDrawPalm.height()/2);
            rcArea.setWidth(m_rcDrawPalm.width());
            rcArea.setHeight(m_rcDrawPalm.height());

            drawDashedRoundAnchorLine( p, QPointF(nCX - m_rcDrawPutNTap.width()/2, nCY), QPointF(nCX + m_rcDrawPutNTap.width()/2, nCY), QColor(255,0,0,128), 6.f );

            p.setClipRect( rcArea, Qt::NoClip );
        }

        if (m_dPercentPutNTap != 0.0)
        {
            snprintf( szTemp, 64, ": %.2lf", m_dPercentPutNTap );
            QString str = strAreaText + szTemp;

            QRectF rcText = m_rcPutNTap;
            rcText.setY( rcText.y() + s_nTitleHeight );
            rcText.setHeight( s_nTitleHeight );
            rcText.adjust(0, 0, -nTextMarginRight, 0);

            p.setPen( QColor(55, 100, 160) );
            p.drawText( rcText, flags, str );
        }
    }
}

void QAreaSettingForm::resizeEvent(QResizeEvent *evt)
{
    if (evt->size().width() <= 0 || evt->size().height() <= 0)
        return;

    QRect rcBody(0, 0, evt->size().width(), evt->size().height());

    int nPutNTapHeight = rcBody.height() / 4;
    int nClickHeight = (rcBody.height() - nPutNTapHeight) / 2;

    m_rcSingleClick.setX(rcBody.x());
    m_rcSingleClick.setWidth(rcBody.width() * 2 / 5);
    m_rcSingleClick.setY(rcBody.y());
    m_rcSingleClick.setHeight(nClickHeight);

    m_rcDoubleClick.setX(rcBody.x());
    m_rcDoubleClick.setWidth(rcBody.width() * 2 / 5);
    m_rcDoubleClick.setY(rcBody.y() + nClickHeight);
    m_rcDoubleClick.setHeight(nClickHeight);

    m_rcPalm.setX(rcBody.x() + rcBody.width() * 2 / 5);
    m_rcPalm.setWidth(rcBody.width() * 3 / 5);
    m_rcPalm.setY(rcBody.y());
    m_rcPalm.setHeight(rcBody.height() - nPutNTapHeight);

    m_rcPutNTap.setX(rcBody.x());
    m_rcPutNTap.setWidth(rcBody.width());
    m_rcPutNTap.setY(rcBody.y() + rcBody.height() - nPutNTapHeight);
    m_rcPutNTap.setHeight(nPutNTapHeight);

    m_rcSingleClick.adjust( 3, 3, -3, -3 );
    m_rcDoubleClick.adjust( 3, 3, -3, -3 );
    m_rcPalm.adjust( 3, 3, -3, -3 );
    m_rcPutNTap.adjust( 3, 3, -3, -3 );

    adjustControls( m_rcSingleClick, ui->btnAreaClickLeft, ui->txtEdtAreaClick, ui->btnAreaClickRight );
    adjustControls( m_rcDoubleClick, ui->btnAreaDblClickLeft, ui->txtEdtAreaDblClick, ui->btnAreaDblClickRight );
    adjustControls( m_rcPalm, ui->btnAreaPalmLeft, ui->txtEdtAreaPalm, ui->btnAreaPalmRight );
    adjustControls( m_rcPutNTap, ui->btnAreaPnTLeft, ui->txtEdtAreaPnT, ui->btnAreaPnTRight );
}

void QAreaSettingForm::onChangeLanguage()
{

}

void QAreaSettingForm::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    char szTemp[64];
    if ( strstr(szCmd, cstrAreaC) == szCmd )
    {
        int nArea = atoi(szCmd + sizeof(cstrAreaC) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        snprintf( szTemp, 64, "%.1f", fArea );
        ui->txtEdtAreaClick->setText( szTemp );
        update( m_rcSingleClick );
    }
    else if ( strstr(szCmd, cstrAreaD) == szCmd )
    {
        int nArea = atoi(szCmd + sizeof(cstrAreaD) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        snprintf( szTemp, 64, "%.1f", fArea );
        ui->txtEdtAreaDblClick->setText( szTemp );
        update( m_rcDoubleClick );
    }
    else if ( strstr(szCmd, cstrAreaM) == szCmd )
    {
        int nArea = atoi(szCmd + sizeof(cstrAreaM) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        snprintf( szTemp, 64, "%.1f", fArea );
        ui->txtEdtAreaPnT->setText( szTemp );
        update( m_rcPutNTap );
    }
    else if ( strstr(szCmd, cstrAreaP) == szCmd )
    {
        int nArea = atoi(szCmd + sizeof(cstrAreaP) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        snprintf( szTemp, 64, "%.1f", fArea );
        ui->txtEdtAreaPalm->setText( szTemp );
        update( m_rcPalm );
    }
}

void QAreaSettingForm::TPDP_OnTPT(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int count, int guess_cnt, t3ktouchpoint *points)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen && points)
    {
        QSize sizeScreen = screen->size();
        int nX = points[0].x * sizeScreen.width()/(TOUCH_RESOLUTION);
        int nY = points[0].y * sizeScreen.height()/(TOUCH_RESOLUTION);
        int nW = points[0].w * sizeScreen.width()/(TOUCH_RESOLUTION);
        int nH = points[0].h * sizeScreen.height()/(TOUCH_RESOLUTION);

        if (!nX && !nY && !nW && !nH && !m_bCheckTouch)
            return;

        int ni;
        points[0].s = 0;
        for ( ni = 1; ni < count; ni++ )
        {
            if ( points[0].s < points[ni].s )
                points[0].s = points[ni].s;
        }
        int nStatus = points[0].s;

        switch (nStatus)
        {
        case 0:
            if ( m_bCheckTouch )
            {
                onTouchLift();
                m_bCheckTouch = false;
            }
            break;
        case 1:
            break;
        case 2:
            {
                QPoint ptCenter(0, 0);
                int nCnt = 0;
                int nL = 0;
                for ( int i=1 ; i<count ; i++ )
                {
                    if( points[i].s != 0 )
                    {
                        QPoint pt(points[i].x * sizeScreen.width()/(TOUCH_RESOLUTION), points[i].y * sizeScreen.height()/(TOUCH_RESOLUTION));
                        ptCenter += pt;
                        nCnt++;
                    }
                }
                if ( nCnt != 0 )
                {
                    ptCenter /= nCnt;
                }

                // for put & tap
                if ( guess_cnt == 2 )
                {
                    for ( int i=1 ; i<count ; i++ )
                    {
                        if ( points[i].s != 0 )
                        {
                            QPoint pt(points[i].x * sizeScreen.width()/(TOUCH_RESOLUTION), points[i].y * sizeScreen.height()/(TOUCH_RESOLUTION));
                            int nW2 = qAbs(pt.x() - ptCenter.x());
                            int nH2 = qAbs(pt.y() - ptCenter.y());
                            if ( (nW2 != 0) || (nH2 != 0) )
                            {
                                int _nL = (int)hypotf( (float)nW2, (float)nH2 );
                                nL += _nL;
                            }
                        }
                    }
                }

                double dPercent = double(points[0].w) / TOUCH_RESOLUTION * 100.;
                onTouchDown( ptCenter, nW, nH, nL, dPercent );
                m_bCheckTouch = true;
            }
            break;
        default:
            break;
        }
    }
}

void QAreaSettingForm::onTouchDown(QPoint pt, int nW, int nH, int nL, double dPercent)
{
    QScreen* screen = QGuiApplication::primaryScreen();

    int nDiameter = nW > nH ? nW : nH;
    QPoint ptLT = mapFromGlobal(pt);
    QRect rcSC = m_rcSingleClick;
    rcSC.setY( rcSC.y() + s_nTitleHeight );
    rcSC.setHeight( rcSC.height() - s_nTitleHeight );

    if (rcSC.contains(ptLT.x(), ptLT.y()))
    {
        m_rcDrawSingleClick.setX(ptLT.x()-nDiameter/2);
        m_rcDrawSingleClick.setY(ptLT.y()-nDiameter/2);
        m_rcDrawSingleClick.setWidth(nDiameter);
        m_rcDrawSingleClick.setHeight(nDiameter);
        m_dPercentSingleClick = dPercent;

        update( m_rcSingleClick );
    }
    else
    {
        m_rcDrawSingleClick.setX(0);
        m_rcDrawSingleClick.setY(0);
        m_rcDrawSingleClick.setWidth(0);
        m_rcDrawSingleClick.setHeight(0);
        m_dPercentSingleClick = 0.0;
    }

    rcSC = m_rcDoubleClick;
    rcSC.setY( rcSC.y() + s_nTitleHeight );
    rcSC.setHeight( rcSC.height() - s_nTitleHeight );

    if (rcSC.contains(ptLT.x(), ptLT.y()))
    {
        m_rcDrawDoubleClick.setX(ptLT.x()-nDiameter/2);
        m_rcDrawDoubleClick.setY(ptLT.y()-nDiameter/2);
        m_rcDrawDoubleClick.setWidth(nDiameter);
        m_rcDrawDoubleClick.setHeight(nDiameter);
        m_dPercentDoubleClick = dPercent;

        update( m_rcDoubleClick );
    }
    else
    {
        m_rcDrawDoubleClick.setX(0);
        m_rcDrawDoubleClick.setY(0);
        m_rcDrawDoubleClick.setWidth(0);
        m_rcDrawDoubleClick.setHeight(0);
        m_dPercentDoubleClick = 0.0;
    }

    rcSC = m_rcPalm;
    rcSC.setY( rcSC.y() + s_nTitleHeight );
    rcSC.setHeight( rcSC.height() - s_nTitleHeight );

    if (rcSC.contains(ptLT.x(), ptLT.y()))
    {
        m_rcDrawPalm.setX(ptLT.x()-nDiameter/2);
        m_rcDrawPalm.setY(ptLT.y()-nDiameter/2);
        m_rcDrawPalm.setWidth(nDiameter);
        m_rcDrawPalm.setHeight(nDiameter);
        m_dPercentPalm = dPercent;

        update( m_rcPalm );
    }
    else
    {
        m_rcDrawPalm.setX(0);
        m_rcDrawPalm.setY(0);
        m_rcDrawPalm.setWidth(0);
        m_rcDrawPalm.setHeight(0);
        m_dPercentPalm = 0.0;
    }

    rcSC = m_rcPutNTap;
    rcSC.setY( rcSC.y() + s_nTitleHeight );
    rcSC.setHeight( rcSC.height() - s_nTitleHeight );

    if (rcSC.contains(ptLT.x(), ptLT.y()))
    {
        m_rcDrawPutNTap.setX(ptLT.x()-nL/2);
        m_rcDrawPutNTap.setY(ptLT.y()-nDiameter/2);
        m_rcDrawPutNTap.setWidth(nL);
        m_rcDrawPutNTap.setHeight(nDiameter);
        dPercent = double(nL) / screen->size().width() * 100.0;
        m_dPercentPutNTap = dPercent;

        update( m_rcPutNTap );
    }
    else
    {
        m_rcDrawPutNTap.setX(0);
        m_rcDrawPutNTap.setY(0);
        m_rcDrawPutNTap.setWidth(0);
        m_rcDrawPutNTap.setHeight(0);
        m_dPercentPutNTap = 0.0;
    }
}

void QAreaSettingForm::onTouchLift()
{
    m_rcDrawSingleClick.setX(0);
    m_rcDrawSingleClick.setY(0);
    m_rcDrawSingleClick.setWidth(0);
    m_rcDrawSingleClick.setHeight(0);
    m_dPercentSingleClick = 0.0;

    m_rcDrawDoubleClick.setX(0);
    m_rcDrawDoubleClick.setY(0);
    m_rcDrawDoubleClick.setWidth(0);
    m_rcDrawDoubleClick.setHeight(0);
    m_dPercentDoubleClick = 0.0;

    m_rcDrawPalm.setX(0);
    m_rcDrawPalm.setY(0);
    m_rcDrawPalm.setWidth(0);
    m_rcDrawPalm.setHeight(0);
    m_dPercentPalm = 0.0;

    m_rcDrawPutNTap.setX(0);
    m_rcDrawPutNTap.setY(0);
    m_rcDrawPutNTap.setWidth(0);
    m_rcDrawPutNTap.setHeight(0);
    m_dPercentPutNTap = 0.0;

    update( m_rcSingleClick );
    update( m_rcDoubleClick );
    update( m_rcPalm );
    update( m_rcPutNTap );
}

void QAreaSettingForm::enableControlsWithoutSingleClick(bool bEnable)
{
    QWidget* widgets[] = {
        ui->txtEdtAreaDblClick, ui->txtEdtAreaPalm, ui->txtEdtAreaPnT,
        ui->btnAreaDblClickLeft, ui->btnAreaDblClickRight,
        ui->btnAreaPalmLeft, ui->btnAreaPalmRight,
        ui->btnAreaPnTLeft, ui->btnAreaPnTRight
    };
    for ( int i=0 ; i<(int)(sizeof(widgets)/sizeof(QWidget*)) ; i++ )
    {
        widgets[i]->setEnabled(bEnable);
    }
    update();
}

void QAreaSettingForm::adjustControls( QRect& rcSection, QPushButton* btnDec, QBorderStyleEdit* edit, QPushButton* btnInc )
{
    int nOffsetX = rcSection.right() - 8;

    QPoint pos;
    QSize  size;
    pos = btnInc->parentWidget()->mapToGlobal(btnInc->pos());
    pos = mapFromGlobal(pos);
    size = btnInc->size();
    btnInc->move( nOffsetX-size.width(), rcSection.top()+(s_nTitleHeight-size.height())/2 );
    nOffsetX -= size.width();

    pos = edit->parentWidget()->mapToGlobal(edit->pos());
    pos = mapFromGlobal(pos);
    size = edit->size();
    edit->move( nOffsetX-size.width(), rcSection.top()+(s_nTitleHeight-size.height())/2 );
    nOffsetX -= size.width();

    pos = btnDec->parentWidget()->mapToGlobal(btnDec->pos());
    pos = mapFromGlobal(pos);
    size = btnDec->size();
    btnDec->move( nOffsetX-size.width(), rcSection.top()+(s_nTitleHeight-size.height())/2 );
}

void QAreaSettingForm::reject()
{

}

void QAreaSettingForm::accept()
{

}

void QAreaSettingForm::onEditModified(QBorderStyleEdit* pEdit, int /*nValue*/, double /*dValue*/)
{
    setModifyEditColor( pEdit );
    m_bIsModified = true;

    QT3kDevice* pDevice = QT3kDevice::instance();
    QString strCmd;
    if (pEdit == ui->txtEdtAreaClick)
    {
        LOG_I( "Manual Edit Click Area" );
        int value = (int)(ui->txtEdtAreaClick->toPlainText().toFloat() * USER_AREA_RANGE_MAX / 100.f);
        strCmd = QString(cstrAreaC) + QString::number(value);
        pDevice->sendCommand( strCmd );

    }
    else if (pEdit == ui->txtEdtAreaDblClick)
    {
        LOG_I( "Manual Edit Double Click Area" );
        int value = (int)(ui->txtEdtAreaDblClick->toPlainText().toFloat() * USER_AREA_RANGE_MAX / 100.f);
        strCmd = QString(cstrAreaD) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
    else if (pEdit == ui->txtEdtAreaPalm)
    {
        LOG_I( "Manual Edit Palm Area" );
        int value = (int)(ui->txtEdtAreaPalm->toPlainText().toFloat() * USER_AREA_RANGE_MAX / 100.f);
        strCmd = QString(cstrAreaP) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
    else if (pEdit == ui->txtEdtAreaPnT)
    {
        LOG_I( "Manual Edit Put&Tap Area" );
        int value = (int)(ui->txtEdtAreaPnT->toPlainText().toFloat() * USER_AREA_RANGE_MAX / 100.f);
        strCmd = QString(cstrAreaM) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
}

float QAreaSettingForm::getVariableStep( bool bIncreament, float fCurrentValue )
{
    float fChangeValue = 0.1f;
    if( fCurrentValue < 2.f )
        fChangeValue = 0.1f;
    else if( fCurrentValue < 5.f )
        fChangeValue = 0.2f;
    else if( fCurrentValue < 11.f )
        fChangeValue = 0.4f;
    else if( fCurrentValue < 20.f )
        fChangeValue = 0.5f;
    else if( fCurrentValue < 25.f )
        fChangeValue = 1.f;
    else
        fChangeValue = 2.f;
    return bIncreament ? fChangeValue : -fChangeValue;
}

void QAreaSettingForm::sendEditValue( QBorderStyleEdit* txtEdit, bool bIncreament, int nMin, int nMax, const QString& strCmd )
{
    setModifyEditColor(txtEdit);
    m_bIsModified = true;

    float fV = txtEdit->toPlainText().toFloat();
    fV += getVariableStep( bIncreament, fV );

    int nV = (int)(fV * nMax / 100.f);
    if (nV < nMin)
    {
        nV = nMin;
        fV = (float)nV * 100.f / nMax;
        fV = RoundP1(fV);
    }
    if (nV >nMax)
    {
        nV = nMax;
        fV = (float)nV * 100.f / nMax;
        fV = RoundP1(fV);
    }

    char szTemp[64];
    snprintf( szTemp, 64, "%.1f", fV );
    txtEdit->setText( szTemp );

    QString strSensorCmd = strCmd + QString::number(nV);
    QT3kDevice::instance()->sendCommand( strSensorCmd, true );
}

void QAreaSettingForm::on_btnAreaClickLeft_clicked()
{
    LOG_B( "Click Area \"-\"" );
    sendEditValue(ui->txtEdtAreaClick, false, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaC);
}

void QAreaSettingForm::on_btnAreaClickRight_clicked()
{
    LOG_B( "Click Area \"+\"" );
    sendEditValue(ui->txtEdtAreaClick, true, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaC);
}

void QAreaSettingForm::on_btnAreaDblClickLeft_clicked()
{
    LOG_B( "Double Click Area \"-\"" );
    sendEditValue(ui->txtEdtAreaDblClick, false, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaD);
}

void QAreaSettingForm::on_btnAreaDblClickRight_clicked()
{
    LOG_B( "Double Click Area \"+\"" );
    sendEditValue(ui->txtEdtAreaDblClick, true, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaD);
}

void QAreaSettingForm::on_btnAreaPnTLeft_clicked()
{
    LOG_B( "Put & Tap Area \"-\"" );
    sendEditValue(ui->txtEdtAreaPnT, false, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaM);
}

void QAreaSettingForm::on_btnAreaPnTRight_clicked()
{
    LOG_B( "Put & Tap Area \"+\"" );
    sendEditValue(ui->txtEdtAreaPnT, true, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaM);
}

void QAreaSettingForm::on_btnAreaPalmLeft_clicked()
{
    LOG_B( "Palm Area \"-\"" );
    sendEditValue(ui->txtEdtAreaPalm, false, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaP);
}

void QAreaSettingForm::on_btnAreaPalmRight_clicked()
{
    LOG_B( "Palm Area \"+\"" );
    sendEditValue(ui->txtEdtAreaPalm, true, USER_AREA_RANGE_START, USER_AREA_RANGE_END, cstrAreaP);
}
