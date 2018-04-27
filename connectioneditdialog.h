#ifndef CONNECTIONEDITDIALOG_H
#define CONNECTIONEDITDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionEditDialog;
}

class ConnectionEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionEditDialog(int id, QWidget *parent = 0);
    ~ConnectionEditDialog();
signals:
    void sendConnectionName(QString);

private slots:
    void on_pushButtonCansel_clicked();
    void on_pushButtonSave_clicked();

private:
    void addNewConnection();        //Новое подключение
    bool validateData();            //Проверка заполнения формы
    bool connectCentralDB();        //Подключение к центрально базе данных
    void editConnections(int ID);         //редактирование подключения
    void newConnSave();             //Сохранение нового соединения
    void updateConn();              //Редактирование соединения

private:
    Ui::ConnectionEditDialog *ui;
    bool connNew;
    int connID;
};

#endif // CONNECTIONEDITDIALOG_H
