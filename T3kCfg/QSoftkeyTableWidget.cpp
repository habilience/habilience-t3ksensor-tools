#include "QSoftkeyTableWidget.h"

#include <QScrollBar>
#include <QHeaderView>

#include "ui/QHoverComboBox.h"
#include "QSoftkeyActionCellWidget.h"

QSoftkeyTableWidget::QSoftkeyTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    connect( this, SIGNAL(cellClicked(int,int)), this, SLOT(on_cellClicked(int,int)) );

    installEventFilter( this );
}

void QSoftkeyTableWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    horizontalHeaderItem( 0 )->setText( Res.getResString( QString::fromUtf8("SOFTKEY SETTING"), QString::fromUtf8("TEXT_HEADER_NUMBER") ) );
    horizontalHeaderItem( 1 )->setText( Res.getResString( QString::fromUtf8("SOFTKEY SETTING"), QString::fromUtf8("TEXT_HEADER_ACTION") ) );

    for( int i=0; i<rowCount(); i++ )
    {
        if( cellWidget( i, 1 )->inherits( "QHoverComboBox" ) )
        {
            QHoverComboBox* pHCB = (QHoverComboBox*)cellWidget( i, 1 );
            pHCB->setItemText( 0, Res.getResString( QString::fromUtf8("SOFTKEY SETTING"), QString::fromUtf8("TEXT_ITEM_NOT_USED") ) );
            pHCB->setItemText( 1, Res.getResString( QString::fromUtf8("SOFTKEY SETTING"), QString::fromUtf8("TEXT_ITEM_STATE") ) );
        }
    }
}

int QSoftkeyTableWidget::AddSoftkeyItem(int nAddCount)
{
    hide();
    for( int i=0; i<nAddCount; i++ )
    {
        int nRowIndex = rowCount();
        setRowCount( nRowIndex+1 );
        QTableWidgetItem* pItem = new QTableWidgetItem( QString("%1").arg(nRowIndex+1) );
        pItem->setTextAlignment( Qt::AlignCenter );
        setItem( nRowIndex, 0, pItem );
        QSoftkeyActionCellWidget* pCE = new QSoftkeyActionCellWidget( this, nRowIndex, 1 );
        pCE->setAttribute( Qt::WA_DeleteOnClose );
        pCE->setParent( this );
        setCellWidget( nRowIndex, 1, pCE );

        connect( ((QSoftkeyActionCellWidget*)cellWidget( nRowIndex, 1 )), SIGNAL(ItemChanged(QObject*,int)), this, SLOT(on_usr_ItemChanged(QObject*,int)) );
        connect( ((QSoftkeyActionCellWidget*)cellWidget( nRowIndex, 1 )), SIGNAL(KeyPressSignal(QObject*,ushort)), this, SLOT(onKeyPress_Signal(QObject*,ushort)) );
        connect( ((QSoftkeyActionCellWidget*)cellWidget( nRowIndex, 1 )), SIGNAL(MousekeyPress(QObject*,ushort)), this, SLOT(on_MousekeyPress(QObject*,ushort)) );
    }

    onChangeLanguage();

    show();
    return rowCount();
}

void QSoftkeyTableWidget::RemoveAllItem()       // Memory Leak
{
    hide();
    while( rowCount() )
    {
        setCellWidget( 0, 1, NULL );
        removeRow( 0 );                         // remove : leak ?
    }

    show();
}

void QSoftkeyTableWidget::on_cellClicked(int nRow, int nColum)
{
    if( nColum < 1 ) return;

    if( cellWidget( nRow, 1 )->inherits( "QSoftkeyActionCellWidget" ) )
    {
        QSoftkeyActionCellWidget* pWidget = (QSoftkeyActionCellWidget*)cellWidget( nRow, 1 );
        pWidget->on_Clicked_Show( pWidget->GetTypeIndex() );
    }
}

void QSoftkeyTableWidget::on_usr_ItemChanged(QObject* pObj, int nIndex)
{
    if( pObj->inherits( "QSoftkeyActionCellWidget" ) )
    {
        QSoftkeyActionCellWidget* pWidget = (QSoftkeyActionCellWidget*)pObj;
        ActionTypecellChanged( pWidget->GetRowIndex(), pWidget->GetColumnIndex(), nIndex );
    }
}

void QSoftkeyTableWidget::onKeyPress_Signal(QObject *pSender, ushort nValue)
{
    if( pSender->inherits( "QSoftkeyActionCellWidget") )
    {
        QSoftkeyActionCellWidget* pSoftkeyCellWidget = (QSoftkeyActionCellWidget*)pSender;
        int nRow = pSoftkeyCellWidget->GetRowIndex();
        KeyPressSignal( nRow, 1, nValue );
    }
}

void QSoftkeyTableWidget::on_MousekeyPress(QObject *pSender, ushort nValue)
{
    if( pSender->inherits( "QSoftkeyActionCellWidget") )
    {
        QSoftkeyActionCellWidget* pSoftkeyCellWidget = (QSoftkeyActionCellWidget*)pSender;
        int nRow = pSoftkeyCellWidget->GetRowIndex();
        MousekeyPress( nRow, 1, nValue );
    }
}
