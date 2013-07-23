#include "QFileNameEdit.h"
#include <QKeyEvent>

QFileNameEdit::QFileNameEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void QFileNameEdit::keyPressEvent(QKeyEvent *e)
{
    QString strBlockString = "\\/:*?\"<>|";
    if (e->type() == QEvent::KeyPress)
    {
        int nKey = e->key();
        if ( nKey == Qt::Key_Back )
            return QTextEdit::keyPressEvent(e);
        if (strBlockString.indexOf(char(nKey)) >= 0)
            return;
    }

    QTextEdit::keyPressEvent(e);
}
