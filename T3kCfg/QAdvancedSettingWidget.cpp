#include "QAdvancedSettingWidget.h"
#include "ui_QAdvancedSettingWidget.h"

#include "QAdvancedCalibrationWidget.h"
#include "QConfigData.h"
#include "QT3kUserData.h"
#include "T3kConstStr.h"
#include "T3kCamNameDef.h"
#include "stdInclude.h"

#include <QMessageBox>


QAdvancedSettingWidget::QAdvancedSettingWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAdvancedSettingWidget)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    setWindowModality( Qt::WindowModal );
    setModal( true );

    connect( ui->BtnCancel, &QPushButton::clicked, this, &QDialog::close );

    T3kHandle* pHandle = QT3kUserData::GetInstance()->getT3kHandle();
    int nRetry = 3;
    bool bOK = false;
    m_bRSE = false;
    QString strCmd(sCam1 + cstrCamPosTrc + "?");
    do
    {
        if (pHandle->SendCommand(strCmd.toUtf8().data(), false))
        {
            bOK = true;
            break;
        }
    } while (--nRetry > 0);

    ui->BtnDefault->setVisible( bOK && !m_bRSE && !m_strCam1PosTrc.isEmpty() );

    onChangeLanguage();
}

QAdvancedSettingWidget::~QAdvancedSettingWidget()
{
    delete ui;
}

void QAdvancedSettingWidget::closeEvent(QCloseEvent *)
{
    ui->EditPassword->clear();
    ui->ChkDetection->setChecked( false );
}

void QAdvancedSettingWidget::OnRSP(ResponsePart part, ushort, const char *, long, bool, const char *szCmd)
{
    if( !winId() ) return;

    if( strstr( szCmd, cstrCamPosTrc ) == szCmd )
    {
        QString strCmd( szCmd );
        strCmd = strCmd.mid( strCmd.indexOf('=')+1 );
        switch( part )
        {
        case CM1:
            m_strCam1PosTrc = strCmd;
            break;
        case CM2:
            m_strCam2PosTrc = strCmd;
            break;
        case CM1_1:
            m_strCamS1PosTrc = strCmd;
            break;
        case CM2_1:
            m_strCamS2PosTrc = strCmd;
            break;
        case MM:
        default:
            break;
        }
    }
    else if( strstr( szCmd, cstrFactoryCalibration ) == szCmd )
    {
        QString strCmd( szCmd );
        m_strFactoryCalibration = strCmd.mid( strCmd.indexOf('=')+1 );
    }
}

void QAdvancedSettingWidget::OnRSE(ResponsePart part, ushort, const char *, long, bool, const char *szCmd)
{
    if( !winId() ) return;

    if( strstr( szCmd, cstrCamPosTrc ) == szCmd )
    {
        qDebug() << QString(szCmd);
        m_bRSE = true;
    }
}

void QAdvancedSettingWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("WINDOW_CAPTION")) );
    ui->TitleAdvanced->setText( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("TITLE_CAPTION_ADJUSTMENT")) );
    ui->ChkBent->setText( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("TEXT_CHECKBOX_BENT")) );
    ui->ChkDetection->setText( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("TEXT_CHECKBOX_DETECTION")) );
    ui->LbPassword->setText( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("TEXT_ID")) );

    ui->BtnStart->setText( Res.getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("BTN_START")) );

    ui->BtnDefault->setText( Res.getResString(QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_DEFAULT")) );
    ui->BtnCancel->setText( Res.getResString(QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_CANCEL")) );
}

void QAdvancedSettingWidget::on_BtnStart_clicked()
{
    QString strKeyCode = QConfigData::instance()->getData("ADVANCED", "ID", "").toString();
    QByteArray aryB = strKeyCode.toUtf8();
    for ( int ni = aryB.length() - 1; ni >= 0; ni-- )
    {
       char b = aryB[ni];
       if ( b >= 33 && b <= 126 )
       {
           b = (((b - 33) + 1) % 94) + 33;
           aryB[ni] = b;
       }
    }
    strKeyCode = aryB;
    if ( strKeyCode.compare( ui->EditPassword->text(), Qt::CaseSensitive ) != 0 )
    {
        QMessageBox::warning( this, "Error", QLangManager::instance()->getResource().getResString(QString::fromUtf8("ADVANCED"), QString::fromUtf8("MSG_INCORRECT_ID")), QMessageBox::Ok );
        ui->EditPassword->setFocus();
        return;
    }

    hide();

    QAdvancedCalibrationWidget* pWidget = new QAdvancedCalibrationWidget( ui->ChkDetection->isChecked() );
    pWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( pWidget, &QAdvancedCalibrationWidget::closeWidget, this, &QDialog::close );
    pWidget->showFullScreen();
}

void QAdvancedSettingWidget::on_ChkDetection_clicked()
{
    ui->EditPassword->setFocus();
}

//static unsigned long hex2u32( const char * pstr )
//{
//    const char * str = pstr;
//    unsigned long u32Ret = 0;

//    if ( str == NULL )
//        return 0;

//    while ( str[0] == ' ' || str[0] == '\t' )
//        str++;

//    while ( 1 )
//    {
//        if ( str[0] >= '0' && str[0] <= '9' )
//            u32Ret = u32Ret * 16 + (*str++ - '0');
//        else if ( str[0] >= 'A' && str[0] <= 'F' )
//            u32Ret = u32Ret * 16 + (*str++ - 'A' + 10);
//        else if ( str[0] >= 'a' && str[0] <= 'f' )
//            u32Ret = u32Ret * 16 + (*str++ - 'a' + 10);
//        else
//            return u32Ret;
//    }
//}

void QAdvancedSettingWidget::on_BtnDefault_clicked()
{
    QLangRes& Res = QLangManager::instance()->getResource();
    QString strMessage = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TEXT_DEFAULT") );
    QString strMsgTitle = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TITLE_DEFAULT") );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setText( strMessage );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
    msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
    msgBox.setFont( font() );

    if( msgBox.exec() != QMessageBox::Yes ) return;

    T3kHandle* pHandle = QT3kUserData::GetInstance()->getT3kHandle();
    // bent
    m_strCam1PosTrc.clear();
    m_strCam2PosTrc.clear();
    m_strCamS1PosTrc.clear();
    m_strCamS2PosTrc.clear();
    m_strFactoryCalibration.clear();

    int nCam = QT3kUserData::GetInstance()->isSubCameraExist() ? 4 : 2;
    QStringList listCmd;
    listCmd << sCam1 + QString(cstrCamPosTrc) + "?" <<
               sCam2 + QString(cstrCamPosTrc) + "?";
    if( nCam > 2 )
    {
        listCmd << sCam1_1 + QString(cstrCamPosTrc) + "?" <<
                   sCam2_1 + QString(cstrCamPosTrc) + "?";
    }
    listCmd << QString(cstrFactoryCalibration) + "?";

    for( int i=0; i<listCmd.size(); i++ )
    {
        int nRetry = 3;
        bool bOK = false;
        QString strCmd = listCmd.at(i);
        do
        {
            if (pHandle->SendCommand(strCmd.toUtf8().data(), false))
            {
                bOK = true;
                break;
            }
        } while (--nRetry > 0);

        if (!bOK)
        {
            QMessageBox::critical( this, "Error", "Command sending fail. \"" + strCmd.left(strCmd.indexOf('=')) + "\"", QMessageBox::Ok );
            return;
        }
    }
    listCmd.clear();

    // reset f42.
    QString strDefaultf42;
    if ( m_strCam1PosTrc.length() > 212 && m_strCam2PosTrc.length() > 212 )
    {
        strDefaultf42 = m_strFactoryCalibration.left(9);

        //	2   3       9   10
        //	|  /| \   / |\  |
        //	| / |   6   | \ |
        //	|/  | /   \ |  \|
        //	0   5       7   12
        const char orderDatas[2][13] = {
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
            { 0, -1, 2, 3, -1, 5, 6, 7, -1, 9, 10, -1, 12 }
        };
        const char * orderData;
        int orderIdx;
        int nSize = (int)sizeof(orderDatas[0]);
        orderIdx = strtoul(m_strCam1PosTrc.mid(2, 2).toUtf8().data(), NULL, 16) & 0x0f;
        if ( orderIdx > 1 ) orderIdx = 1;
        orderData = orderDatas[orderIdx];
        for ( int ni = 0; ni < nSize; ni++ )
        {
            if ( orderData[ni] < 0 )
            {
                strDefaultf42 += "ffc00000";
                continue;
            }
            unsigned long dwS = strtoul( m_strCam1PosTrc.mid(4 + 8 * orderData[ni] * 2, 8).toUtf8().data(), NULL, 16 );
            unsigned long dwE = strtoul( m_strCam1PosTrc.mid(4 + 8 * (orderData[ni] * 2 + 1), 8).toUtf8().data(), NULL, 16 );
            float f = (*(float *)&dwS + *(float *)&dwE) / 2.f;
            unsigned long dwF = (*(unsigned long *)&f);
            strDefaultf42 += QString("%1").arg(dwF, 8, 16, QChar('0'));
        }
        orderIdx = strtoul(m_strCam2PosTrc.mid(2, 2).toUtf8().data(), NULL, 16) & 0x0f;
        if ( orderIdx > 1 ) orderIdx = 1;
        orderData = orderDatas[orderIdx];
        for ( int ni = 0; ni < nSize; ni++ )
        {
            if ( orderData[ni] < 0 )
            {
                strDefaultf42 += "ffc00000";
                continue;
            }
            unsigned long dwS = strtoul( m_strCam2PosTrc.mid(4 + 8 * orderData[ni] * 2, 8).toUtf8().data(), NULL, 16 );
            unsigned long dwE = strtoul( m_strCam2PosTrc.mid(4 + 8 * (orderData[ni] * 2 + 1), 8).toUtf8().data(), NULL, 16 );
            float f = (*(float *)&dwS + *(float *)&dwE) / 2.f;
            unsigned long dwF = (*(unsigned long *)&f);
            strDefaultf42 += QString("%1").arg(dwF, 8, 16, QChar('0'));
        }
    }

    listCmd.push_back(QString(sCam1 + cstrCamPosUserTrc + "*"));
    listCmd.push_back(QString(sCam2 + cstrCamPosUserTrc + "*"));
    QString strCam1( m_strCam1PosTrc.right( 50 ) );
    QString strCam2( m_strCam2PosTrc.right( 50 ) );
    QString strCam1V, strCam2V;
    while( strCam1.size() > 2 )
    {
        strCam1V += strCam1.left(8) + ",";
        strCam1.remove(0, 8);
        strCam2V += strCam2.left(8) + ",";
        strCam2.remove(0, 8);
    }
    strCam1V += strCam1.left(2);
    strCam2V += strCam2.left(2);
    listCmd.push_back(QString(sCam1 + cstrFactorialCamPos + strCam1V));
    listCmd.push_back(QString(sCam2 + cstrFactorialCamPos + strCam2V));

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        listCmd.push_back(QString(sCam1_1 + cstrCamPosUserTrc + "*"));
        listCmd.push_back(QString(sCam2_1 + cstrCamPosUserTrc + "*"));

        QString strCam1( m_strCamS1PosTrc.right( 50 ) );
        QString strCam2( m_strCamS2PosTrc.right( 50 ) );
        QString strCam1V, strCam2V;
        while( strCam1.size() > 2 )
        {
            strCam1V += strCam1.left(8) + ",";
            strCam1.remove(0, 8);
        }
        strCam1V += strCam1.left(2);
        while( strCam2.size() > 2 )
        {
            strCam2V += strCam2.left(8) + ",";
            strCam2.remove(0, 8);
        }
        strCam2V += strCam2.left(2);
        listCmd.push_back(QString(sCam1_1 + cstrFactorialCamPos + strCam1V));
        listCmd.push_back(QString(sCam2_1 + cstrFactorialCamPos + strCam2V));
    }

    listCmd.push_back(QString(cstrFactoryCalibration) + strDefaultf42);

    // detection
    listCmd.push_back(QString(sCam1 + cstrDetectionRange + "*"));
    listCmd.push_back(QString(sCam2 + cstrDetectionRange + "*"));

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        listCmd.push_back(QString(sCam1_1 + cstrDetectionRange + "*"));
        listCmd.push_back(QString(sCam2_1 + cstrDetectionRange + "*"));
    }

    for( int i=0; i<listCmd.size(); i++ )
    {
        int nRetry = 3;
        bool bOK = false;
        QString strCmd = listCmd.at(i);
        do
        {
            if (pHandle->SendCommand(strCmd.toUtf8().data(), false))
            {
                bOK = true;
                break;
            }
        } while (--nRetry > 0);

        if (!bOK)
        {
            QMessageBox::critical( this, "Error", "Command sending fail. \"" + strCmd.left(strCmd.indexOf('=')) + "\"", QMessageBox::Ok );
            return;
        }
    }
}
