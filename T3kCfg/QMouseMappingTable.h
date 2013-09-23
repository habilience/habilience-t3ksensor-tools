#ifndef QMOUSEMAPPINGTABLE_H
#define QMOUSEMAPPINGTABLE_H

#include <QLabel>
#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"

#define EXTP_COUNT              5

enum CellKeyType { KeyTypeNone, KeyTypeEnable, KeyType1Key, KeyType2Way, KeyType4Way };
struct CellInfo
{
public:
    void InitCellInfo()
    {
        rectCell         = QRectF(0.f, 0.f, 0.f, 0.f);
        bEnable          = true;
        bNoUse           = false;
        bDefault         = false;
        strText          = QObject::tr("");
        bBold            = false;
        cKey             = 0xFF;
        wKeyValue[0]     = 0x00;
        wKeyValue[1]     = 0x00;
        wKeyValue[2]     = 0x00;
        wKeyValue[3]     = 0x00;
        eKeyType         = KeyTypeNone;
        pIconImage       = NULL;
    }

    bool		bEnable;
    bool		bNoUse;
    bool		bDefault;
    QRectF		rectCell;
    QString		strText;
    uchar       cKey;
    ushort      wKeyValue[4];
    CellKeyType eKeyType;
    bool		bBold;
    QImage*		pIconImage;
};

#include "QEditActionEDWnd.h"
#include "QEditAction2WDWnd.h"
#include "QEditAction4WDWnd.h"
#include "QEditActionWnd.h"

class QMouseMappingTable : public QLabel, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT
public:
    explicit QMouseMappingTable(QWidget *parent = 0);
    ~QMouseMappingTable();

    void SetT3kHandle( T3kHandle* pHandle ) { m_pT3kHandle = pHandle; }

    void SetProfileIndex( int nIndex );
    void ResetSelect();
    void EnableTable( bool bEnable );

    virtual void onChangeLanguage();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *evt);
    virtual bool eventFilter(QObject *target, QEvent *evt);

    void Init();
    void DrawCellInfo( QPainter& dc, CellInfo& ci, QFont& fntCellNormal, QFont& fntCellBold, int nStringFlags, QString strFamily );
    void DrawExtProperty( QPainter& dc, int nExtIndex, QRectF& rectExtProperty, QFont* pfntCellNormal, int nStringFlags );

    void PainterDrawText( QPainter& dc, QRectF & rectangle, int flags, const QString & text, QRectF * boundingRect = 0 );

    void SetCellInfo( int nCol, int nRow, uchar cV00, uchar cV01, uchar cV10, uchar cV11, uchar cV20, uchar cV21, uchar cV30, uchar cV31 );
    void SetCellInfo( int nCol, int nRow, uchar cV0, uchar cV1 );

    void ParseMouseProfile( const char* szProfile );

    void PopEditActionWnd( CellInfo& ci, int nRow, int nCol );

    void OnMouseLBDown(QPointF point);

    virtual void OnRSP(ResponsePart Part, ushort nTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

private:
    CellInfo* GetAt(int nIndex);

protected:
    T3kHandle*                 m_pT3kHandle;

    QEditActionEDWnd*           m_pEditActionEDWnd;
    QEditAction2WDWnd*          m_pEditAction2WDWnd;
    QEditAction4WDWnd*          m_pEditAction4WDWnd;
    QEditActionWnd*             m_pEditActionWnd;

    QVector<CellInfo>           m_vCell;

    CellInfo                	m_ciZoomHead;
    CellInfo                    m_ciRotateHead;
    CellInfo                    m_ciZoom;
    CellInfo                    m_ciRotate;

    CellInfo*                   m_pSelectCell;
    CellInfo*                   m_pHoverCell;

    QRectF                      m_rectExtProperty[EXTP_COUNT];
    QString                     m_strExtProperty[EXTP_COUNT];
    bool                        m_bCheckExtProperty[EXTP_COUNT+3];    

    ushort                      m_wProfileFlags;

    int                         m_nProfileIndex;

    bool                        m_bIsTitleOver;
    bool                        m_bIsHovered;

    int                         m_nCurInputMode;

    bool                        m_bExtSet;

signals:

public slots:
    void onRecvInputMode(int nCurInputMode);
};

inline CellInfo* QMouseMappingTable::GetAt(int nIndex)
{
    if( nIndex < 0 || nIndex > m_vCell.size() )
        return NULL;
    else
        return &m_vCell[nIndex];
}

#endif // QMOUSEMAPPINGTABLE_H
