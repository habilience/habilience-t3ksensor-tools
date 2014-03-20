#ifndef QAdvancedCalibrationWidget_H
#define QAdvancedCalibrationWidget_H

#include "QAutoDetectionRange.h"
#include "QBentAdjustment.h"
#include "QLangManager.h"


class QAdvancedCalibrationWidget : public QWidget, public QLangManager::ILangChangeNotify
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

    virtual void onChangeLanguage();

    void enterSettings();

protected:
    QPixmap*                    m_pPixmap;

    QAutoDetectionRange         m_DetectionRange;
    QBentAdjustment             m_BentAdjustment;

    enum SettingPart { spDetectionRange, spBent };
    SettingPart                 m_eCurrentPart;

    QString                     m_strEscapeMsg;

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
