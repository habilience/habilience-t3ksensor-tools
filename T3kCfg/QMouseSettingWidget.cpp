#include "QMouseSettingWidget.h"

#include <QApplication>

#include "stdInclude.h"
#include "QT3kUserData.h"


QMouseSettingWidget::QMouseSettingWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QWidget(parent), m_pT3kHandle(pHandle)
{
    setFont( qApp->font() );

    m_pOldMouseProfile = new QOMouseProfileWidget( m_pT3kHandle, this );
    m_pNewMouseProfile = new QNMouseProfileWidget( m_pT3kHandle, this );

    connect( m_pOldMouseProfile, &QOMouseProfileWidget::ByPassKeyPressEvent, this, &QMouseSettingWidget::ByPassKeyPressEvent, Qt::DirectConnection );
    connect( m_pNewMouseProfile, &QNMouseProfileWidget::ByPassKeyPressEvent, this, &QMouseSettingWidget::ByPassKeyPressEvent, Qt::DirectConnection );

    connect( this, &QMouseSettingWidget::enableMacOSXGesture, m_pNewMouseProfile, &QNMouseProfileWidget::onEnableMacOSXGesture );

    m_pOldMouseProfile->hide();
    m_pNewMouseProfile->hide();
}

QMouseSettingWidget::~QMouseSettingWidget()
{
    if (m_pOldMouseProfile)
    {
        delete m_pOldMouseProfile;
        m_pOldMouseProfile = NULL;
    }
    if (m_pNewMouseProfile)
    {
        delete m_pNewMouseProfile;
        m_pNewMouseProfile = NULL;
    }
}

void QMouseSettingWidget::SetDefault()
{
    QString strVer = QT3kUserData::GetInstance()->getFirmwareVersionStr();
    int nExtraVer = strVer.mid( strVer.indexOf( '.' )+2, 1 ).toInt(0, 16);
    if( (nExtraVer >= 0x0A && nExtraVer <= 0x0F) )
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE2 )
            m_pOldMouseProfile->SetDefault();
        else
            m_pNewMouseProfile->setDefault();
    }
    else
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE1 )
            m_pOldMouseProfile->SetDefault();
        else
            m_pNewMouseProfile->setDefault();
    }
}

void QMouseSettingWidget::Refresh()
{
    QString strVer = QT3kUserData::GetInstance()->getFirmwareVersionStr();
    int nExtraVer = strVer.mid( strVer.indexOf( '.' )+2, 1 ).toInt(0, 16);
    if( (nExtraVer >= 0x0A && nExtraVer <= 0x0F) )
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE2 )
            m_pOldMouseProfile->Refresh();
        else
            m_pNewMouseProfile->refresh();
    }
    else
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE1 )
            m_pOldMouseProfile->Refresh();
        else
            m_pNewMouseProfile->refresh();
    }
}

void QMouseSettingWidget::ReplaceLabelName(QCheckableButton *pBtn)
{
    m_pOldMouseProfile->ReplaceLabelName( pBtn );
}

void QMouseSettingWidget::showEvent(QShowEvent *evt)
{
    QString strVer = QT3kUserData::GetInstance()->getFirmwareVersionStr();
    int nExtraVer = strVer.mid( strVer.indexOf( '.' )+2, 1 ).toInt(0, 16);
    if( (nExtraVer >= 0x0A && nExtraVer <= 0x0F) )
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE2 )
        {
            m_pOldMouseProfile->show();
            m_pNewMouseProfile->hide();
        }
        else
        {
            m_pOldMouseProfile->hide();
            m_pNewMouseProfile->show();
        }
    }
    else
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE1 )
        {
            m_pOldMouseProfile->show();
            m_pNewMouseProfile->hide();
        }
        else
        {
            m_pOldMouseProfile->hide();
            m_pNewMouseProfile->show();
        }
    }

    setFocusPolicy( Qt::StrongFocus );

    QWidget::showEvent(evt);
}
