#include "./QFullScreenDialogT.h"

#include <QDesktopWidget>

QFullScreenDialogT::QFullScreenDialogT(QWidget *parent) :
    QDialog(parent)
{
}

int checkBugfixYOffsetForMac()
{
    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );
    int nShiftY = rcPrimaryMon.height();
    for( int i=0; i<DeskWidget.screenCount(); i++ )
    {
        if( nPrimary == i ) continue;
        if( nShiftY < DeskWidget.screenGeometry(i).height() )
            nShiftY = DeskWidget.screenGeometry(i).height();
    }
    nShiftY -= rcPrimaryMon.height();
    if( nShiftY < 0 )
        nShiftY = 0;

    return nShiftY;
}

void QFullScreenDialogT::setGeometry(const QRect& rc)
{
    QDialog::setGeometry( rc );

#ifdef Q_OS_MAC
    QDialog::move( x(), checkBugfixYOffsetForMac() );
#endif
}

void QFullScreenDialogT::move(int ax, int ay)
{
    QDialog::move( ax, ay );

#ifdef Q_OS_MAC
    QDialog::move( x(), checkBugfixYOffsetForMac() );
#endif
}

void QFullScreenDialogT::move(const QPoint &pt)
{
    QDialog::move( pt );

#ifdef Q_OS_MAC
    QDialog::move( x(), checkBugfixYOffsetForMac() );
#endif
}
