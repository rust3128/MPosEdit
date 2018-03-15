#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    void createUI();
private:
    Ui::LoginDialog *ui;
    QSqlTableModel *modelUsers;         //Модель таблицы пользователя
};

#endif // LOGINDIALOG_H
