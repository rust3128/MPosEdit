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

private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);

    void on_toolButtonSelectTerminal_clicked();
    void getSelectedTerminal(int termID);
    void getSelectedShift(int shiftID);
    void on_toolButtonSelectShift_clicked();

    void on_lineEditShiftID_textChanged(const QString &arg1);

    void on_commandLinkButton_clicked();

private:
    void createUI();
    void createModelTerminals();        //Создание модели терминалов
    void createModelShifts();           //Создание модели смен
    void possUICreate();                //Список Касс
    void paytypesUICreate();            //Список видов оплат на АЗС
    void getConnInfo(int terminal_id);  //Параметры подключения к АЗС
    bool validateServer();              //Проверка доступности сервера
    void azsConnections();              //Подключение к базе данных АЗС
private:
    Ui::LostCheckDialog *ui;
    int currentTerminal;                //Выбранный терминал
    int currentShift;                   //Выбранная смена
    int currentUser;                    //Текущий пользователь
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    QSqlQueryModel *modelShifts;        //Модель смен
    QSqlQueryModel *modelPOSs;          //Модель POSS
    QSqlQueryModel *modelPaytypes;      //Модель видов оплат
    QTcpSocket  *tcpSocket;             //Проверка доступности FierBird
    QMap<QString, QString> azsConnInfo; //Параметры подключения к АЗС
};

#endif // LOSTCHECKDIALOG_H
