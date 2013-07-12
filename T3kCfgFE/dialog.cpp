#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);
    setFixedSize(this->size());

    setWindowIcon( QIcon(":/T3kCfgFERes/resources/T3kCfgFE.png") );

    ui->btnSelectSensor->setAlignment(QStyleButton::AlignCenter);
    ui->btnSelectSensor->setCaptionFontHeight(15);

    ui->btnReset->setAlignment(QStyleButton::AlignCenter);
    ui->btnReset->setCaptionFontHeight(19);
    ui->btnExit->setAlignment(QStyleButton::AlignCenter);
    ui->btnExit->setCaptionFontHeight(19);

    ui->btnTouchMark->setAlignment(QStyleButton::AlignCenter);
    ui->btnTouchMark->setCaptionFontHeight(15);

    ui->btnSideview->setAlignment(QStyleButton::AlignLeft);
    ui->btnSideview->setCaptionFontHeight(19);
    ui->btnSideview->setAdditionalText( "     - detection_line, light", 1 );
    ui->btnSideview->setMargin( 5, 0, 0, 0 );

    ui->btnDetection->setAlignment(QStyleButton::AlignLeft);
    ui->btnDetection->setCaptionFontHeight(19);
    ui->btnDetection->setAdditionalText( "     - range, threshold, gain", 1 );
    ui->btnDetection->setMargin( 5, 0, 0, 0 );

    ui->btnBentAdjustment->setAlignment(QStyleButton::AlignLeft);
    ui->btnBentAdjustment->setCaptionFontHeight(19);
    ui->btnBentAdjustment->setAdditionalText( "     - angle, calibration", 1 );
    ui->btnBentAdjustment->setMargin( 5, 0, 0, 0 );

    ui->btnTouhSetting->setAlignment(QStyleButton::AlignLeft);
    ui->btnTouhSetting->setCaptionFontHeight(19);
    ui->btnTouhSetting->setAdditionalText( "     - margin, time, area, sensitivity", 1 );
    ui->btnTouhSetting->setMargin( 5, 0, 0, 0 );

    ui->lblSoftlogicInfo->setVisible(false);
    ui->btnReset->setText("Reset");
    ui->btnTouchMark->setVisible(false);
}

Dialog::~Dialog()
{
    delete ui;
}
