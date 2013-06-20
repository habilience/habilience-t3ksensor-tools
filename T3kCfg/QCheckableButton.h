#ifndef QCHECKABLEBUTTON_H
#define QCHECKABLEBUTTON_H

#include <QPushButton>

class QLineEdit;

class QCheckableButton : public QPushButton
{
    Q_OBJECT

public:
    explicit QCheckableButton(QWidget *parent = 0);
    ~QCheckableButton();

    enum eButtonMode{ BM_OFF, BM_ON, BM_HOVER };

    void ChangeButtonMode( eButtonMode eBM );

    bool IsCheckBox() { return m_bCheckBox; }
    void SetCheckBox(bool bCheck);

    bool IsChecked() { return m_bChecked; }
    void SetChecked( bool bCheck );

    bool IsSubCheckBox() { return m_bSubCheckBox; }
    void SetSubCheckBox( bool bCheck );

    void SetCheckIndex( int nIndex );

    void HideEditBox();
    bool IsShowEditBox();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual bool eventFilter(QObject *target, QEvent *event);

protected:
    int         m_nCheckIndex;
    bool        m_bSubCheckBox;      // 2nd Check Box

    bool        m_bChecked;         // Selected
    bool        m_bCheckBox;        // Check Box
    bool        m_bHover;
    QRect       m_rcCheckBox;

    QColor      clrBoundary;
    QColor      clrFillTop;
    QColor      clrFillbtm;
    QColor      clrCheckBox;

    QLineEdit*  m_pTextEdit;

signals:
    void OnBtnClickEvent();

private slots:
    void OnEditEnter();
};

#endif // QCHECKABLEBUTTON_H
