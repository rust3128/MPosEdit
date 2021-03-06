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

    void on_commandLinkButton_clicked();
    void startActivation();
    void finishActibation();
    void getConnStatus(bool status);               //ПОлучение статуса соедиения
    void finishExecute();                          //Завершение процедуры

    void on_pushButtonClose_clicked();

    void on_pushButtonOtherAzs_clicked();

private:
    void createUI();                    // Первоначальная настройка внешнего вида
    void createModelTerminals();        //Создание модели терминалов
    void getConnInfo(int terminal_id);  //Параметры подключения к АЗС
    bool validateServer();              //Проверка доступности сервера
private:
    Ui::ActivateArticlesDialog *ui;
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    int currentUser;                    //Текуший пользователь системы
    int currentTerminal;                //Выбранный терминал
    QTcpSocket  *tcpSocket;             //Проверка доступности FierBird
    QMap<QString, QString> azsConnInfo; //Параметры подключения к АЗС
};

#endif // ACTIVATEARTICLESDIALOG_H
