#include "clearsaleordersdialog.h"
#include "ui_clearsaleordersdialog.h"
#include "selectterminaldialog.h"
#include "selectshiftdialog.h"
#include <QSqlDatabase>
#include <QDateTime>

ClearSaleordersDialog::ClearSaleordersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearSaleordersDialog)
{
    ui->setupUi(this);
    ui->labelTerminalName->setText("Термиал не указан...");
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->groupBoxShifts->hide();
    createModelTerminals();
}

ClearSaleordersDialog::~ClearSaleordersDialog()
{
    delete ui;
}

void ClearSaleordersDialog::createModelTerminals()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    modelTerminals->setQuery("SELECT t.terminal_id, TRIM(t.name) AS name FROM TERMINALS t "
                             "WHERE t.TERMINALTYPE=3 and t.ISACTIVE='T' "
                             "ORDER BY t.TERMINAL_ID",dbcenter);
}

void ClearSaleordersDialog::createModelShifts()
{
    QString strSQL = QString("select s.SHIFT_ID, s.DATOPEN, s.DATCLOSE from SHIFTS s "
                             "where s.TERMINAL_ID=%1 "
                             "order by s.SHIFT_ID DESC")
            .arg(currentTerminal);
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelShifts = new QSqlQueryModel();
    modelShifts->setQuery(strSQL,dbcenter);
    while(modelShifts->canFetchMore())
        modelShifts->fetchMore();
}

void ClearSaleordersDialog::on_lineEditTerminalID_textChanged(const QString &arg1)
{

    int terminal = arg1.toInt();
    for(int i = 0;i<modelTerminals->rowCount();++i){
        if(terminal == modelTerminals->data(modelTerminals->index(i,0)).toInt()){
            ui->labelTerminalName->setText(modelTerminals->data(modelTerminals->index(i,1)).toString());
            currentTerminal=terminal;
            ui->groupBoxShifts->show();
            createModelShifts();
            return;
        }
    }
    ui->groupBoxShifts->hide();
    ui->labelTerminalName->setText("Термиал не указан...");
}

void ClearSaleordersDialog::on_toolButtonSelectTerminal_clicked()
{
    SelectTerminalDialog *selectTermDlg = new SelectTerminalDialog(modelTerminals);
    connect(selectTermDlg,&SelectTerminalDialog::sendTerminalID,this,&ClearSaleordersDialog::getSelectedTerminal);
//    selectTermDlg->move(this->geometry().center().x() - selectTermDlg->geometry().center().x(), this->geometry().center().y() - selectTermDlg->geometry().center().y());
    selectTermDlg->exec();
}

void ClearSaleordersDialog::getSelectedTerminal(int termID)
{
    currentTerminal=termID;
    ui->lineEditTerminalID->setText(QString::number(termID));
}

void ClearSaleordersDialog::getSelectedShift(int shiftID)
{
    currentShift = shiftID;
    ui->lineEditShiftID->setText(QString::number(shiftID));
}

void ClearSaleordersDialog::on_lineEditShiftID_textChanged(const QString &arg1)
{
    int shift = arg1.toInt();
    for(int i =0; i<modelShifts->rowCount();++i){
        if(shift == modelShifts->data(modelShifts->index(i,0)).toInt()){
            ui->labelShiftData->setText("Смена № "+modelShifts->data(modelShifts->index(i,0)).toString() +
                    " От: "+modelShifts->data(modelShifts->index(i,1)).toDateTime().toString("dd.MM.yyyy hh.mm"));
            currentShift=shift;
            return;
        }
    }
    ui->labelShiftData->setText("Смена не выбрана...");
}

void ClearSaleordersDialog::on_toolButtonSelectShift_clicked()
{
    SelectShiftDialog *selectShDlg = new SelectShiftDialog(modelShifts);
    connect(selectShDlg,&SelectShiftDialog::sendShiftID,this,&ClearSaleordersDialog::getSelectedShift);
    selectShDlg->exec();
}
