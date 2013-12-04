#ifndef QSINGLETONE_H
#define QSINGLETONE_H

#include <stdlib.h>
#include <qglobal.h>

template <typename T> class QSingleton
{
private:
    static T* m_pSingleton;
protected:
    QSingleton()
    {
        Q_ASSERT( !m_pSingleton );
        qint64 offset = (qint64)(T*)1 - (qint64)(QSingleton<T>*)(T*)1;
        m_pSingleton = (T*)( (qint64)this + offset );
        qDebug( "create singleton: %p", m_pSingleton );
        atexit( deleteInstance );
    }
    ~QSingleton()
    {
        qDebug( "destroy singleton: %p", m_pSingleton );
        Q_ASSERT( m_pSingleton );
        m_pSingleton = NULL;
    }
public:
    static T* instance( void )
    {
        if ( !m_pSingleton )
            m_pSingleton = new T;
        Q_ASSERT( m_pSingleton );
        return m_pSingleton;
    }
protected:
    static void deleteInstance()
    {
        if (m_pSingleton)
            delete m_pSingleton;
    }
};

template <typename T> T* QSingleton<T>::m_pSingleton = NULL;

#endif // QSINGLETONE_H
