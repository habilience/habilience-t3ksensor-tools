#ifndef QWIDGETCLOSEEVENTMANAGER_H
#define QWIDGETCLOSEEVENTMANAGER_H

#include <QMap>
#include <QWidget>
#include <QTimer>

class QWidgetCloseEventManager : public QObject
{
public:
    QWidgetCloseEventManager( QObject* pParent = NULL );
    ~QWidgetCloseEventManager();

    class _GC
    {
    public:
        _GC();
        ~_GC();
    };
    friend class _GC;

    static QWidgetCloseEventManager* GetPtr();

    bool AddClosedWidget( QWidget* pWidget, int nTime );

protected:
    virtual void timerEvent(QTimerEvent *evt);

protected:
    QMap<int, QWidget*>         m_mapClosedWidget;

private:
    static QWidgetCloseEventManager* s_pThis;
};

#endif // QWIDGETCLOSEEVENTMANAGER_H
