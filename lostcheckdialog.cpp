#include "lostcheckdialog.h"
#include "ui_lostcheckdialog.h"
#include "selectterminaldialog.h"
#include "selectshiftdialog.h"
#include "passconv.h"
#include "insertlog.h"
#include "myfiledialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QValidator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>



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
    lostCheck.insert("DISPENSER_ID",-1);
    lostCheck.insert("TRK_ID",0);
    lostCheck.insert("TANK_ID",0);
    lostCheck.insert("FUEL_ID",-1);
    lostCheck.insert("GIVE",0);
    lostCheck.insert("ORDERED",0);
    lostCheck.insert("SUMMA",0);
    lostCheck.insert("CASH",0);
    lostCheck.insert("DISCOUNTSUMMA",0);
    lostCheck.insert("PAYTYPE_ID",-1);
    lostCheck.insert("NUM_CHECK",0);
    lostCheck.insert("NUM_CHECK_RETURN",0);
    lostCheck.insert("TRANSACTION_ID",0);
    lostCheck.insert("SEC",0);
    lostCheck.insert("ISLAST","T");
    lostCheck.insert("INFO_CODE",-1);
    lostCheck.insert("INFO_TEXT","");
    lostCheck.insert("POS_ID",1);
    lostCheck.insert("ZNUMBER",0);
    lostCheck.insert("OPERATOR_ID",-1);
    lostCheck.insert("SALEORDER_ID","GEN_ID(GEN_SALEORDERS, 1)");
    lostCheck.insert("PRICE",0);
    lostCheck.insert("ISBEFOREPAY","F");
    lostCheck.insert("POSTRANSACTION_ID",0);
    lostCheck.insert("POSTRNRETURN_ID",0);
    lostCheck.insert("SHARE_ID",0);
    lostCheck.insert("MPOSCHECK_ID",":CHECK_ID");
    lostCheck.insert("PAYTYPE_ID2",1);
    lostCheck.insert("SUMMA2",0.00);
    lostCheck.insert("DISCOUNTSUMMA2",0.00);
    lostCheck.insert("DAT",0);
    lostCheck.insert("GOV_NUMBER","");
    lostCheck.insert("BONUSCARD","");
}


void LostCheckDialog::createUI()
{
    QValidator *valTerminals = new QIntValidator(1, 999999999,this);
//    QValidator *valDouble = new QDoubleValidator(0.00,9999999.99,2,this);
    ui->lineEditTerminalID->setValidator(valTerminals);
    ui->lineEditShiftID->setValidator(valTerminals);
    ui->lineEditNumCheck->setValidator(valTerminals);
//    ui->lineEditGive->setValidator(valDouble);
//    ui->lineEditPrice->setValidator(valDouble);


    ui->lineEditTerminalID->clear();
    ui->labelTerminalName->setText("Терминал не указан...");
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->labelConnAviable->hide();
    ui->groupBoxShifts->hide();
    ui->pushButtonChecAzs->hide();
    ui->frameExecute->hide();
    ui->labelStatusExecute->hide();
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
            lostCheck["ZNUMBER"]=modelShifts->data(modelShifts->index(i,1)).toInt();
            lostCheck["OPERATOR_ID"]=modelShifts->data(modelShifts->index(i,4)).toInt();
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
        ui->lineEditClientInfo->hide();    ui->pushButtonRunScript->setEnabled(true);
        ui->pushButtonSaveScript->setEnabled(true);
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
    ui->pushButtonRunScript->setEnabled(true);
    ui->pushButtonSaveScript->setEnabled(true);
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


void LostCheckDialog::generateScript()
{
    if(!validLostCheck()){
        validateData=false;
        return;
    }
    validateData=true;



    lostCheck["NUM_CHECK"]=ui->lineEditNumCheck->text().toInt();
    lostCheck["GIVE"] = lostCheck["ORDERED"] = ui->lineEditGive->text().toDouble();
    lostCheck["SUMMA"]=ui->lineEditSum->text().toDouble();
    lostCheck["DAT"]=ui->dateTimeCheck->dateTime().toString("yyyy/MM/dd hh:mm:ss");
    lostCheck["PRICE"]=ui->lineEditPrice->text().toDouble();
    lostCheck["DISCOUNTSUMMA"]=ui->lineEditDiscount->text().toDouble();
    if(ui->lineEditClientInfo->isVisible()){
        lostCheck["INFO_CODE"]=ui->lineEditClientCode->text().toInt();
        lostCheck["INFO_TEXT"]=ui->lineEditClientInfo->text().trimmed();
    }


    script.clear();
    strSQL.clear();
    endScript.clear();

    script << "CREATE OR ALTER PROCEDURE TMP_LOST_CHECK ";
    script << "AS ";
    script << "DECLARE VARIABLE CHECK_ID T_INT;";
    script << "BEGIN ";
    script << "CHECK_ID = GEN_ID(GEN_MPOSCHECKS, 1);";
    script << "EXECUTE PROCEDURE APPEND_FSALE(";
    script << QString("/*TERMINAL_ID - терминал*/                           %1,").arg(lostCheck.value("TERMINAL_ID").toString());
    script << QString("/*SHIFT_ID - № смены*/                               %1,").arg(lostCheck.value("SHIFT_ID").toString());
    script << QString("/*DISPENSER_ID - номер колонки*/                     %1,").arg(lostCheck.value("DISPENSER_ID").toString());
    script << QString("/*TRK_ID - номер пистолета*/                         %1,").arg(lostCheck.value("TRK_ID").toString());
    script << QString("/*TANK_ID - номер резервуара */                      %1,").arg(lostCheck.value("TANK_ID").toString());
    script << QString("/*FUEL_ID - код топлива */                           %1,").arg(lostCheck.value("FUEL_ID").toString());
    script << QString("/*GIVE - залито */                                   %1,").arg(lostCheck.value("GIVE").toString());
    script << QString("/*ORDERED - заказано */                              %1,").arg(lostCheck.value("ORDERED").toString());
    script << QString("/*SUMMA - сумма */                                   %1,").arg(lostCheck.value("SUMMA").toString());
    script << QString("/*CASH - не трогать */                               %1,").arg(lostCheck.value("CASH").toString());
    script << QString("/*DISCOUNTSUMMA - сумма скидки НЕ отрицательна */    %1,").arg(lostCheck.value("DISCOUNTSUMMA").toString());
    script << QString("/*PAYTYPE_ID - вид оплаты*/                          %1,").arg(lostCheck.value("PAYTYPE_ID").toString());
    script << QString("/*NUM_CHECK - номер чека */                          %1,").arg(lostCheck.value("NUM_CHECK").toString());
    script << QString("/*NUM_CHECK_RETURN - номер чека возврата*/           %1,").arg(lostCheck.value("NUM_CHECK_RETURN").toString());
    script << QString("/*TRANSACTION_ID - номер транзакции*/                %1,").arg(lostCheck.value("TRANSACTION_ID").toString());
    script << QString("/*SEC - врем€ заправки в секундах*/                  %1,").arg(lostCheck.value("SEC").toString());
    script << QString("/*ISLAST - не трогать */                            '%1',").arg(lostCheck.value("ISLAST").toString());
    script << QString("/*INFO_CODE - код клиента*/                          %1,").arg(lostCheck.value("INFO_CODE").toString());
    script << QString("/*INFO_TEXT - название клиента (номер карты)*/      '%1',").arg(lostCheck.value("INFO_TEXT").toString());
    script << QString("/*POS_ID - не трогать*/                              %1,").arg(lostCheck.value("POS_ID").toString());
    script << QString("/*ZNUMBER - № Z-отчета*/                             %1,").arg(lostCheck.value("ZNUMBER").toString());
    script << QString("/*OPERATOR_ID - код оператора*/                      %1,").arg(lostCheck.value("OPERATOR_ID").toString());
    script << QString("/*SALEORDER_ID - не трогать */                       %1,").arg(lostCheck.value("SALEORDER_ID").toString());
    script << QString("/*PRICE - цена*/                                     %1,").arg(lostCheck.value("PRICE").toString());
    script << QString("/*ISBEFOREPAY - предоплата*/                        '%1',").arg(lostCheck.value("ISBEFOREPAY").toString());
    script << QString("/*POSTRANSACTION_ID*/                                %1,").arg(lostCheck.value("POSTRANSACTION_ID").toString());
    script << QString("/*POSTRETURN_ID*/                                    %1,").arg(lostCheck.value("POSTRNRETURN_ID").toString());
    script << QString("/*SHARE_ID*/                                         %1,").arg(lostCheck.value("SHARE_ID").toString());
    script << QString("/*MPOSCHECK_ID*/                                     %1,").arg(lostCheck.value("MPOSCHECK_ID").toString());
    script << QString("/*PAYTYPE_ID2 - вид оплаты*/                         %1,").arg(lostCheck.value("PAYTYPE_ID2").toString());
    script << QString("/*SUMMA2 - сумма */                                  %1,").arg(lostCheck.value("SUMMA2").toString());
    script << QString("/*DISCOUNTSUMMA2 - сумма скидки не отрицательная*/   %1,").arg(lostCheck.value("DISCOUNTSUMMA2").toString());
    script << QString("/*DAT - дата/времz продажи*/                        '%1',").arg(lostCheck.value("DAT").toString());
    script << QString("/*GOV_NUMBER*/                                      '%1',").arg(lostCheck.value("GOV_NUMBER").toString());
    script << QString("/*BONUSCARD*/                                       '%1');").arg(lostCheck.value("BONUSCARD").toString());
    script << "END;";

    endScript << "EXECUTE PROCEDURE TMP_LOST_CHECK;";
    endScript << "DROP PROCEDURE TMP_LOST_CHECK;";
    endScript << "COMMIT WORK;";

}

bool LostCheckDialog::validLostCheck()
{
    if(lostCheck.value("SHIFT_ID").toInt()==0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не выбрана смена!"));
        return false;
    }
    if(lostCheck.value("PAYTYPE_ID").toInt() == -1){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не выбран вид оплаты!"));
        return false;
    }
    if(lostCheck.value("FUEL_ID").toInt() == -1){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не выбран вид топлива!"));
        return false;
    }
    if(lostCheck.value("DISPENSER_ID").toInt() == -1){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не выбрана ТРК!"));
        return false;
    }
    if(ui->lineEditPrice->text().toDouble()<=0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не указана цена топлива!"));
        return false;
    }
    if(ui->lineEditGive->text().toDouble()<=0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не указан литраж!"));
        return false;
    }
    if(ui->lineEditSum->text().toDouble()<=0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не указана сумма!"));
        return false;
    }
    if(ui->lineEditNumCheck->text().toInt()<=0){
        QMessageBox::critical(0, qApp->tr("Ошибка"),
                              QString("Не указан номер чека!"));
        return false;
    }

    if(ui->lineEditClientInfo->isVisible()){
        if(ui->lineEditClientCode->text().isEmpty()){
            QMessageBox::critical(0, qApp->tr("Ошибка"),
                                  QString("Не указан код клиента!"));
            return false;
        }
        if(ui->lineEditClientInfo->text().isEmpty()){
            QMessageBox::critical(0, qApp->tr("Ошибка"),
                                  QString("Не указано описание клиента!"));
            return false;
        }
    }

    QSqlDatabase azs = QSqlDatabase::database(azsConnInfo.value("conName"));
    QSqlQuery q = QSqlQuery(azs);

    q.prepare("select COUNT(*) from MPOSCHECKS m WHERE m.SHIFT_ID=:shift and m.NUMBERCHECK=:checkNum");
    q.bindValue(":shift",lostCheck.value("SHIFT_ID").toInt());
    q.bindValue(":checkNum",ui->lineEditNumCheck->text().toInt());
    q.exec();
    q.next();
    if(q.value(0).toInt()==1){
        QMessageBox::critical(this, qApp->tr("Ошибка"),
                              QString("Чек с указанным номером уже есть в протоколе продаж!"));
        return false;
    }


    return true;

}



void LostCheckDialog::on_pushButtonRunScript_clicked()
{
    generateScript();
    if(!validateData)
        return;

    lchExec = new LostCheckExecute(azsConnInfo);
    thread = new QThread(this);
    connect(this,&LostCheckDialog::sendScript,lchExec,&LostCheckExecute::getScript);
    emit sendScript(script);

    connect(thread,&QThread::started,this,&LostCheckDialog::startEcecute);
    connect(thread,&QThread::started,lchExec,&LostCheckExecute::lostCheckGo);
    connect(thread,&QThread::finished,this,&LostCheckDialog::finishExecute);

    connect(lchExec,&LostCheckExecute::finLostCheck,thread,&QThread::terminate,Qt::DirectConnection);
    connect(lchExec,&LostCheckExecute::sendExecStatus,this,&LostCheckDialog::getStatusExexc,Qt::DirectConnection);


    thread->start();
    lchExec->moveToThread(thread);

}


void LostCheckDialog::startEcecute()
{
    ui->frameExecute->show();

}

void LostCheckDialog::finishExecute()
{
    if(statusExecute){
        ui->labelStatusExecute->setText("Чек успешно восстановлен!");
        QString strToLog;
        QTextStream in(&strToLog, QIODevice::WriteOnly | QIODevice::Text);
        QListIterator<QString> i(script);
        while (i.hasNext()) {
            in << i.next() << endl;
        }
        QListIterator<QString> e(endScript);
        while (e.hasNext()) {
            in << e.next() << endl;
        }
        insertLog(2, currentUser, lostCheck.value("TERMINAL_ID").toInt(), lostCheck.value("SHIFT_ID").toInt(),
                  lostCheck.value("NUM_CHECK").toInt(), in.readAll() );

    } else {
        ui->labelStatusExecute->setText("Восстановить чек не удалось!");
    }
    ui->groupBoxShifts->hide();
    ui->groupBoxTerminals->hide();
    ui->pushButtonChecAzs->hide();
    ui->progressBarExecute->hide();
    ui->labelStatusExecute->show();

}

void LostCheckDialog::getStatusExexc(bool status)
{
    statusExecute = status;
}

void LostCheckDialog::on_pushButtonSaveScript_clicked()
{
    generateScript();
    if(!validateData)
        return;
    QString curPath = QDir::currentPath()+"//LostCheck";
    QString fileNameLost = QString("Check_%1_%2.sql").arg(lostCheck.value("TERMINAL_ID").toString()).arg(lostCheck.value("NUM_CHECK").toString());

    QDir dir(curPath);
    if(!dir.exists()) {
        dir.mkdir(curPath);
    }
    dir.cd(curPath);

//    MyFileDialog *fileDialog = new MyFileDialog();
//    fileDialog->show();
//    fileDialog->exec();
//    QString fileName = fileDialog->getSaveFileName(this,"Сохранить скрипт",curPath+"//"+fileNameLost,"SQL file (*.sql);;Все файлы (*.*)");

    QString fileName = QFileDialog::getSaveFileName(this,"Сохранить скрипт",curPath+"//"+fileNameLost,
                                           "SQL file (*.sql);;Все файлы (*.*)");

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream qout(&file);

    QListIterator<QString> i(script);
    while (i.hasNext()) {
        qout << i.next() << endl;
    }
    QListIterator<QString> e(endScript);
    while (e.hasNext()) {
        qout << e.next() << endl;
    }

    file.close();

    ui->groupBoxShifts->hide();
    ui->groupBoxTerminals->hide();
    ui->pushButtonChecAzs->hide();
    ui->progressBarExecute->hide();
    ui->labelStatusExecute->show();
}

void LostCheckDialog::on_pushButtonClose_clicked()
{
    this->reject();
}

void LostCheckDialog::on_comboBoxPoss_activated(int idx)
{
    lostCheck["POS_ID"]=modelPOSs->data(modelPOSs->index(idx,0)).toInt();
}
