#include "QPartCheckBox.h"

QPartCheckBox::QPartCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    connect( this, &QCheckBox::toggled, this, &QPartCheckBox::onToggledPart );
}

void QPartCheckBox::onToggledPart(bool bChecked)
{
    emit togglePart( objectName(), bChecked );
}
