#ifndef QFULLSCREENDIALOGT_H
#define QFULLSCREENDIALOGT_H

#include <QDialog>

class QFullScreenDialogT : public QDialog
{
    Q_OBJECT
public:
    explicit QFullScreenDialogT(QWidget *parent = 0);

    inline void setGeometry(int x, int y, int w, int h);
    void setGeometry(const QRect&);

    void move(int x, int y);
    void move(const QPoint &);

signals:

public slots:

};

inline void QFullScreenDialogT::setGeometry(int ax, int ay, int aw, int ah)
{ setGeometry(QRect(ax, ay, aw, ah)); }

#endif // QFULLSCREENDIALOGT_H
