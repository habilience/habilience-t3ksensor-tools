#ifndef QSENSORSENSGAUGEWIDGET_H
#define QSENSORSENSGAUGEWIDGET_H

#include <QWidget>

#define MAX_COLOR_LEVEL		(3)

class QSensorSensGaugeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSensorSensGaugeWidget(QWidget *parent = 0);

    void SetColorLevel( int nColorLevel )
    {
        nColorLevel < 0 ? nColorLevel = 0 : 0;
        nColorLevel > MAX_COLOR_LEVEL ? nColorLevel = MAX_COLOR_LEVEL : 0;
        m_nColorLevel = nColorLevel;
    }
    void SetStep( int nStep );
    void SetRange( int nMaxStep ) { m_nStepCount = nMaxStep; }
    int GetRange() { return m_nStepCount; }

protected:
    void DrawItem( QPainter& pDC, QRectF rectItem, int nLevel );
    QPainterPath CreateRoundRect( const QRect& rect, int nRadius );
    QPainterPath CreateRoundRect( const QRectF& rect, int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB );

    virtual void paintEvent(QPaintEvent *evt);

protected:
    int		m_nColorLevel;

    int		m_nStepCount;
    int		m_nStep;
};

#endif // QSENSORSENSGAUGEWIDGET_H
