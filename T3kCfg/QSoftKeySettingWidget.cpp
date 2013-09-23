#include "QSoftKeySettingWidget.h"
#include "ui_QSoftKeySettingWidget.h"

#include <QScrollBar>
#include <QMessageBox>
#include "QKeyEditWidget.h"

#include "QKeyMapStr.h"
#include <QPalette>
#include <QPixmap>

QSoftKeySettingWidget::QSoftKeySettingWidget(T3kHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QSoftKeySettingWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    m_bInit = false;
    m_bDefault = false;
    m_bSave = false;

    m_bLoad = false;

    m_bUpdateList = false;

    m_nAddCount = 0;

    setFixedSize( width(), height() );

    ui->TitleSotfKey->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_SOFTKEY" );

    connect( ui->TableSoftkeyMap, SIGNAL(ActionTypecellChanged(int,int,int)), this, SLOT(onslot_TableSoftkeyMap_cellChanged(int,int,int)) );
    connect( ui->TableSoftkeyMap, SIGNAL(KeyPressSignal(int,int,ushort)), this, SLOT(onTableSoftkeyMap_KeyPress_Signal(int,int,ushort)) );
    connect( ui->TableSoftkeyMap, SIGNAL(MousekeyPress(int,int,ushort)), this, SLOT(on_TableSoftkeyMap_MousekeyPress(int,int,ushort)) );

    ui->TableSoftkeyMap->verticalHeader()->setVisible( false );
    ui->TableSoftkeyMap->verticalScrollBar()->installEventFilter( this );

    ui->TableSoftkeyMap->setColumnCount( 2 );
    ui->TableSoftkeyMap->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Key" ) );
    ui->TableSoftkeyMap->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Action" ) );

    ui->TableSoftkeyMap->horizontalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->TableSoftkeyMap->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );

    int nVW = ui->TableSoftkeyMap->verticalHeader()->size().width();
    int nW = ui->TableSoftkeyMap->width() - nVW;
    ui->TableSoftkeyMap->setColumnWidth( 0, int(double(nW)/16. + .5) );
    ui->TableSoftkeyMap->setColumnWidth( 1, nW - ui->TableSoftkeyMap->columnWidth( 0 ) );

    onChangeLanguage();
}

QSoftKeySettingWidget::~QSoftKeySettingWidget()
{
    delete ui;
}

void QSoftKeySettingWidget::SetDefault()
{
    RequestSensorData( true );
}

void QSoftKeySettingWidget::Refresh()
{
    RequestSensorData( false );
}

void QSoftKeySettingWidget::showEvent(QShowEvent *evt)
{
    RequestSensorData( false );
    ui->TableSoftkeyMap->show();
    ui->TableSoftkeyMap->setEnabled( true );

    QWidget::showEvent(evt);
}

void QSoftKeySettingWidget::hideEvent(QHideEvent *evt)
{
    ui->TableSoftkeyMap->hide();
    ui->TableSoftkeyMap->setEnabled( false );

    QWidget::hideEvent(evt);
}

bool QSoftKeySettingWidget::eventFilter(QObject *target, QEvent *evt)
{
    if( evt->type() == QEvent::Show )
    {
        if ( target == ui->TableSoftkeyMap->verticalScrollBar() )
        {
            int nVW = ui->TableSoftkeyMap->verticalHeader()->size().width();
            int nVSW = ui->TableSoftkeyMap->verticalScrollBar()->width();

            int nW = ui->TableSoftkeyMap->width() - nVW - nVSW -1;
            ui->TableSoftkeyMap->setColumnWidth( 0, int(double(nW)/16. + .5) );
            ui->TableSoftkeyMap->setColumnWidth( 1, nW - ui->TableSoftkeyMap->columnWidth( 0 ) );
        }
    }

    if( evt->type() == QEvent::Hide )
    {
        if( target == ui->TableSoftkeyMap->verticalScrollBar() )
        {
            int nVW = ui->TableSoftkeyMap->verticalHeader()->size().width();
            int nW = ui->TableSoftkeyMap->width() - nVW;
            ui->TableSoftkeyMap->setColumnWidth( 0, int(double(nW)/16. + .5) );
            ui->TableSoftkeyMap->setColumnWidth( 1, nW - ui->TableSoftkeyMap->columnWidth( 0 ) );
        }
    }

    return QWidget::eventFilter(target, evt);
}

void QSoftKeySettingWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->TitleSotfKey->setText( Res.getResString( QString::fromUtf8("SOFTKEY SETTING"), QString::fromUtf8("TEXT_TITLE") ) );
}

void QSoftKeySettingWidget::OnRSP(ResponsePart /*Part*/, ushort /*nTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if( strstr( sCmd, cstrSoftkey ) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrSoftkey );

        int nAddCount = ParseSoftKey( sCmd );
        m_nAddCount = nAddCount;
        if( m_bDefault || m_bUpdateList )
        {
            UpdateListEnable();
        }
    }
    else if ( strstr( sCmd, cstrSoftlogic ) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrSoftlogic );    

        if( m_nAddCount )
        {
            ui->TableSoftkeyMap->setUpdatesEnabled( false );

            ParseSoftLogic( sCmd );

            ui->TableSoftkeyMap->setUpdatesEnabled( true );

            m_bLoad = false;
        }
        m_nAddCount = 0;
    }
}

int QSoftKeySettingWidget::ParseSoftKey(const char *psSoftKey)
{
    QString strSoftKey( psSoftKey );

    int nE = strSoftKey.indexOf( '=' );
    if( nE < 0 ) return 0;

    strSoftKey.remove( 0, nE+1 );

    if( !strSoftKey.size() || strSoftKey.at( 0 ) == '0' ) return 0;

    qDebug( "[ softkey ]");
    qDebug( "%s", (const char*)strSoftKey.toUtf8().data() );

    m_arySoftKey.Load( strSoftKey.toStdString().c_str(), NULL );
    int nSize = m_arySoftKey.GetShowSize();

    return nSize;
}

int QSoftKeySettingWidget::ParseSoftLogic(const char *psSoftLogic)
{
    QString strSoftLogic( psSoftLogic );

    int nE = strSoftLogic.indexOf( '=' );
    if( nE < 0 ) return 0;

    strSoftLogic.remove( 0, nE+1 );

    if( !strSoftLogic.size() ) return 0;

    qDebug( "[ softlogic ]" );
    qDebug( "%s", (const char*)strSoftLogic.toUtf8().data() );

    m_arySoftLogic.RemoveAll();

    QSoftlogicArray aryTemplogic;
    aryTemplogic.Load( strSoftLogic.toStdString().c_str(), NULL );

    if( !aryTemplogic.GetShowSize() ) return 0;
    for( int i=0; i<m_nAddCount; i++ )
    {
        int nLogicIndex = FindSoftlogic( aryTemplogic, ((QSoftlogic::Port)(int(QSoftlogic::epSoftkey0)+i)) );
        if( nLogicIndex < 0 )
            continue;

        switch( aryTemplogic[nLogicIndex]->getOutPortType() )
        {
        case QSoftlogic::eptError:
            break;
        case QSoftlogic::eptKey:
            m_arySoftLogic.AddSoftlogic_Key( aryTemplogic[nLogicIndex]->getLogicType(), aryTemplogic[nLogicIndex]->getIn1Not(),
                                             aryTemplogic[nLogicIndex]->getIn1Port(), aryTemplogic[nLogicIndex]->getIn2Not(),
                                             aryTemplogic[nLogicIndex]->getIn2Port(), aryTemplogic[nLogicIndex]->getOutKey1ConditionKeys(),
                                             aryTemplogic[nLogicIndex]->getOutKey1(), aryTemplogic[nLogicIndex]->getLogicTrigger(),
                                             false );
            break;
        case QSoftlogic::eptMouse:
            m_arySoftLogic.AddSoftlogic_Mouse( aryTemplogic[nLogicIndex]->getLogicType(), aryTemplogic[nLogicIndex]->getIn1Not(),
                                             aryTemplogic[nLogicIndex]->getIn1Port(), aryTemplogic[nLogicIndex]->getIn2Not(),
                                             aryTemplogic[nLogicIndex]->getIn2Port(), aryTemplogic[nLogicIndex]->getOutKey1ConditionKeys(),
                                             aryTemplogic[nLogicIndex]->getOutMouse(), aryTemplogic[nLogicIndex]->getLogicTrigger(),
                                             false );
            break;
        case QSoftlogic::eptState:
        case QSoftlogic::eptGPIO:
        case QSoftlogic::eptReport:
        case QSoftlogic::eptSoftkey:
        default:
            //Q_ASSERT( false );
            break;
        }
    }

    for( int nNumber=0; nNumber<m_nAddCount; nNumber++ )
    {
        int nLogicIndex = FindSoftlogic( m_arySoftLogic, ((QSoftlogic::Port)(int(QSoftlogic::epSoftkey0)+nNumber)) );//SoftkeyInputIndex( m_arySoftLogic[i]->getIn1Port() );

        QSoftkeyActionCellWidget* pCEWidget = ui->TableSoftkeyMap->cellWidget( nNumber, 1 )->inherits( "QSoftkeyActionCellWidget" ) ?
                                      (QSoftkeyActionCellWidget*)ui->TableSoftkeyMap->cellWidget( nNumber, 1 ) : NULL;
        if( !pCEWidget ) continue;

        if( nLogicIndex < 0 )
        {
            pCEWidget->SetWidgetMode( 0 );
            continue;
        }

        QSoftlogic::PortType eType = m_arySoftLogic[nLogicIndex]->getOutPortType();

        switch( eType )
        {
        case QSoftlogic::eptError:
            break;
        case QSoftlogic::eptKey:
            {
                pCEWidget->SetKeyAction( m_arySoftLogic[nLogicIndex]->getOutKey1ConditionKeys(), m_arySoftLogic[nLogicIndex]->getOutKey1() );
                if( m_arySoftLogic[nLogicIndex]->getOutKey1ConditionKeys() == 0 && m_arySoftLogic[nLogicIndex]->getOutKey1() == 0 )
                {
                    pCEWidget->SetWidgetMode( 0 );
                    break;
                }
                pCEWidget->SetWidgetMode( 1 );
            }
            break;
        case QSoftlogic::eptMouse:
            pCEWidget->SetMouseAction( m_arySoftLogic[nLogicIndex]->getOutKey1ConditionKeys(), m_arySoftLogic[nLogicIndex]->getOutMouse() );
            if( m_arySoftLogic[nLogicIndex]->getOutKey1ConditionKeys() == 0 && m_arySoftLogic[nLogicIndex]->getOutMouse() == 0 )
            {
                pCEWidget->SetWidgetMode( 0 );
                break;
            }
            pCEWidget->SetWidgetMode( 2 );
            break;
        case QSoftlogic::eptState:
        case QSoftlogic::eptGPIO:
        case QSoftlogic::eptReport:
        case QSoftlogic::eptSoftkey:
        default:
            Q_ASSERT( false );
            break;
        }
        setFocus();
    }
    //Save(); //?

    return 0;
}

bool QSoftKeySettingWidget::AddLogicArray( QSoftlogic::PortType eType, QSoftlogic::Port e1InPort, ushort nValue )
{
    int nRet = -1;

    switch( eType )
    {
    case QSoftlogic::eptError:
        Q_ASSERT( false );
        break;
    case QSoftlogic::eptKey:
        {
            uchar cConditionKey = (uchar)(nValue >> 8);
            uchar cKey = (uchar)(nValue & 0x00FF);

            nRet = m_arySoftLogic.AddSoftlogic_Key( QSoftlogic::eltAND, false, e1InPort, false, QSoftlogic::epNoGate, cConditionKey, cKey );
        }
        break;
    case QSoftlogic::eptMouse:
        {
            uchar cConditionKey = (uchar)(nValue >> 8);
            uchar cMouse = (uchar)(nValue & 0x00FF);

            switch( cMouse )
            {
            case MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN:
            case MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN:
            case MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN:
                nRet = m_arySoftLogic.AddSoftlogic_Mouse( QSoftlogic::eltAND, false, e1InPort, false, QSoftlogic::epNoGate, cConditionKey, cMouse );
                break;
            default:
                break;
            }
        }
        break;
    case QSoftlogic::eptState:
    case QSoftlogic::eptGPIO:
    case QSoftlogic::eptReport:
    case QSoftlogic::eptSoftkey:
    default:
        Q_ASSERT( false );
        break;
    }

    return nRet < 0 ? false : true;
}

bool QSoftKeySettingWidget::ReplaceSoftlogic( int nIndex, ushort nValue, int nRow, int nColumn )
{
    Q_ASSERT( nRow >= 0 && nRow < ui->TableSoftkeyMap->rowCount() );

    bool bRet = false;
    QWidget* pWidget = ui->TableSoftkeyMap->cellWidget( nRow, nColumn );

    switch( nIndex )
    {
    case 0: // None
        {
            int nLogicIndex = FindSoftlogic( m_arySoftLogic, nRow );
            m_arySoftLogic.Remove( m_arySoftLogic[nLogicIndex] );
            bRet = true;
        }
        break;

    case 1: // Key
        {
            QSoftkeyActionCellWidget* pSoftkeyCellWidget = NULL;
            if( pWidget->inherits( "QSoftkeyActionCellWidget" ) )
                pSoftkeyCellWidget = ((QSoftkeyActionCellWidget*)pWidget);

            if( !pSoftkeyCellWidget ) break;

            int nLogicIndex = FindSoftlogic( m_arySoftLogic, nRow );
            if( VerifyIndexLogicArray( nLogicIndex ) )
                m_arySoftLogic.Remove(nLogicIndex);

            bRet = AddLogicArray( QSoftlogic::eptKey, ((QSoftlogic::Port)(int(QSoftlogic::epSoftkey0)+nRow)), nValue );
            Q_ASSERT( bRet );
            //if( !bRet ) break;

            //uchar cConditionKey = (uchar)(nValue >> 8);
            //uchar cKey = (uchar)(nValue & 0xFF);

            //pSoftkeyCellWidget->SetKeyAction( cConditionKey, cKey );
        }
        break;

    case 2: // Mouse
        {
            QSoftkeyActionCellWidget* pSoftkeyCellWidget = NULL;
            if( pWidget->inherits( "QSoftkeyActionCellWidget" ) )
                pSoftkeyCellWidget = ((QSoftkeyActionCellWidget*)pWidget);

            if( !pSoftkeyCellWidget ) break;

            int nLogicIndex = FindSoftlogic( m_arySoftLogic, nRow );
            if( VerifyIndexLogicArray( nLogicIndex ) )
                m_arySoftLogic.Remove(nLogicIndex);

            bRet = AddLogicArray( QSoftlogic::eptMouse, ((QSoftlogic::Port)(int(QSoftlogic::epSoftkey0)+nRow)), nValue );
            Q_ASSERT( bRet );
            //if( !bRet ) break;

            //uchar cConditionKey = (uchar)(nValue >> 8);
            //uchar cMouse = (uchar)(nValue & 0xFF);

            //pSoftkeyCellWidget->SetMouseAction( cConditionKey, cMouse );
        }
        break;
    case 3: // State
    case 4: // GPIO
    case 5: // Report
    case 6: // Softkey
    default:
        Q_ASSERT( false );
        break;
    }

    if( bRet )
        Save();

    return true;
}

int QSoftKeySettingWidget::FindSoftlogic( QSoftlogicArray& aryLogic, QSoftlogic::Port eInputPort)
{
    for( int i=0; i< aryLogic.GetShowSize(); i++ )
    {
        if( aryLogic[i]->getIn1Port() == eInputPort )
        {
            //qDebug( QString(" +++++++++ %1 %2 %3").arg(eInputPort).arg(aryLogic[i]->getIn1Port()).arg(i).toStdString().c_str() );
            qDebug( "++++++++ %d %d %d", eInputPort, aryLogic[i]->getIn1Port(), i );
            return i;
        }
    }
    return -1;
}

int QSoftKeySettingWidget::FindSoftlogic( QSoftlogicArray& aryLogic, int nIndex )
{
    for( int i=0; i< aryLogic.GetShowSize(); i++ )
    {
        if( aryLogic[i]->getIn1Port() == ((QSoftlogic::Port)(int(QSoftlogic::epSoftkey0)+nIndex)) )
            return i;
    }
    return -1;
}

int QSoftKeySettingWidget::SoftkeyInputIndex(QSoftlogic::Port ePort)
{
    if( ePort == QSoftlogic::epNoGate )
        return -1;

    int nIndex = -1;
    nIndex = int(ePort) - int(QSoftlogic::epSoftkey0);
    if( nIndex < 0 )
        nIndex = -1;

    //qDebug( QString(" -------- %1 %2 %3").arg(ePort).arg(QSoftlogic::epSoftkey0).arg(nIndex).toStdString().c_str() );
    qDebug( "--------- %d %d %d", ePort, QSoftlogic::epSoftkey0, nIndex );
    return nIndex;
}

bool QSoftKeySettingWidget::VerifyIndexLogicArray(int nIndex)
{
    if( nIndex < 0 || nIndex >= m_arySoftLogic.GetShowSize() )
        return false;
    return true;
}

void QSoftKeySettingWidget::UpdateListEnable()
{
    if( m_nAddCount )
    {
        m_bLoad = true;
        qDebug( "Send logic" );
        if( ui->TableSoftkeyMap->rowCount() != m_nAddCount )
        {
            ui->TableSoftkeyMap->RemoveAllItem();
            ui->TableSoftkeyMap->AddSoftkeyItem( m_nAddCount );
        }

        QString str( m_bDefault ? '*' : '?' );
        m_RequestSensorData.AddItem( cstrSoftlogic, str );

        m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrSoftlogic).arg(str).toUtf8().data(), true );
        if( m_bDefault )
            m_bDefault = false;
    }
    m_bUpdateList = false;
}

bool QSoftKeySettingWidget::Save()
{
    m_bSave = true;

    QString strExtra;
    QString strSave( m_arySoftLogic.Save( strExtra ) );

    m_RequestSensorData.AddItem( cstrSoftlogic, strSave );

    QString strCmd = QString("%1%2").arg(cstrSoftlogic).arg(strSave);
    qDebug( "%s", (const char*)strCmd.toUtf8().data() );
    m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );

    return true;
}

void QSoftKeySettingWidget::RequestSensorData(bool bDefault)
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    m_bUpdateList = true;
    char cQ = bDefault ? '*' : '?';
    m_bDefault = bDefault;
    QString str( cQ );
    m_RequestSensorData.AddItem( cstrSoftkey, str );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrSoftkey).arg(str).toUtf8().data(), true );

    m_RequestSensorData.Start( m_pT3kHandle );
}

void QSoftKeySettingWidget::onslot_TableSoftkeyMap_cellChanged(int nRow, int nColumn, int nIndex)
{
//    if( !m_nAddCount ) return;
    if( nColumn == 1 && !m_bLoad )
    {
        if( nIndex < 0 || nIndex > 3 )
            return;

        QSoftkeyActionCellWidget* pWidget = ui->TableSoftkeyMap->cellWidget( nRow, nColumn )->inherits( "QSoftkeyActionCellWidget" ) ?
                                  ((QSoftkeyActionCellWidget*)ui->TableSoftkeyMap->cellWidget( nRow, nColumn )) : NULL;
        if( !pWidget ) return;

        if( pWidget->IsMouseType() )
            nIndex = 2;
        else if( nIndex != 0  )
        {
            nIndex = 1;
        }
        ushort nValue = pWidget->GetVaule();

        ReplaceSoftlogic( nIndex, nValue, nRow, nColumn );
    }
}

void QSoftKeySettingWidget::onTableSoftkeyMap_KeyPress_Signal(int nRow, int nColumn, ushort nValue)
{
    if( m_bLoad ) return;
    ReplaceSoftlogic( 1, nValue, nRow, nColumn );
}

void QSoftKeySettingWidget::on_TableSoftkeyMap_MousekeyPress(int nRow, int nColumn, ushort nValue)
{
    if( m_bLoad ) return;
    ReplaceSoftlogic( 2, nValue, nRow, nColumn );
}
