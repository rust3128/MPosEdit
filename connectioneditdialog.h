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

private:
    Ui::ConnectionEditDialog *ui;
};

#endif // CONNECTIONEDITDIALOG_H
