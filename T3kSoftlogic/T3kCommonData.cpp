#include "T3kCommonData.h"

class T3kCommonDataGC
{
public:
    T3kCommonDataGC() {}
    ~T3kCommonDataGC()
    {
        if( T3kCommonData::s_pThis )
        {
            delete T3kCommonData::s_pThis;
            T3kCommonData::s_pThis = NULL;
        }
    }
};

T3kCommonDataGC g_GC;

T3kCommonData* T3kCommonData::s_pThis = NULL;

T3kCommonData::T3kCommonData()
{
    s_pThis = this;

    m_eKeyDataMode = KeyDataModeNormal;
}

T3kCommonData* T3kCommonData::instance()
{
    if( !s_pThis )
        s_pThis = new T3kCommonData();

    return s_pThis;
}

CSoftkeyArray& T3kCommonData::getKeys()
{
    switch( m_eKeyDataMode )
    {
    case KeyDataModeNormal:
        return m_SoftKey;
        break;
    case KeyDataModeCalibration:
        return m_SoftKeyForCalibration;
        break;
    case KeyDataModeTest:
        return m_SoftKeyForTest;
        break;
    }
    return m_SoftKey;
}

void T3kCommonData::resetCalibrationData()
{
    QString strExtra;
    QString strData = m_SoftKey.save(strExtra, NULL);
    QString strBindInfo = m_SoftKey.saveBindInfo();
    QString strGPIOInfo = m_SoftKey.saveGPIOInfo();
    m_SoftKeyForCalibration.load(strData, strExtra, NULL);
    m_SoftKeyForCalibration.loadBindInfo(strBindInfo);
    m_SoftKeyForCalibration.loadGPIOInfo(strGPIOInfo);
    double dX, dY;
    m_SoftKey.getScreenDimension(dX, dY);
    m_SoftKeyForCalibration.setScreenDimension(dX, dY);
    m_SoftKey.getPanelDimension(dX, dY);
    m_SoftKeyForCalibration.setPanelDimension(dX, dY);
    m_SoftKey.getScreenOffset(dX, dY);
    m_SoftKeyForCalibration.setScreenOffset(dX, dY);
    m_SoftKeyForCalibration.setPanelName("for calibration");
}

