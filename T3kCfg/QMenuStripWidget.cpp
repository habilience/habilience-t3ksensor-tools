#include "QMenuStripWidget.h"
#include "ui_QMenuStripWidget.h"

#include <QLabel>
#include <QPaintEvent>
#include <QPainter>

#include "T3kCfgWnd.h"
#include "QSoftkey.h"

#define QICON_WIDTH             40
#define QICON_HEIGHT            40

QMenuStripWidget::QMenuStripWidget(T30xHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMenuStripWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    m_arybtnMenu = new QRaisePushButton*[QMENU_COUNT];
    m_arybtnMenu[QMENU_HOME] = ui->BtnHome;
    m_arybtnMenu[QMENU_MOUSE] = ui->BtnMouse;
    m_arybtnMenu[QMENU_CALIBRATION] = ui->BtnCali;
    m_arybtnMenu[QMENU_SENSOR] = ui->BtnSensor;
    m_arybtnMenu[QMENU_SOFTKEY] = ui->BtnSoftkey;
    m_arybtnMenu[QMENU_GENERAL] = ui->BtnGeneral;
    m_arybtnMenu[QMENU_REMOTE] = ui->BtnRemote;

    m_pCurObj = m_arybtnMenu[QMENU_HOME];
    m_pNextObj = NULL;

    setFocusPolicy( Qt::NoFocus );

    setGeometry( 0, 0, 620, 50 );

    QPixmap* pIconArray = new QPixmap( ":/T3kCfgRes/Resources/PNG_ICON_MENU_STRIP.png" );

    int nCount = pIconArray->width()/40;
    for( int i=0; i<nCount; i++ )
    {
        m_listIcon.push_back( new QIcon(pIconArray->copy(QRect( i*QICON_WIDTH, 0, QICON_WIDTH, QICON_HEIGHT ) )) );
    }
    delete pIconArray;

    for( int i=0; i<QMENU_COUNT; i++ )
    {
        m_arybtnMenu[i]->setIcon( m_listIcon.at(i)->pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Disabled ) );
    }

    connect( m_arybtnMenu[QMENU_HOME], SIGNAL(clicked()), this, SLOT(OnBtnClickHome()));
    connect( m_arybtnMenu[QMENU_MOUSE], SIGNAL(clicked()), this, SLOT(OnBtnClickMouse()));
    connect( m_arybtnMenu[QMENU_CALIBRATION], SIGNAL(clicked()), this, SLOT(OnBtnClickCali()));
    connect( m_arybtnMenu[QMENU_SENSOR], SIGNAL(clicked()), this, SLOT(OnBtnClickSensor()));
    connect( m_arybtnMenu[QMENU_GENERAL], SIGNAL(clicked()), this, SLOT(OnBtnClickGeneral()));
    connect( m_arybtnMenu[QMENU_SOFTKEY], SIGNAL(clicked()), this, SLOT(OnBtnClickSoftkey()));

    ui->BtnRemote->setVisible( false );
    ui->BtnSoftkey->setVisible( false );

    OnChangeLanguage();
}

QMenuStripWidget::~QMenuStripWidget()
{
    for( int i=0; i<m_listIcon.size(); i++ )
    {
        if( m_listIcon.at(i) )
        {
            delete m_listIcon.at(i);
            m_listIcon.removeAt(i);
        }
    }
    m_listIcon.clear();

    for( int i=0; i<QMENU_COUNT; i++ )
    {
        if( m_arybtnMenu[i] )
        {
            delete m_arybtnMenu[i];
            m_arybtnMenu[i] = NULL;
        }
    }

    if( m_arybtnMenu )
    {
        delete[] m_arybtnMenu;
        m_arybtnMenu = NULL;
    }

    delete ui;
}

void QMenuStripWidget::ClickMenu(int nMenu)
{
    if( nMenu < 0 || nMenu >= QMENU_COUNT ) return;
    if( m_pCurObj == m_arybtnMenu[nMenu] ) return;

    emit ShowMenuEvent( nMenu );
}

void QMenuStripWidget::showEvent(QShowEvent *evt)
{
    if( evt->type() == QEvent::Show )
    {
        setFocusPolicy( Qt::NoFocus );

        m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrSoftkey).toUtf8().data(), true );
    }
    QWidget::showEvent(evt);
}

void QMenuStripWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    m_arybtnMenu[QMENU_HOME]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_START_PAGE") ) );
    m_arybtnMenu[QMENU_MOUSE]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_MOUSE") ) );
    m_arybtnMenu[QMENU_CALIBRATION]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_CALIBRATION") ) );
    m_arybtnMenu[QMENU_SENSOR]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_SENSOR_SETTING") ) );
    m_arybtnMenu[QMENU_SOFTKEY]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_SOFTKEY_SETTING") ) );
    m_arybtnMenu[QMENU_GENERAL]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_GENERAL_SETTING") ) );
    m_arybtnMenu[QMENU_REMOTE]->setToolTip( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MENU_TOOLTIP_REMOTE_SUPPORT") ) );
}

void QMenuStripWidget::OnRSP(ResponsePart /*Part*/, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !winId() ) return;

    if( strstr(sCmd, cstrSoftkey) == sCmd )
    {
        QString strSoftKey( sCmd );

        int nE = strSoftKey.indexOf( '=' );
        if( nE >= 0 )
        {
            strSoftKey.remove( 0, nE+1 );
            ui->BtnSoftkey->setVisible( strSoftKey.size() ? true : false );
        }
    }
}

void QMenuStripWidget::OnBtnClickHome()
{
    ClickMenu( QMENU_HOME );
}

void QMenuStripWidget::OnBtnClickMouse()
{
    ClickMenu( QMENU_MOUSE );
}

void QMenuStripWidget::OnBtnClickCali()
{
    ClickMenu( QMENU_CALIBRATION );
}

void QMenuStripWidget::OnBtnClickSensor()
{
    ClickMenu( QMENU_SENSOR );
}

void QMenuStripWidget::OnBtnClickGeneral()
{
    ClickMenu( QMENU_GENERAL );
}

void QMenuStripWidget::OnBtnClickSoftkey()
{
    ClickMenu( QMENU_SOFTKEY );
}

void QMenuStripWidget::SetMenuButton(int nIndex)
{
    switch( nIndex )
    {
    case QMENU_HOME:
        m_arybtnMenu[nIndex]->setIcon( m_listIcon.at(0)->pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );
        if( m_pCurObj != m_arybtnMenu[QMENU_HOME] && m_pCurObj->inherits("QToolButton") )
        {
            QToolButton* pPrev = (QToolButton*)m_pCurObj;
            pPrev->setIcon(QIcon( pPrev->icon() ).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Disabled ) );
            pPrev->setIconSize( QSize(32,32) );
        }
        m_pCurObj = m_arybtnMenu[nIndex];
        break;

    case QMENU_MOUSE:
    case QMENU_CALIBRATION:
    case QMENU_SENSOR:
    case QMENU_GENERAL:
    case QMENU_SOFTKEY:
        m_arybtnMenu[nIndex]->setIcon( m_listIcon.at(nIndex)->pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Normal ) );
        m_arybtnMenu[nIndex]->setIconSize( QSize(40,40) );
        if( m_pCurObj != m_arybtnMenu[QMENU_HOME] && m_pCurObj->inherits("QToolButton") )
        {
            QToolButton* pPrev = (QToolButton*)m_pCurObj;
            pPrev->setIcon(QIcon( pPrev->icon() ).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Disabled ) );
            pPrev->setIconSize( QSize(32,32) );
        }
        m_pCurObj = m_arybtnMenu[nIndex];
        break;

    default:
        return;
    }
}
