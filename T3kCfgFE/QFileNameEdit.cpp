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
        if ( nKey == Qt::Key_Backspace )
            return QTextEdit::keyPressEvent(e);
        QString strChar = e->text();
        if (strBlockString.indexOf(strChar) >= 0)
            return;
    }

    QTextEdit::keyPressEvent(e);
}
