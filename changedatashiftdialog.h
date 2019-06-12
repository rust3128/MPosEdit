#ifndef CHANGEDATASHIFTDIALOG_H
#define CHANGEDATASHIFTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlTableModel>

namespace Ui {
class ChangeDataShiftDialog;
}

class ChangeDataShiftDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeDataShiftDialog(QWidget *parent = nullptr);
    ~ChangeDataShiftDialog();
private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);
    void getSelectedTerminal(int termID);

    void on_toolButtonSelectTerminal_clicked();

    void on_commandLinkButtonShifts_clicked();

    void on_pushButton_clicked();

private:
     void createModelTerminals();        //Создание модели терминалов
     void createModelShifts();           //Создание модели смен
private:
    Ui::ChangeDataShiftDialog *ui;
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    QSqlTableModel *modelShifts;        //Модель смен

    int currentTerminal;                //Выбранный терминал
//    int currentShift;                   //Выбранная смена
//    int currentUser;                    //Текущий пользователь
};

#endif // CHANGEDATASHIFTDIALOG_H
