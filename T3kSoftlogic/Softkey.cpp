#include "SoftKey.h"

#include <QMap>
#include <QPoint>

#include "../common/QIniFormat.h"

#ifdef _DEBUG
#define VERIFY(f)   Q_ASSERT(f)
#else
#define VERIFY(f)   (void(f))
#endif

int GroupKey::getGroupIndex() const
{
	if( !m_pOwner ) return -1;

    for( int nI=0 ; nI<m_pOwner->count() ; nI++ )
	{
        GroupKey* pGroup = m_pOwner->at(nI);
		if( pGroup == this )
            return nI;
	}
	return -1;
}

bool GroupKey::swap(int nIdx1, int nIdx2)
{
    if( nIdx1 == nIdx2 ) return false;

    Q_ASSERT( nIdx1 >= 0 || nIdx2 >= 0 || nIdx1 < m_GroupKey.size() || nIdx2 < m_GroupKey.size() );

    CSoftkey* pI = getAt(nIdx1);
    CSoftkey* pJ = getAt(nIdx2);
    m_GroupKey.replace( nIdx2, pI );
    m_GroupKey.replace( nIdx1, pJ );

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// CSoftkey

CSoftkey::CSoftkey( bool bShow, QRect rc )
{
	m_bShow = bShow;
	m_rcPosition = rc;

	m_pSoftkeyBind = NULL;

	m_pGroup = NO_GROUP;

	// extra
    m_ptPosition.setX( 0.1f );
    m_ptPosition.setY( 0.5f );
}

CSoftkey::~CSoftkey()
{
}

bool CSoftkey::setShow( bool bShow )
{
    bool bRet = m_bShow;
	m_bShow = bShow;
	return bRet;
}

QRect CSoftkey::setPosition( QRect rc )
{
    QRect rcRet = m_rcPosition;
	m_rcPosition = rc;
	return rcRet;
}

void CSoftkey::setGroup( GroupKey* pGroup )
{
	m_pGroup = pGroup;
}

void CSoftkey::setName( QString lpszName )
{
	m_strName = lpszName;
}

///////////////////////////////////////////////////////////////////////////////
// CSoftkeyArray

CSoftkeyArray::CSoftkeyArray()
{
	m_dScrnDimWidth = m_dScrnDimHeight = 0.0;
	m_dPanelDimWidth = m_dPanelDimHeight = 0.0;
	m_dScrnOffsetX = m_dScrnOffsetY = 0.0;

	m_nSoftkey = 0;
	m_arySoftkey = NULL;

    m_bIsModified = false;

	if( !m_bIsModified )
	{
        QString strCmd, strExtra;
        strCmd = save( strExtra, NULL, false );
	}
}

CSoftkeyArray::~CSoftkeyArray()
{
    clear();
}

void CSoftkeyArray::setPanelName( QString lpszPanelName )
{
    if( m_strPanelName.compare(lpszPanelName) == 0 )
		return;

	m_strPanelName = lpszPanelName;
    m_bIsModified = true;
}

void CSoftkeyArray::setModified( bool bModified )
{
	m_bIsModified = bModified;

	if( !m_bIsModified )
	{
        QString strCmd, strExtra;
        strCmd = save( strExtra, NULL, false );
	}
}

void CSoftkeyArray::setPanelDimension( double dWidth, double dHeight )
{
	if( (m_dPanelDimWidth == dWidth) && (m_dPanelDimHeight == dHeight) )
		return;

	m_dPanelDimWidth = dWidth;
	m_dPanelDimHeight = dHeight;
    m_bIsModified = true;
}

void CSoftkeyArray::setScreenDimension( double dWidth, double dHeight )
{
	if( (m_dScrnDimWidth == dWidth) && (m_dScrnDimHeight == dHeight) )
		return;

	m_dScrnDimWidth = dWidth;
	m_dScrnDimHeight = dHeight;
    m_bIsModified = true;
}

void CSoftkeyArray::setScreenOffset( double dX, double dY )
{
	if( (m_dScrnOffsetX == dX) && (m_dScrnOffsetY == dY) )
		return;

	m_dScrnOffsetX = dX;
	m_dScrnOffsetY = dY;
    m_bIsModified = true;
}

void CSoftkeyArray::resetPanelInfo()
{
    m_strPanelName.clear();
	m_dPanelDimWidth = m_dPanelDimHeight = 0;
	m_dScrnDimWidth = m_dScrnDimHeight = 0;
	m_dScrnOffsetX = m_dScrnOffsetY = 0;

    m_bIsModified = true;
}

void CSoftkeyArray::reOrder( CSoftlogicArray* logic )
{
	CSoftkey ** arySoftkeyReOrdered = new CSoftkey *[m_nSoftkey];

	CSoftkey *key, *keyNext;
    QRect rcKey, rcKeyNext;

	// 1. group keys
    for( int nG=0 ; nG<m_aryGroupKey.count() ; nG++ )
	{
        GroupKey* pGroup = m_aryGroupKey.at(nG);

        for( int nI=0 ; nI<pGroup->getCount()-1 ; nI++ )
		{
            for( int nJ=nI ; nJ<pGroup->getCount() ; nJ++ )
			{
                key = pGroup->getAt(nI);
                keyNext = pGroup->getAt(nJ);
				rcKey = key->getPosition();
				rcKeyNext = keyNext->getPosition();

                if( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
				{
					// swap
                    pGroup->swap( nI, nJ );
				}
			}
		}
	}

	int nKeyCnt = 0;
    for( int nG=0 ; nG<m_aryGroupKey.count() ; nG++ )
	{
        GroupKey* pGroup = m_aryGroupKey.at(nG);

        for( int nI=0 ; nI<pGroup->getCount() ; nI++ )
		{
            arySoftkeyReOrdered[nKeyCnt++] = pGroup->getAt(nI);
		}
	}

	_GroupKey _NoGroup;
	for( int i=0 ; i<m_nSoftkey ; i++ )
	{
		if( m_arySoftkey[i]->getGroup() == NO_GROUP )
		{
            _NoGroup.push_back( m_arySoftkey[i] );
		}
	}

    for( int nI=0 ; nI<_NoGroup.count()-1 ; nI++ )
	{
        for( int nJ=nI ; nJ<_NoGroup.count() ; nJ++ )
		{
            key = _NoGroup.at(nI);
            keyNext = _NoGroup.at(nJ);
			rcKey = key->getPosition();
			rcKeyNext = keyNext->getPosition();

            if( (rcKey.center().x() + rcKey.center().y()) > (rcKeyNext.center().x() + rcKeyNext.center().y()) )
			{
				// swap
				CSoftkey* pI = _NoGroup[nI];
				CSoftkey* pJ = _NoGroup[nJ];
				_NoGroup[nI] = pJ;
				_NoGroup[nJ] = pI;
			}
		}
	}

    for( int nI=0 ; nI<_NoGroup.count() ; nI++ )
	{
		arySoftkeyReOrdered[nKeyCnt++] = _NoGroup[nI];
	}

    Q_ASSERT( nKeyCnt == m_nSoftkey );

	if ( logic )
	{
		for ( int ni = 0; ni < m_nSoftkey; ni++ )
		{
			if ( m_arySoftkey[ni] == arySoftkeyReOrdered[ni] )
				continue;
			CSoftkey * p = arySoftkeyReOrdered[ni];
			int nj = ni + 1;
			for ( ; nj < m_nSoftkey; nj++ )
			{
				if ( m_arySoftkey[nj] == p )
					break;
			}
            Q_ASSERT( nj != m_nSoftkey );
			CSoftkey * pI = m_arySoftkey[ni];
			CSoftkey * pJ = m_arySoftkey[nj];
			m_arySoftkey[ni] = pJ;
			m_arySoftkey[nj] = pI;
            logic->_exchangePort((CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTKEY_START), (CSoftlogic::Port)(nj + MM_SOFTLOGIC_PORT_SOFTKEY_START));
		}
	}
	else
        memcpy(m_arySoftkey, arySoftkeyReOrdered, sizeof(CSoftkey*) * m_nSoftkey);

    _NoGroup.clear();
	delete[] arySoftkeyReOrdered;
}

void CSoftkeyArray::clear()
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

    m_aryGPIOInfo.clear();;
}

bool CSoftkeyArray::addSoftkey( CSoftkey * pSoftkey )
{
	if ( m_nSoftkey >= SOFT_KEY_MAX )
        return false;

	CSoftkey ** arySoftkeyNew = new CSoftkey *[m_nSoftkey+1];
	if( m_arySoftkey )
	{
		memcpy(arySoftkeyNew, m_arySoftkey, m_nSoftkey * sizeof(CSoftkey *));
		delete [] m_arySoftkey;
	}
	m_arySoftkey = arySoftkeyNew;
	m_arySoftkey[m_nSoftkey++] = pSoftkey;

    return true;
}

CSoftkey * CSoftkeyArray::operator [] ( int idx )
{
	if ( idx < 0 || idx >= m_nSoftkey )
		return NULL;

	return m_arySoftkey[idx];
}

bool CSoftkeyArray::isModified()
{ 
	if( m_bIsModified )
        return true;

    QString strCmd, strExtra;
    strCmd = save(strExtra, NULL, true);

    if( m_strBackupCmd.compare( strCmd ) != 0 )
        return true;
    if( m_strBackupExtra.compare( strExtra ) != 0 )
        return true;

    return false;
}

static float s_fKeyPos_x = 0.1f;
static float s_fKeyPos_y = 0.1f;

bool CSoftkeyArray::load( QString lpctszCmd, QString lpctszExtra, QIniFormat * ini )
{
    clear();

	s_fKeyPos_x = 0.08f;
	s_fKeyPos_y = 0.06f;

    QString strExtra = lpctszExtra;
    QString strE;

    const char* buf = new char[lpctszCmd.size()+1];
    strcpy( (char*)buf, lpctszCmd.toUtf8().data() );
    ushort buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, buf9, buf10;

    QMap<int, GroupKey*> GroupKeyIndexMap;

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

        int nSP = strExtra.indexOf(',');
		if( nSP >= 0 )
		{
            strE = strExtra.left( nSP );
            strExtra.remove( 0, nSP+1 );
		}
		else
		{
            if( !strExtra.isEmpty() )
			{
				strE = strExtra;
			}
		}

        bool show = (buf0 >> 4) != 0x0;
        ushort left = ((buf0 & 0x0f) << 12) | (buf1 << 6) | buf2;
        ushort top = (buf3 << 10) | (buf4 << 4) | (buf5 >> 2);
        ushort right = ((buf5 & 0x03) << 14) | (buf6 << 8) | (buf7 << 2) | (buf8 >> 4);
        ushort bottom = ((buf8 & 0x0f) << 12) | (buf9 << 6) | buf10;

        QRect rc( QPoint((short)left * 2, (short)top * 2), QPoint((short)right * 2, (short)bottom * 2) );

        if( !show && (rc.width()-1 == 0 && rc.height()-1 == 0) )
		{
			continue;
		}
		CSoftkey * pSoftkey = new CSoftkey(show, rc);

		// extra info
		{
			float fLogicF[2];

            QString strLogicInfo;
            if ( ini != NULL )
                strLogicInfo = ini->getValue( QString("key%1").arg(ni) );
			
            if ( strLogicInfo.isEmpty() )
			{
				fLogicF[0] = s_fKeyPos_x;
				fLogicF[1] = s_fKeyPos_y;
				if ( s_fKeyPos_y+0.06f+0.05f >= 1.f )
				{
					s_fKeyPos_x += 0.45f; if ( s_fKeyPos_x >= 1.f ) s_fKeyPos_x = 0.08f;
					s_fKeyPos_y = 0.06f;
				}
				else
				{
					s_fKeyPos_y += 0.07f;
				}
			}
			else
			{
                int nD = strLogicInfo.indexOf(',');
                fLogicF[0] = strLogicInfo.left(nD).toFloat();
                nD = strLogicInfo.indexOf(',');
                if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
                fLogicF[1] = strLogicInfo.toFloat();
			}
            pSoftkey->m_ptPosition.setX( fLogicF[0] );
            pSoftkey->m_ptPosition.setY( fLogicF[1] );
		}

        addSoftkey(pSoftkey);

        QString strName;
        if( !strE.isEmpty() )
		{
            int nE = strE.indexOf(':');
			if( nE >= 0 )
			{
                int nGroupIndex = strE.left( nE ).toInt();

                strName = strE.right( strE.length() - nE - 1 );

				pSoftkey->setName( strName );
				if( nGroupIndex != -1 )
				{
					GroupKey* pGroup = NULL;
                    if( GroupKeyIndexMap.contains( nGroupIndex ) )
					{
                        pGroup = GroupKeyIndexMap.value( nGroupIndex );
						pSoftkey->setGroup( pGroup );
                        pGroup->add( pSoftkey );
					}
					else
					{
						pGroup = new GroupKey( &m_aryGroupKey );
                        GroupKeyIndexMap.insert( nGroupIndex, pGroup );
                        m_aryGroupKey.push_back( pGroup );
						pSoftkey->setGroup( pGroup );
                        pGroup->add( pSoftkey );
					}
				}
			}
		}
	}

    m_bIsModified = false;

    return true;
}

QString CSoftkeyArray::save( QString & strExtra, QIniFormat * ini, bool bCheck/*=false*/ )
{
    QString strRet;
	
    strExtra.clear();

	int ni;
    QString strName;
    QString strKeyEntry, strKeyData;
	for ( ni = 0; ni < SOFT_KEY_MAX; ni++ )
	{
        bool show = false;
        ushort left = 0;
        ushort top = 0;
        ushort right = 0;
        ushort bottom = 0;

		int nGroupIndex = -1;
        strName.clear();

		CSoftkey * pSoftkey = NULL;
		if ( ni < m_nSoftkey )
		{
			pSoftkey = m_arySoftkey[ni];
			show = pSoftkey->m_bShow;
            left = (ushort)(pSoftkey->m_rcPosition.left() / 2);
            top = (ushort)(pSoftkey->m_rcPosition.top() / 2);
            right = (ushort)(pSoftkey->m_rcPosition.right() / 2);
            bottom = (ushort)(pSoftkey->m_rcPosition.bottom() / 2);

			if( pSoftkey->getGroup() )
                nGroupIndex = pSoftkey->getGroup()->getGroupIndex();
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

        strExtra += QString("%1:%2,").arg(nGroupIndex).arg(strName);

		if ( ini != NULL && pSoftkey != NULL )
		{
            strKeyEntry = QString("key%1").arg(ni);
            strKeyData = QString("%1,%1").arg(pSoftkey->m_ptPosition.x(), 8, 'f', -1, '0').arg(pSoftkey->m_ptPosition.y(), 8, 'f', -1, '0');
            ini->setValue( strKeyEntry, strKeyData );
		}
	}

	if( !bCheck )
	{
		m_strBackupCmd = strRet;
		m_strBackupExtra = strExtra;
	}

    m_bIsModified = false;

	return strRet;
}

void CSoftkeyArray::loadGPIOInfo( QString lpctszCmd )
{
    m_aryGPIOInfo.clear();

    const char* buf = new char[lpctszCmd.size()+1];
    strcpy( (char*)buf, lpctszCmd.toUtf8().data() );
    char szGPIO[3] = {0,};

	int ni;
    for ( ni = 0 ; *buf != '\0' ; ni++, buf += 2 )
	{
        szGPIO[0] = buf[0];
        szGPIO[1] = buf[1];

        uchar cGPIO = (uchar)strtol( szGPIO, NULL, 16 );
		GPIOInfo info;
        info.bEnable = false;
		if ( cGPIO & 0x80 )	 // bind info
		{
            info.bEnable = true;
		}
        info.bOutput = false;
		if ( cGPIO & 0x01 )
		{
            info.bOutput = true;
		}

        m_aryGPIOInfo.push_back( info );
	}
}

QString CSoftkeyArray::saveGPIOInfo()
{
    QString strRet;

    int ni;
    for ( ni = 0 ; ni < m_aryGPIOInfo.count() ; ni++ )
	{
        GPIOInfo gpio = m_aryGPIOInfo.at(ni);
        QString strGPIO;
        if ( gpio.bEnable )
            strGPIO = "8";
        else
            strGPIO = "0";
        if ( gpio.bOutput )
            strGPIO += "1";
        else
            strGPIO += "0";

        strRet += strGPIO;
	}
	return strRet;
}

GPIOInfo* CSoftkeyArray::getGPIOInfo( int idx )
{
    if ( m_aryGPIOInfo.count() <= idx )
		return NULL;

    return m_aryGPIOInfo.data() + idx;;
}

void CSoftkeyArray::createGPIO( int nCount )
{
    m_aryGPIOInfo.clear();
	for ( int i=0 ; i<nCount ; i++ )
	{
		GPIOInfo info;
        info.bEnable = true;
        info.bOutput = false;
        m_aryGPIOInfo.push_back( info );
	}
}

int CSoftkeyArray::getGPIOCount()
{
    return m_aryGPIOInfo.count();
}

void CSoftkeyArray::resetBindInfo()
{
	for ( int i=0 ; i<m_nSoftkey ; i++ )
	{
		m_arySoftkey[i]->setBind( NULL );
	}
}

void CSoftkeyArray::loadBindInfo( QString lpctszCmd )
{
    const char* buf = new char[lpctszCmd.size()+1];
    strcpy( (char*)buf, lpctszCmd.toUtf8().data() );
    char szBind[3] = {0,};

	int ni;
	for ( ni = 0 ; ni < m_nSoftkey ; ni++, buf += 2 )
	{
		if ( buf[0] == 0 || buf[1] == 0 )
			break;
        szBind[0] = buf[0];
        szBind[1] = buf[1];

        uchar cBind = (uchar)strtol( szBind, NULL, 16 );

		if ( cBind & 0x80 )	 // bind info
		{
			int nBindIndex = (cBind & ~0x80);
			if ( nBindIndex < m_nSoftkey )
			{
                qDebug( "Bind: [%d] -> [%d]", ni, nBindIndex );
				CSoftkey* toBindKey = m_arySoftkey[nBindIndex];
				m_arySoftkey[ni]->setBind( toBindKey );
			}
			else
			{
                qDebug( "Bind Error: [0x%x]", cBind );
			}
		}
	}
}

int CSoftkeyArray::indexFromSoftkey( CSoftkey* pSoftkey )
{
	for ( int i = 0 ; i < m_nSoftkey ; i++ )
	{
		if ( m_arySoftkey[i] == pSoftkey )
			return i;
	}
	return -1;
}

QString CSoftkeyArray::saveBindInfo()
{
    QString strRet;

	int ni;
	for ( ni = 0 ; ni < SOFT_KEY_MAX ; ni++ )
	{
		if ( ni < m_nSoftkey )
		{
			CSoftkey* key = m_arySoftkey[ni];
			CSoftkey* toBindKey = key->getBind();
			if ( toBindKey )
			{
                int nBindIndex = indexFromSoftkey( toBindKey );
                Q_ASSERT( nBindIndex >= 0 );
                strRet += QString::number(0x80 | (uchar)nBindIndex, 16);
			}
			else
			{
                strRet += "00";
			}
		}
		else
		{
            strRet += "00";
		}
	}

	return strRet;
}

int CSoftkeyArray::addSoftkey( bool bShow, QRect rc )
{
	CSoftkey * pSoftkey;
    if ( !addSoftkey(pSoftkey = new CSoftkey(bShow, rc)) )
	{
		delete pSoftkey;
		return -1;
	}

    pSoftkey->m_ptPosition.setX( s_fKeyPos_x );
	s_fKeyPos_y += 0.07f;
	if ( s_fKeyPos_y >= 1.f )
	{
		s_fKeyPos_x += 0.06f; if ( s_fKeyPos_x >= 1.f ) s_fKeyPos_x = 0.1f;
		s_fKeyPos_y = 0.1f;
	}
    pSoftkey->m_ptPosition.setY( s_fKeyPos_y );

	return m_nSoftkey - 1;
}

void CSoftkeyArray::remove( int nRemoveIndex, CSoftlogicArray* logic  )
{
	if ( nRemoveIndex < 0 || nRemoveIndex >= m_nSoftkey )
		return;

	CSoftkey * removeKey = m_arySoftkey[nRemoveIndex];

	CSoftkey ** arySoftkeyNew = NULL;
	if ( m_nSoftkey > 1 )
	{
		arySoftkeyNew = new CSoftkey *[m_nSoftkey - 1];
		memcpy(arySoftkeyNew, m_arySoftkey, sizeof(CSoftkey *) * nRemoveIndex);
		memcpy(arySoftkeyNew + nRemoveIndex, m_arySoftkey + nRemoveIndex + 1, sizeof(CSoftkey *) * (m_nSoftkey - nRemoveIndex - 1));
	}

	m_nSoftkey--;
	delete [] m_arySoftkey;
	m_arySoftkey = arySoftkeyNew;

	delete removeKey;

	if ( logic == NULL )
		return;
    logic->_replacePort((CSoftlogic::Port)(nRemoveIndex + MM_SOFTLOGIC_PORT_SOFTKEY_START), CSoftlogic::epNoGate);
	for ( int ni = nRemoveIndex; ni < m_nSoftkey; ni++ )
	{
        logic->_replacePort((CSoftlogic::Port)(ni + 1 + MM_SOFTLOGIC_PORT_SOFTKEY_START), (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTKEY_START));
	}
}

void CSoftkeyArray::remove( CSoftkey * key, CSoftlogicArray* logic )
{
	for ( int i=0 ; i<m_nSoftkey ; i++ )
	{
		if ( m_arySoftkey[i] == key )
		{
            remove(i, logic);
			return; 
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// CSoftlogic

CSoftlogic::CSoftlogic()
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

	// extra
    m_ptPosition.setX( 0.5f ); m_ptPosition.setY( 0.5f );
	m_fLines[0] = m_fLines[1] = 0.5f;
}

CSoftlogic::~CSoftlogic()
{
}

CSoftlogic::PortType CSoftlogic::parsePort( Port port, int & idx )
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
		case MM_SOFTLOGIC_STATE_INVERT_DETECTION:
			idx = nPort;
			break;
		default:
			return eptError;
		}
		return eptState;
	}
	return eptError;
}

bool CSoftlogic::setLogicType( LogicType lt )
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

bool CSoftlogic::getEnableNot() const
{
	if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
        return false;
	return m_bEnableNot;
}

bool CSoftlogic::setEnableNot( bool bNot )
{
	if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
        return false;
	m_bEnableNot = bNot;
    return true;
}

CSoftlogic::Port CSoftlogic::getEnablePort() const
{
	if ( m_eOutType != eptState && m_eOutType != eptGPIO && m_eOutType != eptReport )
		return epNoGate;
	return m_eEnablePort;
}

bool CSoftlogic::setEnablePort( Port enablePort )
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

bool CSoftlogic::setIn1Not( bool bNot )
{
	m_bIn1Not = bNot;
    return true;
}

bool CSoftlogic::setIn1Port( Port inPort )
{
	m_eIn1Port = inPort;
    return true;
}

bool CSoftlogic::setIn2Not( bool bNot )
{
	m_bIn2Not = bNot;
    return true;
}

bool CSoftlogic::setIn2Port( Port inPort )
{
	m_eIn2Port = inPort;
    return true;
}

uchar CSoftlogic::getOutKey1ConditionKeys() const
{
	if ( (m_eOutType != eptKey) && (m_eOutType != eptMouse) )
		return 0;
	return m_out1;
}

uchar CSoftlogic::getOutKey1() const
{
	if ( m_eOutType != eptKey )
		return 0;
	return m_out2;
}

bool CSoftlogic::setOutKey1( uchar conditionKeys, uchar key )
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

uchar CSoftlogic::getOutMouse() const
{
	if ( m_eOutType != eptMouse )
		return 0;
	return m_out2;
}

bool CSoftlogic::setOutMouse( uchar conditionKeys, uchar mkey )
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

CSoftlogic::Trigger CSoftlogic::getLogicTrigger() const
{
	return m_eTrigger;
}

bool CSoftlogic::setLogicTrigger( Trigger trigger )
{
	if ( m_eLogicType == eltAND || m_eLogicType == eltOR || m_eLogicType == eltXOR ||
		m_eLogicType == eltAND_RISINGEDGE )
	{
        if ( trigger < etNoTrigger || trigger > etTrigger_U2048_D1024 )
            return false;

		m_eTrigger = trigger;
        return true;
	}
    return false;
}

CSoftlogic::Port CSoftlogic::getOutPort() const
{
	if ( m_eOutType != eptState && m_eOutType != eptGPIO )
		return epNoGate;
	return (CSoftlogic::Port)m_out2;
}

bool CSoftlogic::setOutPort( Port outPort )
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

int CSoftlogic::getReportId() const
{
	if ( m_eOutType != eptReport )
		return -1;
	return (int)m_out2;
}

bool CSoftlogic::setReportId( int report_id )
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
// CSoftlogicArray
static float s_fLogicPos_x = 0.25f;
static float s_fLogicPos_y = 0.1f;

CSoftlogicArray::CSoftlogicArray()
{
	m_nSoftlogic = 0;
	m_arySoftlogic = NULL;
	m_nSoftlogicHidden = 0;
	m_arySoftlogicHidden = NULL;
}

CSoftlogicArray::~CSoftlogicArray()
{
    clear();
}

bool CSoftlogicArray::isModified( QString lpctszCmd, QIniFormat * ini )
{
	s_fLogicPos_x = 0.25f;
	s_fLogicPos_y = 0.1f;

    QString strCmd = lpctszCmd;
    QString strNewCmd = save( NULL );

    if( strCmd.compare( strNewCmd ) != 0 )
        return true;

	int ni;
	for ( ni = 0; ni < SOFT_LOGIC_MAX; ni++ )
	{
		float fPositionX, fPositionY;
		float fLine1, fLine2;
        _getLogicPosFromIni( ini, ni, fPositionX, fPositionY, fLine1, fLine2 );

		CSoftlogic * logic = operator [](ni);

		if( !logic ) break;

        if( logic->m_ptPosition.x() != fPositionX )
            return true;
        if( logic->m_ptPosition.y() != fPositionY )
            return true;
		if( logic->m_fLines[0] != fLine1 )
            return true;
		if( logic->m_fLines[1] != fLine2 )
            return true;
	}

    return false;
}

void CSoftlogicArray::clear()
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
	if ( m_arySoftlogicHidden != NULL )
	{
		for ( m_nSoftlogicHidden--; m_nSoftlogicHidden >= 0; m_nSoftlogicHidden-- )
		{
			delete m_arySoftlogicHidden[m_nSoftlogicHidden];
		}
		delete [] m_arySoftlogicHidden;
		m_arySoftlogicHidden = NULL;
	}
	m_nSoftlogicHidden = 0;
}

void CSoftlogicArray::remove( int idx )
{
    CSoftlogic * pSoftlogic = _removeSoftlogic(idx);
	if ( pSoftlogic == NULL )
		return;

    _replacePort((CSoftlogic::Port)(idx + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), CSoftlogic::epNoGate);

	delete pSoftlogic;

    trimmingLogic();
}

int CSoftlogicArray::_insertSoftlogic( CSoftlogic * pSoftlogic, bool bHidden )
{
	if ( m_nSoftlogic + m_nSoftlogicHidden >= SOFT_LOGIC_MAX )
		return -1;

	if ( bHidden )
	{
		CSoftlogic ** arySoftlogicNew = new CSoftlogic *[m_nSoftlogicHidden + 1];
		if( m_arySoftlogicHidden )
		{
			memcpy(arySoftlogicNew, m_arySoftlogicHidden, m_nSoftlogicHidden * sizeof(CSoftlogic *));
			delete [] m_arySoftlogicHidden;
		}
		m_arySoftlogicHidden = arySoftlogicNew;
		m_arySoftlogicHidden[m_nSoftlogicHidden++] = pSoftlogic;

		return SOFT_LOGIC_MAX - m_nSoftlogicHidden;
	}
	else
	{
		CSoftlogic ** arySoftlogicNew = new CSoftlogic *[m_nSoftlogic+1];
		if( m_arySoftlogic )
		{
			memcpy(arySoftlogicNew, m_arySoftlogic, m_nSoftlogic * sizeof(CSoftlogic *));
			delete [] m_arySoftlogic;
		}
		m_arySoftlogic = arySoftlogicNew;
		m_arySoftlogic[m_nSoftlogic++] = pSoftlogic;

		return m_nSoftlogic - 1;
	}
}

CSoftlogic * CSoftlogicArray::_removeSoftlogic( int idx )
{
	CSoftlogic * pRet = NULL;

	if ( idx >= 0 && idx < m_nSoftlogic )
	{
		pRet = m_arySoftlogic[idx];
		m_arySoftlogic[idx] = NULL;
		return pRet;
	}
	idx = SOFT_LOGIC_MAX - idx - 1;
	if ( idx >= 0 && idx < m_nSoftlogicHidden )
	{
		pRet = m_arySoftlogicHidden[idx];
		m_arySoftlogicHidden[idx] = NULL;
		return pRet;
	}
	return NULL;
}

void CSoftlogicArray::_exchangePort( CSoftlogic::Port port1, CSoftlogic::Port port2 )
{
	int ni;
	for ( ni = 0; ni < m_nSoftlogic; ni++ )
	{
		if (  m_arySoftlogic[ni] == NULL || m_arySoftlogic[ni]->getLogicType() == CSoftlogic::eltNILL )
			continue;

		CSoftlogic * pSoftlogic = m_arySoftlogic[ni];

		if ( pSoftlogic->getEnablePort() == port1 )
            VERIFY(pSoftlogic->setEnablePort(port2));
		else if ( pSoftlogic->getEnablePort() == port2 )
			VERIFY(pSoftlogic->setEnablePort(port1));

		if ( pSoftlogic->getIn1Port() == port1 )
			VERIFY(pSoftlogic->setIn1Port(port2));
		else if ( pSoftlogic->getIn1Port() == port2 )
			VERIFY(pSoftlogic->setIn1Port(port1));

		if ( pSoftlogic->getIn2Port() == port1 )
			VERIFY(pSoftlogic->setIn2Port(port2));
		else if ( pSoftlogic->getIn2Port() == port2 )
			VERIFY(pSoftlogic->setIn2Port(port1));

		if ( pSoftlogic->getOutPort() == port1 )
			VERIFY(pSoftlogic->setOutPort(port2));
		else if ( pSoftlogic->getOutPort() == port2 )
			VERIFY(pSoftlogic->setOutPort(port1));
	}
	for ( ni = 0; ni < m_nSoftlogicHidden; ni++ )
	{
		if (  m_arySoftlogicHidden[ni] == NULL || m_arySoftlogicHidden[ni]->getLogicType() == CSoftlogic::eltNILL )
			continue;

		CSoftlogic * pSoftlogic = m_arySoftlogicHidden[ni];

		if ( pSoftlogic->getEnablePort() == port1 )
			VERIFY(pSoftlogic->setEnablePort(port2));
		else if ( pSoftlogic->getEnablePort() == port2 )
			VERIFY(pSoftlogic->setEnablePort(port1));

		if ( pSoftlogic->getIn1Port() == port1 )
			VERIFY(pSoftlogic->setIn1Port(port2));
		else if ( pSoftlogic->getIn1Port() == port2 )
			VERIFY(pSoftlogic->setIn1Port(port1));

		if ( pSoftlogic->getIn2Port() == port1 )
			VERIFY(pSoftlogic->setIn2Port(port2));
		else if ( pSoftlogic->getIn2Port() == port2 )
			VERIFY(pSoftlogic->setIn2Port(port1));

		if ( pSoftlogic->getOutPort() == port1 )
			VERIFY(pSoftlogic->setOutPort(port2));
		else if ( pSoftlogic->getOutPort() == port2 )
			VERIFY(pSoftlogic->setOutPort(port1));
	}
}

void CSoftlogicArray::_replacePort( CSoftlogic::Port port, CSoftlogic::Port portNew )
{
	int ni;
	for ( ni = 0; ni < m_nSoftlogic; ni++ )
	{
		if (  m_arySoftlogic[ni] == NULL || m_arySoftlogic[ni]->getLogicType() == CSoftlogic::eltNILL )
			continue;

		CSoftlogic * pSoftlogic = m_arySoftlogic[ni];

		if ( pSoftlogic->getEnablePort() == port )
			VERIFY(pSoftlogic->setEnablePort(portNew));

		if ( pSoftlogic->getIn1Port() == port )
			VERIFY(pSoftlogic->setIn1Port(portNew));

		if ( pSoftlogic->getIn2Port() == port )
			VERIFY(pSoftlogic->setIn2Port(portNew));

		if ( pSoftlogic->getOutPort() == port )
			VERIFY(pSoftlogic->setOutPort(portNew));
	}
	for ( ni = 0; ni < m_nSoftlogicHidden; ni++ )
	{
		if (  m_arySoftlogicHidden[ni] == NULL || m_arySoftlogicHidden[ni]->getLogicType() == CSoftlogic::eltNILL )
			continue;

		CSoftlogic * pSoftlogic = m_arySoftlogicHidden[ni];

		if ( pSoftlogic->getEnablePort() == port )
			VERIFY(pSoftlogic->setEnablePort(portNew));

		if ( pSoftlogic->getIn1Port() == port )
			VERIFY(pSoftlogic->setIn1Port(portNew));

		if ( pSoftlogic->getIn2Port() == port )
			VERIFY(pSoftlogic->setIn2Port(portNew));

		if ( pSoftlogic->getOutPort() == port )
			VERIFY(pSoftlogic->setOutPort(portNew));
	}
}

CSoftlogic * CSoftlogicArray::operator [] ( int idx )
{
	if ( idx < 0 || idx >= SOFT_LOGIC_MAX )
		return NULL;

	if ( idx < m_nSoftlogic  )
		return m_arySoftlogic[idx];
	else if ( SOFT_LOGIC_MAX - idx - 1 < m_nSoftlogicHidden )
		return m_arySoftlogicHidden[SOFT_LOGIC_MAX - idx - 1];
	else
		return NULL;
}

int CSoftlogicArray::indexFromSoftlogic( CSoftlogic * pSoftlogic ) const
{
	int ni;
	for ( ni = 0; ni < m_nSoftlogic; ni++ )
	{
		if (  m_arySoftlogic[ni] == pSoftlogic )
			return ni;
	}
	for ( ni = 0; ni < m_nSoftlogicHidden; ni++ )
	{
		if (  m_arySoftlogicHidden[ni] == pSoftlogic )
			return SOFT_LOGIC_MAX - ni - 1;
	}
	return -1;
}

bool CSoftlogicArray::isHiddenLogic( int idx ) const
{
	return idx < (SOFT_LOGIC_MAX - m_nSoftlogicHidden);
}

void CSoftlogicArray::_setLogicPosFromIni( QIniFormat * ini, int idx_logic )
{
	CSoftlogic * logic = operator [](idx_logic);
	if ( logic == NULL || logic->getLogicType() == CSoftlogic::eltNILL )
		return;

	float fLogicF[4];

    QString strLogicInfo;
	if ( ini != NULL )
        strLogicInfo = ini->getValue( QString("logic%1").arg(idx_logic) );
	
    if ( strLogicInfo.isEmpty() )
	{
		s_fLogicPos_x += 0.06f; if ( s_fLogicPos_x >= 1.f ) s_fLogicPos_x = 0.1f;
		fLogicF[0] = s_fLogicPos_x;
		s_fLogicPos_y += 0.07f; if ( s_fLogicPos_y >= 1.f ) s_fLogicPos_y = 0.1f;
		fLogicF[1] = s_fLogicPos_y;
		fLogicF[2] = 0.5f;
		fLogicF[3] = 0.5f;
	}
	else
	{
        int nD = strLogicInfo.indexOf(',');
        fLogicF[0] = strLogicInfo.left(nD).toFloat();
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        nD = strLogicInfo.indexOf(',');
        fLogicF[1] = strLogicInfo.left(nD).toFloat();
        nD = strLogicInfo.indexOf(',');
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        fLogicF[2] = strLogicInfo.left(nD).toFloat();
        nD = strLogicInfo.indexOf(',');
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        fLogicF[3] = strLogicInfo.toFloat();
	}
    logic->m_ptPosition.setX( fLogicF[0] );
    logic->m_ptPosition.setY( fLogicF[1] );
	logic->m_fLines[0] = fLogicF[2];
	logic->m_fLines[1] = fLogicF[3];
}

void CSoftlogicArray::_getLogicPosFromIni( QIniFormat * ini, int idx_logic, float& fPositionX, float& fPositionY, float& fLine1, float& fLine2  )
{
	CSoftlogic * logic = operator [](idx_logic);
	if ( logic == NULL || logic->getLogicType() == CSoftlogic::eltNILL )
		return;

	float fLogicF[4];

    QString strLogicInfo;
	if ( ini != NULL )
        strLogicInfo = ini->getValue( QString("logic%1").arg(idx_logic) );
	
    if ( strLogicInfo.isEmpty() )
	{
		s_fLogicPos_x += 0.06f; if ( s_fLogicPos_x >= 1.f ) s_fLogicPos_x = 0.1f;
		fLogicF[0] = s_fLogicPos_x;
		s_fLogicPos_y += 0.07f; if ( s_fLogicPos_y >= 1.f ) s_fLogicPos_y = 0.1f;
		fLogicF[1] = s_fLogicPos_y;
		fLogicF[2] = fLogicF[3] = 0.5f;
	}
	else
	{
        int nD = strLogicInfo.indexOf(',');
        fLogicF[0] = strLogicInfo.left(nD).toFloat();
        nD = strLogicInfo.indexOf(',');
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        fLogicF[1] = strLogicInfo.left(nD).toFloat();
        nD = strLogicInfo.indexOf(',');
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        fLogicF[2] = strLogicInfo.left(nD).toFloat();
        nD = strLogicInfo.indexOf(',');
        if ( nD > 0 ) strLogicInfo = strLogicInfo.mid(nD + 1);
        fLogicF[3] = strLogicInfo.toFloat();
	}
	fPositionX = fLogicF[0];
	fPositionY = fLogicF[1];
	fLine1 = fLogicF[2];
	fLine2 = fLogicF[3];
}

bool CSoftlogicArray::load( QString lpctszCmd, QIniFormat * ini )
{
    clear();

	s_fLogicPos_x = 0.25f;
    s_fLogicPos_y = 0.1f;

    char* buf = new char[lpctszCmd.size()+1];
    strcpy( buf, lpctszCmd.toUtf8().data() );
    uchar buf0, buf1, buf2, buf3, buf4, buf5;

	int hiddenlogic_cnt = 0;
	if ( buf[0] != 0 )
	{
		if ( buf[0] == 'p' ) buf0 = ('\\' - '0'); else buf0 = buf[0] - '0';

		hiddenlogic_cnt = buf0;
		if ( hiddenlogic_cnt > SOFT_LOGIC_MAX )
			hiddenlogic_cnt = SOFT_LOGIC_MAX;
		buf++;
	}

	int ni;
	for ( ni = 0; ni < SOFT_LOGIC_MAX; ni++ )
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

		CSoftlogic::Trigger trigger = (CSoftlogic::Trigger)(op & MM_SOFTLOGIC_OP_TRIGGER_INF0);

		CSoftlogic::LogicType lt = CSoftlogic::eltNILL;
        //bool bNotEnable = false;
		//CSoftlogic::Port inEnable = CSoftlogic::epNoGate;
        bool bNotPort1 = false;
		CSoftlogic::Port inPort1 = CSoftlogic::epNoGate;
        bool bNotPort2 = false;
		CSoftlogic::Port inPort2 = CSoftlogic::epNoGate;

		switch ( (op >> 4) )
		{
		default:
            Q_ASSERT(false);
		case MM_SOFTLOGIC_NOACTION:
			opex = 0;
		add_softlogic_key:
            if ( addSoftlogic_Key(lt, bNotPort1, inPort1, bNotPort2, inPort2,
			                      enable_ckey, opex, trigger) != ni )
			{
                clear();
                return false;
			}
            _setLogicPosFromIni(ini, ni);
			break;

		case MM_SOFTLOGIC_OP_KEY_AND:
			lt = CSoftlogic::eltAND;
			bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
			bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			goto add_softlogic_key;

		case MM_SOFTLOGIC_OP_KEY_OR:
			lt = CSoftlogic::eltOR;
			bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
			bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			goto add_softlogic_key;

		case MM_SOFTLOGIC_OP_KEY_XOR:
			lt = CSoftlogic::eltXOR;
			bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
			bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			goto add_softlogic_key;

		case MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE:
			lt = CSoftlogic::eltAND_RISINGEDGE;
			bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
			bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
			inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_key;

		case MM_SOFTLOGIC_OP_MKEY:
			switch ( (opex >> 5) )
			{
			default:
                Q_ASSERT(false);
				break;
			case MM_SOFTLOGIC_OPEX_MKEY_AND:
				lt = CSoftlogic::eltAND;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			add_softlogic_mouse:
                if ( addSoftlogic_Mouse(lt, bNotPort1, inPort1, bNotPort2, inPort2,
				                        enable_ckey, opex & MM_SOFTLOGIC_OPEX_MKEY_ACT_MASK, trigger) != ni )
				{
                    clear();
                    return false;
				}
                _setLogicPosFromIni(ini, ni);
				break;

			case MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE:
				lt = CSoftlogic::eltAND_RISINGEDGE;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_mouse;
			}
			break;

		case MM_SOFTLOGIC_OP_STAT:
			switch ( (opex >> 6) )
			{
			default:
                Q_ASSERT(false);
			case MM_SOFTLOGIC_OPEX_STAT_AND:
				lt = CSoftlogic::eltAND;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			add_softlogic_stat:
                if ( addSoftlogic_State(lt, ((enable_ckey & MM_SOFTLOGIC_ENABLE_NOT) != 0x0), (CSoftlogic::Port)(enable_ckey & MM_SOFTLOGIC_ENABLE_PORTMASK),
										bNotPort1, inPort1, bNotPort2, inPort2,
				                        (CSoftlogic::Port)(opex & MM_SOFTLOGIC_OPEX_STAT_OUTPORT),
				                        trigger) != ni )
				{
                    clear();
                    return false;
				}
                _setLogicPosFromIni(ini, ni);
				break;

			case MM_SOFTLOGIC_OPEX_STAT_OR:
				lt = CSoftlogic::eltOR;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_stat;

			case MM_SOFTLOGIC_OPEX_STAT_XOR:
				lt = CSoftlogic::eltXOR;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_stat;

			case MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE:
				lt = CSoftlogic::eltAND_RISINGEDGE;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_stat;
			}
			break;

		case MM_SOFTLOGIC_OP_REPORT:
			switch ( (opex >> 6) )
			{
			default:
                Q_ASSERT(false);
			case MM_SOFTLOGIC_OPEX_REPORT_AND:
				lt = CSoftlogic::eltAND;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
			add_softlogic_report:
                if ( addSoftlogic_Report(lt, ((enable_ckey & MM_SOFTLOGIC_ENABLE_NOT) != 0x0), (CSoftlogic::Port)(enable_ckey & MM_SOFTLOGIC_ENABLE_PORTMASK),
										 bNotPort1, inPort1, bNotPort2, inPort2,
				                         (opex & MM_SOFTLOGIC_OPEX_REPORT_ID),
				                         trigger) != ni )
				{
                    clear();
                    return false;
				}
                _setLogicPosFromIni(ini, ni);
				break;

			case MM_SOFTLOGIC_OPEX_REPORT_OR:
				lt = CSoftlogic::eltOR;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_report;

			case MM_SOFTLOGIC_OPEX_REPORT_XOR:
				lt = CSoftlogic::eltXOR;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_report;

			case MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE:
				lt = CSoftlogic::eltAND_RISINGEDGE;
				bNotPort1 = (in1 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort1 = (CSoftlogic::Port)(in1 & MM_SOFTLOGIC_IN_MASK);
				bNotPort2 = (in2 & MM_SOFTLOGIC_IN_NOT) != 0x0;
				inPort2 = (CSoftlogic::Port)(in2 & MM_SOFTLOGIC_IN_MASK);
				goto add_softlogic_report;
			}
			break;
		}

		buf += 6;
	}

	for ( ni = 0; ni < hiddenlogic_cnt; ni++ )
	{
        hideSoftlogic(SOFT_LOGIC_MAX - ni - 1, true);
	}

    trimmingLogic();

    return true;
}

QString CSoftlogicArray::save( QIniFormat * ini )
{
    QString strRet;

	strRet += (char)('0' + (m_nSoftlogicHidden & 0x3f));

	int ni;
    uchar in1, in2;
    uchar opex, op;
    uchar enable_ckey;

    QString strLogicInfo, strLogicData;

	for ( ni = 0; ni < SOFT_LOGIC_MAX - m_nSoftlogicHidden; ni++ )
	{
		enable_ckey = 0;
		in1 = 0;
		in2 = 0;
		opex = 0;
		op = 0;

		CSoftlogic * pSoftlogic = NULL;

		if ( ni < m_nSoftlogic )
		{
			pSoftlogic = m_arySoftlogic[ni];
			switch ( pSoftlogic->getLogicType() )
			{
			case CSoftlogic::eltAND:
				enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
				enable_ckey |= pSoftlogic->getEnablePort();
				in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in1 |= pSoftlogic->getIn1Port();
				in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in2 |= pSoftlogic->getIn2Port();

				switch ( pSoftlogic->getOutPortType() )
				{
				default:
				case CSoftlogic::eptKey:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_KEY_AND << 4) | pSoftlogic->getLogicTrigger();
					opex = pSoftlogic->getOutKey1();
					break;
				case CSoftlogic::eptMouse:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_MKEY_AND << 5) | pSoftlogic->getOutMouse();
					break;
				case CSoftlogic::eptState:
				case CSoftlogic::eptGPIO:
					op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_STAT_AND << 6) | pSoftlogic->getOutPort();
					break;
				case CSoftlogic::eptReport:
					op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_REPORT_AND << 6) | pSoftlogic->getReportId();
					break;
				}
				break;

			case CSoftlogic::eltOR:
				enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
				enable_ckey |= pSoftlogic->getEnablePort();
				in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in1 |= pSoftlogic->getIn1Port();
				in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in2 |= pSoftlogic->getIn2Port();

				switch ( pSoftlogic->getOutPortType() )
				{
				default:
				case CSoftlogic::eptKey:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_KEY_OR << 4) | pSoftlogic->getLogicTrigger();
					opex = pSoftlogic->getOutKey1();
					break;
				case CSoftlogic::eptMouse:
                    Q_ASSERT(false);
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
					opex = 0;
					break;
				case CSoftlogic::eptState:
				case CSoftlogic::eptGPIO:
					op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_STAT_OR << 6) | pSoftlogic->getOutPort();
					break;
				case CSoftlogic::eptReport:
					op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_REPORT_OR << 6) | pSoftlogic->getReportId();
					break;
				}
				break;

			case CSoftlogic::eltXOR:
				enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
				enable_ckey |= pSoftlogic->getEnablePort();
				in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in1 |= pSoftlogic->getIn1Port();
				in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in2 |= pSoftlogic->getIn2Port();

				switch ( pSoftlogic->getOutPortType() )
				{
				default:
				case CSoftlogic::eptKey:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_KEY_XOR << 4) | pSoftlogic->getLogicTrigger();
					opex = pSoftlogic->getOutKey1();
					break;
				case CSoftlogic::eptMouse:
                    Q_ASSERT(false);
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
					opex = 0;
					break;
				case CSoftlogic::eptState:
				case CSoftlogic::eptGPIO:
					op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_STAT_XOR << 6) | pSoftlogic->getOutPort();
					break;
				case CSoftlogic::eptReport:
					op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_REPORT_XOR << 6) | pSoftlogic->getReportId();
					break;
				}
				break;

			case CSoftlogic::eltAND_RISINGEDGE:
				enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
				enable_ckey |= pSoftlogic->getEnablePort();
				in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in1 |= pSoftlogic->getIn1Port();
				in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
				in2 |= pSoftlogic->getIn2Port();

				switch ( pSoftlogic->getOutPortType() )
				{
				default:
				case CSoftlogic::eptKey:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE << 4) | pSoftlogic->getLogicTrigger();
					opex = pSoftlogic->getOutKey1();
					break;
				case CSoftlogic::eptMouse:
					enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
					op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE << 5) | pSoftlogic->getOutMouse();
					break;
				case CSoftlogic::eptState:
				case CSoftlogic::eptGPIO:
					op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE << 6) | pSoftlogic->getOutPort();
					break;
				case CSoftlogic::eptReport:
					op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
					opex = (MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE << 6) | pSoftlogic->getReportId();
					break;
				}
				break;

			case CSoftlogic::eltNILL:
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

		if ( ini && pSoftlogic != NULL )
		{
            strLogicInfo = QString("logic%1").arg(ni);
            strLogicData = QString("%1,%2,%3,%4").arg(pSoftlogic->m_ptPosition.x(), 8, 'f', -1, '0').arg(pSoftlogic->m_ptPosition.y(), 8, 'f', -1, '0').arg(pSoftlogic->m_fLines[0], 8, 'f', -1, '0').arg(pSoftlogic->m_fLines[1], 8, 'f', -1, '0');
            ini->setValue( strLogicInfo, strLogicData );
		}
	}

	for ( ni = m_nSoftlogicHidden - 1; ni >= 0; ni-- )
	{
		enable_ckey = 0;
		in1 = 0;
		in2 = 0;
		opex = 0;
		op = 0;

		CSoftlogic * pSoftlogic = m_arySoftlogicHidden[ni];
		switch ( pSoftlogic->getLogicType() )
		{
		case CSoftlogic::eltAND:
			enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
			enable_ckey |= pSoftlogic->getEnablePort();
			in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in1 |= pSoftlogic->getIn1Port();
			in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in2 |= pSoftlogic->getIn2Port();

			switch ( pSoftlogic->getOutPortType() )
			{
			default:
			case CSoftlogic::eptKey:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_KEY_AND << 4) | pSoftlogic->getLogicTrigger();
				opex = pSoftlogic->getOutKey1();
				break;
			case CSoftlogic::eptMouse:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_MKEY_AND << 5) | pSoftlogic->getOutMouse();
				break;
			case CSoftlogic::eptState:
			case CSoftlogic::eptGPIO:
				op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_STAT_AND << 6) | pSoftlogic->getOutPort();
				break;
			case CSoftlogic::eptReport:
				op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_REPORT_AND << 6) | pSoftlogic->getReportId();
				break;
			}
			break;

		case CSoftlogic::eltOR:
			enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
			enable_ckey |= pSoftlogic->getEnablePort();
			in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in1 |= pSoftlogic->getIn1Port();
			in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in2 |= pSoftlogic->getIn2Port();

			switch ( pSoftlogic->getOutPortType() )
			{
			default:
			case CSoftlogic::eptKey:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_KEY_OR << 4) | pSoftlogic->getLogicTrigger();
				opex = pSoftlogic->getOutKey1();
				break;
			case CSoftlogic::eptMouse:
                Q_ASSERT(false);
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
				opex = 0;
				break;
			case CSoftlogic::eptState:
			case CSoftlogic::eptGPIO:
				op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_STAT_OR << 6) | pSoftlogic->getOutPort();
				break;
			case CSoftlogic::eptReport:
				op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_REPORT_OR << 6) | pSoftlogic->getReportId();
				break;
			}
			break;

		case CSoftlogic::eltXOR:
			enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
			enable_ckey |= pSoftlogic->getEnablePort();
			in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in1 |= pSoftlogic->getIn1Port();
			in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in2 |= pSoftlogic->getIn2Port();

			switch ( pSoftlogic->getOutPortType() )
			{
			default:
			case CSoftlogic::eptKey:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_KEY_XOR << 4) | pSoftlogic->getLogicTrigger();
				opex = pSoftlogic->getOutKey1();
				break;
			case CSoftlogic::eptMouse:
                Q_ASSERT(false);
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
				opex = 0;
				break;
			case CSoftlogic::eptState:
			case CSoftlogic::eptGPIO:
				op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_STAT_XOR << 6) | pSoftlogic->getOutPort();
				break;
			case CSoftlogic::eptReport:
				op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_REPORT_XOR << 6) | pSoftlogic->getReportId();
				break;
			}
			break;

		case CSoftlogic::eltAND_RISINGEDGE:
			enable_ckey = pSoftlogic->getEnableNot() ? MM_SOFTLOGIC_ENABLE_NOT : 0;
			enable_ckey |= pSoftlogic->getEnablePort();
			in1 = pSoftlogic->getIn1Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in1 |= pSoftlogic->getIn1Port();
			in2 = pSoftlogic->getIn2Not() ? MM_SOFTLOGIC_IN_NOT : 0;
			in2 |= pSoftlogic->getIn2Port();

			switch ( pSoftlogic->getOutPortType() )
			{
			default:
			case CSoftlogic::eptKey:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE << 4) | pSoftlogic->getLogicTrigger();
				opex = pSoftlogic->getOutKey1();
				break;
			case CSoftlogic::eptMouse:
				enable_ckey = pSoftlogic->getOutKey1ConditionKeys();
				op = (MM_SOFTLOGIC_OP_MKEY << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE << 5) | pSoftlogic->getOutMouse();
				break;
			case CSoftlogic::eptState:
			case CSoftlogic::eptGPIO:
				op = (MM_SOFTLOGIC_OP_STAT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE << 6) | pSoftlogic->getOutPort();
				break;
			case CSoftlogic::eptReport:
				op = (MM_SOFTLOGIC_OP_REPORT << 4) | pSoftlogic->getLogicTrigger();
				opex = (MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE << 6) | pSoftlogic->getReportId();
				break;
			}
			break;

		case CSoftlogic::eltNILL:
		default:
			break;
		}

		strRet += (char)('0' + (((enable_ckey << 2) | (in1 >> 6)) & 0x3f));
		strRet += (char)('0' + (in1 & 0x3f));
		strRet += (char)('0' + ((in2 >> 2) & 0x3f));
		strRet += (char)('0' + (((in2 << 4) | (opex >> 4)) & 0x3f));
		strRet += (char)('0' + (((opex << 2) | (op >> 6)) & 0x3f));
		strRet += (char)('0' + (op & 0x3f));

		if ( ini && pSoftlogic != NULL )
		{
            strLogicInfo = QString("logic%1").arg(SOFT_LOGIC_MAX - ni - 1);
            strLogicData = QString("%1,%2,%3,%4").arg(pSoftlogic->m_ptPosition.x(), 8, 'f', -1, '0').arg(pSoftlogic->m_ptPosition.y(), 8, 'f', -1, '0').arg(pSoftlogic->m_fLines[0], 8, 'f', -1, '0').arg(pSoftlogic->m_fLines[1], 8, 'f', -1, '0');
            ini->setValue( strLogicInfo, strLogicData );
		}
	}

	return strRet;
}

int CSoftlogicArray::addSoftlogic_Key( CSoftlogic::LogicType logictype,
                                      bool in1_not, CSoftlogic::Port in1_port,
                                      bool in2_not, CSoftlogic::Port in2_port,
                                      uchar condition_key, uchar key,
                                      CSoftlogic::Trigger trigger, bool bHidden )
{
	CSoftlogic * pSoftlogic = new CSoftlogic;
	do
	{
		if ( !pSoftlogic->setLogicType(logictype) )
			break;

		if ( logictype != CSoftlogic::eltNILL )
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

        if ( _insertSoftlogic(pSoftlogic) < 0 )
			break;

		int nLogicIdx = m_nSoftlogic - 1;
        nLogicIdx = hideSoftlogic( nLogicIdx, bHidden );

		return nLogicIdx;
	}
    while ( false );

	delete pSoftlogic;

	return -1;
}

int CSoftlogicArray::addSoftlogic_Mouse( CSoftlogic::LogicType logictype,
                                        bool in1_not, CSoftlogic::Port in1_port,
                                        bool in2_not, CSoftlogic::Port in2_port,
                                        uchar condition_key, uchar mkey,
                                        CSoftlogic::Trigger trigger, bool bHidden )
{
	CSoftlogic * pSoftlogic = new CSoftlogic;
	do
	{
		if ( !pSoftlogic->setLogicType(logictype) )
			break;

		if ( logictype != CSoftlogic::eltNILL )
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

        if ( _insertSoftlogic(pSoftlogic) < 0 )
			break;

		int nLogicIdx = m_nSoftlogic - 1;
        nLogicIdx = hideSoftlogic( nLogicIdx, bHidden );

		return nLogicIdx;
	}
    while ( false );

	delete pSoftlogic;

	return -1;
}

int CSoftlogicArray::addSoftlogic_State( CSoftlogic::LogicType logictype,
                                        bool enable_not, CSoftlogic::Port enable_port,
                                        bool in1_not, CSoftlogic::Port in1_port,
                                        bool in2_not, CSoftlogic::Port in2_port,
                                        CSoftlogic::Port out_port, CSoftlogic::Trigger trigger, bool bHidden )
{
	CSoftlogic * pSoftlogic = new CSoftlogic;
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

        if ( _insertSoftlogic(pSoftlogic) < 0 )
			break;

		int nLogicIdx = m_nSoftlogic - 1;
        nLogicIdx = hideSoftlogic( nLogicIdx, bHidden );

		return nLogicIdx;
	}
    while ( false );

	delete pSoftlogic;

	return -1;
}

int CSoftlogicArray::addSoftlogic_Report( CSoftlogic::LogicType logictype,
                                         bool enable_not, CSoftlogic::Port enable_port,
                                         bool in1_not, CSoftlogic::Port in1_port,
                                         bool in2_not, CSoftlogic::Port in2_port,
                                         int report_id, CSoftlogic::Trigger trigger, bool bHidden )
{
	CSoftlogic * pSoftlogic = new CSoftlogic;
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

        if ( _insertSoftlogic(pSoftlogic) < 0 )
			break;

		int nLogicIdx = m_nSoftlogic - 1;
        nLogicIdx = hideSoftlogic( nLogicIdx, bHidden );

		return nLogicIdx;
	}
    while ( false );

	delete pSoftlogic;

	return -1;
}

bool CSoftlogicArray::exchangeSotflogic( int idx1, int idx2 )
{
	if ( idx1 < 0 || idx2 < 0 )
        return false;

	CSoftlogic * pSoftlogic1 = operator[](idx1);
	CSoftlogic * pSoftlogic2 = operator[](idx2);
	if ( pSoftlogic1 == NULL || pSoftlogic2 == NULL )
        return false;

	if ( idx1 < m_nSoftlogic  )
		m_arySoftlogic[idx1] = pSoftlogic2;
	else if ( SOFT_LOGIC_MAX - idx1 - 1 < m_nSoftlogicHidden )
		m_arySoftlogicHidden[SOFT_LOGIC_MAX - idx1 - 1] = pSoftlogic2;
	else
	{
        Q_ASSERT(false);
	}

	if ( idx2 < m_nSoftlogic  )
		m_arySoftlogic[idx2] = pSoftlogic1;
	else if ( SOFT_LOGIC_MAX - idx2 - 1 < m_nSoftlogicHidden )
		m_arySoftlogicHidden[SOFT_LOGIC_MAX - idx2 - 1] = pSoftlogic1;
	else
	{
        Q_ASSERT(false);
	}

    _exchangePort((CSoftlogic::Port)(idx1 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), (CSoftlogic::Port)(idx2 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));

    return true;
}

int CSoftlogicArray::moveSoftlogicTo( int from, int to )
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
                    if ( !exchangeSotflogic(ni, ni + 1) )
						return ni;
				}
			}
			else if ( from > to )
			{
				for ( ni = from; ni > to; ni-- )
				{
                    if ( !exchangeSotflogic(ni - 1, ni) )
						return ni;
				}
			}
			return to;
		}
		else
			return -1;
	}
	if ( SOFT_LOGIC_MAX - from - 1 < m_nSoftlogicHidden )
	{
		if ( SOFT_LOGIC_MAX - to - 1 < m_nSoftlogicHidden )
		{
			if ( from < to )
			{
				for ( ni = from; ni < to; ni++ )
				{
                    if ( !exchangeSotflogic(ni, ni + 1) )
						return ni;
				}
			}
			else if ( from > to )
			{
				for ( ni = from; ni > to; ni-- )
				{
                    if ( !exchangeSotflogic(ni - 1, ni) )
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

int CSoftlogicArray::hideSoftlogic( int idx, bool bHidden )
{
	int idxHidden = SOFT_LOGIC_MAX - idx - 1;
	if ( bHidden )
	{
		if ( idxHidden >= 0 && idxHidden < m_nSoftlogicHidden )
			return idx;
		if ( idx < 0 || idx >= m_nSoftlogic )
			return -1;

		if ( idx != m_nSoftlogic - 1 )
            idx = moveSoftlogicTo(idx, m_nSoftlogic - 1);

        CSoftlogic * pSoftlogic = _removeSoftlogic(idx);
        trimmingLogic(true);
        int idxNew = _insertSoftlogic(pSoftlogic, true);
        _exchangePort((CSoftlogic::Port)(idx + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), (CSoftlogic::Port)(idxNew + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));
		return idxNew;
	}
	else
	{
		if ( idx >= 0 && idx < m_nSoftlogic )
			return idx;
		if ( idxHidden < 0 || idxHidden >= m_nSoftlogicHidden )
			return -1;

		if ( idxHidden != m_nSoftlogicHidden - 1 )
            idx = moveSoftlogicTo(idx, SOFT_LOGIC_MAX - m_nSoftlogicHidden - 1);

        CSoftlogic * pSoftlogic = _removeSoftlogic(idx);
        trimmingLogic(true);
        int idxNew = _insertSoftlogic(pSoftlogic);
        _exchangePort((CSoftlogic::Port)(idx + MM_SOFTLOGIC_PORT_SOFTLOGIC_START), (CSoftlogic::Port)(idxNew + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));
		return idxNew;
	}
}

void CSoftlogicArray::trimmingLogic( bool bTrimEndOnly )
{
	int ni, nj;
	CSoftlogic ** arySoftlogicNew;

	for ( ni = m_nSoftlogic - 1; ni >= 0; ni-- )
	{
		if ( m_arySoftlogic[ni] == NULL )
			continue;
		if ( m_arySoftlogic[ni]->getLogicType() == CSoftlogic::eltNILL )
		{
			delete m_arySoftlogic[ni];
			m_arySoftlogic[ni] = NULL;
			continue;
		}
		break;
	}
	if ( ni < m_nSoftlogic - 1 )
	{
		arySoftlogicNew = NULL;
		if ( ni >= 0 )
		{
			arySoftlogicNew = new CSoftlogic *[ni + 1];
			memcpy(arySoftlogicNew, m_arySoftlogic, sizeof(CSoftlogic *) * (ni + 1));
		}
		m_nSoftlogic = ni + 1;
		delete [] m_arySoftlogic;
		m_arySoftlogic = arySoftlogicNew;
	}

	for ( ni = m_nSoftlogicHidden - 1; ni >= 0; ni-- )
	{
		if ( m_arySoftlogicHidden[ni] == NULL )
			continue;
		if ( m_arySoftlogicHidden[ni]->getLogicType() == CSoftlogic::eltNILL )
		{
			delete m_arySoftlogicHidden[ni];
			m_arySoftlogicHidden[ni] = NULL;
			continue;
		}
		break;
	}
	if ( ni < m_nSoftlogicHidden - 1 )
	{
		arySoftlogicNew = NULL;
		if ( ni >= 0 )
		{
			arySoftlogicNew = new CSoftlogic *[ni + 1];
			memcpy(arySoftlogicNew, m_arySoftlogicHidden, sizeof(CSoftlogic *) * (ni + 1));
		}
		m_nSoftlogicHidden = ni + 1;
		delete [] m_arySoftlogicHidden;
		m_arySoftlogicHidden = arySoftlogicNew;
	}

	if ( bTrimEndOnly )
		return;

	for ( ni = m_nSoftlogic - 1; ni >= 0; ni-- )
	{
		if ( m_arySoftlogic[ni] != NULL &&
			m_arySoftlogic[ni]->getLogicType() != CSoftlogic::eltNILL )
			continue;

		if ( m_arySoftlogic[ni] != NULL )
		{
			delete m_arySoftlogic[ni];
			m_arySoftlogic[ni] = NULL;
		}

		arySoftlogicNew = NULL;
		if ( m_nSoftlogic > 1 )
		{
			arySoftlogicNew = new CSoftlogic *[m_nSoftlogic - 1];
			memcpy(arySoftlogicNew, m_arySoftlogic, sizeof(CSoftlogic *) * ni);
			memcpy(arySoftlogicNew + ni, m_arySoftlogic + ni + 1, sizeof(CSoftlogic *) * (m_nSoftlogic - ni - 1));
		}
		m_nSoftlogic--;
		delete [] m_arySoftlogic;
		m_arySoftlogic = arySoftlogicNew;

		for ( nj = ni; nj < m_nSoftlogic; nj++ )
		{
            _replacePort((CSoftlogic::Port)(nj + 1 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START),
						 (CSoftlogic::Port)(nj + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));
		}
	}

	for ( ni = m_nSoftlogicHidden - 1; ni >= 0; ni-- )
	{
		if ( m_arySoftlogicHidden[ni] != NULL &&
			m_arySoftlogicHidden[ni]->getLogicType() != CSoftlogic::eltNILL )
			continue;

		if ( m_arySoftlogicHidden[ni] != NULL )
		{
			delete m_arySoftlogicHidden[ni];
			m_arySoftlogicHidden[ni] = NULL;
		}

		arySoftlogicNew = NULL;
		if ( m_nSoftlogicHidden > 1 )
		{
			arySoftlogicNew = new CSoftlogic *[m_nSoftlogicHidden - 1];
			memcpy(arySoftlogicNew, m_arySoftlogicHidden, sizeof(CSoftlogic *) * ni);
			memcpy(arySoftlogicNew + ni, m_arySoftlogicHidden + ni + 1, sizeof(CSoftlogic *) * (m_nSoftlogicHidden - ni - 1));
		}
		m_nSoftlogicHidden--;
		delete [] m_arySoftlogicHidden;
		m_arySoftlogicHidden = arySoftlogicNew;

		for ( nj = ni; nj < m_nSoftlogicHidden; nj++ )
		{
            _replacePort((CSoftlogic::Port)(SOFT_LOGIC_MAX - nj - 1 + MM_SOFTLOGIC_PORT_SOFTLOGIC_START),
						 (CSoftlogic::Port)(SOFT_LOGIC_MAX - nj + MM_SOFTLOGIC_PORT_SOFTLOGIC_START));
		}
	}
}
