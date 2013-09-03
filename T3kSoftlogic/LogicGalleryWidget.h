#ifndef QLOGICGALLERYWIDGET_H
#define QLOGICGALLERYWIDGET_H

#include "Softkey.h"

#include <QDialog>

#define MAX_IMAGE   6

namespace Ui {
class LogicGalleryWidget;
}

class QRadioButton;
class QLogicGalleryWidget : public QDialog
{
    Q_OBJECT

public:
    QLogicGalleryWidget(QWidget* parent = 0);
    virtual ~QLogicGalleryWidget();

    void setInfo( CSoftlogic::Port portKey, QPoint ptMouse );

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

protected:
	CSoftlogic::Port	m_portKey;
	QPoint				m_ptMouse;
    QPixmap*            m_pGalleryImage[MAX_IMAGE];
    int                 m_nGalleryItemType;
    QRadioButton*       m_pRadioButtons[MAX_IMAGE];

private:
    Ui::LogicGalleryWidget *ui;

signals:
    void addLogicFromGallery(int nGalleryItemType, CSoftlogic::Port portKey, QPoint ptMouse);

private slots:
    void on_BtnInsert_clicked();
    void on_RBTouch_clicked();
    void on_RBCalibration_clicked();
    void on_RBTaskSwitch_clicked();
    void on_RBTouchGPIO_clicked();
    void on_RBCalibrationGPIO_clicked();
    void on_RBTaskSwitchGPIO_clicked();
};

#endif // QLOGICGALLERYWIDGET_H
