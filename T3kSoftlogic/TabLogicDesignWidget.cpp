#include "TabLogicDesignWidget.h"
#include "ui_TabLogicDesignWidget.h"

#include "T3kSoftlogicDlg.h"
#include "T3kConstStr.h"
#include "T3k_ver.h"

#include <QDesktopWidget>
#include <QMessageBox>


TabLogicDesignWidget::TabLogicDesignWidget(QWidget* parent /*=NULL*/) :
    QWidget(parent),
    ui(new Ui::TabLogicDesignWidget)
{
    ui->setupUi(this);

    connect( &m_LogicDesigner, &QLogicDesignWidget::closeWidget, this, &TabLogicDesignWidget::updatePreview );
}

TabLogicDesignWidget::~TabLogicDesignWidget()
{
    delete ui;

    if( m_LogicDesigner.isVisible() )
        m_LogicDesigner.close();
}

void TabLogicDesignWidget::OnFirmwareDownload(bool)
{
    if( !isVisible() ) return;

    if( isValidT3kSensorState() )
        ui->BtnApply->setEnabled( true );
	else
        ui->BtnApply->setEnabled( false );
}

void TabLogicDesignWidget::showEvent(QShowEvent *)
{
    if( isValidT3kSensorState() )
        ui->BtnApply->setEnabled( true );
    else
        ui->BtnApply->setEnabled( false );
}

void TabLogicDesignWidget::closeEvent(QCloseEvent *)
{
    if( m_LogicDesigner.isVisible() )
        m_LogicDesigner.close();
}

bool TabLogicDesignWidget::verifyGPIO( int &nSensorGPIOCount )
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    T3kHandle* pT3kHandle = getT3kHandle();

	m_nSensorGPIOCount = 0;
	char szCmd[256];
	sprintf( szCmd, "%s?", cstrFactorialGPIO );
	int nRetry = 0;
	do
	{
        if ( pT3kHandle->SendCommand( szCmd, false ) )
			break;
		nRetry++;
	} while( nRetry < 3 );

	nSensorGPIOCount = m_nSensorGPIOCount;
    if ( Keys.getGPIOCount() > nSensorGPIOCount )
	{
        return false;
	}

    return true;
}

void TabLogicDesignWidget::OnOpenT3kDevice(T3K_HANDLE)
{
    if( !isVisible() ) return;

    ui->BtnApply->setEnabled( true );
}

void TabLogicDesignWidget::OnCloseT3kDevice(T3K_HANDLE)
{
    if( !isVisible() ) return;

    ui->BtnApply->setEnabled( false );
}

void TabLogicDesignWidget::OnRSP(ResponsePart Part, ushort, const char *, long, bool, const char *szCmd)
{
    if( !isVisible() ) return;

	if ( strstr(szCmd, cstrFirmwareVersion) == szCmd )
	{		
		switch( Part )
		{
		case MM:
			{
				float fFirmwareVersion = (float)atof(szCmd + sizeof(cstrFirmwareVersion) - 1);
				float fMinDesireFW = (float)MM_MIN_FIRMWARE_VERSION;
				float fMaxDesireFW = (float)MM_NEXT_FIRMWARE_VERSION;
				if( (fFirmwareVersion < fMinDesireFW) || (fFirmwareVersion >= fMaxDesireFW) )
                    ui->BtnApply->setEnabled( false );
			}
			break;
        default:
            break;
		}
	}
	else if ( strstr(szCmd, cstrFactorialGPIO) == szCmd )
	{
		const char* buf = szCmd + sizeof(cstrFactorialGPIO) - 1;
		int nLen = (int)strlen( buf );
		m_nSensorGPIOCount = nLen / 2;
	}
}

bool TabLogicDesignWidget::writeToSensor( bool bLogicOnly )
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

    T3kHandle* pT3kHandle = getT3kHandle();

    bool bOK = false;

    if( pT3kHandle )
	{
        QString strRet;
        QString strExtra;
        QString strCmd;
		int nRetry = 0;

		do
		{
            bOK = true;

			if( !bLogicOnly )
			{
                strRet = Keys.save( strExtra, NULL );
                strCmd = QString("%1%2").arg(cstrFactorialSoftkey).arg(strRet);
                if( !pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false ) )
				{
                    bOK = false;
					nRetry++;
					continue;
				}
			}

            strRet = Keys.saveBindInfo();
            strCmd = QString("%1%2").arg(cstrFactorialSoftkeyBind).arg(strRet);
            if( !pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strRet = Keys.saveGPIOInfo();
            strCmd = QString("%1%2").arg(cstrFactorialGPIO).arg(strRet);
            if ( !pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strRet = Logics.save(NULL);
            strCmd = QString("%1%2").arg(cstrFactorialSoftlogic).arg(strRet);
            if( !pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strCmd = QString("%1*").arg(cstrSoftlogic);
            if( !pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

		} while( !bOK && (nRetry < 3) );
	}

	return bOK;
}

void TabLogicDesignWidget::on_BtnLogicdesign_clicked()
{
    QDesktopWidget desktop;
    QRect rcSelMonitor( desktop.screenGeometry(desktop.primaryScreen()) );

    m_LogicDesigner.setGeometry( rcSelMonitor );
    m_LogicDesigner.show();
}

void TabLogicDesignWidget::on_BtnApply_clicked()
{
    if( QMessageBox::warning( this, "Warning", "Would you like to apply softlogic to sensor?", QMessageBox::Yes|QMessageBox::No ) == QMessageBox::No )
        return;

    int nSensorGPIOCount;
    if ( !verifyGPIO( nSensorGPIOCount ) )
    {
        QString strMsg = QString("It's different from the number of GPIO which the Sensor supports.\nSensor supports: %1").arg(nSensorGPIOCount);
        QMessageBox::critical( this, "Error", strMsg, QMessageBox::Ok );
        return;
    }

    if( writeToSensor( true ) )
    {
        QMessageBox::information( this, "Information", "Save Complete", QMessageBox::Ok );
    }
    else
    {
        QMessageBox::critical( this, "Error", "Save Failure", QMessageBox::Ok );
    }
}
