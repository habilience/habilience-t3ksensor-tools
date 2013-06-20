#ifndef QHOVERCOMBOBOX_H
#define QHOVERCOMBOBOX_H

#include <QComboBox>

class QHoverComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QHoverComboBox(QWidget *parent = 0, bool bHover = true, int nRowIndex = -1, int nColumnIndex = -1);

    int GetRowIndex() { return m_nRowIndex; }
    int GetColumnIndex() { return m_nColumnIndex; }

    void ChangeIndex( int nIndex );
    void SetUseHover( bool bHover ) { m_bUseHover = bHover; }
    void SetZeroSet( bool bset ) { m_bZeroSet = bset; }
    void SetDrawBackColor( bool bDraw ) { m_bDrawBackColor = bDraw; }

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual bool eventFilter(QObject *target, QEvent *evt);
    virtual void hidePopup();
    virtual void showPopup();
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

protected:
    bool                m_bHover;

    // TableWidget
    int                 m_nRowIndex;
    int                 m_nColumnIndex;

    int                 m_nCurIndex;

private:
    bool                m_bUseHover;
    bool                m_bZeroSet;
    bool                m_bDrawBackColor;

signals:
    void ItemChanged(QObject* pObj, int nIndex);
    void FocusInCB();
    void ShowPopupSignal();
    void HidePopupSignal();
    void FocusOutSignal();
    void FocusInSignal();

private slots:
    void on_currentIndexChanged(int nIndex);
};

#endif // QHOVERCOMBOBOX_H
