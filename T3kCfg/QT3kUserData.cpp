#include "QT3kUserData.h"

class QT3kUserData_GC
{
public:
    QT3kUserData_GC() {}
    ~QT3kUserData_GC() { if( QT3kUserData::m_pInstance ) delete QT3kUserData::m_pInstance; QT3kUserData::m_pInstance = NULL; }
};

static QT3kUserData_GC T3kUD_GC;

QT3kUserData* QT3kUserData::m_pInstance = NULL;

QT3kUserData::QT3kUserData(QObject *parent) :
    QObject(parent)
{
    m_fFirmwareVersion = 0.0f;
    m_bMacMargin = false;
    m_nSelectedVID = 0;
    m_nSelectedPID = 0;
    m_nSelectedIdx = 0xFFFF;
    m_pTopParent = NULL;
    m_pT3kHandle = NULL;
    m_bSubCameraExist = false;
    m_nCamCount = 0;
}

QT3kUserData::~QT3kUserData()
{

}

QT3kUserData* QT3kUserData::GetInstance()
{
    if( !m_pInstance )
        m_pInstance = new QT3kUserData();
    return m_pInstance;
}
