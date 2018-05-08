#include "activatearticles.h"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

ActivateArticles::ActivateArticles(QMap<QString, QString> conf, QObject *parent) : QObject(parent)
{
    confConn = conf;
}

void ActivateArticles::activadedGo()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE",confConn.value("conName"));

    db.setHostName(confConn.value("server"));
    db.setDatabaseName(confConn.value("basename"));
    db.setUserName("SYSDBA");
    db.setPassword(confConn.value("password"));

    if(!db.open()){
        QString errorString =  db.lastError().text();
        qCritical(logCritical()) << "Не возможно открыть базу данных АЗС." << endl << "Причина:" << errorString;

    } else {
        qInfo(logInfo()) << "Успешное подключение к базе данных АЗС.";
    }
    emit finActivated();
}
