#ifndef QT3KUSERDATA_H
#define QT3KUSERDATA_H

#include <QObject>
#include <QMutex>
#include <QTcpSocket>

class QT3kUserData_GC;
class QT3kUserData : public QObject, public QObjectUserData
{
    Q_OBJECT

public:
    friend class QT3kUserData_GC;
    explicit QT3kUserData(QObject *parent = 0);
    ~QT3kUserData();

    static QT3kUserData* GetInstance();

    float GetFirmwareVersion() { QMutexLocker Lock(&m_Mutex); return m_fFirmwareVersion; }
    void SetFirmwareVersion(float fVersion) { QMutexLocker Lock(&m_Mutex); m_fFirmwareVersion = fVersion; }

    bool IsMacMargin() { QMutexLocker Lock(&m_Mutex); return m_bMacMargin; }
    void SetMacMargin( bool bMacMargin ) { QMutexLocker Lock(&m_Mutex); m_bMacMargin = bMacMargin; }

    ushort GetModel() { QMutexLocker Lock(&m_Mutex); return m_nModel; }
    void SetModel( ushort nModel ) { QMutexLocker Lock(&m_Mutex); m_nModel = nModel; }
    ushort GetSelectedVID() { QMutexLocker Lock(&m_Mutex); return m_nSelectedVID; }
    void SetSelectedVID( ushort nSelectedVID ) { QMutexLocker Lock(&m_Mutex); m_nSelectedVID = nSelectedVID; }
    ushort GetSelectedPID() { QMutexLocker Lock(&m_Mutex); return m_nSelectedPID; }
    void SetSelectedPID( ushort nSelectedPID ) { QMutexLocker Lock(&m_Mutex); m_nSelectedPID = nSelectedPID; }
    ushort GetSelectedIdx() { QMutexLocker Lock(&m_Mutex); return m_nSelectedIdx; }
    void SetSelectedIdx( ushort nSelectedIdx ) { QMutexLocker Lock(&m_Mutex); m_nSelectedIdx = nSelectedIdx; }

    QString GetProgramTitle() { QMutexLocker Lock(&m_Mutex); return m_strTitle; }
    void SetProgramTitle( QString strTitle ) { QMutexLocker Lock(&m_Mutex); m_strTitle = strTitle; }
    QString GetProgramInfo() { QMutexLocker Lock(&m_Mutex); return m_strProgInfo; }
    void SetProgramInfo( QString strInfo ) { QMutexLocker Lock(&m_Mutex); m_strProgInfo = strInfo; }

    QTcpSocket* GetRemoteSocket() { QMutexLocker Lock(&m_Mutex); return &m_RemoteSocket; }

    void setTopParent(QWidget* pParent) { m_pTopParent = pParent; }
    QWidget* getTopParent() { return m_pTopParent; }

protected:
    static QT3kUserData*    m_pInstance;
    QMutex                  m_Mutex;

    float                   m_fFirmwareVersion;
    bool                    m_bMacMargin;
    ushort                  m_nModel;
    short                   m_nSelectedVID;
    short                   m_nSelectedPID;
    short                   m_nSelectedIdx;

    QString                 m_strTitle;
    QString                 m_strProgInfo;

    // remote
    QTcpSocket              m_RemoteSocket;

    QWidget*                m_pTopParent;

signals:

public slots:

};

#endif // QT3KUSERDATA_H
