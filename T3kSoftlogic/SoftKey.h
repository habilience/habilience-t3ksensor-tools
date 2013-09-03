#ifndef CSOFTKEY_H
#define CSOFTKEY_H

#include <QVector>
#include <QRect>
#include <QString>

#include "SoftkeyDef.h"


enum GroupStatus { EnableGroup, EnableUngroup, DisableGroup };

class QIniFormat;

class CSoftkey;
class GroupKey;

typedef QVector<CSoftkey*> _GroupKey;
typedef QVector<GroupKey*> GroupKeyArray;


class GroupKey
{
friend void GroupKeyIndexing( GroupKeyArray& aryGroup );

public:
	GroupKey( GroupKeyArray* pOwner ) { m_pOwner = pOwner; }
	~GroupKey() {}

    int getCount() const { return m_GroupKey.count(); }
    CSoftkey* getAt( int nI ) { return m_GroupKey.at(nI); } // *& -> *
    CSoftkey* const & getAt( int nI ) const { return ((const _GroupKey*)&m_GroupKey)->at(nI); }

    bool swap( int nIdx1, int nIdx2 );

    CSoftkey* const & operator[](int nIndex) const { return getAt(nIndex); }
    CSoftkey* operator[](int nIndex) { return getAt(nIndex); } // *& -> *

    void add( CSoftkey* key ) { return m_GroupKey.push_back(key); }
    int getGroupIndex() const;

protected:
	_GroupKey			m_GroupKey;
	GroupKeyArray*		m_pOwner;
};


class CSoftkey
{
protected:
    CSoftkey( bool bShow, QRect rc/*, ...*/ );
	~CSoftkey();

protected:
    bool                m_bShow;
    QRect               m_rcPosition;

	// bind
    CSoftkey*           m_pSoftkeyBind;

	// extra
    GroupKey*           m_pGroup;
    QString             m_strName;

public:
    QPointF             m_ptPosition;

public:
    bool getShow() const { return m_bShow; }
    bool setShow( bool bShow );
    const QRect & getPosition() const { return m_rcPosition; }
    QRect setPosition( QRect rc );

	CSoftkey * getBind() const { return m_pSoftkeyBind; }
	void setBind( CSoftkey* key ) { m_pSoftkeyBind = key; }

	// extra
	void setGroup( GroupKey* pGroup );
	const GroupKey* getGroup() const { return m_pGroup; }
    void setName( QString lpszName );
    QString getName() const { return m_strName; }

friend class CSoftkeyArray;
};

typedef struct _GPIOInfo
{
    bool bEnable;
    bool bOutput;
} GPIOInfo;


class CSoftlogicArray;
class CSoftkeyArray
{
public:
	CSoftkeyArray();
	~CSoftkeyArray();

protected:
	int         m_nSoftkey;
	CSoftkey ** m_arySoftkey;

	GroupKeyArray m_aryGroupKey;

	// extra panel info.
    QString		m_strPanelName;
	double		m_dPanelDimWidth, m_dPanelDimHeight;
	double		m_dScrnDimWidth, m_dScrnDimHeight;
	double		m_dScrnOffsetX, m_dScrnOffsetY;

    bool		m_bIsModified;
    QString		m_strBackupCmd;
    QString		m_strBackupExtra;

    QVector<GPIOInfo>	m_aryGPIOInfo;

    bool addSoftkey( CSoftkey * pSoftkey );

public:
	CSoftkey * operator [] ( int idx );
    int getSize() { return m_nSoftkey; }

    GroupKeyArray& getGroupKeyArray() { return m_aryGroupKey; }

    GPIOInfo* getGPIOInfo( int idx ); // return *? &?
    int getGPIOCount();
    void createGPIO( int nCount );

public:
    void clear();

    void resetPanelInfo();

    void reOrder( CSoftlogicArray* logic );

    bool isModified();
    void setModified( bool bModified );

    QString getPanelName() const { return m_strPanelName; }
    void setPanelName( QString lpszPanelName );
    void getPanelDimension( double &dWidth, double &dHeight ) const { dWidth = m_dPanelDimWidth; dHeight = m_dPanelDimHeight; }
    void setPanelDimension( double dWidth, double dHeight );
    void getScreenDimension( double &dWidth, double &dHeight ) const { dWidth = m_dScrnDimWidth; dHeight = m_dScrnDimHeight; }
    double getScreenDimensionW() { return m_dScrnDimWidth; }
    double getScreenDimensionH() { return m_dScrnDimHeight; }
    void setScreenDimension( double dWidth, double dHeight );
    void getScreenOffset( double &dX, double &dY ) const { dX = m_dScrnOffsetX; dY = m_dScrnOffsetY; }
    void setScreenOffset( double dX, double dY );

    bool load( QString lpctszCmd, QString lpctszExtra = "", QIniFormat * ini = NULL );
    QString save( QString & strExtra, QIniFormat * ini, bool bCheck = false );

    void resetBindInfo();
    void loadBindInfo( QString lpctszCmd );
    QString saveBindInfo();

    void loadGPIOInfo( QString lpctszCmd );
    QString saveGPIOInfo();

    int indexFromSoftkey( CSoftkey* pSoftkey );

public:
    int addSoftkey( bool bShow, QRect rc/*, ...*/ );

    void remove( CSoftkey* key, CSoftlogicArray* logic );
    void remove( int nRemoveIndex, CSoftlogicArray* logic );
};


class CSoftlogic
{
protected:
	CSoftlogic();
	~CSoftlogic();

public:
    enum LogicType
	{
		eltNILL = 0,
		eltAND, eltOR, eltXOR, eltAND_RISINGEDGE
	};
    enum Port
	{
		epNoGate                = 0,
		epStateWorking          = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_WORKING_AND_BEEP1,
		epStateTouch            = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_TOUCH_AND_BEEP2,
		epStateCalibration      = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_CALIBRATION,
		epStateTouchScreen      = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_TOUCHSCREEN,
		epStateMultiTouchDevice = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_MULTITOUCH_DEVICE,
		epStateInvertDetection  = MM_SOFTLOGIC_PORT_STATE_START + MM_SOFTLOGIC_STATE_INVERT_DETECTION,
		epGpio0                 = MM_SOFTLOGIC_PORT_GPIO_START,
		epSoftkey0              = MM_SOFTLOGIC_PORT_SOFTKEY_START,
		epSoftkey1,  epSoftkey2,  epSoftkey3,  epSoftkey4,  epSoftkey5,
		epSoftkey6,  epSoftkey7,  epSoftkey8,  epSoftkey9,  epSoftkey10,
		epSoftkey11, epSoftkey12, epSoftkey13, epSoftkey14, epSoftkey15,
		epSoftkey16, epSoftkey17, epSoftkey18, epSoftkey19, epSoftkey20,
		epSoftkey21, epSoftkey22, epSoftkey23, epSoftkey24, epSoftkey25,
		epSoftkey26, epSoftkey27, epSoftkey28, epSoftkey29,
		epSoftkeyAll       = MM_SOFTLOGIC_PORT_SOFTKEY_ALL,
        epSoftLogic0       = MM_SOFTLOGIC_PORT_SOFTLOGIC_START
	};
    enum PortType
	{
		eptError = 0, eptKey, eptMouse, eptState, eptGPIO, eptReport, eptSoftkey
	};
    enum Trigger
	{
		etNoTrigger = 0,
        etTrigger_U128_D128, etTrigger_U256_D256, etTrigger_U512_D512, etTrigger_U1024_D1024, etTrigger_U2048_D2048,
        etTrigger_U256_D0,   etTrigger_U512_D0,   etTrigger_U1024_D0,  etTrigger_U2048_D0,    etTrigger_U3072_D0,
        etTrigger_U192_D64,  etTrigger_U384_D128, etTrigger_U768_D256, etTrigger_U1536_D512,  etTrigger_U2048_D1024
	};

protected:
	LogicType m_eLogicType;
    bool      m_bEnableNot;
	Port      m_eEnablePort;
    bool      m_bIn1Not;
	Port      m_eIn1Port;
    bool      m_bIn2Not;
	Port      m_eIn2Port;

	Trigger  m_eTrigger;

	PortType m_eOutType;
    uchar     m_out1;
    uchar     m_out2;

public:
    static PortType parsePort( Port port, int & idx );

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

	// extra info
public:
    QPointF m_ptPosition;
	float  m_fLines[2];

friend class CSoftlogicArray;
};


class CSoftlogicArray
{
public:
	CSoftlogicArray();
	~CSoftlogicArray();

protected:
	int           m_nSoftlogic;
	CSoftlogic ** m_arySoftlogic;
	int           m_nSoftlogicHidden;
	CSoftlogic ** m_arySoftlogicHidden;

    bool            m_bIsModified;


    int _insertSoftlogic( CSoftlogic * pSoftlogic, bool bHidden = false );
    CSoftlogic * _removeSoftlogic( int idx );
    void _exchangePort( CSoftlogic::Port port1, CSoftlogic::Port port2 );
    void _replacePort( CSoftlogic::Port port, CSoftlogic::Port portNew );

    void _setLogicPosFromIni( QIniFormat * ini, int idx );
    void _getLogicPosFromIni( QIniFormat * ini, int idx_logic, float& fPositionX, float& fPositionY, float& fLine1, float& fLine2  );

public:
	CSoftlogic * operator [] ( int idx );
    int getSize() { return m_nSoftlogicHidden > 0 ? SOFT_LOGIC_MAX :m_nSoftlogic; }

    int getShowSize() const { return m_nSoftlogic; }
    int getHideSize() const { return m_nSoftlogicHidden; }
    int indexFromSoftlogic( CSoftlogic * pSoftlogic ) const;
    bool isHiddenLogic( int idx ) const;

public:
    void clear();
    bool load( QString lpctszCmd, QIniFormat * ini = NULL );
    QString save( QIniFormat * ini );

    bool isModified( QString lpctszCmd, QIniFormat * ini = NULL );

public:
    int addSoftlogic_Key( CSoftlogic::LogicType logictype,
        bool in1_not, CSoftlogic::Port in1_port,
        bool in2_not, CSoftlogic::Port in2_port,
        uchar condition_key, uchar key,
        CSoftlogic::Trigger trigger = CSoftlogic::etNoTrigger, bool bHidden = false );
    int addSoftlogic_Mouse( CSoftlogic::LogicType logictype,
        bool in1_not, CSoftlogic::Port in1_port,
        bool in2_not, CSoftlogic::Port in2_port,
        uchar condition_key, uchar mkey,
        CSoftlogic::Trigger trigger = CSoftlogic::etNoTrigger, bool bHidden = false );
    int addSoftlogic_State( CSoftlogic::LogicType logictype,
        bool enable_not, CSoftlogic::Port enable_port,
        bool in1_not, CSoftlogic::Port in1_port,
        bool in2_not, CSoftlogic::Port in2_port,
		CSoftlogic::Port out_port,
        CSoftlogic::Trigger trigger = CSoftlogic::etNoTrigger, bool bHidden = false );
    int addSoftlogic_Report( CSoftlogic::LogicType logictype,
        bool enable_not, CSoftlogic::Port enable_port,
        bool in1_not, CSoftlogic::Port in1_port,
        bool in2_not, CSoftlogic::Port in2_port,
		int report_id,
        CSoftlogic::Trigger trigger = CSoftlogic::etNoTrigger, bool bHidden = false );

public:
    bool exchangeSotflogic( int idx1, int idx2 );
    int moveSoftlogicTo( int from, int to );
    int hideSoftlogic( int idx, bool bHidden );

    void remove( CSoftlogic * logic ) { int idx = indexFromSoftlogic(logic); if ( idx >= 0 ) remove(idx); }
    void remove( int idx );

    void trimmingLogic( bool bTrimEndOnly = false );

friend class CSoftkeyArray;
};


typedef struct tagLOGIC
{
	CSoftlogic::LogicType logictype;
    bool  port2;
    bool  trigger;
    bool  mouse_button;
    bool  condition_key1;
    bool  key1;
    bool  outport; // epNoGate < outport < epSoftkey0
    bool  report;  // 0 ~ 63
} LOGIC;
//static LOGIC softlogic_list[] =
//{	// logictype                     port2  enable m_btn  cond1  key1  outport report
//		// key
//    { CSoftlogic::eltAND,            true , false, false, true , true , false, false },
//    { CSoftlogic::eltAND,            false, false, false, true , true , false, false },
//    { CSoftlogic::eltOR,             true , false, false, true , true , false, false },
//    { CSoftlogic::eltXOR,            true , false, false, true , true , false, false },
//    { CSoftlogic::eltAND_RISINGEDGE, true , false, false, true , true , false, false },
//    { CSoftlogic::eltAND_RISINGEDGE, false, false, false, true , true , false, false },
///*		// mouse
//    { CSoftlogic::eltAND,            true , false, true , true , false, false, false },
//    { CSoftlogic::eltAND,            false, false, true , true , false, false, false },
//    { CSoftlogic::eltAND_RISINGEDGE, true , false, true , true , false, false, false },
//    { CSoftlogic::eltAND_RISINGEDGE, false, false, true , true , false, false, false },*/
//		// state button
//    { CSoftlogic::eltAND,            true , true , false, false, false, true , false },
//    { CSoftlogic::eltAND,            false, true , false, false, false, true , false },
//    { CSoftlogic::eltOR,             true , true , false, false, false, true , false },
//    { CSoftlogic::eltXOR,            true , true , false, false, false, true , false },
//    { CSoftlogic::eltAND_RISINGEDGE, true , true , false, false, false, true , false },
//    { CSoftlogic::eltAND_RISINGEDGE, false, true , false, false, false, true , false },
//		// report button
//    { CSoftlogic::eltAND,            true , true , false, false, false, false, true  },
//    { CSoftlogic::eltAND,            false, true , false, false, false, false, true  },
//    { CSoftlogic::eltOR,             true , true , false, false, false, false, true  },
//    { CSoftlogic::eltXOR,            true , true , false, false, false, false, true  },
//    { CSoftlogic::eltAND_RISINGEDGE, true , true , false, false, false, false, true  },
//    { CSoftlogic::eltAND_RISINGEDGE, false, true , false, false, false, false, true  },
//};

#endif // CSOFTKEY_H
