#ifndef TABKEYDESIGNWIDGET_H
#define TABKEYDESIGNWIDGET_H

#include "KeyDesignWidget.h"
//#include "gpiolistctrl.h"

#include <QWidget>


namespace Ui {
class TabKeyDesignWidget;
}

class TabKeyDesignWidget : public QWidget
{
    Q_OBJECT

public:
    TabKeyDesignWidget(QWidget* pParent = NULL);
    virtual ~TabKeyDesignWidget();

    void refresh();

protected:
    //
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

    void verifySoftlogicGPIO(void);

protected:
    QKeyDesignWidget     m_DesignCanvasWidget;

private:
    Ui::TabKeyDesignWidget* ui;

signals:

private slots:
    void on_BtnKeydesign_clicked();
    void on_BtnSet_clicked();
    void on_TableGPIO_cellChanged(int row, int column);
};

#endif // TABKEYDESIGNWIDGET_H
