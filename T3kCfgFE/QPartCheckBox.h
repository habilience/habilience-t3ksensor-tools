#ifndef QPARTCHECKBOX_H
#define QPARTCHECKBOX_H

#include <QCheckBox>

class QPartCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit QPartCheckBox(QWidget *parent = 0);

signals:
    void togglePart(QString strPart, bool bChecked);
public slots:
    void onToggledPart(bool bChecked);

};

#endif // QPARTCHECKBOX_H
