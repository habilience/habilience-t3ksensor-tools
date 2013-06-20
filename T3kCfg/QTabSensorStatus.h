#ifndef QTABSENSORSTATUS_H
#define QTABSENSORSTATUS_H

#include <QDialog>

#include "QTPDPEventMultiCaster.h"
#include "QLangManager.h"

namespace Ui {
    class QTabSensorStatus;
}

class QTabSensorStatus : public QDialog, public QTPDPEventMultiCaster::ITPDPEventListener, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QTabSensorStatus(T30xHandle*& pHandle, QWidget *parent = 0);
    ~QTabSensorStatus();

protected:
    virtual void OnChangeLanguage();

private:
    Ui::QTabSensorStatus *ui;

    T30xHandle*&            m_pT3kHandle;

private slots:
    void on_BtnInfoSave_clicked();
};

#endif // QTABSENSORSTATUS_H
