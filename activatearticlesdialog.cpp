#include "activatearticlesdialog.h"
#include "ui_activatearticlesdialog.h"
#include "selectterminaldialog.h"
#include "loggingcategories.h"
#include "activatearticles.h"
#include "passconv.h"
#include "insertlog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QThread>

ActivateArticlesDialog::ActivateArticlesDialog(int user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActivateArticlesDialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    createUI();
    createModelTerminals();
    currentUser = user_id;

}

ActivateArticlesDialog::~ActivateArticlesDialog()
{
    delete ui;
}

void ActivateArticlesDialog::createUI()
{
    ui->lineEditTerminalID->clear();
    ui->labelTerminalName->setText("Терминал не указан...");
    ui->labelConnAviable->hide();
    ui->commandLinkButton->setEnabled(false);
    ui->groupBoxProgress->hide();
    ui->frameActivation->hide();
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
            getConnInfo(currentTerminal);
            if(validateServer()) ui->commandLinkButton->setEnabled(true);
            return;
        }
    }
    ui->labelTerminalName->setText("Термиал не указан...");
    ui->commandLinkButton->setEnabled(false);
    ui->labelConnAviable->hide();
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

}

bool ActivateArticlesDialog::validateServer()
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

void ActivateArticlesDialog::on_commandLinkButton_clicked()
{
    ActivateArticles *articles = new ActivateArticles(azsConnInfo);
    QThread *thread = new QThread(this);



    connect(thread,&QThread::started,this,&ActivateArticlesDialog::startActivation);
    connect(thread,&QThread::started,articles,&ActivateArticles::activadedGo);
    connect(thread,&QThread::finished,this,&ActivateArticlesDialog::finishActibation);

    connect(articles,&ActivateArticles::finActivated,thread,&QThread::terminate,Qt::DirectConnection);
    connect(articles,&ActivateArticles::sendConnStatus,this,&ActivateArticlesDialog::getConnStatus,Qt::QueuedConnection);
    connect(articles,&ActivateArticles::sendExecStatus,this,&ActivateArticlesDialog::finishExecute,Qt::QueuedConnection);

    thread->start();
    articles->moveToThread(thread);
}

void ActivateArticlesDialog::startActivation()
{
    ui->labelStatusConn->clear();
    ui->labelStatusActivation->clear();
    ui->groupBoxProgress->show();
    ui->pushButtonClose->setEnabled(false);
    ui->pushButtonOtherAzs->setEnabled(false);
}

void ActivateArticlesDialog::finishActibation()
{
    ui->pushButtonClose->setEnabled(true);
    ui->pushButtonOtherAzs->setEnabled(true);
    qInfo(logInfo()) << "АЗС" << currentTerminal << "Активация товаров с не нулевым остатком.";
    insertLog(3,currentUser,currentTerminal,0,0,"");
}

void ActivateArticlesDialog::getConnStatus(bool status)
{
    if(status){
        ui->labelStatusConn->setStyleSheet("color: green");
        ui->labelStatusConn->setText("Соедиение установлено!");
        ui->progressBar->hide();
        ui->frameActivation->show();
    } else {
        ui->labelStatusConn->setStyleSheet("color: red");
        ui->labelStatusConn->setText("Не возможно соединится с базой АЗС!");
    }

}

void ActivateArticlesDialog::finishExecute()
{
    ui->progressBar_3->hide();
    ui->labelStatusActivation->setStyleSheet("color: green");
    ui->labelStatusActivation->setText("Товары активированы!");

}


void ActivateArticlesDialog::on_pushButtonClose_clicked()
{
    this->reject();
}

void ActivateArticlesDialog::on_pushButtonOtherAzs_clicked()
{
    createUI();
}
