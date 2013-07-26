#ifndef QGRAPHSENSITIVITYCTRL_H
#define QGRAPHSENSITIVITYCTRL_H

#include <QLabel>

#define MAX_COLOR_LEVEL     (3)
class QGraphSensitivityCtrl : public QLabel
{
    Q_OBJECT
private:
    int     m_nColorLevel;
    int     m_nStepCount;
    int     m_nStep;
protected:
    virtual void paintEvent(QPaintEvent *);

    void drawItem(QPainter& p, QRectF rcItem, int nLevel);
public:
    explicit QGraphSensitivityCtrl(QWidget *parent = 0);

    void setColorLevel( int level );
    void setStep( int step );
    void setRange( int max );
    int getRange() const;
signals:
    
public slots:
    
};

#endif // QGRAPHSENSITIVITYCTRL_H
