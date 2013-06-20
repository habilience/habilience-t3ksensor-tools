#ifndef QPROFILELABEL_H
#define QPROFILELABEL_H

#include <QString>
#include <QVector>

class QProfileLabel
{
public:
    QProfileLabel();

    void Default();
    void Load();
    void Save();

    const QString GetLabel( int nProfileIndex );
    void SetLabel( int nProfileIndex, QString& strNewLabel );
protected:
    QVector<QString>        m_vProfileLabels;
};

#endif // QPROFILELABEL_H
