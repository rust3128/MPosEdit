#include "selectshiftdialog.h"
#include "ui_selectshiftdialog.h"


SelectShiftDialog::SelectShiftDialog(QSqlQueryModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectShiftDialog)
{
    ui->setupUi(this);
    modelShifts = new QSqlQueryModel();
    modelShifts = model;
    createUI();
}

SelectShiftDialog::~SelectShiftDialog()
{
    delete ui;
}

void SelectShiftDialog::createUI()
{
    modelShifts->setHeaderData(0,Qt::Horizontal,tr("Смена"));
    modelShifts->setHeaderData(1,Qt::Horizontal, tr("Z-отчет"));
    modelShifts->setHeaderData(2,Qt::Horizontal,tr("Открыта"));
    modelShifts->setHeaderData(3,Qt::Horizontal,tr("Закрыта"));

    ui->tableView->setModel(modelShifts);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(4);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
}

void SelectShiftDialog::on_tableView_doubleClicked(const QModelIndex &idx)
{
    shiftID = modelShifts->data(modelShifts->index(idx.row(),0)).toInt();
    emit sendShiftID(shiftID);
    this->accept();
}

void SelectShiftDialog::on_pushButton_clicked()
{
    QModelIndex idx;
    idx = ui->tableView->currentIndex();
    shiftID = modelShifts->data(modelShifts->index(idx.row(),0)).toInt();
    emit sendShiftID(shiftID);
    this->accept();
}

void SelectShiftDialog::on_pushButtonCancel_clicked()
{
    this->reject();
}
