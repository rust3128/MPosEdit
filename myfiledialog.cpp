#include "myfiledialog.h"

MyFileDialog::MyFileDialog()
{
    this->setOption(QFileDialog::DontUseNativeDialog);
    QGridLayout* mainLayout = qobject_cast<QGridLayout*>(this->layout());
    checkBox = new QCheckBox(tr("Просмотреть скрипт"));
    mainLayout->addWidget(checkBox, 4, 1);
    this->setNameFilter("*.sql");
}

bool MyFileDialog::checkState(){
    if(this->checkBox->checkState())
        return 1;
    else
        return 0;
}
