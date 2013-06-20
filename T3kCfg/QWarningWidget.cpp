#include "QWarningWidget.h"
#include "ui_QWarningWidget.h"

#include "stdInclude.h"

#ifdef Q_OS_WIN
#include "../../WindowApp/Common/nv.h"
#else
#include "Common/nv.h"
#endif

QWarningWidget::QWarningWidget(T30xHandle*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QWarningWidget), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

    ui->setupUi(this);

    QFont ft( parent->font() );
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

    OnChangeLanguage();
}

void QWarningWidget::SetBtnTextFromLanguage(QString strOKGroup, QString strOKText, QString strCancelGroup, QString strCancelText)
{
    m_strOKGroup = strOKGroup;
    m_strOKText = strOKText;
    m_strCancelGroup = strCancelGroup;
    m_strCancelText = strCancelText;

    OnChangeLanguage();
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

void QWarningWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    if( !m_strTitleGroup.isEmpty() && !m_strTitleText.isEmpty() )
        setWindowTitle( Res.GetResString(QString::fromUtf8(m_strTitleGroup.toUtf8().data()), QString::fromUtf8(m_strTitleText.toUtf8().data())) );
    if( !m_strMsgGroup.isEmpty() && !m_strMsgText.isEmpty() )
        ui->LBWarning->setText( Res.GetResString(QString::fromUtf8(m_strMsgGroup.toUtf8().data()), QString::fromUtf8(m_strMsgText.toUtf8().data())) );
    //m_strTimeout = Res.GetResString(QString::fromUtf8("WARNING SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_TIMER_TEXT"));
    if( !m_strOKGroup.isEmpty() && !m_strOKText.isEmpty() )
        ui->BtnContinue->setText( Res.GetResString(QString::fromUtf8(m_strOKGroup.toUtf8().data()), QString::fromUtf8(m_strOKText.toUtf8().data())) );
    if( !m_strCancelGroup.isEmpty() && !m_strCancelText.isEmpty() )
        ui->BtnClose->setText( Res.GetResString(QString::fromUtf8(m_strCancelGroup.toUtf8().data()), QString::fromUtf8(m_strCancelText.toUtf8().data())) );
}
