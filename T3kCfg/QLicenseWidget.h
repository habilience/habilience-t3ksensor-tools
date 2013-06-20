#ifndef QLICENSEWIDGET_H
#define QLICENSEWIDGET_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui {
    class QLicenseWidget;
}

class QLicenseWidget : public QDialog, public QLangManager::LangChangeNotify
{
    Q_OBJECT

public:
    explicit QLicenseWidget(QWidget *parent = 0);
    ~QLicenseWidget();

protected:
    void OnChangeLanguage();

private:
    Ui::QLicenseWidget *ui;

signals:

private slots:
    void on_BTNOK_clicked();
};

#endif // QLICENSEWIDGET_H
