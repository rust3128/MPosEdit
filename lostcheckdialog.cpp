#include "lostcheckdialog.h"
#include "ui_lostcheckdialog.h"
#include "selectterminaldialog.h"
#include "selectshiftdialog.h"
#include "passconv.h"
#include "insertlog.h"
#include <QSqlQuery>
#include <QSqlError>


LostCheckDialog::LostCheckDialog(int user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LostCheckDialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    createUI();
    createModelTerminals();
    currentUser = user_id;
}

LostCheckDialog::~LostCheckDialog()
{
    delete ui;
}

void LostCheckDialog::createUI()
{
    ui->labelTerminalName->setText("Терминал не указан...");
    ui->labelShiftData->setText("Смена не выбрана...");
    ui->labelConnAviable->hide();
    ui->groupBoxShifts->hide();
    ui->commandLinkButton->hide();

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
            .arg(currentTerminal);
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
            currentTerminal=terminal;
            getConnInfo(currentTerminal);
            if(validateServer()){
                ui->commandLinkButton->show();
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
    currentTerminal=termID;
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
    currentShift = shiftID;
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
            currentShift=shift;

            return;
        }
    }
    ui->labelShiftData->setText("Смена не выбрана...");
}

void LostCheckDialog::on_commandLinkButton_clicked()
{
    ui->groupBoxShifts->show();
    azsConnections();
    possUICreate();
    paytypesUICreate();
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
            .arg(currentTerminal);
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
}

