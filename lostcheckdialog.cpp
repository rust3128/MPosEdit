#include "lostcheckdialog.h"
#include "ui_lostcheckdialog.h"
#include "selectterminaldialog.h"
#include "selectshiftdialog.h"
#include "passconv.h"
#include "insertlog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>


LostCheckDialog::LostCheckDialog(int user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LostCheckDialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    initLostCheck();
    createUI();
    createModelTerminals();
    currentUser = user_id;
    connect(this,&LostCheckDialog::getPrice,this,&LostCheckDialog::setPrice);
    connect(this,&LostCheckDialog::calcSumm,this,&LostCheckDialog::setSumm);
}

LostCheckDialog::~LostCheckDialog()
{
    delete ui;
}

void LostCheckDialog::initLostCheck()
{
    lostCheck.insert("TERMINAL_ID",0);
    lostCheck.insert("SHIFT_ID",0);
    lostCheck.insert("DISPENSER_ID",0);
    lostCheck.insert("TRK_ID",0);
    lostCheck.insert("TANK_ID",0);
    lostCheck.insert("FUEL_ID",0);
    lostCheck.insert("GIVE",0);
    lostCheck.insert("ORDERED",0);
    lostCheck.insert("SUMMA",0);
    lostCheck.insert("CASH",0);
    lostCheck.insert("DISCOUNTSUMMA",0);
    lostCheck.insert("PAYTYPE_ID",0);
    lostCheck.insert("NUM_CHECK",0);
    lostCheck.insert("NUM_CHECK_RETURN",0);
    lostCheck.insert("TRANSACTION_ID",0);
    lostCheck.insert("SEC",0);
    lostCheck.insert("ISLAST","T");
    lostCheck.insert("INFO_CODE",0);
    lostCheck.insert("INFO_TEXT","");
    lostCheck.insert("POS_ID",0);
    lostCheck.insert("ZNUMBER",0);
    lostCheck.insert("OPERATOR_ID",0);
    lostCheck.insert("SALEORDER_ID",0);
    lostCheck.insert("PRICE",0);
    lostCheck.insert("ISBEFOREPAY","F");
    lostCheck.insert("POSTRANSACTION_ID",0);
    lostCheck.insert("POSTRNRETURN_ID",0);
    lostCheck.insert("SHARE_ID",0);
    lostCheck.insert("MPOSCHECK_ID",0);
    lostCheck.insert("PAYTYPE_ID2",0);
    lostCheck.insert("SUMMA2",0);
    lostCheck.insert("DISCOUNTSUMMA2",0);
    lostCheck.insert("DAT",0);
    lostCheck.insert("GOV_NUMBER","");
    lostCheck.insert("BONUSCARD","");
}


void LostCheckDialog::createUI()
{


    ui->labelTerminalName->setText("Терминал не указан...");
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->labelConnAviable->hide();
    ui->groupBoxShifts->hide();
    ui->pushButtonChecAzs->hide();
    ui->dateTimeCheck->setDateTime(QDateTime::currentDateTime());

}


void LostCheckDialog::createModelTerminals()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    modelTerminals->setQuery("SELECT t.terminal_id, TRIM(t.name) AS name FROM TERMINALS t "
                             "WHERE t.TERMINALTYPE=3 and t.ISACTIVE='T' "
                             "ORDER BY t.TERMINAL_ID",dbcenter);
}

void LostCheckDialog::createModelShifts()
{
    QString strSQL = QString("select s.SHIFT_ID, s.ZNUMBER, s.DATOPEN, s.DATCLOSE, s.OPERATOR_ID  from SHIFTS s "
                             "where s.TERMINAL_ID=%1 "
                             "order by s.SHIFT_ID DESC")
            .arg(lostCheck.value("TERMINAL_ID").toInt());
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelShifts = new QSqlQueryModel();
    modelShifts->setQuery(strSQL,dbcenter);
    while(modelShifts->canFetchMore())
        modelShifts->fetchMore();
}




void LostCheckDialog::on_lineEditTerminalID_textChanged(const QString &arg1)
{
    int terminal = arg1.toInt();
    for(int i = 0;i<modelTerminals->rowCount();++i){
        if(terminal == modelTerminals->data(modelTerminals->index(i,0)).toInt()){
            ui->labelTerminalName->setText(modelTerminals->data(modelTerminals->index(i,1)).toString());
            lostCheck["TERMINAL_ID"]=terminal;
//            currentTerminal=terminal;
            getConnInfo(lostCheck.value("TERMINAL_ID").toInt());
            if(validateServer()){
                ui->pushButtonChecAzs->show();
                createModelShifts();
            }

            return;
        }
    }
    ui->groupBoxShifts->hide();
    ui->labelTerminalName->setText("Термиал не указан...");
}

void LostCheckDialog::getConnInfo(int terminal_id)
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    QSqlQuery q = QSqlQuery(dbcenter);
    q.prepare("SELECT c.SERVER_NAME, c.DB_NAME, c.CON_PASSWORD FROM CONNECTIONS c WHERE c.TERMINAL_ID=:term_id AND c.CONNECT_ID=2");
    q.bindValue(":term_id",terminal_id);
    if(!q.exec()){
        qCritical(logCritical()) << "Не возможно получить данные о подключении на АЗС." << q.lastError().text();
        return;
    }
    q.next();
    azsConnInfo.insert("conName","azs"+QString::number(terminal_id));
    azsConnInfo.insert("server",q.value("SERVER_NAME").toString().trimmed());
    azsConnInfo.insert("basename",q.value("DB_NAME").toString().trimmed());
    azsConnInfo.insert("password",passConv(q.value("CON_PASSWORD").toString().trimmed()));
}


void LostCheckDialog::on_toolButtonSelectTerminal_clicked()
{
    SelectTerminalDialog *selectTermDlg = new SelectTerminalDialog(modelTerminals);
    connect(selectTermDlg,&SelectTerminalDialog::sendTerminalID,this,&LostCheckDialog::getSelectedTerminal);
    selectTermDlg->exec();
}

void LostCheckDialog::getSelectedTerminal(int termID)
{
    lostCheck["TERMINAL_ID"]=termID;
//    currentTerminal=termID;
    ui->lineEditTerminalID->setText(QString::number(termID));
    ui->lineEditShiftID->setFocus();
}

bool LostCheckDialog::validateServer()
{
    ui->labelConnAviable->show();
    tcpSocket->abort();
    tcpSocket->connectToHost(azsConnInfo.value("server"),3050);
    if(tcpSocket->waitForConnected(30000)){
        qInfo(logInfo()) << "Проверка доступности. Сервер: " << azsConnInfo.value("server") <<  "FireBird доступен.";
        ui->labelConnAviable->setStyleSheet("color: rgb(0, 170, 0);font: 75 14pt 'Noto Sans'");
        ui->labelConnAviable->setText("АЗС на связи!");
        return true;
    } else {
        qInfo(logInfo()) << "Проверка доступности. Сервер: " << azsConnInfo.value("server") <<  "FireBird НЕ доступен.";
        ui->labelConnAviable->setStyleSheet("color: red;font: 75 14pt 'Noto Sans'");
        ui->labelConnAviable->setText("Отсутствует связь с АЗС!");
        return false;
    }
}



void LostCheckDialog::getSelectedShift(int shiftID)
{
    lostCheck["SHIFT_ID"]=shiftID;
//    currentShift = shiftID;
    ui->lineEditShiftID->setText(QString::number(shiftID));
}

void LostCheckDialog::on_toolButtonSelectShift_clicked()
{
    SelectShiftDialog *selectShDlg = new SelectShiftDialog(modelShifts);
    connect(selectShDlg,&SelectShiftDialog::sendShiftID,this,&LostCheckDialog::getSelectedShift);
    selectShDlg->exec();
}

void LostCheckDialog::on_lineEditShiftID_textChanged(const QString &arg1)
{
    int shift = arg1.toInt();
    for(int i =0; i<modelShifts->rowCount();++i){
        if(shift == modelShifts->data(modelShifts->index(i,0)).toInt()){
            ui->labelShiftData->setText("Смена № "+modelShifts->data(modelShifts->index(i,0)).toString() +
                    " От: "+modelShifts->data(modelShifts->index(i,2)).toDateTime().toString("dd.MM.yyyy hh.mm"));
            lostCheck["SHIFT_ID"]=shift;
            shiftDate = modelShifts->data(modelShifts->index(i,2)).toDate();
            emit getPrice();
            return;
        }
    }
    ui->labelShiftData->setText("Смена не выбрана...");
}



void LostCheckDialog::azsConnections()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE",azsConnInfo.value("conName"));

    db.setHostName(azsConnInfo.value("server"));
    db.setDatabaseName(azsConnInfo.value("basename"));
    db.setUserName("SYSDBA");
    db.setPassword(azsConnInfo.value("password"));
    db.open();
}



void LostCheckDialog::possUICreate()
{
    QString strSQL = QString("SELECT p.POS_ID, p.NAME FROM POSS p where p.TERMINAL_ID=%1 and p.ISACTIVE='T'")
            .arg(lostCheck.value("TERMINAL_ID").toInt());
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelPOSs = new QSqlQueryModel();
    modelPOSs->setQuery(strSQL,dbcenter);
    ui->comboBoxPoss->setModel(modelPOSs);
    ui->comboBoxPoss->setModelColumn(1);

}

void LostCheckDialog::paytypesUICreate()
{
    QSqlDatabase azs = QSqlDatabase::database(azsConnInfo.value("conName"));
    modelPaytypes = new QSqlQueryModel();
    modelPaytypes->setQuery("SELECT p.PAYTYPE_ID, p.NAME, p.DLLNAME FROM PAYTYPES p WHERE p.PAYTYPE_ID>0 and p.ISACTIVE='T'",azs);
    ui->comboBoxPaytype->setModel(modelPaytypes);
    ui->comboBoxPaytype->setModelColumn(1);
    ui->comboBoxPaytype->setCurrentIndex(-1);
    showClientsInfo(false);


}

void LostCheckDialog::tanksFuelsUICreate()
{
    QSqlDatabase azs = QSqlDatabase::database(azsConnInfo.value("conName"));
    modelTanks = new QSqlQueryModel();
    modelTanks->setQuery("select t.TANK_ID, f.FUEL_ID, f.SHORTNAME from tanks t "
                         "LEFT JOIN FUELS f ON f.FUEL_ID = t.FUEL_ID "
                         "where t.ISACTIVE='T' AND t.ISACTIVE='T' "
                         "order by t.TANK_ID",azs);
    ui->comboBoxFuels->setModel(modelTanks);
    ui->comboBoxFuels->setModelColumn(2);
    ui->comboBoxFuels->setCurrentIndex(-1);

}

void LostCheckDialog::showClientsInfo(bool sh)
{
    if(!sh){
        ui->labelClient->hide();
        ui->labelInfo->hide();
        ui->lineEditClientCode->hide();
        ui->lineEditClientInfo->hide();
    } else {
        ui->labelClient->show();
        ui->labelInfo->show();
        ui->lineEditClientCode->show();
        ui->lineEditClientInfo->show();
    }

}




void LostCheckDialog::on_comboBoxPaytype_activated(int idx)
{
//    idxModel = modelUsers->index(idx,0,QModelIndex());
    lostCheck["PAYTYPE_ID"]=modelPaytypes->data(modelPaytypes->index(idx,0)).toInt();
    QString dllName = modelPaytypes->data(modelPaytypes->index(idx,2)).toString();
    if(dllName.toLower() == "unipos" || dllName.toLower() == "clientsdb" ) {
        showClientsInfo(true);
    } else {
        showClientsInfo(false);
    }

}

void LostCheckDialog::on_comboBoxFuels_activated(int idx)
{
    lostCheck["TANK_ID"]=modelTanks->data(modelTanks->index(idx,0)).toInt();
    lostCheck["FUEL_ID"]=modelTanks->data(modelTanks->index(idx,1)).toInt();
    ui->labelTanks->setText("Резервуар № "+lostCheck.value("TANK_ID").toString());
    emit getPrice();

    QSqlDatabase azs = QSqlDatabase::database(azsConnInfo.value("conName"));
    modelTRK = new QSqlQueryModel();
    QString strSQL = QString("select k.DISPENSER_ID AS TRK, k.TRK_ID as KRAN from TRKS k "
                             "where k.TANK_ID=%1 "
                             "order by k.DISPENSER_ID").arg(lostCheck.value("TANK_ID").toInt());
    modelTRK->setQuery(strSQL,azs);
    ui->comboBoxTRK->setModel(modelTRK);
    ui->comboBoxTRK->setModelColumn(0);
    ui->comboBoxTRK->setCurrentIndex(-1);
    ui->lineEditKran->clear();
}

void LostCheckDialog::on_comboBoxTRK_activated(int idx)
{
    lostCheck["DISPENSER_ID"]=modelTRK->data(modelTRK->index(idx,0)).toInt();
    lostCheck["TRK_ID"]=modelTRK->data(modelTRK->index(idx,1)).toInt();
    ui->lineEditKran->setText(lostCheck.value("TRK_ID").toString());
}

void LostCheckDialog::setPrice()
{
    if(lostCheck.value("SHIFT_ID").toInt() == 0 || lostCheck.value("TANK_ID").toInt() == 0) {
        price=0.00;
        ui->lineEditPrice->setText("0.00");
    } else {
        QSqlDatabase azs = QSqlDatabase::database(azsConnInfo.value("conName"));
        QSqlQuery q = QSqlQuery(azs);
        QString strSQL = QString("select t.PRICE from TANKSALDOS t "
                                   "where t.SHIFT_ID=%1 and t.TANK_ID=%2")
                .arg(lostCheck.value("SHIFT_ID").toInt())
                .arg(lostCheck.value("TANK_ID").toInt());

        q.exec(strSQL);
        q.next();
        price=q.value(0).toDouble();
        ui->lineEditPrice->setText(q.value(0).toString());
        ui->dateTimeCheck->setDate(shiftDate);
    }

}

void LostCheckDialog::setSumm()
{
    ui->lineEditSum->setText(QString::number(ui->lineEditPrice->text().toDouble()*ui->lineEditGive->text().toDouble(),'f',2));
}

void LostCheckDialog::on_lineEditPrice_textChanged()
{
    emit calcSumm();
}

void LostCheckDialog::on_lineEditGive_textChanged()
{
    emit calcSumm();
}

void LostCheckDialog::on_pushButtonChecAzs_clicked()
{
    ui->groupBoxShifts->show();
    azsConnections();
    possUICreate();
    paytypesUICreate();
    tanksFuelsUICreate();
}
