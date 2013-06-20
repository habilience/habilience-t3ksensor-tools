#ifndef QSOFTKEY_H
#define QSOFTKEY_H

#include "stdInclude.h"

#include <QVector>
#include <QRect>
#include <QString>

#include "T3kSoftkeyDef.h"

#define NO_GROUP		(NULL)

enum GroupStatus { EnableGroup, EnableUngroup, DisableGroup };

class QSoftkey;
class QGroupKey;

typedef QVector<QSoftkey*>      _GroupKey;
typedef QVector<QGroupKey*>     GroupKeyArray;

class QGroupKey
{
    friend void GroupKeyIndexing( GroupKeyArray& aryGroup );

public:
    QGroupKey( GroupKeyArray* pOwner ) { m_pOwner = pOwner; }
    ~QGroupKey() {}

    int GetCount() const { return m_GroupKey.count(); }
	QSoftkey* GetAt( int nI ) const { return m_GroupKey.at( nI ); }
    void Add( QSoftkey* key ) { m_GroupKey.push_back( key ); } // return int ?
    int GetGroupIndex() const;

protected:
    _GroupKey			m_GroupKey;
    GroupKeyArray*		m_pOwner;
};

class QSoftkey
{
protected:
    QSoftkey(bool bShow, QRect rc/*, ...*/);
    ~QSoftkey();

protected:
    bool                        m_bShow;
    QRect                       m_rcPosition;

    // extra
    QGroupKey*                  m_pGroup;
    QString                     m_strName;

public:
    bool getShow() const { return m_bShow; }
    bool setShow( bool bShow );
    const QRect & getPosition() const { return m_rcPosition; }
    QRect setPosition( QRect rc );

    // extra
    void setGroup( QGroupKey* pGroup );
    const QGroupKey* getGroup() const { return m_pGroup; }
    void setName( const char* lpszName );
    const char* getName() const { return m_strName.toStdString().c_str(); }

    friend class QSoftkeyArray;
};

class QSoftkeyArray
{
public:
    QSoftkeyArray();
    ~QSoftkeyArray();

protected:
    int                         m_nSoftkey;
    int                         m_nHiddenSoftkey;
    QSoftkey**                  m_arySoftkey;

    GroupKeyArray               m_aryGroupKey;

    bool AddSoftkey( QSoftkey * pSoftkey );

public:
    QSoftkey * operator [] ( int idx );
    int GetSize() { return m_nSoftkey; }
    int GetShowSize() { return m_nSoftkey-m_nHiddenSoftkey; }

    GroupKeyArray& GetGroupKeyArray() { return m_aryGroupKey; }

public:
    void RemoveAll();
    bool Load( const char* lpctszCmd, const char* lpctszExtra );
    QString Save( QString & strExtra );

public:
    int AddSoftkey( bool bShow, QRect rc/*, ...*/ );

//    void Remove( QSoftkey* key, QSoftlogicArray* logic );
//    void Remove( int nRemoveIndex, QSoftlogicArray* logic );
};

class QSoftlogic
{
protected:
    QSoftlogic();
    ~QSoftlogic();

public:
    typedef enum
    {
        eltNILL = 0,
        eltAND, eltOR, eltXOR, eltAND_RISINGEDGE
    } LogicType;
    typedef enum
    {
        epNoGate                = 0,
        epStateWorking          = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_WORKING_AND_BEEP1,
        epStateTouch            = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_TOUCH_AND_BEEP2,
        epStateCalibration      = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_CALIBRATION,
        epStateTouchScreen      = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_TOUCHSCREEN,
        epStateMultiTouchDevice = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_MULTITOUCH_DEVICE,
        epGpio0                 = MM_SOFTLOGIC_PORT_GPIO_START,
        epSoftkey0              = MM_SOFTLOGIC_PORT_SOFTKEY_START,
        epSoftkey1,  epSoftkey2,  epSoftkey3,  epSoftkey4,  epSoftkey5,
        epSoftkey6,  epSoftkey7,  epSoftkey8,  epSoftkey9,  epSoftkey10,
        epSoftkey11, epSoftkey12, epSoftkey13, epSoftkey14, epSoftkey15,
        epSoftkey16, epSoftkey17, epSoftkey18, epSoftkey19, epSoftkey20,
        epSoftkey21, epSoftkey22, epSoftkey23, epSoftkey24, epSoftkey25,
        epSoftkey26, epSoftkey27, epSoftkey28, epSoftkey29,
        epSoftkeyAll            = MM_SOFTLOGIC_PORT_SOFTKEY_ALL,
        epSoftLogic0            = MM_SOFTLOGIC_PORT_SOFTLOGIC_START,
    } Port;
    typedef enum
    {
        eptError = 0, eptKey, eptMouse, eptState, eptGPIO, eptReport, eptSoftkey
    } PortType;
    typedef enum
    {
        etNoTrigger = 0,
        etTrigger_U100_D100, etTrigger_U250_D250, etTrigger_U500_D500, etTrigger_U1000_D1000, etTrigger_U2000_D2000,
        etTrigger_U250_D0,   etTrigger_U500_D0,   etTrigger_U1000_D0,  etTrigger_U2000_D0,    etTrigger_U3000_D0,
        etTrigger_U200_D50,  etTrigger_U400_D100, etTrigger_U750_D250, etTrigger_U1500_D500,  etTrigger_U2000_D1000,
    } Trigger;

protected:
    LogicType                   m_eLogicType;
    bool                        m_bEnableNot;
    Port                        m_eEnablePort;
    bool                        m_bIn1Not;
    Port                        m_eIn1Port;
    bool                        m_bIn2Not;
    Port                        m_eIn2Port;

    Trigger                     m_eTrigger;

    PortType                    m_eOutType;
    uchar                       m_out1;
    uchar                       m_out2;

public:
    static PortType ParsePort( Port port, int & idx );

    LogicType getLogicType() { return m_eLogicType; }
    bool setLogicType( LogicType lt );

    bool getEnableNot() const;
    bool setEnableNot( bool bNot );
    Port getEnablePort() const;
    bool setEnablePort( Port enablePort );

    bool getIn1Not() const { return m_bIn1Not; }
    bool setIn1Not( bool bNot );
    Port getIn1Port() const { return m_eIn1Port; }
    bool setIn1Port( Port inPort );

    bool getIn2Not() const { return m_bIn2Not; }
    bool setIn2Not( bool bNot );
    Port getIn2Port() const { return m_eIn2Port; }
    bool setIn2Port( Port inPort );

    PortType getOutPortType() const { return m_eOutType; }

    uchar getOutKey1ConditionKeys() const;
    uchar getOutKey1() const;
    bool setOutKey1( uchar conditionKeys, uchar key );

    uchar getOutMouse() const;
    bool setOutMouse( uchar conditionKeys, uchar mkey );
    Trigger getLogicTrigger() const;
    bool setLogicTrigger( Trigger trigger );

    Port getOutPort() const;
    bool setOutPort( Port outPort );

    int getReportId() const;
    bool setReportId( int report_id );

friend class QSoftlogicArray;
};


class QSoftlogicArray
{
public:
    QSoftlogicArray();
    ~QSoftlogicArray();

protected:
    int                         m_nSoftlogic;
    QSoftlogic**                m_arySoftlogic;

    int _InsertSoftlogic( QSoftlogic * pSoftlogic, bool bHidden = false );
    QSoftlogic * _RemoveSoftlogic( int idx );
    void _ExchangePort( QSoftlogic::Port port1, QSoftlogic::Port port2 );
	void _ReplacePort( QSoftlogic::Port port, QSoftlogic::Port portNew );

public:
    QSoftlogic * operator [] ( int idx );
    int GetShowSize() const { return m_nSoftlogic; }
    int IndexFromSoftlogic( QSoftlogic * pSoftlogic ) const;

public:
    void RemoveAll();
    bool Load( const char* lpctszCmd, const char* lpctszExtra );
    QString Save( QString & strExtra );

public:
    int AddSoftlogic_Key( QSoftlogic::LogicType logictype,
            bool in1_not, QSoftlogic::Port in1_port,
            bool in2_not, QSoftlogic::Port in2_port,
            uchar condition_key, uchar key,
            QSoftlogic::Trigger trigger = QSoftlogic::etNoTrigger, bool bHidden = false );
    int AddSoftlogic_Mouse( QSoftlogic::LogicType logictype,
            bool in1_not, QSoftlogic::Port in1_port,
            bool in2_not, QSoftlogic::Port in2_port,
            uchar condition_key, uchar mkey,
            QSoftlogic::Trigger trigger = QSoftlogic::etNoTrigger, bool bHidden = false );
    int AddSoftlogic_State( QSoftlogic::LogicType logictype,
            bool enable_not, QSoftlogic::Port enable_port,
            bool in1_not, QSoftlogic::Port in1_port,
            bool in2_not, QSoftlogic::Port in2_port,
            QSoftlogic::Port out_port,
            QSoftlogic::Trigger trigger = QSoftlogic::etNoTrigger, bool bHidden = false );
    int AddSoftlogic_Report( QSoftlogic::LogicType logictype,
            bool enable_not, QSoftlogic::Port enable_port,
            bool in1_not, QSoftlogic::Port in1_port,
            bool in2_not, QSoftlogic::Port in2_port,
            int report_id,
            QSoftlogic::Trigger trigger = QSoftlogic::etNoTrigger, bool bHidden = false );

public:
    bool ExchangeSotflogic( int idx1, int idx2 );
    int MoveSoftlogicTo( int from, int to );

    void Remove( QSoftlogic * logic ) { int idx = IndexFromSoftlogic(logic); if ( idx >= 0 ) Remove(idx); }
    void Remove( int idx );

    void TrimmingLogic( bool bTrimEndOnly = false );

    friend class QSoftkeyArray;
};

typedef struct tagLOGIC
{
    QSoftlogic::LogicType       logictype;
    bool                        port2;
    bool                        trigger;
    bool                        mouse_button;
    bool                        condition_key1;
    bool                        key1;
    bool                        outport; // epNoGate < outport < epSoftkey0
    bool                        report;  // 0 ~ 63
} LOGIC;
/*
static LOGIC softlogic_list[] =
{   // logictype                     port2  enable m_btn  cond1  key1  outport report
    // key
    { QSoftlogic::eltAND,            true , false, false, true , true , false, false },
    { QSoftlogic::eltAND,            false, false, false, true , true , false, false },
    { QSoftlogic::eltOR,             true , false, false, true , true , false, false },
    { QSoftlogic::eltXOR,            true , false, false, true , true , false, false },
    { QSoftlogic::eltAND_RISINGEDGE, true , false, false, true , true , false, false },
    { QSoftlogic::eltAND_RISINGEDGE, false, false, false, true , true , false, false },
    // mouse
//    { QSoftlogic::eltAND,            true , false, true , true , false, false, false },
//    { QSoftlogic::eltAND,            false, false, true , true , false, false, false },
//    { QSoftlogic::eltAND_RISINGEDGE, true , false, true , true , false, false, false },
//    { QSoftlogic::eltAND_RISINGEDGE, false, false, true , true , false, false, false },
    // state button
    { QSoftlogic::eltAND,            true , true , false, false, false, true , false },
    { QSoftlogic::eltAND,            false, true , false, false, false, true , false },
    { QSoftlogic::eltOR,             true , true , false, false, false, true , false },
    { QSoftlogic::eltXOR,            true , true , false, false, false, true , false },
    { QSoftlogic::eltAND_RISINGEDGE, true , true , false, false, false, true , false },
    { QSoftlogic::eltAND_RISINGEDGE, false, true , false, false, false, true , false },
    // report button
    { QSoftlogic::eltAND,            true , true , false, false, false, false, true  },
    { QSoftlogic::eltAND,            false, true , false, false, false, false, true  },
    { QSoftlogic::eltOR,             true , true , false, false, false, false, true  },
    { QSoftlogic::eltXOR,            true , true , false, false, false, false, true  },
    { QSoftlogic::eltAND_RISINGEDGE, true , true , false, false, false, false, true  },
    { QSoftlogic::eltAND_RISINGEDGE, false, true , false, false, false, false, true  },
};
*/
#endif // QSOFTKEY_H
