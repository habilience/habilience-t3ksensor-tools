#include "QBorderStyleEdit.h"
#include <QTimerEvent>
#include <QKeyEvent>
#include <float.h>
#include <QPainter>

QBorderStyleEdit::QBorderStyleEdit(QWidget *parent) :
    QTextEdit(parent)
{
    m_TimerWarning = 0;

    m_bNowEditing = false;
    m_nMaxTextLength = -1;

    m_dValueRangeMin = -DBL_MAX;
    m_dValueRangeMax = DBL_MAX;
    m_strAllowChars = "0123456789.-";
    m_bIsFloatStyle = true;

    m_bIsBlink = false;
    m_bIsWarning = false;

    m_clrActiveText = QColor(255, 0, 0);
    m_clrText = QColor(0,0,0);
    m_clrBorder = QColor(143,143,143);
    m_clrBackground = Qt::white;

    m_alignText = Qt::AlignLeft;

    QString strValue = toPlainText();
    if (strValue.isEmpty())
    {
        if (m_bIsFloatStyle)
        {
            setText("0.0");
            strValue = "0.0";
        }
        else
        {
            setText("0");
            strValue = "0";
        }
    }

    setStyleSheet(getPreferStyleSheet());
}

void QBorderStyleEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        if (!m_strAllowChars.isEmpty())
        {
            int nKey = e->key();
            if (nKey == Qt::Key_Backspace)
                return QTextEdit::keyPressEvent(e);

            QString strChar = e->text();
            if (m_strAllowChars.indexOf(strChar) < 0)
                return;

            if (m_nMaxTextLength >= 0)
            {
                QTextCursor c = textCursor();
                int nD = 0;
                if (c.hasSelection())
                {
                    QString strSel = c.selectedText();
                    nD = strSel.length();
                }
                QString strCurrent = toPlainText();
                if ((strCurrent.length()-nD) >= m_nMaxTextLength)
                    return;
            }
        }
    }

    QTextEdit::keyPressEvent(e);
}

QString QBorderStyleEdit::getPreferStyleSheet()
{
    QString strStyle;

    QColor clrText;
    if (m_bNowEditing)
    {
        clrText = m_clrActiveText;
    }
    else
    {
        clrText = m_clrText;
    }

    QColor clrBorder;
    if (m_bIsWarning && m_bIsBlink)
        clrBorder = QColor(255, 0, 0);
    else
        clrBorder = m_clrBorder;

    strStyle =
    "QTextEdit {"
        "border: 2px solid rgb("+QString::number(clrBorder.red())+","+QString::number(clrBorder.green())+","+QString::number(clrBorder.blue())+");"
        "background-color : rgb("+QString::number(m_clrBackground.red())+","+QString::number(m_clrBackground.green())+","+QString::number(m_clrBackground.blue())+");"
        "color : rgb("+QString::number(clrText.red())+","+QString::number(clrText.green())+","+QString::number(clrText.blue())+");"
        "border-radius: 5px;"
    "}"

    "QTextEdit:disabled {"
        "border: 2px solid rgb(143,143,143);"
        "background-color : rgb(240,240,240);"
        "color : gray;"
        "border-radius: 5px;"
    "}";

    return strStyle;
}

void QBorderStyleEdit::focusInEvent(QFocusEvent *e)
{
    QTextEdit::focusInEvent(e);

    m_bNowEditing = true;

    //selectAll();

    QString strValue = toPlainText();
    if (strValue.isEmpty())
    {
        if (m_bIsFloatStyle)
        {
            setText("0.0");
            strValue = "0.0";
        }
        else
        {
            setText("0");
            strValue = "0";
        }
    }
    m_dOldValue = strValue.toDouble();

    setStyleSheet(getPreferStyleSheet());
    //update();
}

void QBorderStyleEdit::focusOutEvent(QFocusEvent *e)
{
    QTextEdit::focusOutEvent(e);

    QString strValue = toPlainText();
    if (strValue.isEmpty())
    {
        if (m_bIsFloatStyle)
        {
            setText("0.0");
            strValue = "0.0";
        }
        else
        {
            setText("0");
            strValue = "0";
        }
    }

    double dValue = strValue.toDouble();
    if (dValue < m_dValueRangeMin) dValue = m_dValueRangeMin;
    if (dValue > m_dValueRangeMax) dValue = m_dValueRangeMax;

    if (m_bIsFloatStyle)
    {
        char szTemp[64];
        snprintf( szTemp, 64, "%.1lf", dValue );
        strValue = szTemp;
    }
    else
    {
        strValue.setNum((int)dValue);
    }

    m_bNowEditing = false;

    setStyleSheet(getPreferStyleSheet());
    //update();

    QTextCursor c = textCursor();
    c.setPosition(0);
    setTextCursor(c);

    if (m_dOldValue != dValue)
    {
        int nValue = 0;
        if (!m_bIsFloatStyle)
        {
            nValue = (int)dValue;
            dValue = 0.0;
        }

        emit editModified(this, nValue, dValue);
    }
}

void QBorderStyleEdit::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_TimerWarning)
    {
        m_bIsBlink = !m_bIsBlink;
        setStyleSheet(getPreferStyleSheet());
        //update();
    }
}

void QBorderStyleEdit::setColor( const QColor& clrBorder, const QColor& clrBackground )
{
    m_clrBorder = clrBorder;
    m_clrBackground = clrBackground;

    setStyleSheet(getPreferStyleSheet());
}

void QBorderStyleEdit::setFloatStyle( bool bFloatStyle )
{
    m_bIsFloatStyle = bFloatStyle;
    if( bFloatStyle )
        m_strAllowChars = "0123456789.-";
    else
        m_strAllowChars = "0123456789-";
}

void QBorderStyleEdit::setWarning( bool bWarning )
{
    m_bIsWarning = bWarning;
    if (m_bIsWarning)
    {
        if (m_TimerWarning)
            killTimer(m_TimerWarning);
        m_TimerWarning = startTimer(500);
    }
    else
    {
        if (m_TimerWarning)
            killTimer(m_TimerWarning);
        m_TimerWarning = 0;
    }
}

void QBorderStyleEdit::setAlignment(Qt::Alignment a)
{
    m_alignText = a;
    QTextEdit::setAlignment(m_alignText);
}

void QBorderStyleEdit::setText(const QString &text)
{
    QTextEdit::setText(text);

    QTextEdit::setAlignment(m_alignText);
}
