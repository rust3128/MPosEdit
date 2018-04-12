#ifndef LOGSDIALOG_H
#define LOGSDIALOG_H

#include <QDialog>
#include <QSqlRelationalTableModel>

namespace Ui {
class LogsDialog;
}

class LogsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogsDialog(QWidget *parent = 0);
    ~LogsDialog();
private slots:
    void on_pushButtonClose_clicked();
    void on_tableView_doubleClicked(const QModelIndex &idx);
    void on_toolButtonSaveAs_clicked();

    void on_toolButtonClose_clicked();

    void on_toolButtonCopy_clicked();

private:
    void createUI();
private:
    Ui::LogsDialog *ui;
    QSqlRelationalTableModel *modelLogs;
    QString strSQL;
};

#endif // LOGSDIALOG_H
