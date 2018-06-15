#ifndef LOSTCHECKDIALOG_H
#define LOSTCHECKDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QTcpSocket>
#include <QDate>
#include "lostcheckexecute.h"
#include <QThread>


namespace Ui {
class LostCheckDialog;
}

class LostCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LostCheckDialog(int user_id, QWidget *parent = 0);
    ~LostCheckDialog();

signals:
    void getPrice();                //установка цены
    void calcSumm();                //Вычимлить сумму
    void sendScript(QStringList script);  //Передача скрипта в поток выполнения

private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);

    void on_toolButtonSelectTerminal_clicked();
    void getSelectedTerminal(int termID);
    void getSelectedShift(int shiftID);
    void on_toolButtonSelectShift_clicked();
    void on_lineEditShiftID_textChanged(const QString &arg1);
    void on_comboBoxPaytype_activated(int idx);
    void on_comboBoxFuels_activated(int idx);
    void on_comboBoxTRK_activated(int idx);
    void setPrice();                    //Установка цены в поле ввода
    void setSumm();                     //Расчет суммы
    void on_lineEditPrice_textChanged();
    void on_lineEditGive_textChanged();
    void on_pushButtonChecAzs_clicked();
    void on_pushButtonRunScript_clicked();
    void on_pushButtonSaveScript_clicked();
    void startEcecute();                    //Начало выполнения скрипта
    void finishExecute();                   //Завершение потока

private:
    void createUI();
    void createModelTerminals();        //Создание модели терминалов
    void createModelShifts();           //Создание модели смен
    void possUICreate();                //Список Касс
    void paytypesUICreate();            //Список видов оплат на АЗС
    void tanksFuelsUICreate();          //Резервуары виды топлива, колонки пистолеты
    void getConnInfo(int terminal_id);  //Параметры подключения к АЗС
    bool validateServer();              //Проверка доступности сервера
    void azsConnections();              //Подключение к базе данных АЗС
    void showClientsInfo(bool sh);      //Отображение или скрытие информации о клиенте
    void initLostCheck();               //Инициализации структуры
    void generateScript();              //Генерация скрипта
    bool validLostCheck();              //Валидация данных для скрипта
private:
    Ui::LostCheckDialog *ui;
    QMap<QString, QVariant> lostCheck;  //Контейнер данных для формирования скрипта
    QDate shiftDate;                    //Дата текущей смены
    int currentUser;                    //Текущий пользователь
    double price=0.00;                  //цена
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    QSqlQueryModel *modelShifts;        //Модель смен
    QSqlQueryModel *modelPOSs;          //Модель POSS
    QSqlQueryModel *modelPaytypes;      //Модель видов оплат
    QSqlQueryModel *modelTanks;         //Модель резервуаров и видов топлива
    QSqlQueryModel *modelTRK;           //Модель ТРК
    QTcpSocket  *tcpSocket;             //Проверка доступности FierBird
    QMap<QString, QString> azsConnInfo; //Параметры подключения к АЗС
    QStringList script;                 //Скрипт процедуры
    QStringList endScript;              //Команды завершения скрипта
    QString strSQL;                     //
    bool validateData;
    LostCheckExecute *lchExec;
    QThread *thread;


};

#endif // LOSTCHECKDIALOG_H
