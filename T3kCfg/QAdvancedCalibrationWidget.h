#ifndef QAdvancedCalibrationWidget_H
#define QAdvancedCalibrationWidget_H

#include "QAutoDetectionRange.h"
#include "QBentAdjustment.h"

#include <QDialog>


class QAdvancedCalibrationWidget : public QDialog
{
    Q_OBJECT

public:
    explicit QAdvancedCalibrationWidget(bool bDetection, QWidget *parent = 0);
    ~QAdvancedCalibrationWidget();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void paintEvent(QPaintEvent *);

    void enterSettings();

protected:
    QPixmap*                    m_pPixmap;

    QAutoDetectionRange         m_DetectionRange;
    QBentAdjustment             m_BentAdjustment;

    enum SettingPart { spDetectionRange, spBent };
    SettingPart                 m_eCurrentPart;

signals:
    void closeWidget();

private slots:
    void onUpdateWidget();
    void onUpdateWidgetRect(QRect rc);
    void onFinishDetectionRange(bool bRet);
    void onFinishBentAdjustment();
    void onShowDetectionWaitDialog();
};

#endif // QAdvancedCalibrationWidget_H
