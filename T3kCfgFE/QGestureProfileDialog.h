#ifndef QGESTUREPROFILEDIALOG_H
#define QGESTUREPROFILEDIALOG_H

#include <QDialog>
#include "QGestureMappingTable.h"
#include "QT3kDeviceEventHandler.h"

#include "QEditActionEnableDialog.h"
#include "QEditActionKey1Dialog.h"
#include "QEditActionKey2WayDialog.h"
#include "QEditActionKey4WayDialog.h"

namespace Ui {
class QGestureProfileDialog;
}

class QColorTabWidget;
class QGestureProfileDialog : public QDialog
        , public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

protected:
    void onChangeLanguage();

    virtual void paintEvent(QPaintEvent *);

    void sensorReset();
    void sensorRefresh( bool bTabOnly=false );
    bool sensorWrite();

    void resetDataWithInitData(const QString& strCmd, bool bWithFactoryDefault=true );

    void sendInputMode();
public:
    explicit QGestureProfileDialog(QWidget *parent = 0);
    ~QGestureProfileDialog();

private slots:
    void onTabSelChanged(QColorTabWidget* pTabWidget, int tabIndex);
    
    void on_btnReset_clicked();
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();

    void on_btnSave_clicked();

protected:
    QEditActionEnableDialog         m_editActionEnableDialog;
    QEditActionKey1Dialog           m_editActionKey1Dialog;
    QEditActionKey2WayDialog        m_editActionKey2WayDialog;
    QEditActionKey4WayDialog        m_editActionKey4WayDialog;


private:
    Ui::QGestureProfileDialog *ui;
    QGestureMappingTable    m_widgetGestureMappingTable;

public slots:
    void onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo& ci, ushort nProfileFlags);
};

#endif // QGESTUREPROFILEDIALOG_H
