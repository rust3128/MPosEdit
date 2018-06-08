#ifndef MYFILEDIALOG_H
#define MYFILEDIALOG_H

#include <QCheckBox>
#include <QFileDialog>
#include <QGridLayout>

class MyFileDialog : public QFileDialog
{
public:
    MyFileDialog();
    bool checkState();
private:
    QCheckBox *checkBox;
};

#endif // MYFILEDIALOG_H
