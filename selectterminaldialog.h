#ifndef SELECTTERMINALDIALOG_H
#define SELECTTERMINALDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class SelectTerminalDialog;
}

class SelectTerminalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTerminalDialog(QSqlQueryModel *model, QWidget *parent = 0);
    ~SelectTerminalDialog();

signals:
    void sendTerminalID(int);
private slots:
    void on_tableView_doubleClicked(const QModelIndex &idx);
    void on_pushButtonChoise_clicked();
    void on_pushButtonCancel_clicked();
private:
    void createUI();
private:
    Ui::SelectTerminalDialog *ui;
    QSqlQueryModel *modelTerminal;          //Модель терминалов
    int terminalID;                         //Выбранный терминал
};

#endif // SELECTTERMINALDIALOG_H
