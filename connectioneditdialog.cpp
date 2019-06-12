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
    connID=id;
    if(id==-1){
        connNew = true;
        addNewConnection();
    } else {
        connNew = false;
        editConnections(id);
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

    QString strSQL;
    if(!validateData()){
        return;
    }
    if(connNew) {
        newConnSave();
    } else {
        updateConn();
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

void ConnectionEditDialog::editConnections(int ID)
{
    this->setWindowTitle("Редактирование подключения");

    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);
    q.prepare("SELECT * FROM connections where conn_id = :id");
    q.bindValue(":id",ID);
    if(!q.exec()) {
        qCritical(logCritical()) << "Не возможно получить данные о подключении." << q.lastError().text();
        QMessageBox::critical(this,"Ошибка", QString("Не возможно получить данные о подключении."));
        return;
    }
    q.next();
    ui->lineEditName->setText(q.value("conn_name").toString());
    ui->lineEditServer->setText(q.value("conn_host").toString());
    ui->lineEditDataBase->setText(q.value("conn_db").toString());
    ui->lineEditUser->setText(q.value("conn_user").toString());
    ui->lineEditPassword->setText(q.value("conn_pass").toString());
    if(q.value("conn_curr").toInt() == 1){
        ui->checkBoxIsCurrent->setChecked(true);
    } else {
        ui->checkBoxIsCurrent->setChecked(false);
    }
    q.finish();
}

void ConnectionEditDialog::newConnSave()
{
    if(!connectCentralDB()){
        return;
    }
    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);
    QString strSQL;
    int isactive;
    if(ui->checkBoxIsCurrent->isChecked()) {
        isactive=1;
        emit sendConnectionName(ui->lineEditName->text().trimmed()+" "+ui->lineEditServer->text().trimmed()+":"+ui->lineEditDataBase->text().trimmed());
        strSQL = "UPDATE connections SET conn_curr = 0";
        q.exec(strSQL);
    }
    else
        isactive=0;

    strSQL = QString("INSERT INTO `connections` (`conn_name`,`conn_host`,`conn_db`,`conn_user`,`conn_pass`,`conn_curr`) "
                             "VALUES ('%1','%2','%3','%4','%5','%6')")
            .arg(ui->lineEditName->text().trimmed())
            .arg(ui->lineEditServer->text().trimmed())
            .arg(ui->lineEditDataBase->text().trimmed())
            .arg(ui->lineEditUser->text().trimmed())
            .arg(ui->lineEditPassword->text().trimmed())
            .arg(isactive);
    if(!q.exec(strSQL)) {
        qCritical(logCritical()) << "Не возможно добавить подключение." << q.lastError().text();
        QMessageBox::critical(this,"Ошибка", QString("Не возможно добавить подключение.\nПричина %1.")
                              .arg(q.lastError().text()));
        return;
    }

    this->accept();

}

void ConnectionEditDialog::updateConn()
{
    QString strSQL;
    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);
    int isactive;
    if(ui->checkBoxIsCurrent->isChecked()) {
        isactive=1;
        emit sendConnectionName(ui->lineEditName->text().trimmed()+" "+ui->lineEditServer->text().trimmed()+":"+ui->lineEditDataBase->text().trimmed());
        strSQL = "UPDATE connections SET conn_curr = 0";
        q.exec(strSQL);
        q.finish();
    }
    else
        isactive=0;

    strSQL = QString("UPDATE connections SET conn_name='%1', conn_host='%2', conn_db='%3', conn_user='%4', conn_pass='%5', conn_curr='%6'"
                     "WHERE conn_id = '%7'")
            .arg(ui->lineEditName->text().trimmed())
            .arg(ui->lineEditServer->text().trimmed())
            .arg(ui->lineEditDataBase->text().trimmed())
            .arg(ui->lineEditUser->text().trimmed())
            .arg(ui->lineEditPassword->text().trimmed())
            .arg(isactive)
            .arg(connID);
    if(!q.exec(strSQL)) {
        qCritical(logCritical()) << "Не возможно редактировать подключение." << q.lastError().text();
        QMessageBox::critical(this,"Ошибка", QString("Не возможно редактировать подключение.\nПричина %1.")
                              .arg(q.lastError().text()));
        return;
    }
    q.finish();
    this->accept();
}
