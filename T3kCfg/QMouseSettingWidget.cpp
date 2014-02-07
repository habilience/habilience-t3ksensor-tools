#include "QMouseSettingWidget.h"

#include <QApplication>

#include "QT3kUserData.h"


QMouseSettingWidget::QMouseSettingWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QWidget(parent), m_pT3kHandle(pHandle)
{
    setFont( qApp->font() );

    m_pOldMouseProfile = new QOMouseProfileWidget( m_pT3kHandle, this );
    m_pNewMouseProfile = new QNMouseProfileWidget( m_pT3kHandle, this );

    connect( m_pOldMouseProfile, &QOMouseProfileWidget::ByPassKeyPressEvent, this, &QMouseSettingWidget::ByPassKeyPressEvent, Qt::DirectConnection );
    connect( m_pNewMouseProfile, &QNMouseProfileWidget::ByPassKeyPressEvent, this, &QMouseSettingWidget::ByPassKeyPressEvent, Qt::DirectConnection );

    m_pOldMouseProfile->hide();
    m_pNewMouseProfile->hide();
}

QMouseSettingWidget::~QMouseSettingWidget()
{
}

void QMouseSettingWidget::SetDefault()
{
    if( QT3kUserData::GetInstance()->getFirmwareVersionStr() <= "2.8a" )
        m_pOldMouseProfile->SetDefault();
    else
        m_pNewMouseProfile->setDefault();
}

void QMouseSettingWidget::Refresh()
{
    if( QT3kUserData::GetInstance()->getFirmwareVersionStr() <= "2.8a" )
        m_pOldMouseProfile->Refresh();
    else
        m_pNewMouseProfile->refresh();
}

void QMouseSettingWidget::ReplaceLabelName(QCheckableButton *pBtn)
{
    m_pOldMouseProfile->ReplaceLabelName( pBtn );
}

void QMouseSettingWidget::showEvent(QShowEvent *evt)
{
    if( QT3kUserData::GetInstance()->getFirmwareVersionStr() <= "2.8a" )
    {
        m_pOldMouseProfile->show();
        m_pNewMouseProfile->hide();
    }
    else
    {
        m_pOldMouseProfile->hide();
        m_pNewMouseProfile->show();
    }

    setFocusPolicy( Qt::StrongFocus );

    QWidget::showEvent(evt);
}
