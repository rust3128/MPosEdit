#ifndef SELECTSHIFTDIALOG_H
#define SELECTSHIFTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class SelectShiftDialog;
}

class SelectShiftDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectShiftDialog(QSqlQueryModel *model, QWidget *parent = 0);
    ~SelectShiftDialog();
signals:
    void sendShiftID(int);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &idx);
    void on_pushButton_clicked();
    void on_pushButtonCancel_clicked();

private:
    void createUI();
private:
    Ui::SelectShiftDialog *ui;
    QSqlQueryModel *modelShifts;        //Модель Смен
    int shiftID;                        //Выбранная смена
};

#endif // SELECTSHIFTDIALOG_H
