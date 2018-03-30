#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlRecord>
#include <QLabel>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show();
public slots:
    void updateConnectionName(QString connName);

private:
    void showLoginDialog();             //Отображение Диалога пользователя
    void createStatusBar();             //Создание строки состояния
    void sendUser2StatusBar();          //Отображение пользователя в строке состояния
    void connCentralDB(int connID);     //Подключение к центральной базе
    void addNewConnection();            //Создание нового подключения
    void selectCentralDB();             //Выбор подключения к цетральной базе
    void modelsCreate();                //Создание модель из ЦБ

private:
    Ui::MainWindow *ui;
    QSqlRecord currentUser;             //Текущий пользователь
    QSqlRecord centralDBInfo;           //Данные для подключения к центральной базе
    QSqlTableModel *modelConnect;       //Модель подключений
    QSqlQueryModel *modelTerminals;     //Модель терминалов
};

#endif // MAINWINDOW_H
