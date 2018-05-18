#ifndef LOSTCHECKDIALOG_H
#define LOSTCHECKDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QTcpSocket>

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
private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);

    void on_toolButtonSelectTerminal_clicked();
    void getSelectedTerminal(int termID);
    void getSelectedShift(int shiftID);
    void on_toolButtonSelectShift_clicked();
    void on_lineEditShiftID_textChanged(const QString &arg1);
    void on_commandLinkButton_clicked();
    void on_comboBoxPaytype_activated(int idx);
    void on_comboBoxFuels_activated(int idx);
    void on_comboBoxTRK_activated(int idx);
    void setPrice();                    //Установка цены в поле ввода
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
private:
    Ui::LostCheckDialog *ui;
    int currentTerminal=0;                //Выбранный терминал
    int currentShift=0;                   //Выбранная смена
    int currentUser;                    //Текущий пользователь
    int paytypeID;                      //Вид оплаты
    int fuelID=0;                         //Вид топлива
    int tankID;                         //Резервуар
    int trkID;                          //ТРК
    int kranID;                         //Пистолет
    double price=0.00;                  //цена
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    QSqlQueryModel *modelShifts;        //Модель смен
    QSqlQueryModel *modelPOSs;          //Модель POSS
    QSqlQueryModel *modelPaytypes;      //Модель видов оплат
    QSqlQueryModel *modelTanks;         //Модель резервуаров и видов топлива
    QSqlQueryModel *modelTRK;           //Модель ТРК
    QTcpSocket  *tcpSocket;             //Проверка доступности FierBird
    QMap<QString, QString> azsConnInfo; //Параметры подключения к АЗС

};

#endif // LOSTCHECKDIALOG_H
