#ifndef T3kCommonData_H
#define T3kCommonData_H

#include "SoftKey.h"

#define DEV_COORD		(0x7fff)


class T3kCommonDataGC;
class T3kCommonData
{
public:
    T3kCommonData();

    enum KeyDataMode { KeyDataModeNormal, KeyDataModeCalibration, KeyDataModeTest };

    void setKeyDataMode( KeyDataMode eMode ) { m_eKeyDataMode = eMode; }
    KeyDataMode getKeyDataMode() { return m_eKeyDataMode; }

    CSoftkeyArray& getKeys();
    CSoftlogicArray& getLogics() { return m_SoftLogic; }
    GroupKeyArray& getGroupKeys() { return getKeys().getGroupKeyArray(); }
    void resetCalibrationData();

    QString getLoadedModelPathName() { return m_strLoadedModelPathName; }
    void setLoadedModelPathName(QString str) { m_strLoadedModelPathName = str; }

    friend class T3kCommonDataGC;
    static T3kCommonData* instance();

protected:
    static T3kCommonData* s_pThis;

    CSoftkeyArray		m_SoftKey;
    CSoftkeyArray		m_SoftKeyForCalibration;
    CSoftkeyArray		m_SoftKeyForTest;
    CSoftlogicArray		m_SoftLogic;

    KeyDataMode         m_eKeyDataMode;

    QString             m_strLoadedModelPathName;
};

#endif // T3kCommonData_H
