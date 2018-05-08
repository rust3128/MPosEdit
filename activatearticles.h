#ifndef ACTIVATEARTICLES_H
#define ACTIVATEARTICLES_H

#include <QObject>
#include <QMap>


class ActivateArticles : public QObject
{
    Q_OBJECT
public:
    explicit ActivateArticles(QMap<QString,QString> conf, QObject *parent = nullptr);

signals:
    void finActivated();                    //Завершение операции

public slots:
    void activadedGo();                     //TODO реализация активации
private:
    QMap<QString,QString> confConn;         //Параметры подключения к АЗС

};

#endif // ACTIVATEARTICLES_H
