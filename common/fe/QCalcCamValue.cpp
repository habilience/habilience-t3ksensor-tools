#include "QCalcCamValue.h"

#include <QtNumeric>
#include <math.h>
#include <float.h>
#include <QVector>

#define NaN ((float)qQNaN())

typedef struct tagCCP
{
    char input1;
    char input2;
} CCP;
#define SIZEOF_CCP  ((int)(sizeof(s_ccp) / sizeof(CCP)))

/*
    10   2         3     11

    5    7    0    8     6

    9    1         4     12
*/
static CCP s_ccp[] = {
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6},
    {1, 7},
    {1, 8},
    {2, 3},
    {2, 4},
    {2, 5},
    {2, 6},
    {2, 7},
    {2, 8},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7},
    {3, 8},
    {4, 5},
    {4, 6},
    {4, 7},
    {4, 8},
    {5, 6},
    {5, 7},
    {5, 8},
    {6, 7},
    {6, 8},
    {7, 8},
};

QCalcCamValue::QCalcCamValue()
{
    m_fCamScale = 1.f;
    m_fCamRatio = 1.f;
    m_bNewMethod = true;
}

void QCalcCamValue::SinCosBy12( float fx1, float fy1, float fx2, float fy2, float & sin, float & cos )
{
    float fr = fx1 * fx1 + fy1 * fy1;
    float fc = (fx2 * fx1 + fy2 * fy1) / fr;
    float fs = (fx2 * fy1 - fy2 * fx1) / fr;
    float fa = sqrtf(fc * fc + fs * fs);
    cos = fc / fa;
    sin = fs / fa;
}


void QCalcCamValue::calcCamFunc( float fObx0, float fOby0, float * pfObc, int nObc, float fCamScale, float fCamRatio, float & fSCx, float & fSCy )
{
    float fObc = pfObc[nObc];
    float fObx = fCamScale * fObc + 0.5f;
    float fOby = (1.f - fObx) * fCamRatio;

    float fCos, fSin;
    SinCosBy12(fObx0, fOby0, fObx, fOby, fSin, fCos);

    float fSC = fCos / fSin;
    switch ( nObc )
    {
    default:
        Q_ASSERT(false);
    case 1:
        fSCx = -fSC / 2.f - 0.5f;
        fSCy = fSC / 2.f - 0.5f;
        break;
    case 2:
        fSCx = fSC / 2.f - 0.5f;
        fSCy = fSC / 2.f + 0.5f;
        break;
    case 3:
        fSCx = fSC / 2.f + 0.5f;
        fSCy = -fSC / 2.f + 0.5f;
        break;
    case 4:
        fSCx = -fSC / 2.f + 0.5f;
        fSCy = -fSC / 2.f - 0.5f;
        break;
    case 5:
        fSCx = -1.f;
        fSCy = fSC;
        break;
    case 6:
        fSCx = 1.f;
        fSCy = -fSC;
        break;
    case 7:
        fSCx = -0.5f;
        fSCy = fSC / 2.f;
        if ( !m_bNewMethod )
        {
            fSCx *= 1.5f;
            fSCy *= 1.5f;
        }
        break;
    case 8:
        fSCx = 0.5f;
        fSCy = -fSC / 2.f;
        if ( !m_bNewMethod )
        {
            fSCx *= 1.5f;
            fSCy *= 1.5f;
        }
        break;
    case 9:
        fSCx = -fSC / 2.f - 1.f;
        fSCy = fSC - 0.5f;
        break;
    case 10:
        fSCx = fSC / 2.f - 1.f;
        fSCy = fSC + 0.5f;
        break;
    case 11:
        fSCx = fSC / 2.f + 1.f;
        fSCy = -fSC + 0.5f;
        break;
    case 12:
        fSCx = -fSC / 2.f + 1.f;
        fSCy = -fSC - 0.5f;
        break;
    }
}

QPointF QCalcCamValue::calcCamPosByFunc( float fAx, float fAy, float fBx, float fBy )
{
    QPointF ptRet;
    float fa = (fAx - fBx);
    float fb = (fAy - fBy);
    float ft = (-fa * fBx - fb * fBy) / (fa * fa + fb * fb);
    ptRet.setX(2.f * (fa * ft + fBx));
    ptRet.setY(2.f * (fb * ft + fBy));
    return ptRet;
}


float QCalcCamValue::calcVariance( float * pfObcS, float * pfObcE, QPointF * pPts )
{
    float fObc0 = (pfObcS[0] + pfObcE[0]) / 2.f;
    if ( qIsNaN(fObc0) )
        return FLT_MAX;

    float pfObc[13];
    for ( int ni = 0; ni < 13; ni++ )
    {
        pfObc[ni] = (pfObcS[ni] + pfObcE[ni]) / 2.f;
    }

    QPointF pts[CALC_PNTCNT_MAX];
    if ( pPts == NULL )
        pPts = pts;
    float fObx0 = m_fCamScale * fObc0 + 0.5f;
    float fOby0 = (1.f - fObx0) * m_fCamRatio;
    for ( int idxM = 1; idxM <= CALC_PNTCNT_MAX; idxM++ )
    {
        pPts[idxM - 1].setX(NaN);
        if ( qIsNaN(pfObc[idxM]) )
            continue;

        float fMsx, fMsy;
        calcCamFunc(fObx0, fOby0, pfObcS, idxM, m_fCamScale, m_fCamRatio, fMsx, fMsy);
        float fMex, fMey;
        calcCamFunc(fObx0, fOby0, pfObcE, idxM, m_fCamScale, m_fCamRatio, fMex, fMey);

        int idxSel = -1;
        float fSelMin = FLT_MAX;
        for ( int ni = 1; ni <= CALC_PNTCNT_MAX; ni++ )
        {
            if ( ni == idxM || qIsNaN(pfObc[ni]) )
                continue;

            int nj = 0;
            for ( ; nj < SIZEOF_CCP; nj++ )
            {
                if ( (s_ccp[nj].input1 == idxM && s_ccp[nj].input2 == ni ) ||
                     (s_ccp[nj].input2 == idxM && s_ccp[nj].input1 == ni ) )
                    break;
            }
            if ( nj >= SIZEOF_CCP )
                break;

            float fIsx, fIsy;
            calcCamFunc(fObx0, fOby0, pfObcS, ni, m_fCamScale, m_fCamRatio, fIsx, fIsy);
            float fIex, fIey;
            calcCamFunc(fObx0, fOby0, pfObcE, ni, m_fCamScale, m_fCamRatio, fIex, fIey);

            QPointF ptss = calcCamPosByFunc(fMsx, fMsy, fIsx, fIsy);
            QPointF ptse = calcCamPosByFunc(fMsx, fMsy, fIex, fIey);
            QPointF ptes = calcCamPosByFunc(fMex, fMey, fIsx, fIsy);
            QPointF ptee = calcCamPosByFunc(fMex, fMey, fIex, fIey);
            float dx = ptss.x() - ptee.x();
            float dy = ptss.y() - ptee.y();
            float fd1 = dx * dx + dy * dy;
            dx = ptse.x() - ptes.x();
            dy = ptse.y() - ptes.y();
            float fd2 = dx * dx + dy * dy;
            float fR = sqrtf(fd1) + sqrtf(fd2);//sqrtf(fd1 / fd2);
            if ( fR < 1.f ) fR = 1.f / fR;

            if ( fSelMin > fR )
            {
                fSelMin = fR;
                idxSel = ni;
            }
        }

        if ( idxSel > 0 )
        {
            float fMx, fMy;
            calcCamFunc(fObx0, fOby0, pfObc, idxM, m_fCamScale, m_fCamRatio, fMx, fMy);
            float fIx, fIy;
            calcCamFunc(fObx0, fOby0, pfObc, idxSel, m_fCamScale, m_fCamRatio, fIx, fIy);
            pPts[idxM - 1] = calcCamPosByFunc(fMx, fMy, fIx, fIy);
        }
    }

    //
    float fmax = 0.f;
    for ( int ns = 0; ns < CALC_PNTCNT_MAX - 1; ns++ )
    {
        if ( qIsNaN(pPts[ns].x()) )
            continue;

        for ( int ne = ns + 1; ne < CALC_PNTCNT_MAX; ne++ )
        {
            if ( qIsNaN(pPts[ne].x()) )
                continue;

            float dx = pPts[ne].x() - pPts[ns].x();
            float dy = pPts[ne].y() - pPts[ns].y();
            float fd = dx * dx + dy * dy;
            if ( fmax < fd )
                fmax = fd;
        }
    }

    fmax = sqrtf(fmax);
    return fmax;
}

void QCalcCamValue::scanCamFactor( float * pfObcS, float * pfObcE, float * pfCamS, float * pfCamR,
                                   CB_PROGRESS cb, void* lpUser )
{
    float fmin = FLT_MAX;

    float scale_start = SCALE_START;
    float scale_end = SCALE_END;
    int   scale_step = SCALE_STEP;
    float ratio_start = RATIO_START;
    float ratio_end = RATIO_END;
    int   ratio_step = RATIO_STEP;

    int scale, ratio;
    float fCamS, fCamR;
    for ( scale = 0; scale < scale_step; scale++ )
    {
        fCamS = scale_start + (scale_end - scale_start) * scale / scale_step;
        if ( fCamS == 0.f ) continue;
        for ( ratio = 0; ratio < ratio_step; ratio++ )
        {
            fCamR = ratio_start + (ratio_end - ratio_start) * ratio / ratio_step;
            if ( fCamR == 0.f ) continue;

            setCamFactor(fCamS, fCamR);

            float fV = calcVariance(pfObcS, pfObcE);
            if ( fmin > fV )
            {
                fmin = fV;
                *pfCamS = fCamS;
                *pfCamR = fCamR;
            }
        }

        if ( cb )
        {
            if ( !(*cb)((scale + 1.f) / scale_step, lpUser) )
                break;
        }
    }
}


QPointF QCalcCamValue::vectorToCircle( float x, float y, float fa, float fb, float fr )
{
    QPointF vec;

    if ( qIsFinite(fr) )
    {
        float ax = fa - x;
        float by = fb - y;
        float d = sqrtf(ax * ax + by * by);
        vec.setX(ax * (1.f - fr / d));
        vec.setY(by * (1.f - fr / d));
    }
    else
    {
        float t = (fa * x + fb * y) / (fa * fa + fb * fb);
        vec.setX(fa * t - x);
        vec.setY(fb * t - y);
    }

    return vec;
}

static struct FINGER {
    float x, y;
} s_finger[2][13] = {
    {   // old 13
        { 0.f,  0.f}, // FINGER_0
        {-1.f, -1.f}, // FINGER_1
        {-1.f,  1.f}, // FINGER_2
        { 1.f,  1.f}, // FINGER_3
        { 1.f, -1.f}, // FINGER_4
        {-2.f,  0.f}, // FINGER_5
        { 2.f,  0.f}, // FINGER_6
        {-1.5f, 0.f}, // FINGER_7
        { 1.5f, 0.f}, // FINGER_8
        {-2.f, -1.f}, // FINGER_9
        {-2.f,  1.f}, // FINGER_10
        { 2.f,  1.f}, // FINGER_11
        { 2.f, -1.f}, // FINGER_12
    },
    {   // new 13
        { 0.f,  0.f}, // FINGER_0
        {-1.f, -1.f}, // FINGER_1
        {-1.f,  1.f}, // FINGER_2
        { 1.f,  1.f}, // FINGER_3
        { 1.f, -1.f}, // FINGER_4
        {-2.f,  0.f}, // FINGER_5
        { 2.f,  0.f}, // FINGER_6
        {-1.f,  0.f}, // FINGER_72
        { 1.f,  0.f}, // FINGER_82
        {-2.f, -1.f}, // FINGER_9
        {-2.f,  1.f}, // FINGER_10
        { 2.f,  1.f}, // FINGER_11
        { 2.f, -1.f}, // FINGER_12
    }
};

struct FingerInfo {
    float fa, fb, fr;
};

float QCalcCamValue::calc2Variance( float * pfObcS, float * pfObcE, QPointF * pCamPos )
{
    float fObc0 = (pfObcS[0] + pfObcE[0]) / 2.f;
    if ( qIsNaN(fObc0) )
        return FLT_MAX;

    float fCos, fSin;

    QVector<FingerInfo> aryFingerInfo;
    FingerInfo fi;

    FINGER * finger = s_finger[1];
    if ( !m_bNewMethod )
        finger = s_finger[0];

    float fObx0 = m_fCamScale * fObc0 + 0.5f;
    float fOby0 = (1.f - fObx0) * m_fCamRatio;
    for ( int idxM = 1; idxM <= CALC_PNTCNT_MAX; idxM++ )
    {
        float fObcM = (pfObcS[idxM] + pfObcE[idxM]) / 2.f;
        if ( qIsNaN(fObcM) )
            continue;

        float finger_x = finger[idxM].x;
        float finger_y = finger[idxM].y;

        float fObxM = m_fCamScale * fObcM + 0.5f;
        float fObyM = (1.f - fObxM) * m_fCamRatio;

        SinCosBy12(fObx0, fOby0, fObxM, fObyM, fSin, fCos);
        fi.fr = qAbs(1.f / fSin * sqrtf(finger_x * finger_x + finger_y * finger_y) / 2.f);
        if ( qIsFinite(fi.fr) )
        {
            fi.fa = finger_y / 2.f * fCos / fSin + finger_x / 2.f;
            fi.fb = -finger_x / 2.f * fCos / fSin + finger_y / 2.f;
        }
        else
        {
            fi.fa = finger_x;
            fi.fb = finger_y;
        }
        aryFingerInfo.push_back(fi);
    }

    QPointF ptCam;
    QPointF pts[CALC_PNTCNT_MAX];
    calcVariance(pfObcS, pfObcE, pts);
    int cnt = 0;
    ptCam.setX(0.f);
    ptCam.setY(0.f);
    for ( int ni = 0; ni < CALC_PNTCNT_MAX; ni++ )
    {
        if ( qIsNaN(pts[ni].x()) )
            continue;

        ptCam += pts[ni];
        cnt++;
    }
    ptCam /= cnt;

    for ( int nRecalc = 0; nRecalc < 50; nRecalc++ )
    {
        QPointF vecTot;
        for ( int ni = 0; ni < aryFingerInfo.size(); ni++ )
        {
            QPointF vec = vectorToCircle(ptCam.x(), ptCam.y(), aryFingerInfo[ni].fa, aryFingerInfo[ni].fb, aryFingerInfo[ni].fr);
            if ( ni == 0 )
                vecTot = vec;
            else
                vecTot += vec;
        }
        vecTot /= aryFingerInfo.size();
        ptCam += vecTot;
        if ( qAbs(ptCam.x()) < 0.1f && qAbs(ptCam.y()) < 0.1f )
            return 2.f;
    }

    if ( pCamPos )
        *pCamPos = ptCam;

    float fds[CALC_PNTCNT_MAX];
    calc2ViewlineToPoints(ptCam.x(), ptCam.y(), pfObcS, pfObcE, fds);

    float fd2Tot = 0.f;
    for ( int idxM = 0; idxM < CALC_PNTCNT_MAX; idxM++ )
    {
        if ( qIsNaN(fds[idxM]) )
            continue;

        float fKeyRatio = 1.f;
        switch ( idxM )
        {
        case 1: // Finger_2
        case 2: // Finger_3
            fKeyRatio = 4;
            break;
//		case 9: // Finger_10
//		case 10:// Finger_11
//			fKeyRatio = 2;
//			break;
        default:
            break;
        }
        fd2Tot += fds[idxM] * fKeyRatio;
    }

    return fd2Tot;
}
void QCalcCamValue::calc2ViewlineToPoints( float cam_x, float cam_y,
                                           float * pfObcS, float * pfObcE, float * pFds )
{
    float fObc0 = (pfObcS[0] + pfObcE[0]) / 2.f;
    float fObx0 = m_fCamScale * fObc0 + 0.5f;
    float fOby0 = (1.f - fObx0) * m_fCamRatio;

    float fCmdx = -cam_x;
    float fCmdy = -cam_y;
    float fCamAngle = atan2f(fOby0, fObx0);
    fCamAngle -= atan2f(fCmdy, fCmdx);

    float fCamCos = cosf(fCamAngle);
    float fCamSin = sinf(fCamAngle);
    float fa = m_fCamScale * fCamCos - m_fCamScale * m_fCamRatio * fCamSin;
    float fb = -m_fCamScale * fCamSin - m_fCamScale * m_fCamRatio * fCamCos;
    float fc = (fCamCos + m_fCamRatio * fCamSin) / 2.f;
    float fd = (-fCamSin + m_fCamRatio * fCamCos) / 2.f;

    FINGER * finger = s_finger[1];
    if ( !m_bNewMethod )
        finger = s_finger[0];

    for ( int idxM = 1; idxM <= CALC_PNTCNT_MAX; idxM++ )
    {
        float fObcM = (pfObcS[idxM] + pfObcE[idxM]) / 2.f;
        if ( qIsNaN(fObcM) )
        {
            pFds[idxM - 1] = NaN;
            continue;
        }
        float fdcx = fObcM * fa + fc;
        float fdcy = fObcM * fb + fd;

        float fdcx2dcy2 = fdcx * fdcx + fdcy * fdcy;
        float fdCamxFinx = cam_x - finger[idxM].x;
        float fdCamyFiny = cam_y - finger[idxM].y;
        float ft = (-fdcx * fdCamxFinx - fdcy * fdCamyFiny) / fdcx2dcy2;
        float fdx = fdcx * ft + fdCamxFinx;
        float fdy = fdcy * ft + fdCamyFiny;
        float fd2 = fdx * fdx + fdy * fdy;
        pFds[idxM - 1] = fd2;
    }
}

QPointF QCalcCamValue::scan2CamFactor( float * pfObcS, float * pfObcE, float * pfCamS, float * pfCamR,
                                      CB_PROGRESS cb, void* lpUser )
{
    float fMin = FLT_MAX;

    float scale_width = (SCALE_END - SCALE_START) / SCALE_STEP * 2.f;
    float scale_start = *pfCamS - scale_width;
    float scale_end = *pfCamS + scale_width;
    int   scale_step = SCALE_STEP / 2;
    float ratio_width = (RATIO_END - RATIO_START) / RATIO_STEP * 3.f;
    float ratio_start = *pfCamR - ratio_width;
    float ratio_end = *pfCamR + ratio_width;
    int   ratio_step = RATIO_STEP / 2;

    QPointF ptResultCamPos(0, 0);
    int scale, ratio;
    float fCamS, fCamR;
    for ( scale = 0; scale < scale_step; scale++ )
    {
        fCamS = scale_start + (scale_end - scale_start) * scale / scale_step;
        if ( fCamS == 0.f ) continue;
        for ( ratio = 0; ratio < ratio_step; ratio++ )
        {
            fCamR = ratio_start + (ratio_end - ratio_start) * ratio / ratio_step;
            if ( fCamR == 0.f ) continue;

            setCamFactor(fCamS, fCamR);

            QPointF ptCamPos;
            float fV = calc2Variance(pfObcS, pfObcE, &ptCamPos);
            if ( fMin > fV )
            {
                fMin = fV;
                ptResultCamPos = ptCamPos;
                *pfCamS = fCamS;
                *pfCamR = fCamR;
            }
        }

        if ( cb )
        {
            if ( !(*cb)((scale + 1.f) / scale_step, lpUser) )
                break;
        }
    }

    return ptResultCamPos;
}


float QCalcCamValue::nextStep( float fPrev, float fCur, float fNext, float fStep )
{
    float fMid = (fPrev + fNext) / 2.f;
    if ( fCur >= fMid )
    {
        if ( fPrev > fNext )
        {
            if ( fNext < fMid )
            {
                float fN = fMid / (fMid - fNext) * fStep;
                if ( qAbs(fN) > qAbs(fStep) * 10.f )
                    return qAbs(fStep) * 10.f;
                else
                    return fN;
            }
            else
            {
                return 0.f;
            }
        }
        else
        {
            if ( fPrev < fMid )
            {
                float fN = fMid / (fPrev - fMid) * fStep;
                if ( qAbs(fN) > qAbs(fStep) * 10.f )
                    return -qAbs(fStep) * 10.f;
                else
                    return fN;
            }
            else
            {
                return 0.f;
            }
        }
    }
    else
    {
        float fN = fStep * (fPrev - fNext) / 2.f / (fPrev + fNext - 2.f * fCur);
        fStep = qAbs(fStep);
        if ( fStep < qAbs(fN) )
        {
            if ( fN < 0.f )
                return -fStep;
            else
                return fStep;
        }
        return fN;
    }
}

#define CALCS_CNT  10
#define CALCR_CNT  10

float QCalcCamValue::convergeCamS( float * pfObcS, float * pfObcE, float step )
{
    float cfd = (SCALE_END - SCALE_START) / SCALE_STEP / CALCS_CNT;
    step = qAbs(step);
    if ( step <= 0.000001f )
        return 0.f;
    if ( cfd > step )
        cfd = step;

    float fdO = calcVariance(pfObcS, pfObcE);

    m_fCamScale -= cfd;
    float fdA = calcVariance(pfObcS, pfObcE);

    m_fCamScale += cfd + cfd;
    float fdB = calcVariance(pfObcS, pfObcE);

    float ft = nextStep(fdA, fdO, fdB, cfd);
    m_fCamScale -= cfd;
    m_fCamScale += ft;
    return ft;
}

float QCalcCamValue::convergeCamR( float * pfObcS, float * pfObcE, float step )
{
    float cfd = (RATIO_END - RATIO_START) / RATIO_STEP / CALCR_CNT;
    step = qAbs(step);
    if ( step <= 0.000001f )
        return 0.f;
    if ( cfd > step )
        cfd = step;

    float fdO = calcVariance(pfObcS, pfObcE);

    m_fCamRatio -= cfd;
    float fdA = calcVariance(pfObcS, pfObcE);

    m_fCamRatio += cfd + cfd;
    float fdB = calcVariance(pfObcS, pfObcE);

    float ft = nextStep(fdA, fdO, fdB, cfd);
    m_fCamRatio -= cfd;
    m_fCamRatio += ft;
    return ft;
}

QPointF QCalcCamValue::findCamSR( float * pfObcS, float * pfObcE,
                                 float * pfInitCamS, float * pfInitCamR, QPointF * pPts )
{
    float cfd = (SCALE_END - SCALE_START) / SCALE_STEP / CALCS_CNT;
    m_fCamScale = *pfInitCamS;
    m_fCamRatio = *pfInitCamR;

    float sstep;
    for ( int nj = CALCS_CNT + 5; nj > 0; nj-- )
    {
        sstep = 1.f;
        for ( int ni = CALCR_CNT + 5; ni > 0; ni-- )
            sstep = convergeCamR(pfObcS, pfObcE, sstep);

        float fdO = calcVariance(pfObcS, pfObcE);

        m_fCamScale -= cfd;
        float fdA = calcVariance(pfObcS, pfObcE);

        m_fCamScale += cfd + cfd;
        float fdB = calcVariance(pfObcS, pfObcE);

        float ft = nextStep(fdA, fdO, fdB, cfd);
        m_fCamScale -= cfd;
        m_fCamScale += ft;

        ft = qAbs(ft);
        if ( ft <= 0.000001f )
            break;
        if ( cfd > ft )
            cfd = ft;
    }

    *pfInitCamS = m_fCamScale;
    *pfInitCamR = m_fCamRatio;

    QPointF pts[CALC_PNTCNT_MAX];
    if ( pPts == NULL ) pPts = pts;
    calcVariance(pfObcS, pfObcE, pPts);
    int cnt = 0;
    QPointF ptSum(0, 0);
    for ( int ni = 0; ni < CALC_PNTCNT_MAX; ni++ )
    {
        if ( qIsNaN(pPts[ni].x()) )
            continue;

        ptSum += pPts[ni];
        cnt++;
    }
    ptSum /= cnt;
    return ptSum;
}


float QCalcCamValue::converge2CamR( float * pfObcS, float * pfObcE, float step )
{
    float cfd = (RATIO_END - RATIO_START) / RATIO_STEP / CALCR_CNT;
    step = qAbs(step);
    if ( step <= 0.000001f )
        return 0.f;
    if ( cfd > step )
        cfd = step;

    float fdO = calc2Variance(pfObcS, pfObcE);

    m_fCamRatio -= cfd;
    float fdA = calc2Variance(pfObcS, pfObcE);

    m_fCamRatio += cfd + cfd;
    float fdB = calc2Variance(pfObcS, pfObcE);

    float ft = nextStep(fdA, fdO, fdB, cfd);
    m_fCamRatio -= cfd;
    m_fCamRatio += ft;
    return ft;
}

QPointF QCalcCamValue::find2CamSR( float * pfObcS, float * pfObcE,
                                  float * pfInitCamS, float * pfInitCamR, float * pFds )
{
    float cfd = (SCALE_END - SCALE_START) / SCALE_STEP / CALCS_CNT;
    m_fCamScale = *pfInitCamS;
    m_fCamRatio = *pfInitCamR;

    float sstep;
    for ( int nj = CALCS_CNT + 5; nj > 0; nj-- )
    {
        sstep = 1.f;
        for ( int ni = CALCR_CNT + 5; ni > 0; ni-- )
            sstep = converge2CamR(pfObcS, pfObcE, sstep);

        float fdO = calc2Variance(pfObcS, pfObcE);

        m_fCamScale -= cfd;
        float fdA = calc2Variance(pfObcS, pfObcE);

        m_fCamScale += cfd + cfd;
        float fdB = calc2Variance(pfObcS, pfObcE);

        float ft = nextStep(fdA, fdO, fdB, cfd);
        m_fCamScale -= cfd;
        m_fCamScale += ft;

        ft = qAbs(ft);
        if ( ft <= 0.000001f )
            break;
        if ( cfd > ft )
            cfd = ft;
    }

    *pfInitCamS = m_fCamScale;
    *pfInitCamR = m_fCamRatio;

    QPointF ptCamPos;
    calc2Variance(pfObcS, pfObcE, &ptCamPos);

    if ( pFds )
        calc2ViewlineToPoints(ptCamPos.x(), ptCamPos.y(), pfObcS, pfObcE, pFds);

    return ptCamPos;
}
