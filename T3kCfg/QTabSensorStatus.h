#ifndef QTABSENSORSTATUS_H
#define QTABSENSORSTATUS_H

#include <QDialog>

#include "QT3kDeviceR.h"
#include "QLangManager.h"

namespace Ui {
    class QTabSensorStatus;
}

class QTabSensorStatus : public QDialog, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QTabSensorStatus(QT3kDeviceR*& pHandle, QWidget *parent = 0);
    ~QTabSensorStatus();

protected:
    virtual void onChangeLanguage();

    virtual void keyPressEvent(QKeyEvent *);

private:
    Ui::QTabSensorStatus *ui;

    QT3kDeviceR*&            m_pT3kHandle;

private slots:
    void on_BtnInfoSave_clicked();
};

#endif // QTABSENSORSTATUS_H
