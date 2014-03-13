#include "QPredefProfileEditDialog.h"
#include "ui_QPredefProfileEditDialog.h"

#include "QT3kUserData.h"

#include <QFile>
#include <QMessageBox>
#include <QByteArray>

QPredefProfileEditDialog::QPredefProfileEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QPredefProfileEditDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    setWindowFlags( flags );

    setFixedSize( width(), height() );

    ui->GestureProfileTable->setOnlyDisplay( true );
    ui->GestureProfileTable->setMargin( 10,10,10,10 );

    ui->LBSelectedProfile->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_MOUSE_MAP.png" );
    ui->LBSelectedProfile->setText( "Add" );

    connect( ui->BtnClose, &QPushButton::clicked, this, &QDialog::close );
    connect( ui->GestureProfileTable, &QGestureMappingTable::updateProfile, this, &QPredefProfileEditDialog::onUpdateProfile, Qt::DirectConnection );

    m_pEditActionWnd = new QEditActionWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditActionEWnd = new QEditActionEDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction2WDWnd = new QEditAction2WDWnd( QT3kUserData::GetInstance()->getTopParent() );
    m_pEditAction4WDWnd = new QEditAction4WDWnd( QT3kUserData::GetInstance()->getTopParent() );

    connect( m_pEditActionWnd, &QEditActionWnd::sendCommand, this, &QPredefProfileEditDialog::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditActionEWnd, &QEditActionEDWnd::sendCommand, this, &QPredefProfileEditDialog::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction2WDWnd, &QEditAction2WDWnd::sendCommand, this, &QPredefProfileEditDialog::onSendCommand, Qt::QueuedConnection );
    connect( m_pEditAction4WDWnd, &QEditAction4WDWnd::sendCommand, this, &QPredefProfileEditDialog::onSendCommand, Qt::QueuedConnection );

    ui->CBProfileList->setEditable( true );
    ui->CBProfileList->clear();

    QString strFilePath( QApplication::applicationDirPath() );
    strFilePath += "/config/gestureprofiles.ini";
    if( QFile::exists( strFilePath ) )
    {
        QFile file( strFilePath );
        if( file.open( QFile::ReadOnly ) )
        {
            while( !file.atEnd() )
            {
                QByteArray bt = file.readLine();
                QString strName( bt.left(bt.indexOf('=')).trimmed() );
                QString strValue( bt.mid(bt.indexOf('=') + 1).trimmed() );

                ui->CBProfileList->addItem( strName, strValue );
            }

            file.close();
        }
    }

    ui->CBProfileList->setCurrentIndex( -1 );

    ui->BtnDelete->setVisible( false );
    ui->BtnAdd->setVisible( true );

    onChangeLanguage();
}

QPredefProfileEditDialog::~QPredefProfileEditDialog()
{
    delete ui;

    if( m_pEditActionWnd )
    {
        m_pEditActionWnd->close();
        delete m_pEditActionWnd;
        m_pEditActionWnd = NULL;
    }
    if( m_pEditActionEWnd )
    {
        m_pEditActionEWnd->close();
        delete m_pEditActionEWnd;
        m_pEditActionEWnd = NULL;
    }
    if( m_pEditAction2WDWnd )
    {
        m_pEditAction2WDWnd->close();
        delete m_pEditAction2WDWnd;
        m_pEditAction2WDWnd = NULL;
    }
    if( m_pEditAction4WDWnd )
    {
        m_pEditAction4WDWnd->close();
        delete m_pEditAction4WDWnd;
        m_pEditAction4WDWnd = NULL;
    }
}

void QPredefProfileEditDialog::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("PREDEFINED PROFILE"), QString::fromUtf8("TITLE_CAPTION_MANAGE")) );
    ui->BtnAdd->setText( Res.getResString(QString::fromUtf8("PREDEFINED PROFILE"), QString::fromUtf8("TEXT_ADD")) );
    ui->BtnDelete->setText( Res.getResString(QString::fromUtf8("PREDEFINED PROFILE"), QString::fromUtf8("TEXT_DELETE")) );
    ui->BtnClose->setText( Res.getResString(QString::fromUtf8("COMMON"), QString::fromUtf8("TEXT_CLOSE")) );
}

void QPredefProfileEditDialog::onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo &ci, ushort nProfileFlags)
{
    switch (ci.keyType)
    {
    default:
        break;
    case QGestureMappingTable::KeyTypeEnable:
        m_pEditActionEWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], nProfileFlags );
        m_pEditActionEWnd->exec();
        break;
    case QGestureMappingTable::KeyType1Key:
        m_pEditActionWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], nProfileFlags );
        m_pEditActionWnd->exec();
        break;
    case QGestureMappingTable::KeyType2Way:
        m_pEditAction2WDWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1] );
        m_pEditAction2WDWnd->exec();
        break;
    case QGestureMappingTable::KeyType4Way:
        m_pEditAction4WDWnd->SetProfileInfo( nProfileIndex, ci.cKey, ci.wKeyValue[0], ci.wKeyValue[1], ci.wKeyValue[2], ci.wKeyValue[3] );
        m_pEditAction4WDWnd->exec();
        break;
    }
}

void QPredefProfileEditDialog::onSendCommand(QString strCmd, bool /*bAsync*/, unsigned short /*nTimeout*/)
{
    ui->GestureProfileTable->parseMouseProfile( strCmd.toUtf8().data() );
}

void QPredefProfileEditDialog::on_CBProfileList_activated(int index)
{
    if( index < 0 ) return;

    QString strV = "=" + ui->CBProfileList->itemData( index ).toString();

    ui->GestureProfileTable->parseMouseProfile( strV.toUtf8().data() );

    ui->LBSelectedProfile->setText( "Edit - " + ui->CBProfileList->currentText() );
    ui->BtnAdd->setText( "Save" );
    ui->BtnDelete->setVisible( true );
}

void QPredefProfileEditDialog::on_BtnAdd_clicked()
{
    QString strProfileName = ui->CBProfileList->currentText();
    if( strProfileName.isEmpty() )
    {
        QMessageBox::warning( this, "Warning", "Please enter profile name.", QMessageBox::Ok );
        return;
    }

    QString strV = ui->GestureProfileTable->mergeMouseProfile();

    QString strFilePath( QApplication::applicationDirPath() );
    strFilePath += "/config/gestureprofiles.ini";
    if( !QFile::exists( strFilePath ) )
        return;

    QFile file( strFilePath );
    if( !file.open( QFile::WriteOnly ) )
        return;

    QTextStream out(&file);
    for( int i=0; i<ui->CBProfileList->count(); i++ )
        out << ui->CBProfileList->itemText(i) + "=" + ui->CBProfileList->itemData(i).toString() + "\n";

    out << strProfileName + "=" + strV;
    ui->CBProfileList->addItem( strProfileName, strV );

    file.close();

    ui->CBProfileList->setCurrentIndex( ui->CBProfileList->findText( strProfileName ) );
    on_CBProfileList_activated( ui->CBProfileList->currentIndex() );
}

void QPredefProfileEditDialog::on_BtnDelete_clicked()
{
    int nIndex = ui->CBProfileList->currentIndex();
    if( nIndex < 0 ) return;

    ui->CBProfileList->removeItem( nIndex );

    QString strFilePath( QApplication::applicationDirPath() );
    strFilePath += "/config/gestureprofiles.ini";
    if( !QFile::exists( strFilePath ) )
        return;

    QFile file( strFilePath );
    if( !file.open( QFile::WriteOnly ) )
        return;

    for( int i=0; i<ui->CBProfileList->count(); i++ )
    {
        QString strV( ui->CBProfileList->itemText(i) );
        strV += "=" + ui->CBProfileList->itemData(i).toString() + "\n";

        file.write( strV.toUtf8().constData() );
    }

    file.close();
}
