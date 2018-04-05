#include "clearsaleordersdialog.h"
#include "ui_clearsaleordersdialog.h"
#include "selectterminaldialog.h"
#include "selectshiftdialog.h"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

ClearSaleordersDialog::ClearSaleordersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClearSaleordersDialog)
{
    ui->setupUi(this);
    ui->labelTerminalName->setText("Термиал не указан...");
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->frameCheckInfo->hide();
    ui->groupBoxShifts->hide();
    ui->groupBoxCheck->hide();
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
    ui->groupBoxCheck->hide();
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
    ui->lineEditShiftID->setFocus();
}

void ClearSaleordersDialog::getSelectedShift(int shiftID)
{
    currentShift = shiftID;
    ui->lineEditShiftID->setText(QString::number(shiftID));
    ui->lineEditNumCheck->setFocus();
}

void ClearSaleordersDialog::on_lineEditShiftID_textChanged(const QString &arg1)
{
    int shift = arg1.toInt();
    for(int i =0; i<modelShifts->rowCount();++i){
        if(shift == modelShifts->data(modelShifts->index(i,0)).toInt()){
            ui->labelShiftData->setText("Смена № "+modelShifts->data(modelShifts->index(i,0)).toString() +
                    " От: "+modelShifts->data(modelShifts->index(i,1)).toDateTime().toString("dd.MM.yyyy hh.mm"));
            currentShift=shift;
            ui->groupBoxCheck->show();
            return;
        }
    }
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->groupBoxCheck->hide();
}

void ClearSaleordersDialog::on_toolButtonSelectShift_clicked()
{
    SelectShiftDialog *selectShDlg = new SelectShiftDialog(modelShifts);
    connect(selectShDlg,&SelectShiftDialog::sendShiftID,this,&ClearSaleordersDialog::getSelectedShift);
    selectShDlg->exec();
}

void ClearSaleordersDialog::on_pushButtonFindCheck_clicked()
{
    modelSale = new QSqlQueryModel();

    QSqlDatabase dbc = QSqlDatabase::database("central");
    QString strSql;


    strSql=QString("SELECT a.TERMINAL_ID, t.name, a.SALEORDER_ID, a.NUM_CHECK,f.SHORTNAME, "
                   "a.GIVE, a.ORDERED, a.SUMMA, a.DISCOUNTSUMMA,a.GIVE1 "
                   "FROM SALEORDERS a "
                   "LEFT JOIN fuels f ON a.FUEL_ID=f.FUEL_ID "
                   "LEFT JOIN TERMINALS t ON a.TERMINAL_ID=t.TERMINAL_ID "
                   "where a.TERMINAL_ID=%1 and a.SHIFT_ID=%2 and a.NUM_CHECK=%3")
            .arg(ui->lineEditTerminalID->text().toInt())
            .arg(ui->lineEditShiftID->text().toInt())
            .arg(ui->lineEditNumCheck->text().toInt());
    modelSale->setQuery(strSql,dbc);

    if(modelSale->rowCount()==0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Транзакия с заданными параметрами отсутсвует в базе данных\n"
                                      "Проверьте информацю о транзакции и повторите попытку."));
        return;
    }
    ui->frameCheckInfo->show();
    ui->labelTermInfo->setText("Терминал "+ui->lineEditTerminalID->text()+" "+ui->labelTerminalName->text());
    ui->labelShiftInfo->setText(ui->labelShiftData->text());

    modelSale->setHeaderData(3,Qt::Horizontal,"Чек");
    modelSale->setHeaderData(4,Qt::Horizontal,"Топливо");
    modelSale->setHeaderData(5,Qt::Horizontal,"Получено");
    modelSale->setHeaderData(6,Qt::Horizontal,"Заказано");
    modelSale->setHeaderData(7,Qt::Horizontal,"Сумма");
    modelSale->setHeaderData(8,Qt::Horizontal,"Скидка");


    ui->tableView->setModel(modelSale);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(1);
    ui->tableView->hideColumn(2);
    ui->tableView->hideColumn(9);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->selectRow(0);

}

void ClearSaleordersDialog::on_pushButtonCancel_clicked()
{
    ui->frameCheckInfo->hide();
    ui->lineEditShiftID->clear();
    ui->groupBoxShifts->hide();
    ui->lineEditNumCheck->clear();
    ui->groupBoxCheck->hide();
    ui->lineEditTerminalID->clear();
}

void ClearSaleordersDialog::on_pushButtonExit_clicked()
{
    this->reject();
}

void ClearSaleordersDialog::on_pushButtonClear_clicked()
{
    QModelIndex idx = ui->tableView->currentIndex();
    QSqlDatabase dbc = QSqlDatabase::database("central");

    QString strUpdate,strSql;
    QSqlQuery q =QSqlQuery(dbc);
    qInfo(logInfo()) << "Обнуляем транзакцию. АЗС" << modelSale->data(modelSale->index(idx.row(),0)).toString().trimmed()
                     << "Смена" << ui->lineEditShiftID->text()
                     << "Чек №" << ui->lineEditNumCheck->text();
    strUpdate = "UPDATE SALEORDERS SET ";
    strUpdate += QString("GIVE = '%1' ").arg(modelSale->data(modelSale->index(idx.row(),5)).toString().trimmed());
    strUpdate += QString("ORDERED = '%1' ").arg(modelSale->data(modelSale->index(idx.row(),6)).toDouble());
    strUpdate += QString("SUMMA = '%1' ").arg(modelSale->data(modelSale->index(idx.row(),7)).toString().trimmed());
    strUpdate += QString("DISCOUNTSUMMA = '%1' ").arg(modelSale->data(modelSale->index(idx.row(),8)).toString().trimmed());
    strUpdate += QString("GIVE1 = '%1' ").arg(modelSale->data(modelSale->index(idx.row(),8)).toDouble());
    strUpdate += QString("WHERE TERMINAL_ID = '%1' "
                          "AND SALEORDER_ID = '%2'")
                  .arg(modelSale->data(modelSale->index(idx.row(),0)).toString().trimmed())
                  .arg(modelSale->data(modelSale->index(idx.row(),2)).toString().trimmed());
    qInfo(logInfo()) << "--------------------------------------------------------";
    qInfo(logInfo()) << "Обнуляем Чек №" << ui->lineEditNumCheck->text() << "AЗС"
                     << ui->lineEditTerminalID->text() << ui->labelShiftData->text();
    qInfo(logInfo()) << "Скрипт для восстановление обнуляемой транзакции";
    qInfo(logInfo()) << strUpdate;
    qInfo(logInfo()) << "--------------------------------------------------------";

    strSql = QString("UPDATE SALEORDERS SET "
                             "GIVE = '0.00', "
                             "ORDERED = '0.00', "
                             "SUMMA = '0.00', "
                             "DISCOUNTSUMMA = '0.00', "
                             "GIVE1 = '0.000' "
                             "WHERE TERMINAL_ID = '%1' AND SALEORDER_ID = '%2'")
            .arg(modelSale->data(modelSale->index(idx.row(),0)).toString().trimmed())
            .arg(modelSale->data(modelSale->index(idx.row(),2)).toString().trimmed());
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, QString::fromUtf8("Подтверждение действия"),
                          QString::fromUtf8("Вы действительно хотите обнулить указанную транзакцию?"),
                          QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No){
        return;
    }


    if(!q.exec(strSql)) {
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не возможно выблнить очистку транзакции\n"
                                      "Причина: %1"
                                      )
                              .arg(q.lastError().text()));
        qCritical(logCritical()) << "Не удалось обнулить транзакцию. АЗС" << modelSale->data(modelSale->index(idx.row(),0)).toString().trimmed()
                         << "Смена" << ui->lineEditShiftID->text()
                         << "Чек №" << ui->lineEditNumCheck->text();
        return;
    }
    QMessageBox::information(0, qApp->tr("Операция завершена"),
                          QString("Очистка транзакции успешно произведена\n"));
    qInfo(logInfo()) << "Транзакция обнулена.";
    modelSale->setQuery(modelSale->query().lastQuery(),dbc);
}
