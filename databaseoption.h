#ifndef DATABASEOPTION_H
#define DATABASEOPTION_H

#include "loggingcategories.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

//Файл базы данных с настройками
#define DATABASE_NAME "./MposEdit.opt"
#define DATABASE_HOSTNAME "OptionsHost"



bool createOptionsTables()
{
    qInfo(logInfo()) << "Создание файла настроек приложения.";
    QStringList listSQL;
    ///  Создание таблицы пользователей
    listSQL << "CREATE TABLE users "
               "(user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "user_name VARCHAR(50) NOT NULL, "
               "user_fio VARCHAR(50) NOT NULL, "
               "user_pass VARCHAR(50) NOT NULL,"
               "user_active INTEGER )";
    listSQL << "INSERT INTO `users`(`user_name`,`user_fio`,`user_pass`,`user_active`) "
               "VALUES ('Администратор','Администратор А.А.','defrag',1)";
    ///Создание таблицы подключений
    listSQL << "CREATE TABLE `connections` ( "
               "`conn_id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
               "`conn_name`	VARCHAR(50) NOT NULL UNIQUE, "
               "`conn_host`	varchar(50) NOT NULL, "
               "`conn_db`	VARCHAR(100) NOT NULL, "
               "`conn_user`	INTEGER NOT NULL, "
               "`conn_pass`	INTEGER NOT NULL,"
               "`conn_curr` INTEGER NOT NULL)";
    ///Создание таблицы типов операций
    listSQL << "CREATE TABLE `logtypes` ( "
               "`LOGTYPE_ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
               "`LOGTYPE_TEXT`	TEXT NOT NULL"
               ")";
    listSQL << "INSERT INTO `logtypes`(`LOGTYPE_TEXT`) VALUES ('Удаление продажи')";
    listSQL << "INSERT INTO `logtypes`(`LOGTYPE_TEXT`) VALUES ('Восстановление продажи')";
    listSQL << "INSERT INTO `logtypes`(`LOGTYPE_TEXT`) VALUES ('Активация товаров')";
    /// Создание таблицы логирования
    listSQL << "CREATE TABLE `logs` ( "
               "`log_id`        INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
               "`logtype_id`	INTEGER NOT NULL, "
               "`operator_id`	INTEGER NOT NULL, "
               "`dat`           TEXT NOT NULL, "
               "`terminal_id`	INTEGER NOT NULL, "
               "`shift_id`      INTEGER, "
               "`check_num` 	INTEGER, "
               "`sql`           BLOB)";


    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);

    for(int i=0; i < listSQL.size(); ++i){
        if(!q.exec(listSQL.at(i))){
            qCritical(logCritical()) << "Ошибка выполнения запроса." << q.lastError().text();
            return false;
        }
    }
    qInfo(logInfo()) << "Файл настроек приложения успешно создан.";
    return true;
}

bool openDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","options");
    db.setHostName(DATABASE_HOSTNAME);
    db.setDatabaseName(DATABASE_NAME);
    if(db.open()){
        return true;
    } else {
        qCritical(logCritical()) << "Не удалось открыть базу данных.";
        return false;
    }
}

bool createDatabase()
{
    if(openDatabase()){
        if(createOptionsTables()){
            return true;
        } else {
            return false;
        }
    } else {
        qCritical(logCritical()) << "Не удалось восстановить базу данных настроек приложения.";
        return false;
    }
}

static bool connectionOptions()
{
    if(QFile(DATABASE_NAME).exists()){
        qInfo(logInfo()) << "Настройки приложения успешно прочитаны.";
        return openDatabase();
    } else {
        qCritical(logCritical()) << "Отсутствует файл настроек приложения.";
        return createDatabase();
    }
}


#endif // DATABASEOPTION_H
