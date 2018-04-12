#ifndef INSERTLOG_H
#define INSERTLOG_H
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

static void insertLog(int logtype, int operator_id, int terminal_id, int shift_id, int check_num, QString sql){
    QSqlDatabase dblite = QSqlDatabase::database("options");
    QSqlQuery q = QSqlQuery(dblite);
    QString strSQL = QString("INSERT INTO `logs`(`logtype_id`,`operator_id`,`dat`, `terminal_id`,`shift_id`,`check_num`, `sql`) "
                             "VALUES (%1,%2,'%3',%4,%5,%6,'%7')")
            .arg(logtype)
            .arg(operator_id)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(terminal_id)
            .arg(shift_id)
            .arg(check_num)
            .arg(sql);
    qInfo(logInfo()) << "SQL log" << strSQL;
    if(!q.exec(strSQL)) qCritical(logCritical()) << "Не удалось добавить запись в log файл." << endl
                                                 << q.lastError().text();
}


#endif // INSERTLOG_H
