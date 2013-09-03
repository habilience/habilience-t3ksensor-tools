#ifndef QEDITGATEWIDGET_H
#define QEDITGATEWIDGET_H

#include "UserDefinedKeyWidget.h"

#include "Softkey.h"
//#include "borderstyleedit.h"

#include <QDialog>
#include <QComboBox>

#define V_KEY_VOLUME_INC_VALUE		0x74
#define V_KEY_VOLUME_DEC_VALUE		0x75
#define V_KEY_VOLUME_MUTE_VALUE		0x76
#define V_KEY_TRANSPORT_PLAY		0x77
#define V_KEY_TRANSPORT_PAUSE		0x78
#define V_KEY_TRANSPORT_FFORWARD	0x79
#define V_KEY_TRANSPORT_REWIND		0x80
#define V_KEY_TRANSPORT_NEXT_TRACK	0x81
#define V_KEY_TRANSPORT_PREV_TRACK	0x82
#define V_KEY_TRANSPORT_STOP		0x83
#define V_KEY_TRANSPORT_EJECT		0x84
#define V_KEY_TRANSPORT_PLAYPAUSE	0x85

#define V_KEY_AL_EMAIL_VIEWER		0x95
#define V_KEY_AL_CALCULATOR			0x96
#define V_KEY_AL_MY_COMPUTER		0x97
#define V_KEY_AL_WEB_BROWSER		0x98

namespace Ui {
class EditGateWidget;
}

//class CLogicDesignerWnd;
class QEditGateWidget : public QDialog
{
    Q_OBJECT

public:
	QEditGateWidget(QWidget* pParent = NULL);
	virtual ~QEditGateWidget();

	void setLogic( CSoftlogic* pSoftlogic, bool bShow );

	CSoftlogic* getLogic() { return m_pSoftlogic; }

    enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

protected:
    //
    virtual void closeEvent(QCloseEvent *);
    virtual void paintEvent(QPaintEvent *);

	void updateItems();
	void updateOutputPort( CSoftlogic::PortType );
	void updateOutputPortType( int nGateType );

	void applyItems();
	void applyHiddenLogics();

    void onUpdateKeyValue( ushort wKeyValue );
    void setEditMode( EditMode Mode, ushort wKeyValue );

    void updateInputCombo( CSoftlogic::Port port, QComboBox& cbInput );

    void displayKeyValue( ushort wKeyValue );

protected:
//	CLogicDesignerWnd*		m_pWndLogicCanvas;
    CSoftlogic*			m_pSoftlogic;
    bool				m_bShowLogic;

    QFont				m_fntFixed;

    int                 m_nGateType;

private:
    Ui::EditGateWidget* ui;

signals:
    void updateLogic();

private slots:
    void on_CBInputPort1_activated(int index);
    void on_CBEnablePort_activated(int index);
    void on_CBInputPort2_activated(int index);
    void on_CBOutputType_activated(int index);
    void on_CBOutputPort_activated(int index);
    void on_CBTrigger_activated(int index);
    void on_CBHidden_activated(int index);
    void on_RBAND_clicked();
    void on_RBANDRisingEdge_clicked();
    void on_RBOR_clicked();
    void on_RBXOR_clicked();

    void onChkInvert(bool bChecked=false);
    void onUpdateItem();
    void onChangePortType();
    void onEditChangedKey(ushort wKeyValue);
    void onEditReportFinished();
};

#endif // QEDITGATEWIDGET_H
