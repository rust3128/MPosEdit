#include "logindialog.h"
#include "ui_logindialog.h"
#include "loggingcategories.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    createUI();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::createUI()
{
    ui->labelInfo->clear();
    QSqlDatabase dblite = QSqlDatabase::database("options");

    modelUsers = new QSqlTableModel(this,dblite);
    modelUsers->setTable("users");
    modelUsers->select();
    modelUsers->setFilter("user_active=1");

    ui->comboBoxUser->setModel(modelUsers);
    ui->comboBoxUser->setModelColumn(1);
    ui->comboBoxUser->setCurrentIndex(-1);

}

void LoginDialog::on_comboBoxUser_activated(int idx)
{
    userPass.clear();
    idxModel = modelUsers->index(idx,0,QModelIndex());
    userID = modelUsers->data(modelUsers->index(idx,0)).toInt();
    userPass = modelUsers->data(modelUsers->index(idx,3)).toString();
}


void LoginDialog::on_pushButtonLogin_clicked()
{
    if(ui->comboBoxUser->currentIndex()>=0) {
        if(userPass==ui->lineEditPass->text().trimmed()) {
            qInfo(logInfo()) << QString("Пользователь: %1. Успешный вход в систему.").arg(ui->comboBoxUser->currentText());
            this->accept();
        } else {
            ui->labelInfo->setText("Не верный пароль!");
            qWarning(logInfo()) << QString("Пользователь: %1. Не верный пароль!.").arg(ui->comboBoxUser->currentText());
            ui->lineEditPass->clear();
        }
    }
}

void LoginDialog::on_pushButtonCancel_clicked()
{
    this->reject();
}

QSqlRecord LoginDialog::getUserData()
{
    return modelUsers->record(idxModel.row());
}
