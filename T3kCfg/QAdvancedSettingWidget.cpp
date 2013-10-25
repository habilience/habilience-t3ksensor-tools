#include "QAdvancedSettingWidget.h"
#include "ui_QAdvancedSettingWidget.h"

#include "QAdvancedCalibrationWidget.h"
#include "QConfigData.h"
#include "QT3kUserData.h"
#include "T3kConstStr.h"
#include "T3kCamNameDef.h"
#include "QWarningWidget.h"

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
}

QAdvancedSettingWidget::~QAdvancedSettingWidget()
{
    delete ui;
}

void QAdvancedSettingWidget::OnRSP(ResponsePart part, ushort, const char *, long, bool, const char *szCmd)
{
    if( !isVisible() ) return;

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

void QAdvancedSettingWidget::on_BtnStart_clicked()
{
    if( QConfigData::instance()->getData( "ADVANCED", "PASSWORD", "" ).toString().compare( ui->EditPassword->text(), Qt::CaseSensitive ) != 0 )
    {
        QMessageBox::warning( this, "Password", "The password is not correct.", QMessageBox::Ok );
        ui->EditPassword->setFocus();
        return;
    }

    QWarningWidget warning( this );
    if( warning.exec() == QDialog::Rejected )
    {
        close();
        return;
    }

    QAdvancedCalibrationWidget* pWidget = new QAdvancedCalibrationWidget( ui->ChkDetection->isChecked() );
    pWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( pWidget, &QAdvancedCalibrationWidget::closeWidget, this, &QDialog::close );
    pWidget->exec();
}

void QAdvancedSettingWidget::on_ChkDetection_clicked()
{

}

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

    // reset f42.
    QString strDefaultf42;
    const char* pszCamPos1Trc = m_strCam1PosTrc.toUtf8().data();
    const char* pszCamPos2Trc = m_strCam2PosTrc.toUtf8().data();
    const char* pszCamPosS1Trc = m_strCamS1PosTrc.toUtf8().data();
    const char* pszCamPosS2Trc = m_strCamS2PosTrc.toUtf8().data();
    const char* pszFactoryCalibration = m_strFactoryCalibration.toUtf8().data();


    //

//    QStringList listCmd;
//    listCmd.push_back(QString(sCam1 + cstrCamPosUserTrc + "*"));
//    listCmd.push_back(QString(sCam2 + cstrCamPosUserTrc + "*"));
//    listCmd.push_back(QString(sCam1 + cstrFactorialCamPos) + m_strCam1PosDefault);
//    listCmd.push_back(QString(sCam2 + cstrFactorialCamPos) + m_strCam2PosDefault);

//    if( QT3kUserData::GetInstance()->isSubCameraExist() )
//    {
//        listCmd.push_back(QString(sCam1_1 + cstrCamPosUserTrc + "*"));
//        listCmd.push_back(QString(sCam2_1 + cstrCamPosUserTrc + "*"));
//        listCmd.push_back(QString(sCam1_1 + cstrFactorialCamPos) + m_strCamS1PosDefault);
//        listCmd.push_back(QString(sCam2_1 + cstrFactorialCamPos) + m_strCamS2PosDefault);
//    }

    // detection
    listCmd.push_back(QString(sCam1 + cstrDetectionRange + "*"));
    listCmd.push_back(QString(sCam2 + cstrDetectionRange + "*"));

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        listCmd.push_back(QString(sCam1_1 + cstrDetectionRange + "*"));
        listCmd.push_back(QString(sCam2_1 + cstrDetectionRange + "*"));
    }

    listCmd.push_back(QString(cstrFactoryCalibration) + "*");

    foreach( QString str, listCmd )
    {
        pHandle->SendCommand( str.toUtf8().data(), true );
    }
}
