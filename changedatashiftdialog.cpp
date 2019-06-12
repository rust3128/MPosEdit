#include "selectterminaldialog.h"
#include "changedatashiftdialog.h"
#include "loggingcategories.h"
#include "ui_changedatashiftdialog.h"

ChangeDataShiftDialog::ChangeDataShiftDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeDataShiftDialog)
{
    ui->setupUi(this);
    ui->labelTerminalName->setText("Терминал не указан...");
    ui->groupBox->hide();
    ui->commandLinkButtonShifts->hide();
    createModelTerminals();
}

ChangeDataShiftDialog::~ChangeDataShiftDialog()
{
    delete ui;
}

void ChangeDataShiftDialog::createModelTerminals()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    modelTerminals->setQuery("SELECT t.terminal_id, TRIM(t.name) AS name FROM TERMINALS t "
                             "WHERE t.TERMINALTYPE=3 and t.ISACTIVE='T' "
                             "ORDER BY t.TERMINAL_ID",dbcenter);
}

void ChangeDataShiftDialog::createModelShifts()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelShifts = new QSqlTableModel(this, dbcenter);
    modelShifts->setTable("SHIFTS");

}

void ChangeDataShiftDialog::on_lineEditTerminalID_textChanged(const QString &arg1)
{
    int terminal = arg1.toInt();
    for(int i = 0;i<modelTerminals->rowCount();++i){
        if(terminal == modelTerminals->data(modelTerminals->index(i,0)).toInt()){
            ui->labelTerminalName->setText(modelTerminals->data(modelTerminals->index(i,1)).toString());
            currentTerminal=terminal;
            ui->commandLinkButtonShifts->show();
            createModelShifts();
            return;
        }
    }
    ui->labelTerminalName->setText("Термиал не указан...");
    ui->commandLinkButtonShifts->hide();
}

void ChangeDataShiftDialog::getSelectedTerminal(int termID)
{
    currentTerminal=termID;
    ui->lineEditTerminalID->setText(QString::number(termID));
//    ui->lineEditShiftID->setFocus();
}


void ChangeDataShiftDialog::on_toolButtonSelectTerminal_clicked()
{
    SelectTerminalDialog *selectTermDlg = new SelectTerminalDialog(modelTerminals);
    connect(selectTermDlg,&SelectTerminalDialog::sendTerminalID,this,&ChangeDataShiftDialog::getSelectedTerminal);
//    selectTermDlg->move(this->geometry().center().x() - selectTermDlg->geometry().center().x(), this->geometry().center().y() - selectTermDlg->geometry().center().y());
    selectTermDlg->exec();
}

void ChangeDataShiftDialog::on_commandLinkButtonShifts_clicked()
{

    QString fil = QString("TERMINAL_ID=%1").arg(currentTerminal);

    qInfo(logInfo()) << "Filner" << fil;

    modelShifts->setHeaderData(1,Qt::Horizontal,"№ смены");
    modelShifts->setHeaderData(2,Qt::Horizontal,"Дата открытия");
    modelShifts->setHeaderData(3,Qt::Horizontal,"Дата закрытия");
    modelShifts->setHeaderData(5,Qt::Horizontal,"Z-отчет");
    modelShifts->setHeaderData(6,Qt::Horizontal,"Закрыта");

    modelShifts->setFilter(fil);
    modelShifts->setSort(1,Qt::DescendingOrder);
    modelShifts->select();

    ui->groupBox->show();

    ui->tableView->setModel(modelShifts);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(7);
    ui->tableView->hideColumn(8);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();


}

void ChangeDataShiftDialog::on_pushButton_clicked()
{
    this->close();
}
