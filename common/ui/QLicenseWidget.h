#ifndef QLICENSEWIDGET_H
#define QLICENSEWIDGET_H

#include <QDialog>
#include "QLangManager.h"

namespace Ui {
    class QLicenseWidget;
}

class QLicenseWidget : public QDialog
        #ifndef NO_LANGUAGE
        , public QLangManager::ILangChangeNotify
        #endif
{
    Q_OBJECT

public:
    explicit QLicenseWidget(QString strHtml, QWidget *parent = 0);
    ~QLicenseWidget();

protected:
#ifndef NO_LANGUAGE
    void onChangeLanguage();
#endif

private:
    Ui::QLicenseWidget *ui;

signals:

private slots:
    void on_BTNOK_clicked();
};

#endif // QLICENSEWIDGET_H
