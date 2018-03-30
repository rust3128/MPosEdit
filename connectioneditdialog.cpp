#include "connectioneditdialog.h"
#include "ui_connectioneditdialog.h"
#include "loggingcategories.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

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
    if(!connectCentralDB()){
        return;
    }
    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);

    QString strSQL = QString("INSERT INTO `connections` (`conn_name`,`conn_host`,`conn_db`,`conn_user`,`conn_pass`,`conn_curr`) "
                             "VALUES ('%1','%2','%3','%4','%5','%6')")
            .arg(ui->lineEditName->text().trimmed())
            .arg(ui->lineEditServer->text().trimmed())
            .arg(ui->lineEditDataBase->text().trimmed())
            .arg(ui->lineEditUser->text().trimmed())
            .arg(ui->lineEditPassword->text().trimmed())
            .arg('1');
    if(!q.exec(strSQL)) {
        qCritical(logCritical()) << "Не возможно добавить подключение." << q.lastError().text();
        QMessageBox::critical(this,"Ошибка", QString("Не возможно открыть базу данных!\n%1\nПроверьте настройку подключения.")
                              .arg(q.lastError().text()));
        return;
    }
    emit sendConnectionName(ui->lineEditName->text().trimmed()+" "+ui->lineEditServer->text().trimmed()+":"+ui->lineEditDataBase->text().trimmed());
    this->accept();

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

bool ConnectionEditDialog::connectCentralDB()
{
    QSqlDatabase dbcentr = QSqlDatabase::addDatabase("QIBASE","central");

    dbcentr.setHostName(ui->lineEditServer->text().trimmed());
    dbcentr.setDatabaseName(ui->lineEditDataBase->text().trimmed());
    dbcentr.setUserName(ui->lineEditUser->text().trimmed());
    dbcentr.setPassword(ui->lineEditPassword->text().trimmed());

    if(!dbcentr.open()) {
        qCritical(logCritical()) << "Не возможно подключится к центральной базе данных" << dbcentr.lastError().text();
        QMessageBox::critical(0,"Ошибка подключения", QString("Не возможно открыть базу данных!\n%1\nПроверьте настройку подключения.")
                .arg(dbcentr.lastError().text()),
                QMessageBox::Ok);
        return false;
    }
    return true;

}
