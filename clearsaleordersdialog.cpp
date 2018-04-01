#include "clearsaleordersdialog.h"
#include "ui_clearsaleordersdialog.h"
#include <QSqlDatabase>

ClearSaleordersDialog::ClearSaleordersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearSaleordersDialog)
{
    ui->setupUi(this);
    ui->labelTerminalName->setText("Термиал не указан...");
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

void ClearSaleordersDialog::on_lineEditTerminalID_textChanged(const QString &arg1)
{

    int terminal = arg1.toInt();
    for(int i = 0;i<modelTerminals->rowCount();++i){
        if(terminal == modelTerminals->data(modelTerminals->index(i,0)).toInt()){
            ui->labelTerminalName->setText(modelTerminals->data(modelTerminals->index(i,1)).toString());
            return;
        }
    }
    ui->labelTerminalName->setText("Термиал не указан...");

}
