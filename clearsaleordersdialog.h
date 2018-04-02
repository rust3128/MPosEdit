#ifndef CLEARSALEORDERSDIALOG_H
#define CLEARSALEORDERSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class ClearSaleordersDialog;
}

class ClearSaleordersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClearSaleordersDialog(QWidget *parent = 0);
    ~ClearSaleordersDialog();

private slots:
    void on_lineEditTerminalID_textChanged(const QString &arg1);
    void on_toolButtonSelectTerminal_clicked();
    void on_lineEditShiftID_textChanged(const QString &arg1);
    void on_toolButtonSelectShift_clicked();

    void getSelectedTerminal(int termID);
    void getSelectedShift(int shiftID);

private:
    void createModelTerminals();        //Создание модели терминалов
    void createModelShifts();           //Создание модели смен
private:
    Ui::ClearSaleordersDialog *ui;
    QSqlQueryModel *modelTerminals;     //Модель терминалов
    QSqlQueryModel *modelShifts;        //Модель смен
    int currentTerminal;                //Выбранный терминал
    int currentShift;                   //Выбранная смена
};

#endif // CLEARSALEORDERSDIALOG_H
