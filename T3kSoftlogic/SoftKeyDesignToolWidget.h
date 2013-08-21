#ifndef QSOFTKEYDESIGNTOOLWIDGET_H
#define QSOFTKEYDESIGNTOOLWIDGET_H

//#include "LayoutToolBar.h"
#include "Softkey.h"
//#include "BorderStyleEdit.h"

#include <QDialog>

enum ScreenUnit { UnitRes, UnitMM };

namespace Ui {
class SoftKeyDesignToolWidget;
}

class QGraphicsKeyItem;
class QGraphicsItemGroup;
class QKeyDesignWidget;
class QSoftKeyDesignToolWidget : public QDialog
{
    Q_OBJECT

public:
    QSoftKeyDesignToolWidget(QWidget* parent = 0);
    virtual ~QSoftKeyDesignToolWidget();

    void updateUIButtonState( int nSelectKeyCount, GroupStatus eGroupStatus, QVector<CSoftkey*>& SelectKeys );
    void updateLayoutButton();

    void setScaleFactor( double dScaleWidth, double dScaleHeight );

    ScreenUnit getUnit() { return m_eUnit; }

protected:
    //
    virtual void closeEvent(QCloseEvent *);

    void updateUnit();
    void EditModified();

protected:
    ScreenUnit              m_eUnit;
    //DLayoutToolBar          m_wndLayoutToolBar;

    QGraphicsItemGroup*     m_pSelectedKeys;

    QRect                   m_rcOld;

protected:
    double                  m_dD2PScaleWidth;
    double                  m_dD2PScaleHeight;

private:
    Ui::SoftKeyDesignToolWidget* ui;

signals:
    void closeWidget();
    int addNewKey();
    void removeSelectedKeys();
    void screenSize(int eSize);
    void groupSelectedKeys();
    void ungroupSelectedKeys(bool bPushHistory);
    void reorderKeys();
    void invalidateKey(QGraphicsKeyItem* key);
    void recalcSelectionKeys(QRect rcOld, QRect rcNew );
    void updateScreen();
    void resetKeys();

private slots:
    void on_BtnFitScreen_clicked();
    void on_BtnShowToolbar_clicked();
    void on_BtnAdd_clicked();
    void on_BtnRemove_clicked();
    void on_BtnGroup_clicked();
    void on_BtnUngroup_clicked();
    void on_BtnReorder_clicked();
    void on_CBUnit_currentIndexChanged(int index);
    void on_EditPosX_textChanged(const QString &arg1);
    void on_EditName_editingFinished();
    void on_CBVisible_currentIndexChanged(int index);
    void on_EditPosX_editingFinished();
    void on_EditPosY_textChanged(const QString &arg1);
    void on_EditPosY_editingFinished();
    void on_EditWidth_textChanged(const QString &arg1);
    void on_EditWidth_editingFinished();
    void on_EditHeight_textChanged(const QString &arg1);
    void on_EditHeight_editingFinished();
    void on_BtnReset_clicked();
    void on_BtnMatch_clicked();
    void on_BtnClose_clicked();

public slots:
    void onKeyStateCount(bool bAdd);
    void onSelectedKeys(bool bGroup, int nSelectedCount);
};

#endif // QSOFTKEYDESIGNTOOLWIDGET_H
