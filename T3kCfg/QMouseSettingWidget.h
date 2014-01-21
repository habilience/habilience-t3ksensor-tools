#ifndef QMOUSESETTINGWIDGET_H
#define QMOUSESETTINGWIDGET_H

#include <QWidget>

#include "QOMouseProfileWidget.h"
#include "QNMouseProfileWidget.h"


namespace Ui {
    class QMouseSettingWidget;
}

class QMouseSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QMouseSettingWidget(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QMouseSettingWidget();

    // old
    void ReplaceLabelName(QCheckableButton* pBtn);

    void SetDefault();
    void Refresh();

protected:
    virtual void showEvent(QShowEvent *evt);

protected:
    QOMouseProfileWidget*    m_pOldMouseProfile;
    QNMouseProfileWidget*    m_pNewMouseProfile;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void SendInputModeState(int nCurInputMode);

private:
    QT3kDeviceR*&            m_pT3kHandle;

private slots:

};

#endif // QMOUSESETTINGWIDGET_H
