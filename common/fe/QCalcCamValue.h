#ifndef QCALCCAMVALUE_H
#define QCALCCAMVALUE_H

#define SCALE_STEP   60
#define SCALE_START +0.2f
#define SCALE_END   +4.2f
#define RATIO_STEP   200
#define RATIO_START -7.f
#define RATIO_END   +7.f

#include <QPointF>

class QCalcCamValue
{
public:
    QCalcCamValue();
public:
    inline void setCamFactor( float fScale, float fRatio )
        { m_fCamScale = fScale; m_fCamRatio = fRatio; }
    inline float getCamScale() { return m_fCamScale; }
    inline float getCamRatio() { return m_fCamRatio; }
    inline void  setNewMethod( bool new_method = true ) { m_bNewMethod = new_method; }

    static void SinCosBy12( float fx1, float fy1, float fx2, float fy2, float & sin, float & cos );
    static QPointF calcCamPosByFunc( float fAx, float fAy, float fBx, float fBy );
    void calcCamFunc( float fObx0, float fOby0, float * pfObc, int nObc, float fCamScale, float fCamRatio, float & fSCx, float & fSCy );

#define CALC_PNTCNT_MAX  12
    typedef bool (* CB_PROGRESS)( float fProg, void* lpUser );

    float calcVariance( float * pfObcS, float * pfObcE, QPointF * pPts = NULL );
    void  scanCamFactor( float * pfObcS, float * pfObcE, float * pfCamS, float * pfCamR, CB_PROGRESS cb = NULL, void* lpUser = NULL );

    static QPointF vectorToCircle( float x, float y, float fa, float fb, float fr );
    void calc2ViewlineToPoints( float cam_x, float cam_y, float * pfObcS, float * pfObcE, float * pFds );
    float calc2Variance( float * pfObcS, float * pfObcE, QPointF * pCamPos = NULL );
    QPointF scan2CamFactor( float * pfObcS, float * pfObcE, float * pfCamS, float * pfCamR, CB_PROGRESS cb = NULL, void* lpUser = NULL );

    static float nextStep( float fPrev, float fCur, float fNext, float fStep );
    float convergeCamS( float * pfObcS, float * pfObcE, float step );
    float convergeCamR( float * pfObcS, float * pfObcE, float step );
    QPointF findCamSR( float * pfObcS, float * pfObcE, float * pfInitCamS, float * pfInitCamR, QPointF * pPts = NULL );

    float converge2CamR( float * pfObcS, float * pfObcE, float step );
    QPointF find2CamSR( float * pfObcS, float * pfObcE, float * pfInitCamS, float * pfInitCamR, float * pFds = NULL );

protected:
    float m_fCamScale;
    float m_fCamRatio;
    bool  m_bNewMethod;
};

#endif // QCALCCAMVALUE_H
