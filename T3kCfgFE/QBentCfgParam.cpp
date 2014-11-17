#include "QBentCfgParam.h"

QBentCfgParam::QBentCfgParam()
{
    m_fBentMargin[0] = BENT_MARGIN_INIT_LEFT / 100.f;
    m_fBentMargin[1] = BENT_MARGIN_INIT_TOP / 100.f;
    m_fBentMargin[2] = BENT_MARGIN_INIT_RIGHT / 100.f;
    m_fBentMargin[3] = BENT_MARGIN_INIT_BOTTOM / 100.f;
    m_nBentDirection = BENT_MARGIN_INIT_DIR;
    m_nAlgorithm = 2;
    m_bBentWithDummy = false;
    m_bIgnoreCameraPair = false;
}
