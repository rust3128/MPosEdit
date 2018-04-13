#include "logsdialog.h"
#include "ui_logsdialog.h"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QClipboard>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>

LogsDialog::LogsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogsDialog)
{
    ui->setupUi(this);
    createUI();
    ui->groupBoxFiltres->hide();
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
    curIdx = idx;

}

void LogsDialog::on_toolButtonSaveAs_clicked()
{
    QString curPath = QDir::currentPath()+"//SQL";
    QString sqlFileName = modelLogs->data(modelLogs->index(curIdx.row(),4)).toString();
    sqlFileName += "_"+modelLogs->data(modelLogs->index(curIdx.row(),5)).toString();
    sqlFileName += "_"+modelLogs->data(modelLogs->index(curIdx.row(),6)).toString()+".sql";

    QDir dir(curPath);
    if(!dir.exists()) {
        dir.mkdir(curPath);
    }
    dir.cd(curPath);
//    qInfo(logInfo()) << "QDir" << dir.current() << "QString" << dir.canonicalPath();
    QString fileName = QFileDialog::getSaveFileName(this,"Сохранить скрипт",curPath+"//"+sqlFileName,
                                           "SQL file (*.sql);;Все файлы (*.*)");

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "/* Скрипт восстановления чека "+modelLogs->data(modelLogs->index(curIdx.row(),6)).toString() +
                  " АЗС " + modelLogs->data(modelLogs->index(curIdx.row(),4)).toString() +
                  " Смена " + modelLogs->data(modelLogs->index(curIdx.row(),5)).toString() + " */" << endl;
        stream << modelLogs->data(modelLogs->index(curIdx.row(),7)).toString()+";" << endl;
        stream << "commit work;" << endl;
        if (stream.status() != QTextStream::Ok){
            qCritical(logCritical()) << "Ошибка записи в файл";
            QMessageBox::information(this,"Сохранение файла",
                                         "Не удалось сохранить файл.");
        } else {
            QMessageBox::information(this,"Сохранение файла",
                                         "Файл восстановления успешно сохранен.");
        }
    }
    on_toolButtonClose_clicked();
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
    on_toolButtonClose_clicked();
}
