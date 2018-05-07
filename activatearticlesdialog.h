#ifndef ACTIVATEARTICLESDIALOG_H
#define ACTIVATEARTICLESDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QTcpSocket>

namespace Ui {
class ActivateArticlesDialog;
}

class ActivateArticlesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActivateArticlesDialog(int user_id, QWidget *parent = 0);
    ~ActivateArticlesDialog();

private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);

    void on_toolButtonSelectTerminal_clicked();
    void getSelectedTerminal(int termID);

private:
    void createModelTerminals();        //Создание модели терминалов
    void getConnInfo(int terminal_id);  //Параметры подключения к АЗС
private:
    Ui::ActivateArticlesDialog *ui;
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    int currentUser;                    //Текуший пользователь системы
    int currentTerminal;                //Выбранный терминал
    QTcpSocket  *tcpSocket;             //Проверка доступности FierBird
    QMap<QString, QString> azsConnInfo; //Параметры подключения к АЗС
};

#endif // ACTIVATEARTICLESDIALOG_H
