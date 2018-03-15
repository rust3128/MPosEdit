#include "logindialog.h"
#include "ui_logindialog.h"

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

    ui->comboBoxUser->setModel(modelUsers);
    ui->comboBoxUser->setModelColumn(1);
    ui->comboBoxUser->setCurrentIndex(-1);

}
