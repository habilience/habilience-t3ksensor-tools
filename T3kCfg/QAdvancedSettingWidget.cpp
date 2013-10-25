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
        if( !strCmd.isEmpty() )
        {
            strCmd = strCmd.right( 50 ); // 8,8,8,8,8,8,2
            QString str;
            while( strCmd.size() > 2 )
            {
                str += strCmd.left( 8 ) + ',';
                strCmd.remove( 0, 8 );
            }
            str += strCmd.left( 2 );
            strCmd.remove( 0, 2 );
            Q_ASSERT( strCmd.isEmpty() );

            switch( part )
            {
            case CM1:
                m_strCam1PosDefault = str;
                break;
            case CM2:
                m_strCam2PosDefault = str;
                break;
            case CM1_1:
                m_strCamS1PosDefault = str;
                break;
            case CM2_1:
                m_strCamS2PosDefault = str;
                break;
            case MM:
            default:
                break;
            }
        }
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
    m_strCam1PosDefault.clear();
    m_strCam2PosDefault.clear();
    m_strCamS1PosDefault.clear();
    m_strCamS2PosDefault.clear();

    int nCam = QT3kUserData::GetInstance()->isSubCameraExist() ? 4 : 2;
    for( int i=0; i<nCam; i++ )
    {
        int nRetry = 3;
        bool bOK = false;
        QString strCmd = getCameraPrefix(i+1) + QString(cstrCamPosTrc) + "?";
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

    QStringList listCmd;
    listCmd.push_back(QString(sCam1 + cstrCamPosUserTrc + "*"));
    listCmd.push_back(QString(sCam2 + cstrCamPosUserTrc + "*"));
    listCmd.push_back(QString(sCam1 + cstrFactorialCamPos) + m_strCam1PosDefault);
    listCmd.push_back(QString(sCam2 + cstrFactorialCamPos) + m_strCam2PosDefault);

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        listCmd.push_back(QString(sCam1_1 + cstrCamPosUserTrc + "*"));
        listCmd.push_back(QString(sCam2_1 + cstrCamPosUserTrc + "*"));
        listCmd.push_back(QString(sCam1_1 + cstrFactorialCamPos) + m_strCamS1PosDefault);
        listCmd.push_back(QString(sCam2_1 + cstrFactorialCamPos) + m_strCamS2PosDefault);
    }

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
