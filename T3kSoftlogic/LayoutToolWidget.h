#ifndef QLAYOUTTOOLWIDDGET_H
#define QLAYOUTTOOLWIDDGET_H

#include "ArrangeHelperWidget.h"
#include "SoftKey.h"

#include <QDialog>

namespace Ui {
class LayoutToolWidget;
}

enum KeyAlign { KeyAlignLeft, KeyAlignCenter, KeyAlignRight, KeyAlignTop, KeyAlignMiddle, KeyAlignBottom };
enum AdjustSize { AdjustSizeSameWidth, AdjustSizeSameHeight, AdjustSizeSameBoth };
enum Distrib { DistribHorzEqualGap, DistribVertEqualGap };


class QLayoutToolWidget : public QDialog
{
    Q_OBJECT

public:
    QLayoutToolWidget(QVector<CSoftkey*>* pSelectedKeys, QWidget* parent = NULL);
    virtual ~QLayoutToolWidget();

    void updateUIButtonState( int nSelectKeyCount );
    void setUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight );

protected:
    virtual void closeEvent(QCloseEvent *);

protected:
	double m_dD2PScaleWidth;
	double m_dD2PScaleHeight;

    ScreenUnit			m_eUnit;

    bool                m_bSelectedkeys;

    QVector<CSoftkey*>* m_pSelectedKeys;

private:
    Ui::LayoutToolWidget* ui;

signals:
    void generateKeys(KeyArrange eArrange, int nCount, int nW, int nH, int nInterval);

    void alignSelectedKeys(KeyAlign eAlign);
    void adjustSizeSelectedKeys(AdjustSize eSize);
    void distribSelectKeys(Distrib eDistrib);
    void reorderKeys();

    void updateLayoutButton(bool bVisible);
    void enableDesignTool(bool bEnable);

private slots:
    void on_BtnLeft_clicked();
    void on_BtnCenter_clicked();
    void on_BtnRight_clicked();
    void on_BtnTop_clicked();
    void on_BtnMiddle_clicked();
    void on_BtnBottom_clicked();
    void on_BtnWidth_clicked();
    void on_BtnHeight_clicked();
    void on_BtnBoth_clicked();
    void on_BtnHorizon_clicked();
    void on_BtnVertical_clicked();
    void on_BtnAraange_clicked();
    void on_BtnReorder_clicked();
};

#endif // QLAYOUTTOOLWIDDGET_H
