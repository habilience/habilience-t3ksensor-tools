#ifndef QGESTUREMAPPINGTABLE_H
#define QGESTUREMAPPINGTABLE_H

#include <QWidget>

#define MM_GESTURE_NO_ACTION           0x00
#define MM_GESTURE_SINGLE_MOVE         0x01
#define MM_GESTURE_SINGLE_TAP          0x02
#define MM_GESTURE_SINGLE_DOUBLE_TAP   0x03
#define MM_GESTURE_SINGLE_LONG_TAP     0x04
#define MM_GESTURE_FINGERS_MOVE        0x11
#define MM_GESTURE_FINGERS_TAP         0x12
#define MM_GESTURE_FINGERS_DOUBLE_TAP  0x13
#define MM_GESTURE_FINGERS_LONG_TAP    0x14
#define MM_GESTURE_PALM_MOVE           0x21
#define MM_GESTURE_PALM_TAP            0x22
#define MM_GESTURE_PALM_DOUBLE_TAP     0x23
#define MM_GESTURE_PALM_LONG_TAP       0x24
#define MM_GESTURE_PUTAND_TAP          0x32
#define MM_GESTURE_PUTAND_DOUBLE_TAP   0x33
#define MM_GESTURE_PUTAND_LONG_TAP     0x34
#define MM_GESTURE_ACTIONMASK          0x3f

#define MM_GESTURE_ZOOM                0x80

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_CKEYS   (MM_MOUSE_KEY0_CTRL | MM_MOUSE_KEY0_SHIFT | MM_MOUSE_KEY0_ALT | MM_MOUSE_KEY0_WINDOW)
#define MM_MOUSE_KEY0_MOUSE   0x80
#define MM_MOUSE_KEY1_NOACTION             0x0
#define MM_MOUSE_KEY1_MOUSE_L_MOVE         0x01
#define MM_MOUSE_KEY1_MOUSE_L_CLICK        0x02
#define MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK  0x03
#define MM_MOUSE_KEY1_MOUSE_R_MOVE         0x04
#define MM_MOUSE_KEY1_MOUSE_R_CLICK        0x05
#define MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK  0x06
#define MM_MOUSE_KEY1_MOUSE_M_MOVE         0x07
#define MM_MOUSE_KEY1_MOUSE_M_CLICK        0x08
#define MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK  0x09
#define MM_MOUSE_KEY1_MOUSE_WHEEL          0xfe
#define MM_MOUSE_KEY1_MOUSE_MOVE           0xff

#define EXTP_COUNT              5

#define GESTURE_PROFILE_COUNT   2

#include "QT3kDeviceEventHandler.h"
#include <QVector>
#include <QRegion>


class QGestureMappingTable : public QWidget
        , public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT
public:
    explicit QGestureMappingTable(QWidget *parent = 0);

    void setProfileIndex( int nIndex );
    void resetSelect();

    enum CellKeyType { KeyTypeNone, KeyTypeEnable, KeyType1Key, KeyType2Way, KeyType4Way };
    struct CellInfo
    {
        bool bEnable;
        bool bNotUsed;
        bool bNotEdit;
        bool bDefault;
        QRectF rectCell;
        QString strText;
        unsigned char cKey;
        unsigned short wKeyValue[4];
        CellKeyType keyType;
        bool bBold;
    };

protected:

    QVector<CellInfo>	m_aryCell;

    CellInfo    m_ciZoomHead;
    CellInfo    m_ciRotateHead;
    CellInfo    m_ciZoom;
    CellInfo    m_ciRotate;

    CellInfo*   m_pSelectCell;
    CellInfo*   m_pHoverCell;

    QRectF      m_rectExtProperty[EXTP_COUNT];
    QString     m_strExtProperty[EXTP_COUNT];
    bool        m_bCheckExtProperty[EXTP_COUNT+3];
    int         m_nHoverExtProperty;

    unsigned short  m_wProfileFlags;

    int     m_nProfileIndex;
    bool    m_bIsTitleOver;
    bool    m_bIsHovered;
    QRegion m_rgnHover;

    QFont   m_fntSystem;

    int         m_nMultiProfile;

    void setCellInfo( int nCol, int nRow,
                      unsigned char cV00, unsigned char cV01,
                      unsigned char cV10, unsigned char cV11,
                      unsigned char cV20, unsigned char cV21,
                      unsigned char cV30, unsigned char cV31 );
    void setCellInfo( int nCol, int nRow,
                      unsigned char cV0, unsigned char cV1 );

    void parseMouseProfile( const char* szProfile );

    void drawExtProperty(QPainter& p, int nExtIndex, QRectF &rectExtProperty, QFont& fntCellNormal, int flags, const QColor &cellFontColor );
    void drawCellInfo(QPainter& p, const CellInfo& ci, QFont& fntCellNormal, QFont& fntCellBold, const QColor& cellFontColor, int &flags, const QString &strFontFamily );

    void initCellInfo( CellInfo& ci );
    void popEditActionWnd(const CellInfo &ci );

    // override QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual bool event(QEvent *);

    void initControl();
public:
    
signals:
    void updateProfile(int nProfileIndex, const CellInfo& ci, ushort nProfileFlags);
    void modifiedExtProperty();
    
public slots:
    
};

inline void QGestureMappingTable::initCellInfo(CellInfo &ci)
{
    ci.rectCell = QRectF(0,0,0,0);
    ci.bEnable = true;
    ci.bNotUsed = false;
    ci.bNotEdit = false;
    ci.bDefault = false;
    ci.strText = "";
    ci.bBold = false;
    ci.cKey = 0xFF;
    ci.wKeyValue[0] = ci.wKeyValue[1]
            = ci.wKeyValue[2]
            = ci.wKeyValue[3] = 0x00;
    ci.keyType = KeyTypeNone;
}

#endif // QGESTUREMAPPINGTABLE_H
