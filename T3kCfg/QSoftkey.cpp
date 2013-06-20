#include "QSoftkey.h"

#include <QMap>

#ifndef QT_NO_DEBUG
#define Q_VERIFY(f)         (Q_ASSERT(f))
#else
#define Q_VERIFY(f)         ((void)(f))
#endif

int QGroupKey::GetGroupIndex() const
{
    if( !m_pOwner ) return -1;

    for( int nI=0 ; nI<m_pOwner->count() ; nI++ )
    {
        QGroupKey* pGroup = m_pOwner->at( nI );
        if( pGroup == this )
            return nI;
    }
    return -1;
}

///////////////////////////////////////////////////////////////////////////////
// CSoftkey

QSoftkey::QSoftkey( bool bShow, QRect rc )
{
    m_bShow = bShow;
    m_rcPosition = rc;
    m_pGroup = NO_GROUP;
}

QSoftkey::~QSoftkey()
{
}

bool QSoftkey::setShow( bool bShow )
{
    bool bRet = m_bShow;
    m_bShow = bShow;
    return bRet;
}

QRect QSoftkey::setPosition( QRect rc )
{
    QRect rcRet = m_rcPosition;
    m_rcPosition = rc;
    return rcRet;
}

void QSoftkey::setGroup( QGroupKey* pGroup )
{
    m_pGroup = pGroup;
}

void QSoftkey::setName( const char* lpszName )
{
    m_strName = lpszName;
}

///////////////////////////////////////////////////////////////////////////////
// QSoftkeyArray

QSoftkeyArray::QSoftkeyArray()
{
    m_nSoftkey = 0;
    m_nHiddenSoftkey = 0;
    m_arySoftkey = NULL;
}

QSoftkeyArray::~QSoftkeyArray()
{
    RemoveAll();
}

void QSoftkeyArray::RemoveAll()
{
    for( int nI=0 ; nI<m_aryGroupKey.count() ; nI++ )
    {
        delete m_aryGroupKey[nI];
    }

    m_aryGroupKey.clear();

    if ( m_arySoftkey != NULL )
    {
        for ( m_nSoftkey--; m_nSoftkey >= 0; m_nSoftkey-- )
        {
            delete m_arySoftkey[m_nSoftkey];
        }
        delete [] m_arySoftkey;
        m_arySoftkey = NULL;
    }
    m_nSoftkey = 0;
    m_nHiddenSoftkey = 0;
}

bool QSoftkeyArray::AddSoftkey( QSoftkey* pSoftkey )
{
    if ( m_nSoftkey >= SOFT_KEY_MAX )
        return false;

    QSoftkey** arySoftkeyNew = new QSoftkey*[m_nSoftkey+1];
    if( m_arySoftkey )
    {
        memcpy(arySoftkeyNew, m_arySoftkey, m_nSoftkey * sizeof(QSoftkey*));
        delete [] m_arySoftkey;
    }
    m_arySoftkey = arySoftkeyNew;
    m_arySoftkey[m_nSoftkey++] = pSoftkey;
    if( !pSoftkey->getShow() ) m_nHiddenSoftkey++;
    return true;
}

QSoftkey* QSoftkeyArray::operator[]( int idx )
{
    if ( idx < 0 || idx >= m_nSoftkey )
            return NULL;

    return m_arySoftkey[idx];
}

bool QSoftkeyArray::Load( const char* lpctszCmd, const char* lpctszExtra )
{
    RemoveAll();

    QString strExtra = lpctszExtra;
    QString strE;

    // Check : TCHAR
    const char * buf = lpctszCmd;
    unsigned short buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, buf9, buf10;

    QMap<int, QGroupKey*> GroupKeyIndexMap;

    int ni;
    for ( ni = 0; ni < SOFT_KEY_MAX; ni++, buf += 11 )
    {
        if ( buf[0] == 0 || buf[1] == 0 || buf[2] == 0 || buf[3] == 0 ||
             buf[4] == 0 || buf[5] == 0 || buf[6] == 0 || buf[7] == 0 ||
             buf[8] == 0 || buf[9] == 0 || buf[10] == 0 )
            break;

        if ( buf[0] == 'p' ) buf0 = ('\\' - '0'); else buf0 = buf[0] - '0';
        if ( buf[1] == 'p' ) buf1 = ('\\' - '0'); else buf1 = buf[1] - '0';
        if ( buf[2] == 'p' ) buf2 = ('\\' - '0'); else buf2 = buf[2] - '0';
        if ( buf[3] == 'p' ) buf3 = ('\\' - '0'); else buf3 = buf[3] - '0';
        if ( buf[4] == 'p' ) buf4 = ('\\' - '0'); else buf4 = buf[4] - '0';
        if ( buf[5] == 'p' ) buf5 = ('\\' - '0'); else buf5 = buf[5] - '0';
        if ( buf[6] == 'p' ) buf6 = ('\\' - '0'); else buf6 = buf[6] - '0';
        if ( buf[7] == 'p' ) buf7 = ('\\' - '0'); else buf7 = buf[7] - '0';
        if ( buf[8] == 'p' ) buf8 = ('\\' - '0'); else buf8 = buf[8] - '0';
        if ( buf[9] == 'p' ) buf9 = ('\\' - '0'); else buf9 = buf[9] - '0';
        if ( buf[10] == 'p' ) buf10 = ('\\' - '0'); else buf10 = buf[10] - '0';

        int nSP = strExtra.indexOf( ',' );
        if( nSP >= 0 )
        {
            strE = strExtra.left( nSP );
            strExtra.remove( 0, nSP+1 );
        }
        else
        {
            if( strExtra.size() )
            {
                strE = strExtra;
            }
        }

        bool show = (buf0 >> 4) != 0x0;
        ushort left = ((buf0 & 0x0f) << 12) | (buf1 << 6) | buf2;
        ushort top = (buf3 << 10) | (buf4 << 4) | (buf5 >> 2);
        ushort right = ((buf5 & 0x03) << 14) | (buf6 << 8) | (buf7 << 2) | (buf8 >> 4);
        ushort bottom = ((buf8 & 0x0f) << 12) | (buf9 << 6) | buf10;

        QRect rc;
        rc.setLeft( (short)left * 2 );
        rc.setTop( (short)top * 2 );
        rc.setRight( (short)right * 2 );
        rc.setBottom( (short)bottom * 2 );

        if( !show && rc.isEmpty() )
        {
            continue;
        }
        QSoftkey * pSoftkey = new QSoftkey(show, rc);
        AddSoftkey(pSoftkey);

        QString strName;
        if( strE.size() )
        {
            int nE = strE.indexOf( ':' );
            if( nE >= 0 )
            {
                QString strV = strE.left( nE );
                int nGroupIndex = (int)strtol( strV.toStdString().c_str(), NULL, 10 );

                strName = strE.right( strE.length() - nE - 1 );

                if( nGroupIndex != -1 )
                {
                    QGroupKey* pGroup = NULL;
                    if( GroupKeyIndexMap.contains( nGroupIndex ) )
                    {
                        pGroup = GroupKeyIndexMap.value( nGroupIndex );
                        pSoftkey->setGroup( pGroup );
                        pGroup->Add( pSoftkey );
                    }
                    else
                    {
                        pGroup = new QGroupKey( &m_aryGroupKey );
                        GroupKeyIndexMap.insert( nGroupIndex, pGroup );
                        m_aryGroupKey.push_back( pGroup );
                        pSoftkey->setGroup( pGroup );
                        pGroup->Add( pSoftkey );
                    }
                }
            }
        }
    }

    return true;
}

QString QSoftkeyArray::Save( QString & strExtra )
{
    QString strRet;
    strExtra.clear();

    int ni;
    QString strE;
    QString strName;
    for ( ni = 0; ni < SOFT_KEY_MAX; ni++ )
    {
        bool show = false;
        ushort left = 0;
        ushort top = 0;
        ushort right = 0;
        ushort bottom = 0;

        int nGroupIndex = -1;
        strName.clear();;

        if ( ni < m_nSoftkey )
        {
            QSoftkey * pSoftkey = m_arySoftkey[ni];
            show = pSoftkey->m_bShow;
            left = (ushort)(pSoftkey->m_rcPosition.left() / 2);
            top = (ushort)(pSoftkey->m_rcPosition.top() / 2);
            right = (ushort)(pSoftkey->m_rcPosition.right() / 2);
            bottom = (ushort)(pSoftkey->m_rcPosition.bottom() / 2);

            if( pSoftkey->getGroup() )
                nGroupIndex = pSoftkey->getGroup()->GetGroupIndex();
            strName = pSoftkey->getName();
        }

        strRet += (char)('0' + (((ushort)(show & 0x01) << 4) | ((ushort)left >> 12)));
        strRet += (char)('0' + (((ushort)left >> 6) & 0x3f));
        strRet += (char)('0' + ((ushort)left & 0x3f));
        strRet += (char)('0' + ((ushort)top >> 10));
        strRet += (char)('0' + (((ushort)top >> 4) & 0x3f));
        strRet += (char)('0' + ((((ushort)top << 2) | ((ushort)right >> 14)) & 0x3f));
        strRet += (char)('0' + (((ushort)right >> 8) & 0x3f));
        strRet += (char)('0' + (((ushort)right >> 2) & 0x3f));
        strRet += (char)('0' + ((((ushort)right << 4) | ((ushort)bottom >> 12)) & 0x3f));
        strRet += (char)('0' + (((ushort)bottom >> 6) & 0x3f));
        strRet += (char)('0' + ((ushort)bottom & 0x3f));
        ////////////////////// Check : crazkode
        strE = "%1:%2,";
        strE.arg(nGroupIndex).arg(strName);
        if( ni == SOFT_KEY_MAX-1 )
            strE.remove( 0, strE.length()-1 );

        strExtra += strE;
    }

    return strRet;
}

int QSoftkeyArray::AddSoftkey( bool bShow, QRect rc )
{
    QSoftkey * pSoftkey;
    if ( !AddSoftkey(pSoftkey = new QSoftkey(bShow, rc)) )
    {
        delete pSoftkey;
        return -1;
    }

    return m_nSoftkey - 1;
}
/*
void QSoftkeyArray::Remove( int nRemoveIndex, QSoftlogicArray* logic )
{
    if( nRemoveIndex < 0 || nRemoveIndex >= m_nSoftkey )
        return;

    QSoftkey* removeKey = m_arySoftkey[nRemoveIndex];

    QSoftkey ** arySoftkeyNew = NULL;
    if( m_nSoftkey > 1 )
    {
        arySoftkeyNew = new QSoftkey *[m_nSoftkey - 1];
        memcpy(arySoftkeyNew, m_arySoftkey, sizeof(QSoftkey *) * nRemoveIndex);
        memcpy(arySoftkeyNew + nRemoveIndex, m_arySoftkey + nRemoveIndex + 1, sizeof(QSoftkey *) * (m_nSoftkey - nRemoveIndex - 1));
    }

    m_nSoftkey--;
    delete[] m_arySoftkey;
    m_arySoftkey = arySoftkeyNew;

    delete removeKey;

    if ( logic == NULL )
        return;
    logic->_ReplacePort((QSoftlogic::Port)(nRemoveIndex + MM_SOFTLOGIC_PORT_SOFTKEY_START), QSoftlogic::epNoGate);
    for ( int ni = nRemoveIndex; ni < m_nSoftkey; ni++ )
    {
        logic->_ReplacePort((QSoftlogic::Port)(ni + 1 + MM_SOFTLOGIC_PORT_SOFTKEY_START), (QSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTKEY_START));
    }
}

void QSoftkeyArray::Remove( QSoftkey * key, QSoftlogicArray* logic )
{
    for ( int i=0 ; i<m_nSoftkey ; i++ )
    {
        if( m_arySoftkey[i] == key )
        {
            Remove( i, logic );
            return;
        }
    }
}
*/

///////////////////////////////////////////////////////////////////////////////
// QSoftlogic

QSoftlogic::QSoftlogic()
{
    m_eLogicType = eltNILL;
    m_bEnableNot = false;
    m_eEnablePort = epNoGate;
    m_bIn1Not = false;
    m_eIn1Port = epNoGate;
    m_bIn2Not = false;
    m_eIn2Port = epNoGate;

    m_eTrigger = etNoTrigger;

    m_eOutType = eptError;
    m_out1 = 0;
    m_out2 = 0;
}

QSoftlogic::~QSoftlogic()
{
}

QSoftlogic::PortType QSoftlogic::ParsePort( Port port, int & idx )
{
    int nPort = (int)port;
    if ( nPort >= MM_SOFTLOGIC_PORT_SOFTKEY_START )
    {
        nPort -= MM_SOFTLOGIC_PORT_SOFTKEY_START;
        if ( nPort >= SOFT_KEY_MAX )
            return eptError;
        idx = nPort;
        return eptSoftkey;
    }
    if ( nPort >= MM_SOFTLOGIC_PORT_GPIO_START )
    {
        nPort -= MM_SOFTLOGIC_PORT_GPIO_START;
        return eptError; //!
        //! idx = nPort;
    }
    if ( nPort >= MM_SOFTLOGIC_PORT_STATE_START )
    {
        nPort -= MM_SOFTLOGIC_PORT_STATE_START;
        switch ( nPort )
        {
        case MM_SOFTLOGIC_STATE_WORKING_AND_BEEP1:
        case MM_SOFTLOGIC_STATE_TOUCH_AND_BEEP2:
        case MM_SOFTLOGIC_STATE_CALIBRATION:
        case MM_SOFTLOGIC_STATE_TOUCHSCREEN:
        case MM_SOFTLOGIC_STATE_MULTITOUCH_DEVICE:
            idx = nPort;
            break;
        default:
            return eptError;
        }
        return eptState;
    }
    return eptError;
}

bool QSoftlogic::setLogicType( LogicType lt )
{
    switch ( lt )
    {
    case eltAND:
    case eltOR:
    case eltXOR:
    case eltAND_RISINGEDGE:
        break;

    case eltNILL:
        break;

    default:
        return false;
    }

    m_eLogicType = lt;

    return true;
}

bool QSoftlogic::getEnableNot() const
{
    if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
        return false;
    return m_bEnableNot;
}
bool QSoftlogic::setEnableNot( bool bNot )
{
    if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
        return false;
    m_bEnableNot = bNot;
    return true;
}

QSoftlogic::Port QSoftlogic::getEnablePort() const
{
    if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
        return epNoGate;
    return m_eEnablePort;
}
bool QSoftlogic::setEnablePort( Port enablePort )
{
	if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
	{
            if ( enablePort == epNoGate )
                return true;
            return false;
	}
    if ( (enablePort & MM_SOFTLOGIC_ENABLE_PORTMASK) != enablePort )
        return false;
    m_eEnablePort = enablePort;
    return true;
}

bool QSoftlogic::setIn1Not( bool bNot )
{
    m_bIn1Not = bNot;
    return true;
}

bool QSoftlogic::setIn1Port( Port inPort )
{
    m_eIn1Port = inPort;
    return true;
}

bool QSoftlogic::setIn2Not( bool bNot )
{
    m_bIn2Not = bNot;
    return true;
}

bool QSoftlogic::setIn2Port( Port inPort )
{
    m_eIn2Port = inPort;
    return true;
}

uchar QSoftlogic::getOutKey1ConditionKeys() const
{
    if ( (m_eOutType != eptKey) && (m_eOutType != eptMouse) )
        return 0;
    return m_out1;
}

uchar QSoftlogic::getOutKey1() const
{
    if ( m_eOutType != eptKey )
        return 0;
    return m_out2;
}

bool QSoftlogic::setOutKey1( uchar conditionKeys, uchar key )
{
    if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
         m_eLogicType == eltAND_RISINGEDGE )
    {
        m_eOutType = eptKey;

        m_out1 = conditionKeys & MM_SOFTLOGIC_ENABLE_CKEY_MASK;
        m_out2 = key;
        return true;
    }
    return false;
}

uchar QSoftlogic::getOutMouse() const
{
    if ( m_eOutType != eptMouse )
        return 0;
    return m_out2;
}

bool QSoftlogic::setOutMouse( uchar conditionKeys, uchar mkey )
{
    if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
         m_eLogicType == eltAND_RISINGEDGE )
    {
        m_eOutType = eptMouse;

        m_out1 = conditionKeys & MM_SOFTLOGIC_ENABLE_CKEY_MASK;
        m_out2 = mkey;
        return true;
    }
    return false;
}

QSoftlogic::Trigger QSoftlogic::getLogicTrigger() const
{
    return m_eTrigger;
}

bool QSoftlogic::setLogicTrigger( Trigger trigger )
{
    if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
         m_eLogicType == eltAND_RISINGEDGE )
    {
        if ( trigger < etNoTrigger || trigger > etTrigger_U2000_D1000 )
            return false;

        m_eTrigger = trigger;
        return true;
    }
    return false;
}

QSoftlogic::Port QSoftlogic::getOutPort() const
{
    if ( m_eOutType != eptState && m_eOutType != eptGPIO )
        return epNoGate;
    return (QSoftlogic::Port)m_out2;
}

bool QSoftlogic::setOutPort( Port outPort )
{
    if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
         m_eLogicType == eltAND_RISINGEDGE )
    {
        if ( outPort < epNoGate || outPort >= epSoftkey0 )
            return false;

        if ( outPort < epGpio0 )
            m_eOutType = eptState;
        else
            m_eOutType = eptGPIO;

        m_out2 = (uchar)outPort;
        return true;
    }
    return false;
}

int QSoftlogic::getReportId() const
{
    if ( m_eOutType != eptReport )
        return -1;
    return (int)m_out2;
}

bool QSoftlogic::setReportId( int report_id )
{
    if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
         m_eLogicType == eltAND_RISINGEDGE )
    {
        if ( report_id < 0 || report_id > MM_SOFTLOGIC_OPEX_REPORT_ID )
            return false;

        m_eOutType = eptReport;

        m_out2 = (uchar)report_id;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// QSoftlogicArray

QSoftlogicArray::QSoftlogicArray()
{
    m_nSoftlogic = 0;
    m_arySoftlogic = NULL;
}

QSoftlogicArray::~QSoftlogicArray()
{
    RemoveAll();
}

void QSoftlogicArray::RemoveAll()
{
    if ( m_arySoftlogic != NULL )
    {
        for ( m_nSoftlogic--; m_nSoftlogic >= 0; m_nSoftlogic-- )
        {
            delete m_arySoftlogic[m_nSoftlogic];
        }
        delete [] m_arySoftlogic;
        m_arySoftlogic = NULL;
    }
    m_nSoftlogic = 0;
}

void QSoftlogicArray::Remove( int idx )
{
    QSoftlogic * pSoftlogic = _RemoveSoftlogic(idx);
    if ( pSoftlogic == NULL )
        return;

    _ReplacePort( (QSoftlogic::Port)(idx + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), QSoftlogic::epNoGate );

    delete pSoftlogic;

    TrimmingLogic();
}

int QSoftlogicArray::_InsertSoftlogic( QSoftlogic * pSoftlogic, bool bHidden )
{
    if ( m_nSoftlogic >= SOFT_LOGIC_MAX )
        return -1;

    if ( bHidden )
        return -1;

    QSoftlogic ** arySoftlogicNew = new QSoftlogic *[m_nSoftlogic+1];
    if( m_arySoftlogic )
    {
        memcpy(arySoftlogicNew, m_arySoftlogic, m_nSoftlogic * sizeof(QSoftlogic *));
        delete [] m_arySoftlogic;
    }
    m_arySoftlogic = arySoftlogicNew;
    m_arySoftlogic[m_nSoftlogic++] = pSoftlogic;

    return m_nSoftlogic - 1;
}

QSoftlogic * QSoftlogicArray::_RemoveSoftlogic( int idx )
{
    QSoftlogic * pRet = NULL;

    if ( idx >= 0 && idx < m_nSoftlogic )
    {
        pRet = m_arySoftlogic[idx];
        m_arySoftlogic[idx] = NULL;
        return pRet;
    }

    return NULL;
}

void QSoftlogicArray::_ExchangePort( QSoftlogic::Port port1, QSoftlogic::Port port2 )
{
    int ni;
    for ( ni = 0; ni < m_nSoftlogic; ni++ )
    {
        if (  m_arySoftlogic[ni] == NULL || m_arySoftlogic[ni]->getLogicType() == QSoftlogic::eltNILL )
            continue;

        QSoftlogic * pSoftlogic = m_arySoftlogic[ni];

        if ( pSoftlogic->getEnablePort() == port1 )
            Q_VERIFY(pSoftlogic->setEnablePort(port2));
        else if ( pSoftlogic->getEnablePort() == port2 )
            Q_VERIFY(pSoftlogic->setEnablePort(port1));

        if ( pSoftlogic->getIn1Port() == port1 )
            Q_VERIFY(pSoftlogic->setIn1Port(port2));
        else if ( pSoftlogic->getIn1Port() == port2 )
            Q_VERIFY(pSoftlogic->setIn1Port(port1));

        if ( pSoftlogic->getIn2Port() == port1 )
            Q_VERIFY(pSoftlogic->setIn2Port(port2));
        else if ( pSoftlogic->getIn2Port() == port2 )
            Q_VERIFY(pSoftlogic->setIn2Port(port1));

        if ( pSoftlogic->getOutPort() == port1 )
            Q_VERIFY(pSoftlogic->setOutPort(port2));
        else if ( pSoftlogic->getOutPort() == port2 )
            Q_VERIFY(pSoftlogic->setOutPort(port1));
    }
}

void QSoftlogicArray::_ReplacePort( QSoftlogic::Port port, QSoftlogic::Port portNew )
{
    int ni;
    for ( ni = 0; ni < m_nSoftlogic; ni++ )
    {
        if (  m_arySoftlogic[ni] == NULL || m_arySoftlogic[ni]->getLogicType() == QSoftlogic::eltNILL )
            continue;

        QSoftlogic * pSoftlogic = m_arySoftlogic[ni];

        if ( pSoftlogic->getEnablePort() == port )
            Q_VERIFY(pSoftlogic->setEnablePort(portNew));

        if ( pSoftlogic->getIn1Port() == port )
            Q_VERIFY(pSoftlogic->setIn1Port(portNew));

        if ( pSoftlogic->getIn2Port() == port )
            Q_VERIFY(pSoftlogic->setIn2Port(portNew));

        if ( pSoftlogic->getOutPort() == port )
            Q_VERIFY(pSoftlogic->setOutPort(portNew));
    }
}

QSoftlogic* QSoftlogicArray::operator[]( int idx )
{
    if ( idx < 0 || idx >= SOFT_LOGIC_MAX )
        return NULL;

    if ( idx < m_nSoftlogic  )
        return m_arySoftlogic[idx];

    return NULL;
}

int QSoftlogicArray::IndexFromSoftlogic( QSoftlogic * pSoftlogic ) const
{
    int ni;
    for ( ni = 0; ni < m_nSoftlogic; ni++ )
    {
        if (  m_arySoftlogic[ni] == pSoftlogic )
            return ni;
    }

    return -1;
}

bool QSoftlogicArray::Load( const char* lpctszCmd, const char* /*lpctszExtra*/ )
{
    RemoveAll();

    if( !lpctszCmd ) return false;

    //////////////////////////////////////////////////
    // Check : TCHAR -> char ? wchar_t ?
    const char* buf = lpctszCmd;
    unsigned char buf0, buf1, buf2, buf3, buf4, buf5;

    for ( int ni = 0; ni < SOFT_LOGIC_MAX; ni++ )
    {
        if ( buf[0] == 0 || buf[1] == 0 || buf[2] == 0 ||
             buf[3] == 0 || buf[4] == 0 || buf[5] == 0 )
            break;

        if ( buf[0] == 'p' ) buf0 = ('\\' - '0'); else buf0 = buf[0] - '0';
        if ( buf[1] == 'p' ) buf1 = ('\\' - '0'); else buf1 = buf[1] - '0';
        if ( buf[2] == 'p' ) buf2 = ('\\' - '0'); else buf2 = buf[2] - '0';
        if ( buf[3] == 'p' ) buf3 = ('\\' - '0'); else buf3 = buf[3] - '0';
        if ( buf[4] == 'p' ) buf4 = ('\\' - '0'); else buf4 = buf[4] - '0';
        if ( buf[5] == 'p' ) buf5 = ('\\' - '0'); else buf5 = buf[5] - '0';

        uchar enable_ckey = (buf0 >> 2) & MM_SOFTLOGIC_ENABLE_CKEY_MASK;
        uchar in1 = (buf0 << 6) | buf1;
        uchar in2 = (buf2 << 2) | (buf3 >> 4);
        uchar opex = (buf3 << 4) | (buf4 >> 2);
        uchar op = (buf4 << 6) | buf5;

        QSoftlogic::Trigger trigger = (QSoftlogic::Trigger)(op & MM_SOFTLOGIC_OP_TRIGGER_INF0);

        QSoftlogic::LogicType lt = QSoftlogic::eltNILL;
        //bool bNotEnable = false;
        //QSoftlogic::Port inEnable = QSoftlogic::epNoGate;
        bool bNotPort1 = false;
        QSoftlogic::Port inPort1 = QSoftlogic::epNoGate;
        bool bNotPort2 = false;
        QSoftlogic::Port inPort2 = QSoftlogic::epNoGate;

        switch ( (op >> 4) )
        {
        default:
            Q_ASSERT( false );
        case MM_SOFTLOGIC_NOACTION:
            opex = 0;
        add_softlogic_key:
            if ( AddSoftlogic_Key(lt, bNotPort1, inPort1, bNotPort2, inPort2,
                                  enable_ckey, opex, trigger) != ni )
            {
                qDebug( "Softlogic : Add faild" );
                RemoveAll();
                return false;
            }
            break;

        case MM_SOFTLOGIC_OP_KEY_AND:
            lt = QSoftlogic::eltAND;
            bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
            bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            goto add_softlogic_key;

        case MM_SOFTLOGIC_OP_KEY_OR:
            lt = QSoftlogic::eltOR;
            bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
            bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            goto add_softlogic_key;

        case MM_SOFTLOGIC_OP_KEY_XOR:
            lt = QSoftlogic::eltXOR;
            bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
            bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            goto add_softlogic_key;

        case MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE:
            lt = QSoftlogic::eltAND_RISINGEDGE;
            bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
            bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
            inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            goto add_softlogic_key;

        case MM_SOFTLOGIC_OP_MKEY:
            switch ( (opex >> 5) )
            {
            default:
                Q_ASSERT( false );
                break;
            case MM_SOFTLOGIC_OPEX_MKEY_AND:
                lt = QSoftlogic::eltAND;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
        add_softlogic_mouse:
                if ( AddSoftlogic_Mouse(lt, bNotPort1, inPort1, bNotPort2, inPort2,
                                        enable_ckey, opex & MM_SOFTLOGIC_OPEX_MKEY_ACT_MASK, trigger) != ni )
                {
                    RemoveAll();
                    return false;
                }
                break;

            case MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE:
                lt = QSoftlogic::eltAND_RISINGEDGE;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_mouse;
            }
            break;

        case MM_SOFTLOGIC_OP_STAT:
            switch ( (opex >> 6) )
            {
            default:
                Q_ASSERT( false );
            case MM_SOFTLOGIC_OPEX_STAT_AND:
                lt = QSoftlogic::eltAND;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            add_softlogic_stat:
                if ( AddSoftlogic_State(lt, ((enable_ckey & MM_SOFTLOGIC_ENABLE_NOT) != 0x0), (QSoftlogic::Port)(enable_ckey & MM_SOFTLOGIC_ENABLE_PORTMASK),
                                        bNotPort1, inPort1, bNotPort2, inPort2,
                                        (QSoftlogic::Port)(opex & MM_SOFTLOGIC_OPEX_STAT_OUTPORT),
                                        trigger) != ni )
                {
                    RemoveAll();
                    return false;
                }
                break;

            case MM_SOFTLOGIC_OPEX_STAT_OR:
                lt = QSoftlogic::eltOR;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_stat;

            case MM_SOFTLOGIC_OPEX_STAT_XOR:
                lt = QSoftlogic::eltXOR;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_stat;

            case MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE:
                lt = QSoftlogic::eltAND_RISINGEDGE;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_stat;
            }
            break;

        case MM_SOFTLOGIC_OP_REPORT:
            switch ( (opex >> 6) )
            {
            default:
                Q_ASSERT( false );
            case MM_SOFTLOGIC_OPEX_REPORT_AND:
                lt = QSoftlogic::eltAND;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
            add_softlogic_report:
                if ( AddSoftlogic_Report(lt, ((enable_ckey & MM_SOFTLOGIC_ENABLE_NOT) != 0x0), (QSoftlogic::Port)(enable_ckey & MM_SOFTLOGIC_ENABLE_PORTMASK),
                                         bNotPort1, inPort1, bNotPort2, inPort2,
                                         (opex & MM_SOFTLOGIC_OPEX_REPORT_ID),
                                         trigger) != ni )
                {
                    RemoveAll();
                    return false;
                }
                break;

            case MM_SOFTLOGIC_OPEX_REPORT_OR:
                lt = QSoftlogic::eltOR;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_report;

            case MM_SOFTLOGIC_OPEX_REPORT_XOR:
                lt = QSoftlogic::eltXOR;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_report;

            case MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE:
                lt = QSoftlogic::eltAND_RISINGEDGE;
                bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort1 = (QSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
                bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
                inPort2 = (QSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
                goto add_softlogic_report;
            }
            break;
        }

        buf += 6;
    }

    TrimmingLogic();

    return true;
}

QString QSoftlogicArray::Save( QString& strExtra )
{
    QString strRet;
    strExtra.clear();

    int ni;
    uchar in1, in2;
    uchar opex, op;
    uchar enable_ckey;

    int nlogicCount = m_nSoftlogic;

    if( !m_nSoftlogic )
        nlogicCount = SOFT_LOGIC_MAX;

    for ( ni = 0; ni < nlogicCount; ni++ )
    {
        enable_ckey = 0;
        in1 = 0;
        in2 = 0;
        opex = 0;
        op = 0;

        if ( ni < m_nSoftlogic )
        {
            QSoftlogic * pSoftlogic = m_arySoftlogic[ni];
            switch ( pSoftlogic->getLogicType() )
            {
            case QSoftlogic::eltAND:
                enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
                enable_ckey |= pSoftlogic->getEnablePort();
                in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in1 |= pSoftlogic->getIn1Port();
                in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in2 |= pSoftlogic->getIn2Port();

                switch ( pSoftlogic->getOutPortType() )
                {
                default:
                case QSoftlogic::eptKey:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_KEY_AND << 4) | pSoftlogic->getLogicTrigger();
                    opex = pSoftlogic->getOutKey1();
                    break;
                case QSoftlogic::eptMouse:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_MKEY_AND << 5) | pSoftlogic->getOutMouse();
                    break;
                case QSoftlogic::eptState:
                case QSoftlogic::eptGPIO:
                    op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_STAT_AND << 6) | pSoftlogic->getOutPort();
                    break;
                case QSoftlogic::eptReport:
                    op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_REPORT_AND << 6) | pSoftlogic->getReportId();
                    break;
                }
                break;

            case QSoftlogic::eltOR:
                enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
                enable_ckey |= pSoftlogic->getEnablePort();
                in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in1 |= pSoftlogic->getIn1Port();
                in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in2 |= pSoftlogic->getIn2Port();

                switch ( pSoftlogic->getOutPortType() )
                {
                default:
                case QSoftlogic::eptKey:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_KEY_OR << 4) | pSoftlogic->getLogicTrigger();
                    opex = pSoftlogic->getOutKey1();
                    break;
                case QSoftlogic::eptMouse:
                    Q_ASSERT( false );
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
                    opex = 0;
                    break;
                case QSoftlogic::eptState:
                case QSoftlogic::eptGPIO:
                    op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_STAT_OR << 6) | pSoftlogic->getOutPort();
                    break;
                case QSoftlogic::eptReport:
                    op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_REPORT_OR << 6) | pSoftlogic->getReportId();
                    break;
                }
                break;

            case QSoftlogic::eltXOR:
                enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
                enable_ckey |= pSoftlogic->getEnablePort();
                in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in1 |= pSoftlogic->getIn1Port();
                in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in2 |= pSoftlogic->getIn2Port();

                switch ( pSoftlogic->getOutPortType() )
                {
                default:
                case QSoftlogic::eptKey:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_KEY_XOR << 4) | pSoftlogic->getLogicTrigger();
                    opex = pSoftlogic->getOutKey1();
                    break;
                case QSoftlogic::eptMouse:
                    Q_ASSERT( false );
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
                    opex = 0;
                    break;
                case QSoftlogic::eptState:
                case QSoftlogic::eptGPIO:
                    op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_STAT_XOR << 6) | pSoftlogic->getOutPort();
                    break;
                case QSoftlogic::eptReport:
                    op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_REPORT_XOR << 6) | pSoftlogic->getReportId();
                    break;
                }
                break;

            case QSoftlogic::eltAND_RISINGEDGE:
                enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
                enable_ckey |= pSoftlogic->getEnablePort();
                in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in1 |= pSoftlogic->getIn1Port();
                in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
                in2 |= pSoftlogic->getIn2Port();

                switch ( pSoftlogic->getOutPortType() )
                {
                default:
                case QSoftlogic::eptKey:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE << 4) | pSoftlogic->getLogicTrigger();
                    opex = pSoftlogic->getOutKey1();
                    break;
                case QSoftlogic::eptMouse:
                    enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
                    op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE << 5) | pSoftlogic->getOutMouse();
                    break;
                case QSoftlogic::eptState:
                case QSoftlogic::eptGPIO:
                    op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE << 6) | pSoftlogic->getOutPort();
                    break;
                case QSoftlogic::eptReport:
                    op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
                    opex = (MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE << 6) | pSoftlogic->getReportId();
                    break;
                }
                break;

            case QSoftlogic::eltNILL:
            default:
                break;
            }
        }
        strRet += (char)('0' + (((enable_ckey << 2) | (in1 >> 6)) & 0x3f));
        strRet += (char)('0' + (in1 & 0x3f));
        strRet += (char)('0' + ((in2 >> 2) & 0x3f));
        strRet += (char)('0' + (((in2 << 4) | (opex >> 4)) & 0x3f));
        strRet += (char)('0' + (((opex << 2) | (op >> 6)) & 0x3f));
        strRet += (char)('0' + (op & 0x3f));
    }

    return strRet;
}

int QSoftlogicArray::AddSoftlogic_Key( QSoftlogic::LogicType logictype,
                                       bool in1_not, QSoftlogic::Port in1_port,
                                       bool in2_not, QSoftlogic::Port in2_port,
                                       uchar condition_key, uchar key,
									   QSoftlogic::Trigger trigger, bool /*bHidden*/ )
{
    QSoftlogic * pSoftlogic = new QSoftlogic;
    do
    {
        if ( !pSoftlogic->setLogicType(logictype) )
            break;

        if ( logictype != QSoftlogic::eltNILL )
        {
            if ( !pSoftlogic->setOutKey1(condition_key, key) )
                break;

            if ( !pSoftlogic->setIn1Not(in1_not) ||
                 !pSoftlogic->setIn1Port(in1_port) )
                break;

            if ( !pSoftlogic->setIn2Not(in2_not) ||
                 !pSoftlogic->setIn2Port(in2_port) )
                break;

            if ( !pSoftlogic->setLogicTrigger(trigger) )
                break;
        }

        if ( _InsertSoftlogic(pSoftlogic) < 0 )
            break;

        return m_nSoftlogic - 1;
    }
    while ( false );

    delete pSoftlogic;

    return -1;
}

int QSoftlogicArray::AddSoftlogic_Mouse( QSoftlogic::LogicType logictype,
                                         bool in1_not, QSoftlogic::Port in1_port,
                                         bool in2_not, QSoftlogic::Port in2_port,
                                         uchar condition_key, uchar mkey,
										 QSoftlogic::Trigger trigger, bool /*bHidden*/ )
{
    QSoftlogic * pSoftlogic = new QSoftlogic;
    do
    {
        if ( !pSoftlogic->setLogicType(logictype) )
            break;

        if ( logictype != QSoftlogic::eltNILL )
        {
            if ( !pSoftlogic->setOutMouse(condition_key, mkey) )
                break;

            if ( !pSoftlogic->setIn1Not(in1_not) ||
                 !pSoftlogic->setIn1Port(in1_port) )
                break;

            if ( !pSoftlogic->setIn2Not(in2_not) ||
                 !pSoftlogic->setIn2Port(in2_port) )
                break;

            if ( !pSoftlogic->setLogicTrigger(trigger) )
                break;
        }

        if ( _InsertSoftlogic(pSoftlogic) < 0 )
            break;

        return m_nSoftlogic - 1;
    }
    while ( false );

    delete pSoftlogic;

    return -1;
}

int QSoftlogicArray::AddSoftlogic_State( QSoftlogic::LogicType logictype,
                                         bool enable_not, QSoftlogic::Port enable_port,
                                         bool in1_not, QSoftlogic::Port in1_port,
                                         bool in2_not, QSoftlogic::Port in2_port,
										 QSoftlogic::Port out_port, QSoftlogic::Trigger trigger, bool /*bHidden*/ )
{
    QSoftlogic * pSoftlogic = new QSoftlogic;
    do
    {
        if ( !pSoftlogic->setLogicType(logictype) )
			break;

        if ( !pSoftlogic->setOutPort(out_port) )
            break;

        if ( !pSoftlogic->setEnableNot(enable_not) ||
             !pSoftlogic->setEnablePort(enable_port) )
            break;

        if ( !pSoftlogic->setIn1Not(in1_not) ||
             !pSoftlogic->setIn1Port(in1_port) )
            break;

        if ( !pSoftlogic->setIn2Not(in2_not) ||
             !pSoftlogic->setIn2Port(in2_port) )
            break;

        if ( !pSoftlogic->setLogicTrigger(trigger) )
            break;

        if ( _InsertSoftlogic(pSoftlogic) < 0 )
            break;

        return m_nSoftlogic - 1;
    }
    while ( false );

    delete pSoftlogic;

    return -1;
}

int QSoftlogicArray::AddSoftlogic_Report( QSoftlogic::LogicType logictype,
                                          bool enable_not, QSoftlogic::Port enable_port,
                                          bool in1_not, QSoftlogic::Port in1_port,
                                          bool in2_not, QSoftlogic::Port in2_port,
										  int report_id, QSoftlogic::Trigger trigger, bool /*bHidden*/ )
{
    QSoftlogic * pSoftlogic = new QSoftlogic;
    do
    {
        if ( !pSoftlogic->setLogicType(logictype) )
            break;

        if ( !pSoftlogic->setReportId(report_id) )
            break;

        if ( !pSoftlogic->setEnableNot(enable_not) ||
             !pSoftlogic->setEnablePort(enable_port) )
            break;

        if ( !pSoftlogic->setIn1Not(in1_not) ||
             !pSoftlogic->setIn1Port(in1_port) )
            break;

        if ( !pSoftlogic->setIn2Not(in2_not) ||
             !pSoftlogic->setIn2Port(in2_port) )
            break;

        if ( !pSoftlogic->setLogicTrigger(trigger) )
            break;

        if ( _InsertSoftlogic(pSoftlogic) < 0 )
            break;

        return m_nSoftlogic - 1;
    }
    while ( false );

    delete pSoftlogic;

    return -1;
}

bool QSoftlogicArray::ExchangeSotflogic( int idx1, int idx2 )
{
    if ( idx1 < 0 || idx2 < 0 )
        return false;

    QSoftlogic * pSoftlogic1 = operator[](idx1);
    QSoftlogic * pSoftlogic2 = operator[](idx2);
    if ( pSoftlogic1 == NULL || pSoftlogic2 == NULL )
        return false;

    if ( idx1 < m_nSoftlogic  )
        m_arySoftlogic[idx1] = pSoftlogic2;
    else
    {
        Q_ASSERT( false );
    }

    if ( idx2 < m_nSoftlogic  )
        m_arySoftlogic[idx2] = pSoftlogic1;
    else
    {
        Q_ASSERT( false );
    }

    _ExchangePort( (QSoftlogic::Port)(idx1 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), (QSoftlogic::Port)(idx2 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START) );

    return true;
}

int QSoftlogicArray::MoveSoftlogicTo( int from, int to )
{
    if ( from < 0 || to < 0 )
        return -1;

    int ni;
    if ( from < m_nSoftlogic )
    {
        if ( to < m_nSoftlogic )
        {
            if ( from < to )
            {
                for ( ni = from; ni < to; ni++ )
                {
                    if ( !ExchangeSotflogic(ni, ni + 1) )
                        return ni;
                }
            }
            else if ( from > to )
            {
                for ( ni = from; ni > to; ni-- )
                {
                    if ( !ExchangeSotflogic(ni - 1, ni) )
                        return ni;
                }
            }
            return to;
        }
        else
            return -1;
    }

    return -1;
}

void QSoftlogicArray::TrimmingLogic( bool bTrimEndOnly )
{
    int ni, nj;
    QSoftlogic ** arySoftlogicNew;

    for ( ni = m_nSoftlogic - 1; ni >= 0; ni-- )
    {
        if ( m_arySoftlogic[ni] == NULL )
                continue;
        if ( m_arySoftlogic[ni]->getLogicType() == QSoftlogic::eltNILL )
        {
            delete m_arySoftlogic[ni];
            m_arySoftlogic[ni] = NULL;
            continue;
        }
        break;
    }
    if ( ni < m_nSoftlogic - 1)
    {
        arySoftlogicNew = NULL;
        if ( ni >= 0 )
        {
            arySoftlogicNew = new QSoftlogic *[ni + 1];
            memcpy(arySoftlogicNew, m_arySoftlogic, sizeof(QSoftlogic *) * (ni + 1));
        }
        m_nSoftlogic = ni + 1;
        delete [] m_arySoftlogic;
        m_arySoftlogic = arySoftlogicNew;
    }

    if ( bTrimEndOnly )
        return;

    for ( ni = m_nSoftlogic - 1; ni >= 0; ni-- )
    {
        if ( m_arySoftlogic[ni] != NULL &&
             m_arySoftlogic[ni]->getLogicType() != QSoftlogic::eltNILL )
            continue;

        if ( m_arySoftlogic[ni] != NULL )
        {
            delete m_arySoftlogic[ni];
            m_arySoftlogic[ni] = NULL;
        }

        arySoftlogicNew = NULL;
        if ( m_nSoftlogic > 1 )
        {
            arySoftlogicNew = new QSoftlogic *[m_nSoftlogic - 1];
            memcpy(arySoftlogicNew, m_arySoftlogic, sizeof(QSoftlogic *) * ni);
            memcpy(arySoftlogicNew + ni, m_arySoftlogic + ni + 1, sizeof(QSoftlogic *) * (m_nSoftlogic - ni - 1));
        }
        m_nSoftlogic--;
        delete [] m_arySoftlogic;
        m_arySoftlogic = arySoftlogicNew;

        for ( nj = ni; nj < m_nSoftlogic; nj++ )
        {
            _ReplacePort((QSoftlogic::Port)(nj + 1 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START),
                         (QSoftlogic::Port)(nj + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));
        }
    }
}
