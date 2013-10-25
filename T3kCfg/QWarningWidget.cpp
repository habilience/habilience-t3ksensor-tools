#include "QWarningWidget.h"
#include "ui_QWarningWidget.h"

#include "stdInclude.h"

#include "Common/nv.h"

QWarningWidget::QWarningWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QWarningWidget)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

    ui->setupUi(this);

    QFont ft( qApp->font() );
#ifdef Q_OS_MAC
    ft.setPointSize( ft.pointSize()+1 );
#endif
    setFont( ft );

    setFixedSize( width(), height() );

    connect( ui->BtnClose, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( ui->BtnContinue, SIGNAL(clicked()), this, SLOT(accept()) );

    SetTextFromLanguage( "WARNING SENSOR DIAGNOSIS", "TITLE_CAPTION", "WARNING SENSOR DIAGNOSIS", "TEXT_MESSAGE" );
    SetBtnTextFromLanguage( "WARNING SENSOR DIAGNOSIS", "BTN_CAPTION_CONTINUE", "WARNING SENSOR DIAGNOSIS", "BTN_CAPTION_CLOSE" );
}

QWarningWidget::~QWarningWidget()
{
    delete ui;
}

void QWarningWidget::SetTextFromLanguage(QString strTitleGroup, QString strTitleText, QString strMsgGroup, QString strMsgText)
{
    m_strTitleGroup = strTitleGroup;
    m_strTitleText = strTitleText;
    m_strMsgGroup = strMsgGroup;
    m_strMsgText = strMsgText;

    onChangeLanguage();
}

void QWarningWidget::SetBtnTextFromLanguage(QString strOKGroup, QString strOKText, QString strCancelGroup, QString strCancelText)
{
    m_strOKGroup = strOKGroup;
    m_strOKText = strOKText;
    m_strCancelGroup = strCancelGroup;
    m_strCancelText = strCancelText;

    onChangeLanguage();
}
#include <QPainter>
void QWarningWidget::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    QRect rc( 0, 0, width(), height() );

    dc.fillRect( rc, Qt::white );

    dc.end();
}

void QWarningWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();
    if( !m_strTitleGroup.isEmpty() && !m_strTitleText.isEmpty() )
        setWindowTitle( Res.getResString(QString::fromUtf8(m_strTitleGroup.toUtf8().data()), QString::fromUtf8(m_strTitleText.toUtf8().data())) );
    if( !m_strMsgGroup.isEmpty() && !m_strMsgText.isEmpty() )
        ui->LBWarning->setText( Res.getResString(QString::fromUtf8(m_strMsgGroup.toUtf8().data()), QString::fromUtf8(m_strMsgText.toUtf8().data())) );
    //m_strTimeout = Res.getResString(QString::fromUtf8("WARNING SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_TIMER_TEXT"));
    if( !m_strOKGroup.isEmpty() && !m_strOKText.isEmpty() )
        ui->BtnContinue->setText( Res.getResString(QString::fromUtf8(m_strOKGroup.toUtf8().data()), QString::fromUtf8(m_strOKText.toUtf8().data())) );
    if( !m_strCancelGroup.isEmpty() && !m_strCancelText.isEmpty() )
        ui->BtnClose->setText( Res.getResString(QString::fromUtf8(m_strCancelGroup.toUtf8().data()), QString::fromUtf8(m_strCancelText.toUtf8().data())) );
}
