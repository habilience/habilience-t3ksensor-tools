#include "QIni.h"

#include <QFile>
#include "QUtils.h"


QIni::QIni()
{
}

QIni::~QIni()
{
    close();
}

void QIni::close()
{
    QSection * pSection;
    while ( (pSection = getLastSection()) != &m_SectionRoot )
    {
        delete pSection;
    }
    m_SectionRoot.clear();
}

bool QIni::load( const QString& strIni )
{
    QFile file(strIni);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    return load(file);
}

bool QIni::save( const QString& strIni )
{
    QFile file(strIni);
    if (file.exists())
        file.remove();
    if (!file.open(QIODevice::WriteOnly))
        return false;

    return save(file);
}


#define INI_LOADBUFFER_UNIT  100

bool QIni::load( const char* pResBuffer, unsigned long dwSize )
{
    LOAD Load;
    memset(&Load, 0, sizeof(LOAD));

    QString strBufLine;

    // check UTF-8
    char* pCheckHeader;
    pCheckHeader = (char*)pResBuffer;
    {
        const char* UTF8Header = "\xEF\xBB\xBF";
        if( pCheckHeader[0] == UTF8Header[0] &&
            pCheckHeader[1] == UTF8Header[1] &&
            pCheckHeader[2] == UTF8Header[2] )
        {
            //UTF-8
            pResBuffer += 3;
        }
    }

    char* pReadBuffer = (char*)pResBuffer;
    int nBufLineSize = INI_LOADBUFFER_UNIT;

    char * pBufLine = new char[nBufLineSize];
    char * pBufLineCur = pBufLine;
    while ( true )
    {
        if( pReadBuffer+sizeof(char) > (pResBuffer+dwSize) )
            break;
        memcpy( pBufLineCur, pReadBuffer, sizeof(char) );
        pReadBuffer += sizeof(char);

        pBufLineCur++;
        if ( (pBufLineCur - pBufLine) >= nBufLineSize )
        {
            // increase the size of a buffer
            char * pBufLineNew = new char[nBufLineSize + INI_LOADBUFFER_UNIT];
            memcpy(pBufLineNew, pBufLine, nBufLineSize * sizeof(char));
            delete [] pBufLine;
            pBufLineCur = pBufLineNew + (pBufLineCur - pBufLine);
            pBufLine = pBufLineNew;
            nBufLineSize += INI_LOADBUFFER_UNIT;
        }

        if ( pBufLineCur[-1] == '\n' )
        {
            pBufLineCur[0] = '\0';

            strBufLine = QString::fromUtf8( pBufLine );

            onReadString(strBufLine, &Load);
            pBufLineCur = pBufLine;
        }
    }
    if ( pBufLineCur != pBufLine )
    {
        pBufLineCur[0] = '\0';

        strBufLine = QString::fromUtf8( pBufLine );

        onReadString(strBufLine, &Load);
    }
    delete [] pBufLine;

    QSection * pLastSection = getLastSection();
    Q_ASSERT( pLastSection );
    pLastSection->m_strNotesTail = rstrip( pLastSection->m_strNotesTail, "\r\n" );

    if ( !pLastSection->m_strNotesTail.isEmpty() )
        pLastSection->m_strNotesTail += "\r\n";

    return true;
}

bool QIni::load( QFile & fileIni )
{
    LOAD Load;
    memset(&Load, 0, sizeof(LOAD));

    QString strBufLine;

    char pCheckHeader[3];
    if( fileIni.read( pCheckHeader, 3 ) )
    {
        const char* UTF8Header = "\xEF\xBB\xBF";
        if( pCheckHeader[0] == UTF8Header[0] &&
            pCheckHeader[1] == UTF8Header[1] &&
            pCheckHeader[2] == UTF8Header[2] )
        {
        }
        else
        {
            fileIni.seek( 0 );
        }
    }

    int nBufLineSize = INI_LOADBUFFER_UNIT;

    char * pBufLine = new char[nBufLineSize];
    char * pBufLineCur = pBufLine;
    while ( true )
    {
        if ( fileIni.read(pBufLineCur, sizeof(char)) != sizeof(char) )
            break;
        pBufLineCur++;
        if ( (pBufLineCur - pBufLine) >= nBufLineSize )
        {
            // increase the size of a buffer
            char * pBufLineNew = new char[nBufLineSize + INI_LOADBUFFER_UNIT];
            memcpy(pBufLineNew, pBufLine, nBufLineSize * sizeof(char));
            delete [] pBufLine;
            pBufLineCur = pBufLineNew + (pBufLineCur - pBufLine);
            pBufLine = pBufLineNew;
            nBufLineSize += INI_LOADBUFFER_UNIT;
        }

        if ( pBufLineCur[-1] == '\n' )
        {
            pBufLineCur[0] = '\0';

            strBufLine = QString::fromUtf8( pBufLine );

            onReadString(strBufLine, &Load);
            pBufLineCur = pBufLine;
        }
    }
    if ( pBufLineCur != pBufLine )
    {
        pBufLineCur[0] = '\0';

        strBufLine = QString::fromUtf8( pBufLine );

        onReadString(strBufLine, &Load);
        pBufLineCur = pBufLine;
    }
    delete [] pBufLine;

    QSection * pLastSection = getLastSection();
    Q_ASSERT( pLastSection );
    pLastSection->m_strNotesTail = rstrip( pLastSection->m_strNotesTail, "\r\n" );
    if ( !pLastSection->m_strNotesTail.isEmpty() )
        pLastSection->m_strNotesTail += "\r\n";

    return true;
}

void QIni::onReadString( QString strLine, LOAD * pLoad )
{
    QSection * pLastSection = getLastSection();
    Q_ASSERT( pLastSection );

    QString strLineOrg = strLine;
    strLine = trim(strLine);
    if ( strLine.size() > 0 && strLine.at(0).toLatin1() == '[' && strLine.at(strLine.size() - 1).toLatin1() == ']' )
    {
        pLastSection->m_strNotesTail = rstrip( pLastSection->m_strNotesTail, "\r\n" );
        if ( !pLastSection->m_strNotesTail.isEmpty() )
            pLastSection->m_strNotesTail += "\r\n";

        // new section
        pLoad->bSectionTailNotes = false;
        addSection(strLine.mid(1, strLine.size() - 2));
        return;
    }

    pLastSection->onReadString(strLineOrg, pLoad);
}

QIni::QSection * QIni::addSection( const QString& strName )
{
    Q_ASSERT( getSection(strName) == NULL );

    QSection * pNewSection = new QSection(strName);

    QSection * pLastSection = getLastSection();
    Q_ASSERT( pLastSection );

    pLastSection->addToNext(pNewSection);

    return pNewSection;
}

QIni::QSection * QIni::getSection( const QString& strSection )
{
    if ( strSection.isEmpty() )
        return &m_SectionRoot;

    QSection * pSection = m_SectionRoot.getNext();
    while ( pSection != &m_SectionRoot )
    {
        if ( pSection->getName() == strSection )
            return pSection;
        pSection = pSection->getNext();
    }
    return NULL;
}

QIni::QSection * QIni::getSectionNoCase( QString strSection )
{
    strSection = strSection.toLower();

    QSection * pSection = m_SectionRoot.getNext();
    while ( pSection != &m_SectionRoot )
    {
        QString strName = pSection->getName();
        strName = strName.toLower();
        if ( strName == strSection )
            return pSection;
        pSection = pSection->getNext();
    }
    return NULL;
}

bool QIni::save( QFile & fileIni )
{
    // head of notes of a root section
    QString strWrite = m_SectionRoot.getNotesHead();
    if ( !strWrite.isEmpty() )
    {
        writeString(fileIni, strWrite);
    }

    if ( !strWrite.isEmpty() ||
        (m_SectionRoot.getDataCount() > 0 && !m_SectionRoot.getNotes(0).isEmpty()) )
    {
        strWrite = "\r\n";
        writeString(fileIni, strWrite);
    }

    // data of a root section
    for ( int ni = 0; ni < m_SectionRoot.getDataCount(); ni++ )
    {
        strWrite = m_SectionRoot.getNotes(ni);
        strWrite += m_SectionRoot.getEntry(ni);
        if ( !m_SectionRoot.getData(ni).isEmpty() )
            strWrite += " = " + m_SectionRoot.getData(ni) + "\r\n";
        writeString(fileIni, strWrite);
    }

    if ( m_SectionRoot.getDataCount() > 0 )
    {
        strWrite = "\r\n";
        writeString(fileIni, strWrite);
    }

    // tail of notes of a root section
    strWrite = m_SectionRoot.getNotesTail();
    if ( !strWrite.isEmpty() )
    {
        writeString(fileIni, strWrite);
        strWrite = "\r\n";
        writeString(fileIni, strWrite);
    }

    for ( QSection * pSection = m_SectionRoot.getNext();
        pSection != &m_SectionRoot;
        pSection = pSection->getNext() )
    {
        pSection->save(fileIni);
    }

    return true;
}

void QIni::writeString( QFile & fileIni, const QString& strLine )
{
    if ( strLine.isEmpty() )
        return;

    fileIni.write(strLine.toUtf8());
}


// QIni::QSection

QIni::QSection::QSection( const QString& strName )
{
    m_pPrev = m_pNext = this;

    m_strName = strName;

    m_ppData = NULL;
    m_nData = 0;
}

QIni::QSection::~QSection()
{
    cutThis();
    clear();
}

void QIni::QSection::clear()
{
    m_strName = "";
    m_strNotesHead = "";
    m_strNotesTail = "";

    for ( int ni = 0; ni < m_nData; ni++ )
    {
        delete m_ppData[ni];
    }
    if ( m_ppData )
    {
        delete [] m_ppData;
        m_ppData = NULL;
    }
    m_nData = 0;
}

void QIni::QSection::onReadString( QString strLine, LOAD * pLoad )
{
    strLine = trim( strLine );

    if ( strLine.isEmpty() )
    {
        if ( !m_strNotesTail.isEmpty() )
        {
            if ( m_nData == 0 )
            {
                m_strNotesHead += "\r\n" + m_strNotesTail;
                m_strNotesTail = "";
            }
            else
            {
                m_strNotesTail += "\r\n";
            }
        }
        pLoad->bSectionTailNotes = true;
        return;
    }

    if ( strLine.size() > 0 && strLine.at(0) == ';' )
    {
        if ( pLoad->bSectionTailNotes )
            m_strNotesTail += strLine + "\r\n";
        else
            m_strNotesHead += strLine + "\r\n";
        return;
    }

    pLoad->bSectionTailNotes = true;

    QData * pDataNew = new QData;
    Q_ASSERT( pDataNew );

    // data
    int nD = strLine.indexOf('=');
    if ( nD > 0 )
    {
        pDataNew->m_strEntry = strLine.left(nD);
        pDataNew->m_strEntry = rstrip(pDataNew->m_strEntry, " ");
        pDataNew->m_strData = strLine.mid(nD + 1);
        pDataNew->m_strData = lstrip(pDataNew->m_strData, " ");
    }
    else
    {
        pDataNew->m_strEntry = strLine;
    }
    if ( !m_strNotesTail.isEmpty() )
    {
        pDataNew->m_strNotes = m_strNotesTail;
        m_strNotesTail = "";
    }

    QData ** ppDataNew = new QData *[m_nData + 1];
    memcpy(ppDataNew, m_ppData, sizeof(QData *) * m_nData);
    ppDataNew[m_nData] = pDataNew;
    if ( m_ppData )
        delete [] m_ppData;
    m_ppData = ppDataNew;
    m_nData++;
}

int QIni::QSection::getDataIndex( const QString& strEntry, int nStart )
{
    if ( nStart < 0 ) nStart = -1;

    for ( int ni = nStart + 1; ni < m_nData; ni++ )
    {
        if ( strEntry.isEmpty() || m_ppData[ni]->m_strEntry == strEntry )
            return ni;
    }
    return -1;
}

int QIni::QSection::getDataIndexNoCase( QString& strEntry, int nStart )
{
    strEntry = strEntry.toLower();

    if ( nStart < 0 ) nStart = -1;

    for ( int ni = nStart + 1; ni < m_nData; ni++ )
    {
        QString strEntryTmp = m_ppData[ni]->m_strEntry;
        strEntryTmp = strEntryTmp.toLower();
        if ( strEntry.isEmpty() || strEntryTmp == strEntry )
            return ni;
    }
    return -1;
}

int QIni::QSection::addData( const QString& strEntry, const QString& strData, const QString& strNotes )
{
    QData * pDataNew = new QData;

    QData ** ppDataNew = new QData *[m_nData + 1];
    memcpy(ppDataNew, m_ppData, sizeof(QData *) * m_nData);
    ppDataNew[m_nData] = pDataNew;
    if ( m_ppData )
        delete [] m_ppData;
    m_ppData = ppDataNew;

    int nData = m_nData++;
    setEntry(nData, strEntry);
    setData(nData, strData);
    setNotes(nData, strNotes);
    return nData;
}

QString QIni::QSection::setEntry( int idx, const QString& strEntry )
{
    Q_ASSERT( idx < m_nData );

    QString strRet = m_ppData[idx]->m_strEntry;
    m_ppData[idx]->m_strEntry = strEntry;
    m_ppData[idx]->m_strEntry = trim(m_ppData[idx]->m_strEntry);

    return strRet;
}

QString QIni::QSection::setData( int idx, const QString& strData )
{
    Q_ASSERT( idx < m_nData );

    QString strRet = m_ppData[idx]->m_strData;
    m_ppData[idx]->m_strData = strData;
    m_ppData[idx]->m_strData = trim(m_ppData[idx]->m_strData);

    return strRet;
}

QString QIni::QSection::addNotesMark( const QString& strNote )
{
    QString strNotes = strNote;
    QString strNotesOut;
    while ( !strNotes.isEmpty() )
    {
        QString strLine;
        int nD = strNotes.indexOf('\n');
        if ( nD > 0 )
        {
            strLine = strNotes.left(nD);
            strNotes = strNotes.mid(nD + 1);
        }
        else
        {
            strLine = strNotes;
            strNotes = "";
        }

        strLine = rstrip(strLine, " ");
        if ( strLine.size() <= 0 || strLine.at(0).toLatin1() != ';' )
            strLine = ';' + strLine + "\r\n";
        strNotesOut += strLine;
    }

    return strNotesOut;
}

QString QIni::QSection::setNotes( int idx, const QString& strNotes )
{
    Q_ASSERT( idx < m_nData );

    QString strRet = m_ppData[idx]->m_strNotes;

    m_ppData[idx]->m_strNotes = addNotesMark(strNotes);

    return strRet;
}

void QIni::QSection::removeData( int idx )
{
    Q_ASSERT( idx < m_nData );

    QData ** ppDataNew = NULL;
    if ( m_nData > 1 )
    {
        ppDataNew = new QData *[m_nData - 1];
        memcpy(ppDataNew, m_ppData, sizeof(QData *) * idx);
        memcpy(ppDataNew + idx, m_ppData + idx + 1, sizeof(QData *) * (m_nData - idx - 1));
    }
    if ( m_ppData )
        delete [] m_ppData;
    m_ppData = ppDataNew;
    m_nData--;
}

void QIni::QSection::setNotesHead( const QString& strNotes )
{
    m_strNotesHead = addNotesMark(strNotes);
}

void QIni::QSection::setNotesTail( const QString& strNotes )
{
    m_strNotesTail = addNotesMark(strNotes);
}

void QIni::QSection::save( QFile & fileIni )
{
    QString strWrite;

    // section name
    strWrite = "[" + getName() + "]\r\n";
    writeString(fileIni, strWrite);

    // head of notes
    strWrite = getNotesHead();
    if ( !strWrite.isEmpty() )
    {
        writeString(fileIni, strWrite);
    }

    if ( !strWrite.isEmpty() ||
        (getDataCount() > 0 && !getNotes(0).isEmpty()) )
    {
        strWrite = "\r\n";
        writeString(fileIni, strWrite);
    }

    for ( int ni = 0; ni < getDataCount(); ni++ )
    {
        strWrite = getNotes(ni);
        strWrite += getEntry(ni);
        if ( !getData(ni).isEmpty() )
            strWrite += " = " + getData(ni) + "\r\n";
        writeString(fileIni, strWrite);
    }

    // tail of notes
    strWrite = getNotesTail();
    if ( !strWrite.isEmpty() )
    {
        writeString(fileIni, "\r\n" + strWrite);
    }

    strWrite = "\r\n";
    writeString(fileIni, strWrite);
}
