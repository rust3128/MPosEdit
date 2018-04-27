#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "logsdialog.h"
#include "loggingcategories.h"
#include "connectioneditdialog.h"
#include "clearsaleordersdialog.h"
#include "connectionlistdialog.h"
#include "usersdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1024,768);
    createStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    QMainWindow::show();
    showLoginDialog();
}

void MainWindow::updateConnectionName(QString connName)
{
    ui->labelConnName->setStyleSheet("color: rgb(0, 85, 0)");
    ui->labelConnName->setText(connName);
}

void MainWindow::showLoginDialog()
{
    LoginDialog *loginDlg = new LoginDialog();

    loginDlg->setFixedSize(322,211);
    loginDlg->move(this->geometry().center().x() - loginDlg->geometry().center().x(), this->geometry().center().y() - loginDlg->geometry().center().y());
    loginDlg->exec();
    if(loginDlg->result()==QDialog::Accepted){
        currentUser = loginDlg->getUserData();
        setupUserInterface();
        sendUser2StatusBar();
        selectCentralDB();
    } else {
        QMessageBox::critical(this,"Ошибка входа",
                              "Не выполнен вход в систему!<br>Дальнейшая работа не возможна.");
        qCritical(logCritical()) << "Не выполнен вход в систему. Закрытие программы.";
        this->close();
    }
}

void MainWindow::createStatusBar()
{
    ui->labelUser->hide();
    ui->statusBar->addPermanentWidget(ui->labelUser);
    ui->statusBar->addPermanentWidget(ui->labelConnText);
    ui->statusBar->addPermanentWidget(ui->labelConnName);
    ui->labelConnName->setStyleSheet("color: rgb(255, 0, 0)");
    ui->labelConnName->setText("Нет");
}

void MainWindow::sendUser2StatusBar()
{
    ui->labelUser->setText("Пользователь: "+currentUser.value("user_fio").toString());
    ui->labelUser->show();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    qInfo(logInfo()) << "Завершение работы программы.";
}


void MainWindow::selectCentralDB()
{
    int conCur;
    QSqlDatabase dblite = QSqlDatabase::database("options");
    modelConnect = new QSqlTableModel(this,dblite);

    modelConnect->setTable("connections");
    modelConnect->select();

    switch (modelConnect->rowCount()) {
    case 0:
        //Создание нового подключения
        addNewConnection();
        break;
    case 1:
        connCentralDB(1);
        break;
    default:
        for(int i; i<modelConnect->rowCount();++i) {
                    if(modelConnect->data(modelConnect->index(i,6,QModelIndex())).toInt()==1){
                        conCur = i;
                        break;
                    }
                }
                qDebug() << "Currentd ID connections" << conCur;
                connCentralDB(conCur);
        break;
    }
}

void MainWindow::setupUserInterface()
{
    if(currentUser.value("user_id").toInt()!=1){
        ui->actionUsers->setEnabled(false);
    }
}

void MainWindow::addNewConnection()
{
    ConnectionEditDialog *connNewDlg = new ConnectionEditDialog(-1);
    connect(connNewDlg,&ConnectionEditDialog::sendConnectionName,this,&MainWindow::updateConnectionName);
    connNewDlg->move(this->geometry().center().x() - connNewDlg->geometry().center().x(), this->geometry().center().y() - connNewDlg->geometry().center().y());
    connNewDlg->exec();
}

void MainWindow::connCentralDB(int connID)
{

    QSqlDatabase dbcentr = QSqlDatabase::addDatabase("QIBASE","central");

    dbcentr.setHostName(modelConnect->data(modelConnect->index(connID,2)).toString());
    dbcentr.setDatabaseName(modelConnect->data(modelConnect->index(connID,3)).toString());
    dbcentr.setUserName(modelConnect->data(modelConnect->index(connID,4)).toString());
    dbcentr.setPassword(modelConnect->data(modelConnect->index(connID,5)).toString());

    if(!dbcentr.open()) {
        qCritical(logCritical()) << "Не возможно подключится к центральной базе данных" << dbcentr.lastError().text();
        QMessageBox::critical(0,"Ошибка подключения", QString("Не возможно открыть базу данных!\n%1\nПроверьте настройку подключения.")
                .arg(dbcentr.lastError().text()),
                QMessageBox::Ok);
        updateConnectionName("Ошибка подключения!");
        return;
    }
    updateConnectionName(modelConnect->data(modelConnect->index(connID,1)).toString()+" "
                         +modelConnect->data(modelConnect->index(connID,2)).toString()+":"
                         +modelConnect->data(modelConnect->index(connID,3)).toString());

}



void MainWindow::on_actionClearSaleorders_triggered()
{
    ClearSaleordersDialog *clearSaleordersDlg = new ClearSaleordersDialog(currentUser.value("user_id").toInt());
    this->setCentralWidget(clearSaleordersDlg);
    this->setWindowTitle(this->windowTitle()+" Удаление продаж");
    clearSaleordersDlg->exec();
}

void MainWindow::on_actionUsers_triggered()
{
    UsersDialog *userDlg = new UsersDialog();
    userDlg->exec();

}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionProtokol_triggered()
{
    LogsDialog *logsDlg = new LogsDialog();
    this->setCentralWidget(logsDlg);
    this->setWindowTitle(this->windowTitle()+" Протокол операций");
    logsDlg->exec();
}

void MainWindow::on_actionConnectionList_triggered()
{
    ConnectionListDialog *connListDlg = new ConnectionListDialog();
    connListDlg->exec();
}
