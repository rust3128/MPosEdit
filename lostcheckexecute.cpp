#include "lostcheckexecute.h"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

LostCheckExecute::LostCheckExecute(QMap<QString, QString> conf, QObject *parent) : QObject(parent)
{
    confConn = conf;
}

void LostCheckExecute::lostCheckGo()
{

    bool execStatus;
    QListIterator<QString> i(script);
    while (i.hasNext()) {
        strSQL += i.next();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE",confConn.value("conName"));

    db.setHostName(confConn.value("server"));
    db.setDatabaseName(confConn.value("basename"));
    db.setUserName("SYSDBA");
    db.setPassword(confConn.value("password"));

    if(!db.open()){
        QString errorString =  db.lastError().text();
        qCritical(logCritical()) << "Не возможно открыть базу данных АЗС." << endl << "Причина:" << errorString;
        execStatus = true;
    } else {
        QSqlQuery q = QSqlQuery(db);
        if(!q.exec(strSQL)){
            qCritical(logCritical()) << "Не удалось выполнить скрипт генерации процедуры" << q.lastError().text() ;
            execStatus = false;
        } else {
            q.exec("EXECUTE PROCEDURE TMP_LOST_CHECK;");
            q.exec("DROP PROCEDURE TMP_LOST_CHECK;");
            q.exec("COMMIT WORK;");
            execStatus = true;
        }

    }
    emit sendExecStatus(execStatus);
    emit finLostCheck();
}

void LostCheckExecute::getScript(QStringList scr)
{
    script = scr;
}
