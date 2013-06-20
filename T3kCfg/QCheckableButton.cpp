#include "QCheckableButton.h"

#include "QMouseSettingWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QLineEdit>
#include <QApplication>

QCheckableButton::QCheckableButton(QWidget *parent) :
    QPushButton(parent)
{
    QFont ft( parent->font() );
#ifdef Q_OS_MAC
    ft.setPixelSize( 1 );
#endif
    setFont( ft );

    m_bChecked = false;
    m_bCheckBox = false;
    m_bSubCheckBox = false;
    m_bHover = false;

    m_nCheckIndex = 0;

    clrBoundary.setRgb(112,112,112);
    clrFillTop.setRgb(239,239,239);
    clrFillbtm.setRgb(214,214,214);
    clrCheckBox.setRgb(112,112,112);

    setAttribute( Qt::WA_Hover );
    setAutoExclusive( true );
    setMouseTracking( true );

    installEventFilter( this );

    m_pTextEdit = new QLineEdit( this );
    m_pTextEdit->setAttribute( Qt::WA_DeleteOnClose );
    m_pTextEdit->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_pTextEdit->setFont( parent->font() );
    m_pTextEdit->hide();

    m_pTextEdit->installEventFilter( this );
    connect( m_pTextEdit, SIGNAL(returnPressed()), this, SLOT(OnEditEnter()) );
}

QCheckableButton::~QCheckableButton()
{
    if( m_pTextEdit )
    {
        delete m_pTextEdit;
        m_pTextEdit = NULL;
    }
}

void QCheckableButton::paintEvent(QPaintEvent */*event*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rcButton( 0, 0, width()-1, height()-1 );
    rcButton.adjust( 1,1,-1,-1);

    QPen penRect( Qt::SolidLine );
    penRect.setWidth( 1 );
    penRect.setColor( clrBoundary );
    dc.setPen( penRect );

    dc.fillRect( rcButton.left(), rcButton.top(), rcButton.width(), rcButton.height()/2, clrFillTop );
    dc.fillRect( rcButton.left(), rcButton.top()+rcButton.height()/2, rcButton.width(), rcButton.height()/2, clrFillbtm );

    dc.drawRoundedRect( rcButton, 4.f, 4.f );
    rcButton.adjust( 1,1,-1,-1 );
    penRect.setColor( QColor(255,255,255,128) );
    dc.setPen( penRect );
    dc.drawRoundedRect( rcButton, 4.f, 4.f );

    bool bR2L = QApplication::layoutDirection() == Qt::RightToLeft;
    QRect rcCheckBox( rcButton );
    if( bR2L )
        rcCheckBox.setLeft( rcButton.right()-rcButton.height() );
    else
        rcCheckBox.setRight( rcButton.left()+rcButton.height() );

    penRect.setWidth( 3 );
    dc.setPen( penRect );
    if( bR2L )
        dc.drawLine( rcCheckBox.left(), rcCheckBox.top()+1, rcCheckBox.left(), rcCheckBox.bottom()-1 );
    else
        dc.drawLine( rcCheckBox.right(), rcCheckBox.top()+1, rcCheckBox.right(), rcCheckBox.bottom()-1 );

    penRect.setColor( QColor(0,0,0,128) );
    penRect.setWidth( 1 );
    dc.setPen( penRect );
    if( bR2L )
        dc.drawLine( rcCheckBox.left(), rcCheckBox.top()+1, rcCheckBox.left(), rcCheckBox.bottom()-1 );
    else
        dc.drawLine( rcCheckBox.right(), rcCheckBox.top()+1, rcCheckBox.right(), rcCheckBox.bottom()-1 );

    if( m_rcCheckBox.isEmpty() )
        m_rcCheckBox = rcCheckBox;
    rcCheckBox.adjust( 1,1,-1,-1 );//rcCheckBox.width()/5, rcCheckBox.height()/5, -rcCheckBox.width()/5, -rcCheckBox.height()/5 );

    dc.setRenderHint( QPainter::Antialiasing );

    if( m_bCheckBox && m_bSubCheckBox )
    {
        QRect rcArea( rcCheckBox );

        if( m_nCheckIndex == 1 )
        {
            dc.drawImage( rcArea, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MUTI.png") );
            dc.drawImage( rcArea, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MOUSE_OVER.png") );
        }
        else if( m_nCheckIndex == 2 )
        {
            dc.drawImage( rcArea, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MOUSE.png") );
            dc.drawImage( rcArea, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MUTI_OVER.png") );
        }
    }
    else if( m_bCheckBox )
    {
        QRect rcMark( rcCheckBox );
        if( m_nCheckIndex == 1 )
            dc.drawImage( rcMark, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MOUSE.png") );
        else if( m_nCheckIndex == 2 )
            dc.drawImage( rcMark, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MUTI") );
    }
    else if( m_bSubCheckBox )
    {
        QRect rcMark( rcCheckBox );
        if( m_nCheckIndex == 2 )
            dc.drawImage( rcMark, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MOUSE_2ND.png") );
        else if( m_nCheckIndex == 1 )
            dc.drawImage( rcMark, QImage(":/T3kCfgRes/Resources/PNG_PROFILE_MUTI_2ND.png") );
    }

    QFont ftText( font() );
#if defined(Q_OS_WIN)
    ftText.setPointSizeF( ftText.pointSizeF()-1.5 );
#elif defined(Q_OS_MAC)
    ftText.setPointSizeF( ftText.pointSizeF()-2.0 );
#else
    ftText.setPointSizeF( ftText.pointSizeF()-2.5 );
#endif

    ftText.setBold( true );
    dc.setFont( ftText );
    dc.setPen( Qt::SolidLine );

    QRect rcText( rcButton );
    if( bR2L )
        rcText.adjust( 0,0, -rcButton.height(),0);
    else
        rcText.adjust( rcButton.height(),0,0,0);

    dc.drawText( rcText , Qt::AlignVCenter | Qt::AlignHCenter, text() );

    if( !isEnabled() )
    {
        dc.fillRect( 0, 0, width()-1, height()-1, QColor(255,255,255,128));
    }

    dc.end();
}

bool QCheckableButton::eventFilter(QObject *target, QEvent *event)
{
    if( target == this )
    {
        if( isEnabled() && !IsChecked() )
        {
            if (event->type() == QEvent::HoverLeave )
            {
                ChangeButtonMode( BM_OFF );
                m_bHover = false;
            }
            else if (event->type() == QEvent::HoverEnter)
            {
                ChangeButtonMode( BM_HOVER );
                m_bHover = true;
            }
        }
    }

    if( target == m_pTextEdit )
    {
        if( event->type() == QEvent::FocusOut )
        {
            m_pTextEdit->hide();
        }
    }

    return QObject::eventFilter(target, event);
}

void QCheckableButton::mousePressEvent(QMouseEvent *event)
{
    QObject* pObj = parent()->parent();
    if( event->type() == QEvent::MouseButtonDblClick )
    {
        if( pObj->inherits( "QMouseSettingWidget" ) )
        {
            QMouseSettingWidget* pMS = (QMouseSettingWidget*)pObj;
            QList<QCheckableButton*> pChildren = pMS->findChildren<QCheckableButton*>();
            QCheckableButton* pChild = NULL;

            foreach( pChild, pChildren )
            {
                pChild->HideEditBox();
            }
        }
        m_pTextEdit->setGeometry( 0, 0, width()-1, height()-1 );
        m_pTextEdit->setText( text() );
        m_pTextEdit->setFocus();
        m_pTextEdit->selectAll();
        m_pTextEdit->show();

        QPushButton::mousePressEvent(event);
        return;
    }

    if( pObj->inherits( "QMouseSettingWidget" ) )
    {
        QMouseSettingWidget* pMS = (QMouseSettingWidget*)pObj;
        QList<QCheckableButton*> pChildren = pMS->findChildren<QCheckableButton*>();
        QCheckableButton* pChild = NULL;

        if( m_rcCheckBox.contains(event->pos()) )
        {
            foreach( pChild, pChildren )
            {
                pChild->HideEditBox();
                if( pChild->IsCheckBox() )
                {
                    pChild->SetCheckBox( false );
                    pChild->update();
                }
            }
            SetCheckBox( true );
            update();
        }

        foreach( pChild, pChildren )
        {
            pChild->HideEditBox();
            if( pChild->IsChecked() )
            {
                pChild->ChangeButtonMode( BM_OFF );
                qDebug( "%s:%d", (const char*)pChild->text().toUtf8().data(), 0 );
                pChild->SetChecked( false );
                pChild->update();
            }
        }
        ChangeButtonMode( BM_ON );
        qDebug( "%s:%d", (const char*)text().toUtf8().data(), 1 );
        SetChecked(true);
        update();
    }

    QPushButton::mousePressEvent(event);
}

void QCheckableButton::ChangeButtonMode(eButtonMode eBM)
{
    switch( eBM )
    {
    case BM_OFF:
        clrBoundary.setRgb(112,112,112);
        clrFillTop.setRgb(239,239,239);
        clrFillbtm.setRgb(214,214,214);
        break;
    case BM_ON:
        clrBoundary.setRgb(44,98,139);
        clrFillTop.setRgb(216,238,250);
        clrFillbtm.setRgb(120,189,226);
        break;
    case BM_HOVER:
        clrBoundary.setRgb(60,127,177);
        clrFillTop.setRgb(225,243,252);
        clrFillbtm.setRgb(178,224,249);
        break;
    default:
        break;
    }    
}

void QCheckableButton::SetChecked(bool bCheck)
{
    m_bChecked = bCheck;
}

void QCheckableButton::SetCheckBox(bool bCheck)
{
    if( bCheck )
        clrCheckBox.setRgb(85,134,163);
    else
        clrCheckBox.setRgb(112,112,112);
    m_bCheckBox = bCheck;
}

void QCheckableButton::SetSubCheckBox(bool bCheck)
{
    m_bSubCheckBox = bCheck;
}

void QCheckableButton::SetCheckIndex(int nIndex)
{
    m_nCheckIndex = nIndex;
}

void QCheckableButton::HideEditBox()
{
    if( m_pTextEdit )
        m_pTextEdit->hide();
}

bool QCheckableButton::IsShowEditBox()
{
    if( m_pTextEdit )
    {
        return m_pTextEdit->isVisible();
    }
    return false;
}

void QCheckableButton::OnEditEnter()
{
    setText( m_pTextEdit->text() );
    m_pTextEdit->hide();

    if( parent()->parent()->inherits("QMouseSettingWidget") )
    {
        QMouseSettingWidget* pWidget = (QMouseSettingWidget*)parent();
        pWidget->ReplaceLabelName( this );
    }
}
