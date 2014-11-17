#ifndef QBENTCFGPARAM_H
#define QBENTCFGPARAM_H

#include "QSingletone.h"

#define BENT_MARGIN_INIT_LEFT			(2)
#define BENT_MARGIN_INIT_TOP			(2)
#define BENT_MARGIN_INIT_RIGHT			(2)
#define BENT_MARGIN_INIT_BOTTOM			(2)
#define BENT_MARGIN_INIT_DIR			(0)
#define BENT_INIT_ALGORITHM2			(2)
#define BENT_INIT_ALGORITHM4			(1)
#define BENT_INIT_CALIBRATION           (0)

#define BENT_WITH_DUMMY                 (false)
#define BENT_WITH_CALIBRATION           (false)
#define BENT_IGNORE_CAMERAPAIR          (false)

#define BENT_DIR_UP     (0)
#define BENT_DIR_DOWN   (1)
#define BENT_DIR_LEFT   (2)
#define BENT_DIR_RIGHT  (3)
class QBentCfgParam : public QSingleton<QBentCfgParam>
{
private:
    float   m_fBentMargin[4]; // left, top, right, bottom
    int		m_nBentDirection;
    int     m_nAlgorithm;
    bool    m_bBentWithDummy;
    bool    m_bIgnoreCameraPair;
public:
    QBentCfgParam();

    float marginLeft() const { return m_fBentMargin[0]; }
    float marginTop() const { return m_fBentMargin[1]; }
    float marginRight() const { return m_fBentMargin[2]; }
    float marginBottom() const { return m_fBentMargin[3]; }

    int direction() const { return m_nBentDirection; }
    int algorithm() const { return m_nAlgorithm; }      // algorithm 0: old 13 points, 1: new 13 points, 2: new 9 points

    bool isBentWithDummy() { return m_bBentWithDummy; }
    bool isIgnoreCameraPair() { return m_bIgnoreCameraPair; }

    void setMarginLeft( float fLeft ) { m_fBentMargin[0] = fLeft; }
    void setMarginTop( float fTop ) { m_fBentMargin[1] = fTop; }
    void setMarginRight( float fRight ) { m_fBentMargin[2] = fRight; }
    void setMarginBottom( float fBottom ) { m_fBentMargin[3] = fBottom; }

    void setMargin( float fLeft, float fTop, float fRight, float fBottom )
    {
        m_fBentMargin[0] = fLeft;
        m_fBentMargin[1] = fTop;
        m_fBentMargin[2] = fRight;
        m_fBentMargin[3] = fBottom;
    }

    void getMargin( float* pfLeft, float* pfTop, float* pfRight, float* pfBottom )
    {
        *pfLeft = m_fBentMargin[0];
        *pfTop = m_fBentMargin[1];
        *pfRight = m_fBentMargin[2];
        *pfBottom = m_fBentMargin[3];
    }

    void setDirection( int direction ) { m_nBentDirection = direction; }
    void setAlgorithm( int algorithm ) { m_nAlgorithm = algorithm; }
    void setBentWithDummy( bool bWithDummy ) { m_bBentWithDummy = bWithDummy; }
    void setIgnoreCameraPair( bool bIgnore ) { m_bIgnoreCameraPair = bIgnore; }
};

#endif // QBENTCFGPARAM_H
