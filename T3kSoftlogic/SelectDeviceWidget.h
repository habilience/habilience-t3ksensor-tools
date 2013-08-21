#ifndef QSelectDeviceWidget_H
#define QSelectDeviceWidget_H

#include <QDialog>


namespace Ui {
class SelectDeviceWidget;
}

class QTableWidgetItem;

class QSelectDeviceWidget : public QDialog
{
    Q_OBJECT

public:
    QSelectDeviceWidget(QWidget* parent = NULL);
    virtual ~QSelectDeviceWidget();

	struct DEVICE_ID
	{
        ushort nVID;
        ushort nPID;
        ushort nMI;
		int nDeviceIndex;
	};

    DEVICE_ID getDevId() { return m_DeviceId; }

    void refreshDeviceList();

protected:
    virtual void showEvent(QShowEvent *);

    void insertListItem( QString strModelName, int nIdx, ushort nVID, ushort nPID, ushort nMI, int nDeviceIndex );

protected:
	DEVICE_ID	m_DeviceId;

private:
    Ui::SelectDeviceWidget*  ui;

signals:

private slots:
    void on_TableDeviceList_itemSelectionChanged();
    void on_TableDeviceList_itemDoubleClicked(QTableWidgetItem *item);
    void on_BtnSelect_clicked();
    void on_BtnBuzzer_clicked();
};

#endif // QSelectDeviceWidget_H
