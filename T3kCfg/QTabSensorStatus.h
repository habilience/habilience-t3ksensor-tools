#ifndef QTABSENSORSTATUS_H
#define QTABSENSORSTATUS_H

#include <QDialog>

#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"

namespace Ui {
    class QTabSensorStatus;
}

class QTabSensorStatus : public QDialog, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QTabSensorStatus(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QTabSensorStatus();

protected:
    virtual void onChangeLanguage();

    virtual void keyPressEvent(QKeyEvent *);

private:
    Ui::QTabSensorStatus *ui;

    T3kHandle*&            m_pT3kHandle;

private slots:
    void on_BtnInfoSave_clicked();
};

#endif // QTABSENSORSTATUS_H
