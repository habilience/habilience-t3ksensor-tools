#ifndef QCOLORTABWIDGET_H
#define QCOLORTABWIDGET_H

#include <QWidget>
#include <QVector>

class QColorTabWidget : public QWidget
{
    Q_OBJECT
public:
    enum TabDirection { TabDirectionHorzLeftTop, TabDirectionHorzRightTop, TabDirectionVertLeftTop, TabDirectionVertLeftBottom };
private:
    bool    m_bIsHovered;
    bool    m_bIsMouseDown;
    bool    m_bEnableTabChange;

    bool            m_bNotifyTab;
    QVector<int>    m_aryNotifyTabIndex;

    int     m_nBlinkCount;
    int     m_TimerBlink;
    int     m_nBarOffset;

    TabDirection m_eTabDirection;
    QSize   m_sizeTabHeader;

    QFont   m_fntTab;

    struct TabInfo
    {
        QString     strCaption;
        QRect       rcArea;
        QWidget*    pChildWidget;
        QSize       sizeTab;
        QImage*     pIconImage;
        QColor      clrTab;
    };

    QVector<TabInfo>    m_aryTabInfo;
    int                 m_nActiveTabIndex;
    int                 m_nHoverTabIndex;
    QRect               m_rcChildArea;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent *evt);
    virtual bool event(QEvent *evt);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

    void drawTabs( QPainter& p, QRect rc );
    void drawTab( QPainter& p, QRect& rectTab, const TabInfo& ti, bool bHover, bool bActive, bool bNotify );

    QSize calcTabSize( QPainter& p, const TabInfo& ti );
    int isNotifyTab( int tabIndex );

public:
    explicit QColorTabWidget(QWidget *parent = 0);

    bool isHorzTab();
    void setTabDirection( TabDirection dir, int nBarSize, int nBarOffset=0 );
    int addTab( const QString& strCaption, QWidget* pChildWidget, QColor clrTab=QColor(255,60,60,200), QImage* pIconImage=NULL);

    void selectTab( int tabIndex );
    int getActiveTab() { return m_nActiveTabIndex; }
    void setTabText( int tabIndex, const QString& text );
    void notifyTab( int tabIndex );
    void resetNotify();

    void enableTabChange( bool bEnable );
    
signals:
    void tabSelectChange(QColorTabWidget* sender, int tabIndex);
    void tabSelectChanged(QColorTabWidget* sender, int tabIndex);
public slots:
    
};

inline bool QColorTabWidget::isHorzTab()
{
    return (m_eTabDirection == TabDirectionHorzLeftTop || m_eTabDirection == TabDirectionHorzRightTop) ? true : false;
}

#endif // QCOLORTABWIDGET_H
