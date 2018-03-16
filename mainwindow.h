#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlRecord>
#include <QLabel>

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

private:
    void showLoginDialog();             //Отображение Диалога пользователя
    void createStatusBar();             //Создание строки состояния
    void sendUser2StatusBar();          //Отображение пользователя в строке состояния

private:
    Ui::MainWindow *ui;
    QSqlRecord currentUser;             //Текущий пользователь
    QSqlRecord centralDBInfo;           //Данные для подключения к центральной базе
};

#endif // MAINWINDOW_H
