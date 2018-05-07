#include "activatearticlesdialog.h"
#include "ui_activatearticlesdialog.h"
#include "selectterminaldialog.h"
#include "loggingcategories.h"
#include "passconv.h"
#include <QSqlQuery>
#include <QSqlError>

ActivateArticlesDialog::ActivateArticlesDialog(int user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActivateArticlesDialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    ui->labelTerminalName->setText("Терминал не указан...");
    ui->commandLinkButton->setEnabled(false);
    createModelTerminals();
    currentUser = user_id;

}

ActivateArticlesDialog::~ActivateArticlesDialog()
{
    delete ui;
}

void ActivateArticlesDialog::createModelTerminals()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    modelTerminals->setQuery("SELECT t.terminal_id, TRIM(t.name) AS name FROM TERMINALS t "
                             "WHERE t.terminal_id<30000 and t.TERMINALTYPE=3 and t.ISACTIVE='T' "
                             "ORDER BY t.TERMINAL_ID",dbcenter);
}



void ActivateArticlesDialog::on_lineEditTerminalID_textChanged(const QString &arg1)
{
    int terminal = arg1.toInt();
    for(int i = 0;i<modelTerminals->rowCount();++i){
        if(terminal == modelTerminals->data(modelTerminals->index(i,0)).toInt()){
            ui->labelTerminalName->setText(modelTerminals->data(modelTerminals->index(i,1)).toString());
            currentTerminal=terminal;
            ui->commandLinkButton->setEnabled(true);
            getConnInfo(currentTerminal);
            return;
        }
    }
    ui->labelTerminalName->setText("Термиал не указан...");
    ui->commandLinkButton->setEnabled(false);
}

void ActivateArticlesDialog::on_toolButtonSelectTerminal_clicked()
{
    SelectTerminalDialog *selectTermDlg = new SelectTerminalDialog(modelTerminals);
    connect(selectTermDlg,&SelectTerminalDialog::sendTerminalID,this,&ActivateArticlesDialog::getSelectedTerminal);
//    selectTermDlg->move(this->geometry().center().x() - selectTermDlg->geometry().center().x(), this->geometry().center().y() - selectTermDlg->geometry().center().y());
    selectTermDlg->exec();
}

void ActivateArticlesDialog::getSelectedTerminal(int termID)
{
    currentTerminal=termID;
    ui->lineEditTerminalID->setText(QString::number(termID));

}

void ActivateArticlesDialog::getConnInfo(int terminal_id)
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
    qInfo(logInfo()) << azsConnInfo;

}
