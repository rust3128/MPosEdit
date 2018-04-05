#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
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

private slots:
    void on_actionClearSaleorders_triggered();
    void on_actionUsers_triggered();
    void on_actionExit_triggered();

private:
    void showLoginDialog();             //Отображение Диалога пользователя
    void createStatusBar();             //Создание строки состояния
    void sendUser2StatusBar();          //Отображение пользователя в строке состояния
    void connCentralDB(int connID);     //Подключение к центральной базе
    void addNewConnection();            //Создание нового подключения
    void selectCentralDB();             //Выбор подключения к цетральной базе
    void setupUserInterface();          //настройка интерефейса пользователя
    void closeEvent(QCloseEvent *event);//Закрытие программы


private:
    Ui::MainWindow *ui;
    QSqlRecord currentUser;             //Текущий пользователь
    QSqlRecord centralDBInfo;           //Данные для подключения к центральной базе
    QSqlTableModel *modelConnect;       //Модель подключений

};

#endif // MAINWINDOW_H
