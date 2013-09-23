#ifndef QINI_H
#define QINI_H

#include <QString>
#include <QFile>

class QIni
{
public:
    QIni();
    ~QIni();

    void close();
    bool load( const QString& strIni );
    bool load( const char* pResBuffer, unsigned long dwSize );
    bool load( QFile & fileIni );
    bool save( const QString& strIni );
    bool save( QFile & fileIni );

protected:
    typedef struct tagLOAD
    {
        bool bSectionTailNotes;
    } LOAD;
    class QData
    {
    public:
        QString m_strEntry;
        QString m_strData;
        QString m_strNotes;
    };
public:
    class QSection
    {
    public:
        QSection( const QString& strName = "" );
        ~QSection();
    private:
        QSection * m_pPrev;
        QSection * m_pNext;
    protected:
        QSection * getPrev() { return m_pPrev; }
        QSection * getNext() { return m_pNext; }
        void addToNext( QSection * pSection )
        {
            Q_ASSERT( pSection );
            QSection * pNext = getNext();
            QSection * pSrcEnd = pSection->getPrev();

            m_pNext = pSection;
            pNext->m_pPrev = pSrcEnd;
            pSection->m_pPrev = this;
            pSrcEnd->m_pNext = pNext;
        }
        void cutThis()
        {
            QSection * pPrev = getPrev();
            QSection * pNext = getNext();

            pPrev->m_pNext = pNext;
            pNext->m_pPrev = pPrev;
            m_pPrev = m_pNext = this;
        }
    private:
        QString m_strName;
        QString m_strNotesHead;
        QString m_strNotesTail;
        QData ** m_ppData;
        int      m_nData;
        void onReadString( QString strLine, LOAD * pLoad );
        static QString addNotesMark( const QString& strNotes );
    protected:
        void clear();
    public:
        void save( QFile & fileIni );
        QString getNotesHead() { return m_strNotesHead; }
        void setNotesHead( const QString& strNotes );
        QString getNotesTail() { return m_strNotesTail; }
        void setNotesTail( const QString& strNotes );
        QString getName() { return m_strName; }
        int getDataCount() { return m_nData; }
        QString getEntry( int idx ) { Q_ASSERT(idx < m_nData); return m_ppData[idx]->m_strEntry; }
        QString getData( int idx ) { Q_ASSERT(idx < m_nData); return m_ppData[idx]->m_strData; }
        QString getNotes( int idx ) { Q_ASSERT(idx < m_nData); return m_ppData[idx]->m_strNotes; }
        int getDataIndex( const QString& strEntry, int nStart = -1 );
        int getDataIndexNoCase( QString& strEntry, int nStart = -1 );
        int addData( const QString& strEntry, const QString& strData, const QString& strNotes = "" );
        QString setEntry( int idx, const QString& strEntry );
        QString setData( int idx, const QString& strData );
        QString setNotes( int idx, const QString& strNotes );
        void removeData( int idx );
        friend class QIni;
    };
protected:
    QSection	m_SectionRoot;

public:
    QSection * getLastSection()
        { return m_SectionRoot.getPrev(); }
    QSection * addSection( const QString& strName );
    QSection * getSection( const QString& strSection );
    QSection * getSectionNoCase( QString strSection );

private:
    void onReadString( QString strLine, LOAD * pLoad );
    static void writeString( QFile & fileIni, const QString& strLine );

protected:
};

#endif // QINI_H
