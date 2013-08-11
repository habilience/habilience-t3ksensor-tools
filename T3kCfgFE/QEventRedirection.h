#ifndef QEVENTREDIRECTION_H
#define QEVENTREDIRECTION_H

#include <QObject>

#ifndef UNUSE_ARG
#define UNUSE_ARG(xxx) xxx=xxx
#endif

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QEventRedirection : public QObject
{
    Q_OBJECT
public:
    class IEventListener
    {
    public:
        virtual bool onKeyPress( QKeyEvent* evt ) { UNUSE_ARG(evt); return false; }
        virtual bool onKeyRelease( QKeyEvent* evt ) { UNUSE_ARG(evt); return false; }

        virtual void onRButtonDblClicked() {}
        virtual void onRButtonClicked() {}
        virtual bool onMouseWheel( QWheelEvent* evt ) { UNUSE_ARG(evt); return false; }
    };
private:
    IEventListener* m_pListener;
    int     m_TimerSingleClickCheck;
    bool    m_bRButtonDblClicked;
protected:
    virtual bool eventFilter(QObject *obj, QEvent *evt);
    virtual void timerEvent(QTimerEvent *);
public:
    explicit QEventRedirection(QObject *parent = 0);
    void installEventListener( IEventListener* l ) { m_pListener = l; }
    
signals:
    
public slots:
    
};

#endif // QEVENTREDIRECTION_H
