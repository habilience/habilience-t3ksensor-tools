#ifndef QFILENAMEEDIT_H
#define QFILENAMEEDIT_H

#include <QTextEdit>

class QFileNameEdit : public QTextEdit
{
protected:
    virtual void keyPressEvent(QKeyEvent *e);
public:
    explicit QFileNameEdit(QWidget *parent = 0);
};

#endif // QFILENAMEEDIT_H
