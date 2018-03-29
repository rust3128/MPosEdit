#include "connectioneditdialog.h"
#include "ui_connectioneditdialog.h"
#include <QMessageBox>

ConnectionEditDialog::ConnectionEditDialog(int id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionEditDialog)
{
    ui->setupUi(this);
    if(id==-1){
        addNewConnection();
    }
}

ConnectionEditDialog::~ConnectionEditDialog()
{
    delete ui;
}

void ConnectionEditDialog::addNewConnection()
{
    this->setWindowTitle("Новое соединение");
    ui->checkBoxIsCurrent->setChecked(true);
}


void ConnectionEditDialog::on_pushButtonCansel_clicked()
{
    this->reject();
}

void ConnectionEditDialog::on_pushButtonSave_clicked()
{
    if(!validateData()){
        return;
    }


}

bool ConnectionEditDialog::validateData()
{
    if(ui->lineEditName->text().trimmed().length()==0) {
        QMessageBox::warning(this,"Ошибка",
                             "Не указано имя подключения.");
        return false;
    }
    if(ui->lineEditServer->text().trimmed().length()==0) {
        QMessageBox::warning(this,"Ошибка",
                             "Не указан сервер базы данных.");
        return false;
    }
    if(ui->lineEditDataBase->text().trimmed().length()==0) {
        QMessageBox::warning(this,"Ошибка",
                             "Не указан файл базы данных.");
        return false;
    }
    if(ui->lineEditUser->text().trimmed().length()==0) {
        QMessageBox::warning(this,"Ошибка",
                             "Не указан пользователь базы данных.");
        return false;
    }
    if(ui->lineEditPassword->text().trimmed().length()==0) {
        QMessageBox::warning(this,"Ошибка",
                             "Не указан пароль для подключения.");
        return false;
    }
    return true;
}
