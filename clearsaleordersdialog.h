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

private:
    void createModelTerminals();        //Создание модели терминалов
private:
    Ui::ClearSaleordersDialog *ui;
    QSqlQueryModel *modelTerminals;     //Модель терминалов
};

#endif // CLEARSALEORDERSDIALOG_H
