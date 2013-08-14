#ifndef QAREASETTINGFORM_H
#define QAREASETTINGFORM_H

#include <QWidget>
#include "QT3kDeviceEventHandler.h"
#include "QLangManager.h"
#include "t3kcomdef.h"

namespace Ui {
class QAreaSettingForm;
}

class QPushButton;
class QBorderStyleEdit;
class QAreaSettingForm : public QWidget
        , public QT3kDeviceEventHandler::IListener
        , public QLangManager::ILangChangeNotify
{
    Q_OBJECT
private:
    bool    m_bIsModified;
    bool    m_bCheckTouch;

    QRect   m_rcSingleClick;
    QRect   m_rcDoubleClick;
    QRect   m_rcPalm;
    QRect   m_rcPutNTap;

    QRect   m_rcDrawSingleClick;
    QRect   m_rcDrawDoubleClick;
    QRect   m_rcDrawPalm;
    QRect   m_rcDrawPutNTap;

    double  m_dPercentSingleClick;
    double  m_dPercentDoubleClick;
    double  m_dPercentPalm;
    double  m_dPercentPutNTap;

protected:
    void resetEditColors();
    void setModifyAllEditColors();
    void setModifyEditColor(QBorderStyleEdit* pEdit);

    void adjustControls( QRect& rcSection, QPushButton* btnDec, QBorderStyleEdit* edit, QPushButton* btnInc );

    void drawSectionTitle( QPainter& p, QRect& rcSection, const QString& strTitle, bool bEnabled );
    void drawCircleArea( QPainter& p, QRect& rcSection, QBorderStyleEdit* edit );
    void drawLinearArea( QPainter& p, QRect& rcSection, QBorderStyleEdit* edit );
    void drawTouchCircle( QPainter& p );
    void drawArrowLine(QPainter& p, const QPointF &pt1, const QPointF &pt2 , const QColor &clrLine, float fLineWidth);

    void drawDashedRoundAnchorLine( QPainter& p, const QPointF &pt1, const QPointF &pt2 , const QColor &clrLine, float fLineWidth);

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

    // override QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnTPT(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int count, int guess_cnt, t3ktouchpoint *points);

    void onTouchDown(QPoint pt, int nW, int nH, int nL, double dPercent);
    void onTouchLift();

    float getVariableStep( bool bIncreament, float fCurrentValue );
    void sendEditValue(QBorderStyleEdit* txtEdit, bool bIncreament, int nMin, int nMax, const QString& strCmd );
public:
    explicit QAreaSettingForm(QWidget *parent = 0);
    ~QAreaSettingForm();

    void setModified( bool bModified );
    bool isModified() { return m_bIsModified; }

    void enableControlsWithoutSingleClick(bool bEnable);

private slots:
    virtual void reject();
    virtual void accept();

    void onEditModified(QBorderStyleEdit* pEdit, int nValue, double dValue);
    void on_btnAreaClickLeft_clicked();
    void on_btnAreaClickRight_clicked();
    void on_btnAreaDblClickLeft_clicked();
    void on_btnAreaDblClickRight_clicked();
    void on_btnAreaPnTLeft_clicked();
    void on_btnAreaPnTRight_clicked();
    void on_btnAreaPalmLeft_clicked();
    void on_btnAreaPalmRight_clicked();

private:
    Ui::QAreaSettingForm *ui;
};

#endif // QAREASETTINGFORM_H
