#include "connectionlistdialog.h"
#include "connectioneditdialog.h"
#include "ui_connectionlistdialog.h"
#include <QDebug>
#include <QSqlError>


ConnectionListDialog::ConnectionListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionListDialog)
{
    ui->setupUi(this);
    ui->pbDelete->hide();
    createUI();
}

ConnectionListDialog::~ConnectionListDialog()
{
    delete ui;
}

void ConnectionListDialog::createUI()
{
    dblite = QSqlDatabase::database("options");
    db = QSqlDatabase::cloneDatabase(dblite,"qt_sql_default_connection");

    db.open();

    modelConnect = new TanleModelConnect();
    modelConnect->setTable("connections");
    modelConnect->setHeaderData(1,Qt::Horizontal,"Имя");
    modelConnect->setHeaderData(2,Qt::Horizontal,"Сервер");
    modelConnect->setHeaderData(3,Qt::Horizontal,"База данных");
    modelConnect->setHeaderData(6,Qt::Horizontal,"Статус");

    modelConnect->select();


    ui->tableView->setModel(modelConnect);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(5);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->selectRow(0);


}

void ConnectionListDialog::on_pbNew_clicked()
{
    ConnectionEditDialog *conEditDlg = new ConnectionEditDialog(-1);
    conEditDlg->exec();
    modelConnect->select();
}

void ConnectionListDialog::on_pbExit_clicked()
{
    this->accept();
}

void ConnectionListDialog::on_pbEdit_clicked()
{
    QModelIndex idx = ui->tableView->currentIndex();
    ConnectionEditDialog *conEditDlg = new ConnectionEditDialog(modelConnect->data(modelConnect->index(idx.row(),0),Qt::DisplayRole).toInt());
    conEditDlg->exec();
    modelConnect->select();
}
