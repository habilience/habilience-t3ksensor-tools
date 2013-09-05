#ifndef QSOFTKEYDESIGNTOOLWIDGET_H
#define QSOFTKEYDESIGNTOOLWIDGET_H

#include "LayoutToolWidget.h"
#include "SoftKey.h"
//#include "BorderStyleEdit.h"

#include <QDialog>

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
    QSoftKeyDesignToolWidget(QVector<CSoftkey*>* pSelectedKeys, QWidget* parent = 0);
    virtual ~QSoftKeyDesignToolWidget();

    void updateUIButtonState( int nSelectKeyCount, GroupStatus eGroupStatus, QVector<CSoftkey*>& SelectKeys );
    void updateLayoutButton(bool bVisible);

    void setScaleFactor( double dScaleWidth, double dScaleHeight );

    ScreenUnit getUnit() { return m_eUnit; }

protected:
    //
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

    void updateUnit();
    void EditModified();

protected:
    ScreenUnit              m_eUnit;
    QLayoutToolWidget*      m_pLayoutToolWidget;

    QRect                   m_rcOld;

    QVector<CSoftkey*>*     m_pvSelectedKeys;

    QString                 m_strPosX;
    QString                 m_strPosY;
    QString                 m_strWidth;
    QString                 m_strHeight;

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
    void invalidateKey(CSoftkey* key);
    void recalcSelectionKeys(QRect rcOld, QRect rcNew );
    void updateScreen();
    void resetKeys();

    void reorderKeys();

    // LayoutToolBar
    void generateKeys(KeyArrange eArrange, int nCount, int nW, int nH, int nInterval);
    void alignSelectedKeys(KeyAlign eAlign);
    void adjustSizeSelectedKeys(AdjustSize eSize);
    void distribSelectKeys(Distrib eDistrib);

private slots:
    void on_BtnFitScreen_clicked();
    void on_BtnShowToolbar_clicked();
    void on_BtnAdd_clicked();
    void on_BtnRemove_clicked();
    void on_BtnGroup_clicked();
    void on_BtnUngroup_clicked();
    void on_BtnReorder_clicked();
    void on_CBUnit_activated(int index);
    void on_EditPosX_textEdited(const QString &arg1);
    void on_EditName_editingFinished();
    void on_CBVisible_activated(int index);
    void on_EditPosX_editingFinished();
    void on_EditPosY_textEdited(const QString &arg1);
    void on_EditPosY_editingFinished();
    void on_EditWidth_textEdited(const QString &arg1);
    void on_EditWidth_editingFinished();
    void on_EditHeight_textEdited(const QString &arg1);
    void on_EditHeight_editingFinished();
    void on_BtnReset_clicked();
    void on_BtnMatch_clicked();
    void on_BtnClose_clicked();

public slots:
    void onKeyStateCount(bool bAdd);
    void onSelectedKeys(bool bGroup, int nSelectedCount);

    // LayoutToolWidget
    void onUpdateLayoutButton(bool bVisible);
    void onEnableDesignTool(bool bEnable);
};

#endif // QSOFTKEYDESIGNTOOLWIDGET_H
