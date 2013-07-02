#include "stdInclude.h"

#include "QMouseSettingWidget.h"
#include "ui_QMouseSettingWidget.h"

#include "T3kCfgWnd.h"
#include "QCustomDefaultSensor.h"

#include "Common/nv.h"

#include <QShowEvent>
#include <QFile>
#ifdef HITACHI_VER
#include <QStringList>
#endif

QMouseSettingWidget::QMouseSettingWidget(T3kHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMouseSettingWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    ui->MouseSettingTableCtrl->SetT3kHandle( m_pT3kHandle );
    ui->TitleMouseMapping->setFont( font() );
    ui->Profile1->setFont( font() );
    ui->Profile2->setFont( font() );
    ui->Profile3->setFont( font() );
    ui->Profile4->setFont( font() );
    ui->Profile5->setFont( font() );

    ui->TitleMouseMapping->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_MOUSE_MAP.png" );

    ui->Profile1->setText( m_ProfileLabel.GetLabel(0) );
    ui->Profile2->setText( m_ProfileLabel.GetLabel(1) );
    ui->Profile3->setText( m_ProfileLabel.GetLabel(2) );
    ui->Profile4->setText( m_ProfileLabel.GetLabel(3) );
    ui->Profile5->setText( m_ProfileLabel.GetLabel(4) );

    connect( this, SIGNAL(SendInputModeState(int)), ui->MouseSettingTableCtrl, SLOT(onRecvInputMode(int)), Qt::QueuedConnection );

    m_nMouseProfileIndex = -1;
    m_nSelectedProfileIndex = -1;

    m_nProfileIndexData = -1;

    m_nCurInputMode = -1;

    m_bSetDefault = false;

    // usbconfigmode
    ui->MouseSettingTableCtrl->setEnabled(false);
    ui->Profile1->setEnabled(false);
    ui->Profile2->setEnabled(false);
    ui->Profile3->setEnabled(false);
    ui->Profile4->setEnabled(false);
    ui->Profile5->setEnabled(false);

    OnChangeLanguage();
}

QMouseSettingWidget::~QMouseSettingWidget()
{
    m_ProfileLabel.Save();
    m_RequestSensorData.Stop();

    delete ui;
}

void QMouseSettingWidget::SetDefault()
{
    RequestSensorData( true );
    m_ProfileLabel.Default();
    ui->Profile1->setText( m_ProfileLabel.GetLabel(0) );
    ui->Profile2->setText( m_ProfileLabel.GetLabel(1) );
    ui->Profile3->setText( m_ProfileLabel.GetLabel(2) );
    ui->Profile4->setText( m_ProfileLabel.GetLabel(3) );
    ui->Profile5->setText( m_ProfileLabel.GetLabel(4) );
}

void QMouseSettingWidget::Refresh()
{
    RequestSensorData( false );
}

void QMouseSettingWidget::ReplaceLabelName(QCheckableButton *pBtn)
{
    QString str = pBtn->text();
    if( pBtn == ui->Profile1 )
        m_ProfileLabel.SetLabel( 0, str );
    else if( pBtn == ui->Profile2 )
        m_ProfileLabel.SetLabel( 1, str );
    else if( pBtn == ui->Profile3 )
        m_ProfileLabel.SetLabel( 2, str );
    else if( pBtn == ui->Profile4 )
        m_ProfileLabel.SetLabel( 3, str );
    else if( pBtn == ui->Profile5 )
        m_ProfileLabel.SetLabel( 4, str );
}

void QMouseSettingWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->TitleMouseMapping->setText( Res.GetResString(QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TITLE_CAPTION_MOUSE_BUTTON_MAPPING")) );
}

void QMouseSettingWidget::OnRSP(ResponsePart /*Part*/, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if( strstr(sCmd, cstrUsbConfigMode) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );

        int nMode = strtol(sCmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( nMode )
        {
        case 0x04: // digitizer
            ui->MouseSettingTableCtrl->setEnabled(false);
            ui->Profile1->setEnabled(false);
            ui->Profile2->setEnabled(false);
            ui->Profile3->setEnabled(false);
            ui->Profile4->setEnabled(false);
            ui->Profile5->setEnabled(false);
            break;
        case 0x07: // full
            ui->MouseSettingTableCtrl->setEnabled(true);
            ui->Profile1->setEnabled(true);
            ui->Profile2->setEnabled(true);
            ui->Profile3->setEnabled(true);
            ui->Profile4->setEnabled(true);
            ui->Profile5->setEnabled(true);
            break;
        default:
            break;
        }
    }
    else if( strstr(sCmd, cstrInputMode) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrInputMode );

        char* pInputMode = NULL;
        pInputMode = (char*)strchr( sCmd, ',' );

        int nMode1 = -1;

        if( !pInputMode )
            m_nCurInputMode = strtol(sCmd + sizeof(cstrInputMode) - 1, NULL, 16);
        else
        {
            nMode1 = strtol(sCmd + sizeof(cstrInputMode) - 1, NULL, 16);
            if( nMode1 == 0xFF )
                m_nCurInputMode = strtol( pInputMode+1, NULL, 16 );
            else
                m_nCurInputMode = nMode1;
        }

        emit SendInputModeState( m_nCurInputMode );
    }
    else if ( strstr(sCmd, cstrMouseProfile) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrMouseProfile );

        int nProfile = atoi(sCmd + sizeof(cstrMouseProfile) - 1 );
        char* pProfile = (char*)strchr( sCmd, ',' );
        if( pProfile )
        {
            int nMouseIdx = strtol( sCmd + sizeof(cstrMouseProfile) - 1, NULL, 10 );
            int nMultiIdx = strtol( pProfile+1, NULL, 10 );

            m_nProfileIndexData = (nMouseIdx << 16) | nMultiIdx;

            switch( m_nCurInputMode )
            {
            case 0xFF:
            case -1:
                Q_ASSERT(false);
                break;
            case 0x02:
                nProfile = nMultiIdx;
                break;
            case 0x00:
            default:
                nProfile = nMouseIdx;
                break;
            }
        }

        if( nProfile < NV_DEF_MOUSE_PROFILE_RANGE_START || nProfile > NV_DEF_MOUSE_PROFILE_RANGE_END+1 ) nProfile = NV_DEF_MOUSE_PROFILE;


        int i = 0;
        QList<QCheckableButton*> pChildren = findChildren<QCheckableButton*>();
        QCheckableButton* pChild = NULL;
        foreach( pChild, pChildren )
        {
            if( i == nProfile-1 )
            {
                pChild->SetCheckBox( true );
                if( m_nSelectedProfileIndex < 0 )
                {
                    pChild->ChangeButtonMode( QCheckableButton::BM_ON );
                    pChild->SetChecked( true );
                }
                pChild->update();
            }
            else
            {
                if( pChild->IsCheckBox() )
                {
                    pChild->SetCheckBox( false );
                    pChild->update();
                }
                if( m_nSelectedProfileIndex < 0 )
                {
                    pChild->ChangeButtonMode( QCheckableButton::BM_OFF );
                    pChild->SetChecked( false );
                    pChild->update();
                }
            }

            int nIndex = -1;
            switch( m_nCurInputMode )
            {
            case 0xFF:
            case -1:
                Q_ASSERT(false);
                break;
            case 0x02:
                nIndex = m_nProfileIndexData >> 16;
                pChild->SetCheckIndex( 2 );
                break;
            case 0x00:
            default:
                nIndex = m_nProfileIndexData & 0x00FF;
                pChild->SetCheckIndex( 1 );
                break;
            }

            if( i == nIndex-1 )
            {
                pChild->SetSubCheckBox( true );
            }
            else
            {
                pChild->SetSubCheckBox( false );
            }

            i++;
        }

        m_nSelectedProfileIndex = nProfile-1;

        if( m_bSetDefault || m_nMouseProfileIndex < 0 )
        {
            m_nMouseProfileIndex = nProfile-1;

            QString strCmd;
            char cQ = '?';//m_bSetDefault ? '*' : '?';
            switch( m_nMouseProfileIndex )
            {
            case 0:
                strCmd = QString("%1%2").arg(cstrMouseProfile1).arg(cQ);
                m_RequestSensorData.RemoveItem( cstrMouseProfile1 );
                break;
            case 1:
                strCmd = QString("%1%2").arg(cstrMouseProfile2).arg(cQ);
                m_RequestSensorData.RemoveItem( cstrMouseProfile2 );
                break;
            case 2:
                strCmd = QString("%1%2").arg(cstrMouseProfile3).arg(cQ);
                m_RequestSensorData.RemoveItem( cstrMouseProfile3 );
                break;
            case 3:
                strCmd = QString("%1%2").arg(cstrMouseProfile4).arg(cQ);
                m_RequestSensorData.RemoveItem( cstrMouseProfile4 );
                break;
            case 4:
                strCmd = QString("%1%2").arg(cstrMouseProfile5).arg(cQ);
                m_RequestSensorData.RemoveItem( cstrMouseProfile5 );
                break;
            default:
                strCmd = QString("%1%2").arg(cstrMouseProfile2).arg(cQ);
                break;
            }

            m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );

            ui->MouseSettingTableCtrl->SetProfileIndex( m_nMouseProfileIndex );

            m_bSetDefault = false;
        }
    }

    if ( strstr(sCmd, cstrMouseProfile1) == sCmd )
        m_RequestSensorData.RemoveItem( cstrMouseProfile1 );

    if ( strstr(sCmd, cstrMouseProfile2) == sCmd )
        m_RequestSensorData.RemoveItem( cstrMouseProfile2 );

    if ( strstr(sCmd, cstrMouseProfile3) == sCmd )
        m_RequestSensorData.RemoveItem( cstrMouseProfile3 );

    if ( strstr(sCmd, cstrMouseProfile4) == sCmd )
        m_RequestSensorData.RemoveItem( cstrMouseProfile4 );

    if ( strstr(sCmd, cstrMouseProfile5) == sCmd )
        m_RequestSensorData.RemoveItem( cstrMouseProfile5 );
}

void QMouseSettingWidget::RequestSensorData( bool bDefault )
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    m_bSetDefault = bDefault;

    char cQ = bDefault ? '*' : '?';

    QCustomDefaultSensor* pInstance = QCustomDefaultSensor::Instance();
    if( bDefault )
    {
#ifdef HITACHI_VER
        QStringList listMPCustomData;
        if( LoadMPCustomData( &listMPCustomData ) )
        {
            m_RequestSensorData.AddItem( cstrMouseProfile1, listMPCustomData.at(0) );
            m_RequestSensorData.AddItem( cstrMouseProfile2, listMPCustomData.at(1) );
            m_RequestSensorData.AddItem( cstrMouseProfile3, listMPCustomData.at(2) );
            m_RequestSensorData.AddItem( cstrMouseProfile4, listMPCustomData.at(3) );
            m_RequestSensorData.AddItem( cstrMouseProfile5, listMPCustomData.at(4) );

            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile1).arg(listMPCustomData.at(0)).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile2).arg(listMPCustomData.at(1)).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile3).arg(listMPCustomData.at(2)).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile4).arg(listMPCustomData.at(3)).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile5).arg(listMPCustomData.at(4)).toUtf8().data(), false );
        }
        else
        {
            QString str( cQ );
            m_RequestSensorData.AddItem( cstrMouseProfile1, str );
            m_RequestSensorData.AddItem( cstrMouseProfile2, str );
            m_RequestSensorData.AddItem( cstrMouseProfile3, str );
            m_RequestSensorData.AddItem( cstrMouseProfile4, str );
            m_RequestSensorData.AddItem( cstrMouseProfile5, str );

            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile1).arg(cQ).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile2).arg(cQ).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile3).arg(cQ).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile4).arg(cQ).toUtf8().data(), false );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile5).arg(cQ).toUtf8().data(), false );
        }
#else
        if( pInstance->IsLoaded() )
        {
            QString strQ = QString( cQ );
            QString strP1 = pInstance->GetDefaultData( cstrMouseProfile1, strQ );
            QString strP2 = pInstance->GetDefaultData( cstrMouseProfile2, strQ );
            QString strP3 = pInstance->GetDefaultData( cstrMouseProfile3, strQ );
            QString strP4 = pInstance->GetDefaultData( cstrMouseProfile4, strQ );
            QString strP5 = pInstance->GetDefaultData( cstrMouseProfile5, strQ );

            m_RequestSensorData.AddItem( cstrMouseProfile1, strP1 );
            m_RequestSensorData.AddItem( cstrMouseProfile2, strP2 );
            m_RequestSensorData.AddItem( cstrMouseProfile3, strP3 );
            m_RequestSensorData.AddItem( cstrMouseProfile4, strP4 );
            m_RequestSensorData.AddItem( cstrMouseProfile5, strP5 );

            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile1).arg(strP1).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile2).arg(strP2).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile3).arg(strP3).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile4).arg(strP4).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile5).arg(strP5).toUtf8().data(), true );
        }
        else
        {
            QString str( cQ );
            m_RequestSensorData.AddItem( cstrMouseProfile1, str );
            m_RequestSensorData.AddItem( cstrMouseProfile2, str );
            m_RequestSensorData.AddItem( cstrMouseProfile3, str );
            m_RequestSensorData.AddItem( cstrMouseProfile4, str );
            m_RequestSensorData.AddItem( cstrMouseProfile5, str );

            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile1).arg(cQ).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile2).arg(cQ).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile3).arg(cQ).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile4).arg(cQ).toUtf8().data(), true );
            m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile5).arg(cQ).toUtf8().data(), true );
        }
#endif
        m_nSelectedProfileIndex = -1;
    }

    m_RequestSensorData.AddItem( cstrUsbConfigMode, "?" );
    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrUsbConfigMode).toUtf8().data(), true );

    int nRet = 3;
    do
    {
    if( !m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrInputMode).toUtf8().data(), false ) )
        nRet--;
    else
        break;
    } while( nRet );

    if( !nRet ) m_RequestSensorData.AddItem( cstrInputMode, "?" );

    if( pInstance->IsLoaded() )
    {
        QString str = pInstance->GetDefaultData( cstrMouseProfile, QString(cQ) );
        m_RequestSensorData.AddItem( cstrMouseProfile, str );
        m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile).arg(str).toUtf8().data(), true );
    }
    else
    {
        m_RequestSensorData.AddItem( cstrMouseProfile, (QString)cQ );
        m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrMouseProfile).arg(cQ).toUtf8().data(), true );
    }

    m_RequestSensorData.Start( m_pT3kHandle );
}
#ifdef HITACHI_VER
bool QMouseSettingWidget::LoadMPCustomData( QStringList* Out_ListMPData )
{
    Out_ListMPData->clear();
    QString strPathName( QCoreApplication::applicationDirPath() + "/MPCustomData.dat" );
    if( QFile::exists( strPathName ) )
    {
        m_bLoadMPCustomData = true;

        QSettings setMPData( strPathName, QSettings::IniFormat );
        QString str;
        str = QString(cstrMouseProfile1); str.remove('=');
        Out_ListMPData->push_back( setMPData.value( str, "" ).toString() );
        str = QString(cstrMouseProfile2); str.remove('=');
        Out_ListMPData->push_back( setMPData.value( str, "" ).toString() );
        str = QString(cstrMouseProfile3); str.remove('=');
        Out_ListMPData->push_back( setMPData.value( str, "" ).toString() );
        str = QString(cstrMouseProfile4); str.remove('=');
        Out_ListMPData->push_back( setMPData.value( str, "" ).toString() );
        str = QString(cstrMouseProfile5); str.remove('=');
        Out_ListMPData->push_back( setMPData.value( str, "" ).toString() );

        if( Out_ListMPData->at(0).isEmpty() ||
            Out_ListMPData->at(1).isEmpty() ||
            Out_ListMPData->at(2).isEmpty() ||
            Out_ListMPData->at(3).isEmpty() ||
            Out_ListMPData->at(4).isEmpty() )
        {
            Out_ListMPData->clear();
            return false;
        }
        return true;
    }
    return false;
}
#endif
void QMouseSettingWidget::showEvent(QShowEvent *evt)
{
    QWidget::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        RequestSensorData( false );
        setFocusPolicy( Qt::StrongFocus );
    }
}

void QMouseSettingWidget::hideEvent(QHideEvent *evt)
{
    QWidget::hideEvent(evt);

    if( evt->type() == QEvent::Hide )
    {
        setFocusPolicy( Qt::NoFocus );

        m_RequestSensorData.Stop();

        m_nSelectedProfileIndex = -1;
        m_nMouseProfileIndex = -1;
    }
}

void QMouseSettingWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        ByPassKeyPressEvent( evt );
        return;
    }

    QWidget::keyPressEvent(evt);
}

void QMouseSettingWidget::on_Profile1_clicked()
{
    ChangeProfile( ui->Profile1, 0 );
}

void QMouseSettingWidget::on_Profile2_clicked()
{
    ChangeProfile( ui->Profile2, 1 );
}

void QMouseSettingWidget::on_Profile3_clicked()
{
    ChangeProfile( ui->Profile3, 2 );
}

void QMouseSettingWidget::on_Profile4_clicked()
{
    ChangeProfile( ui->Profile4, 3 );
}

void QMouseSettingWidget::on_Profile5_clicked()
{
    ChangeProfile( ui->Profile5, 4 );
}

void QMouseSettingWidget::ChangeProfile(QCheckableButton *pBtn, int nIndex)
{
    if( pBtn->IsCheckBox() && m_nSelectedProfileIndex != nIndex )
    {
        m_nSelectedProfileIndex = nIndex;

        int nMouseIdx = m_nProfileIndexData >> 16;
        int nMultiIdx = m_nProfileIndexData & 0x00FF;

        QString strCmd;
        switch( m_nCurInputMode )
        {
        case 0xFF:
        case -1:
            Q_ASSERT(false);
            break;
        case 0x02:
            strCmd = QString("%1%2,%3").arg(cstrMouseProfile).arg(nMouseIdx).arg(m_nSelectedProfileIndex+1);
            break;
        case 0x00:
        default:
            strCmd = QString("%1%2,%3").arg(cstrMouseProfile).arg(m_nSelectedProfileIndex+1).arg(nMultiIdx);
            break;
        }

        m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false );
    }

    if( pBtn->isChecked() )
    {
        m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrInputMode).toUtf8().data(), false );

        m_nMouseProfileIndex = nIndex;
        QString strCmd;
        switch( m_nMouseProfileIndex )
        {
        case 0:
            strCmd = QString("%1%2").arg(cstrMouseProfile1).arg('?');
            break;
        case 1:
            strCmd = QString("%1%2").arg(cstrMouseProfile2).arg('?');
            break;
        case 2:
            strCmd = QString("%1%2").arg(cstrMouseProfile3).arg('?');
            break;
        case 3:
            strCmd = QString("%1%2").arg(cstrMouseProfile4).arg('?');
            break;
        case 4:
            strCmd = QString("%1%2").arg(cstrMouseProfile5).arg('?');
            break;
        default:
            strCmd = QString("%1%2").arg(cstrMouseProfile2).arg('?');
            break;
        }

        m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );

        ui->MouseSettingTableCtrl->SetProfileIndex( m_nMouseProfileIndex );
    }
}
