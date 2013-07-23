#ifndef QBENTDIRECTIONMARGINFORM_H
#define QBENTDIRECTIONMARGINFORM_H

#include <QWidget>

namespace Ui {
class QBentDirectionMarginForm;
}

class QBentDirectionMarginForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit QBentDirectionMarginForm(QWidget *parent = 0);
    ~QBentDirectionMarginForm();
    
private:
    Ui::QBentDirectionMarginForm *ui;
};

#endif // QBENTDIRECTIONMARGINFORM_H
