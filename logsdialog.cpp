#include "logsdialog.h"
#include "ui_logsdialog.h"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QClipboard>
#include <QMessageBox>

LogsDialog::LogsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogsDialog)
{
    ui->setupUi(this);
    createUI();
}

LogsDialog::~LogsDialog()
{
    delete ui;
}

void LogsDialog::createUI()
{
    QSqlDatabase dblite = QSqlDatabase::database("options");

    modelLogs = new QSqlRelationalTableModel(this,dblite);
    modelLogs->setTable("logs");
    modelLogs->setRelation(1,QSqlRelation("logtypes","logtype_id","logtype_text"));
    modelLogs->setRelation(2,QSqlRelation("users","user_id","user_fio"));

    modelLogs->setHeaderData(1,Qt::Horizontal,"Тип операции");
    modelLogs->setHeaderData(2,Qt::Horizontal,"Автор");
    modelLogs->setHeaderData(3,Qt::Horizontal,"Дата операции");
    modelLogs->setHeaderData(4,Qt::Horizontal,"Терминал");
    modelLogs->setHeaderData(5,Qt::Horizontal,"Смена");
    modelLogs->setHeaderData(6,Qt::Horizontal,"№ Чека");

    modelLogs->select();
    ui->tableView->setModel(modelLogs);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(7);

    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());

    ui->groupBoxSQL->hide();
}

void LogsDialog::on_pushButtonClose_clicked()
{
    this->reject();
}

void LogsDialog::on_tableView_doubleClicked(const QModelIndex &idx)
{
    ui->textBrowser->clear();
    ui->groupBoxSQL->show();
    ui->tableView->setEnabled(false);
    strSQL = modelLogs->data(modelLogs->index(idx.row(),7)).toString();
    ui->textBrowser->setPlainText(strSQL);
    ui->textBrowser->selectAll();

}

void LogsDialog::on_toolButtonSaveAs_clicked()
{

}

void LogsDialog::on_toolButtonClose_clicked()
{
    ui->groupBoxSQL->hide();
    ui->tableView->setEnabled(true);
}

void LogsDialog::on_toolButtonCopy_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(strSQL);
    QMessageBox::information(this,"Копирование",
                             "Скрипт успешно скопирован в буфер обмена.");
}
