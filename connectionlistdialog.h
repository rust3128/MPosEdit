#ifndef CONNECTIONLISTDIALOG_H
#define CONNECTIONLISTDIALOG_H

#include <QDialog>
#include "tanlemodelconnect.h"

namespace Ui {
class ConnectionListDialog;
}

class ConnectionListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionListDialog(QWidget *parent = 0);
    ~ConnectionListDialog();

private slots:
    void on_pbNew_clicked();

private:
    void createUI();
private:
    Ui::ConnectionListDialog *ui;
//    QSqlTableModel *modelConnect;
    TanleModelConnect *modelConnect;
    QSqlDatabase dblite,db;
};

#endif // CONNECTIONLISTDIALOG_H
