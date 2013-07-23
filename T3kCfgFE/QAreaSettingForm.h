#ifndef QAREASETTINGFORM_H
#define QAREASETTINGFORM_H

#include <QWidget>

namespace Ui {
class QAreaSettingForm;
}

class QAreaSettingForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit QAreaSettingForm(QWidget *parent = 0);
    ~QAreaSettingForm();
    
private:
    Ui::QAreaSettingForm *ui;
};

#endif // QAREASETTINGFORM_H
