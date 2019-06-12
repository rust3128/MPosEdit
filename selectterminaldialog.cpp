#include "selectterminaldialog.h"
#include "ui_selectterminaldialog.h"

SelectTerminalDialog::SelectTerminalDialog(QSqlQueryModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectTerminalDialog)
{
    ui->setupUi(this);
    modelTerminal = new QSqlQueryModel();
    modelTerminal=model;
    createUI();
}

SelectTerminalDialog::~SelectTerminalDialog()
{
    delete ui;
}

void SelectTerminalDialog::createUI()
{
    modelTerminal->setHeaderData(0,Qt::Horizontal,tr("АЗС"));
    modelTerminal->setHeaderData(1,Qt::Horizontal,tr("Расположение"));

    ui->tableView->setModel(modelTerminal);
    ui->tableView->verticalHeader()->hide();

    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
}

void SelectTerminalDialog::on_tableView_doubleClicked(const QModelIndex &idx)
{
    terminalID = modelTerminal->data(modelTerminal->index(idx.row(),0)).toInt();
    emit sendTerminalID(terminalID);
    this->accept();
}

void SelectTerminalDialog::on_pushButtonChoise_clicked()
{
    QModelIndex idx;
    idx = ui->tableView->currentIndex();
    terminalID = modelTerminal->data(modelTerminal->index(idx.row(),0)).toInt();
    emit sendTerminalID(terminalID);
    this->accept();
}

void SelectTerminalDialog::on_pushButtonCancel_clicked()
{
    this->reject();
}
