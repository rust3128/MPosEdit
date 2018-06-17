#ifndef LOSTCHECKEXECUTE_H
#define LOSTCHECKEXECUTE_H

#include <QObject>
#include <QMap>

class LostCheckExecute : public QObject
{
    Q_OBJECT
public:
    explicit LostCheckExecute(QMap<QString,QString> conf, QObject *parent = nullptr);

signals:
    void finLostCheck();            //Завершение операции
    void sendExecStatus(bool);          //Статус выполнения

public slots:
    void lostCheckGo();             //Реализация скрипта
    void getScript(QStringList scr);//получение скрипта

private:
    QMap<QString,QString> confConn;         //Параметры подключения к АЗС
    QStringList script;             //Скрипт для исполнения
    QString strSQL;
};

#endif // LOSTCHECKEXECUTE_H
