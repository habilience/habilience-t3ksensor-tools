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
    void updateDesignWidget() { m_DesignCanvasWidget.updateKeys(); }
    void setInvertDrawing(bool bInvert) { m_DesignCanvasWidget.setInvertDrawing(bInvert); if( isVisible() ) update(); }

protected:
    //
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

    void verifySoftlogicGPIO(void);

protected:
    QKeyDesignWidget     m_DesignCanvasWidget;

    QStringList             m_TableLabals;

private:
    Ui::TabKeyDesignWidget* ui;

signals:
    void updatePreview();

private slots:
    void on_BtnKeydesign_clicked();
    void on_BtnSet_clicked();
    void onTableGPIOcellChanged(QObject* row, int index);
};

#endif // TABKEYDESIGNWIDGET_H
