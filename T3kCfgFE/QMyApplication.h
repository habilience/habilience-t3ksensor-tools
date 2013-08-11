#ifndef QMYAPPLICATION_H
#define QMYAPPLICATION_H

#include <QApplication>

class QMyApplication : public QApplication
{
    Q_OBJECT

private:
    bool    m_bWatchMouseMovement;

protected:
    virtual bool eventFilter(QObject *, QEvent *);
public:
    explicit QMyApplication(int &argc, char **argv);

    void setMonitoringMouseMovement( bool bMonitoring );
    
signals:
    void mouseMoved();
public slots:
    
};

extern QMyApplication* g_pApp;

#endif // QMYAPPLICATION_H
