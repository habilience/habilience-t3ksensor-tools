#ifndef QARRANGEHELPERWIDGET_H
#define QARRANGEHELPERWIDGET_H

//#include "borderstyleedit.h"

#include <QDialog>

enum ScreenUnit { UnitRes, UnitMM };
enum KeyArrange { KeyArrangeHorizontal, KeyArrangeVertical };

namespace Ui {
class ArrangeHelperWidget;
}

class CDesignCanvasWnd;
class QArrangeHelperWidget : public QDialog
{
    Q_OBJECT

public:
    QArrangeHelperWidget(bool bMake, QWidget* parent = 0);
    virtual ~QArrangeHelperWidget();

    void setUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight );

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void keyPressEvent(QKeyEvent *);

protected:
    ScreenUnit			m_eUnit;
    double              m_dD2PScaleWidth;
    double              m_dD2PScaleHeight;

    QString             m_strKeycount;
    QString             m_strWidth;
    QString             m_strHeight;
    QString             m_strInterval;

    QPixmap*            m_pImageArrangePicture;

private:
    Ui::ArrangeHelperWidget* ui;

signals:
    void generateKeys(KeyArrange eArrange, int nCount, int nW, int nH, int nInterval);

private slots:
    void on_RBHorizontal_clicked();
    void on_RBVertical_clicked();
    void on_EditInterval_editingFinished();
    void on_EditKeyCount_editingFinished();
    void on_EditWidth_editingFinished();
    void on_EditHeight_editingFinished();
    void on_BtnApply_clicked();
    void on_BtnCancel_clicked();
    void on_EditKeyCount_textEdited(const QString &arg1);
    void on_EditWidth_textEdited(const QString &arg1);
    void on_EditHeight_textEdited(const QString &arg1);
    void on_EditInterval_textEdited(const QString &arg1);
};

#endif // QARRANGEHELPERWIDGET_H
