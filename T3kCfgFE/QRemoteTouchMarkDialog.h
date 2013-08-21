#ifndef QREMOTETOUCHMARKDIALOG_H
#define QREMOTETOUCHMARKDIALOG_H

#include <QDialog>
#include "QT3kDeviceEventHandler.h"
#include <QVector>

namespace Ui {
class QRemoteTouchMarkDialog;
}

class Dialog;
class QRemoteTouchMarkDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT
    
public:
    explicit QRemoteTouchMarkDialog(Dialog *parent = 0);
    ~QRemoteTouchMarkDialog();

protected:
    QVector<QPoint> m_aryTouchPoints;
    QVector<QSize>  m_aryTouchSizes;

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void drawTouchPoints( QPainter& p );
    void drawMark( QPainter& p, QRect& rectMark );

    void drawBackground(QPainter& p);

    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnTPT(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int count, int guess_cnt, t3ktouchpoint *points);

    enum Status { StatusMarkX=0, StatusMarkY, StatusTouchX, StatusTouchY, StatusDeltaX, StatusDeltaY, StatusCount };
    unsigned short m_StatusDatas[7];
    QRect m_rcStatus;

    QPoint  m_ptMark;
    bool    m_bShowMark;
    QRect   m_rcMark;
    
private:
    Dialog* m_pMainDlg;
    Ui::QRemoteTouchMarkDialog *ui;
};

#endif // QREMOTETOUCHMARKDIALOG_H
