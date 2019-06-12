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
        emit sendConnStatus(true);

    } else {
        qInfo(logInfo()) << "Успешное подключение к базе данных АЗС.";
        emit sendConnStatus(true);
        QSqlQuery q = QSqlQuery(db);
        q.exec("CREATE OR ALTER PROCEDURE TMP1C_T1 "
               "AS "
               "DECLARE variable ARTICLE_ID INTEGER; "
               "DECLARE variable TERMINAL_ID SMALLINT; "
               "BEGIN "
                   "SELECT CAST(F_MID(CAST(F_BLOBASPCHAR(RDB$DEFAULT_SOURCE) AS VARCHAR(20)), 8, 10) AS INTEGER) "
                   "FROM RDB$FIELDS WHERE RDB$FIELD_NAME = 'T_TERMINAL' INTO :TERMINAL_ID; "
                   "FOR SELECT A.article_id FROM get_asaldos (:TERMINAL_ID, 9999, NULL, 0) GA "
                   "JOIN articles A ON A.article_id = GA.article_id AND A.terminal_id = :TERMINAL_ID "
                   "WHERE A.isactive = 'F' "
                   "INTO :ARTICLE_ID "
                   "DO "
                       "UPDATE articles AR SET AR.isactive = 'T' "
                       "WHERE AR.terminal_id = :TERMINAL_ID AND AR.article_id = :ARTICLE_ID; "
               "END");
        q.exec("GRANT EXECUTE ON PROCEDURE GET_ASALDOS TO PROCEDURE TMP1C_T1");
        q.exec("GRANT SELECT,UPDATE ON ARTICLES TO PROCEDURE TMP1C_T1");
        q.exec("GRANT EXECUTE ON PROCEDURE TMP1C_T1 TO SYSDBA");
        q.exec("EXECUTE PROCEDURE tmp1c_t1");
        emit sendExecStatus();

    }

    emit finActivated();
}


