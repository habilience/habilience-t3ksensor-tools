#ifndef QDETECTIONGRAPHFORM_H
#define QDETECTIONGRAPHFORM_H

#include <QWidget>

namespace Ui {
class QDetectionGraphForm;
}

class QDetectionGraphForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit QDetectionGraphForm(QWidget *parent = 0);
    ~QDetectionGraphForm();
    
private:
    Ui::QDetectionGraphForm *ui;
};

#endif // QDETECTIONGRAPHFORM_H
