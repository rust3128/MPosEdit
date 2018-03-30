#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "loggingcategories.h"
#include "connectioneditdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1600,900);
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



void MainWindow::selectCentralDB()
{
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
        //Единственное подключение
        connCentralDB(0);

        break;
    default:
        break;
    }
    modelsCreate();
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


void MainWindow::modelsCreate()
{
    QSqlDatabase dbcenter = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    modelTerminals->setQuery("SELECT t.TERMINAL_ID, TRIM(t.NAME) FROM TERMINALS t "
                             "WHERE t.TERMINALTYPE=3 and t.ISACTIVE='T' "
                             "ORDER BY t.TERMINAL_ID",dbcenter);


    QTableView *tv = new QTableView(this);
    tv->setModel(modelTerminals);
//    tv->horizontalHeader()->hide();
    tv->verticalHeader()->hide();

    tv->setBaseSize(200,100);

    tv->resizeColumnsToContents();
    tv->verticalHeader()->setDefaultSectionSize(tv->verticalHeader()->minimumSectionSize());
    tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->comboBox->setModel(modelTerminals);
    ui->comboBox->setView(tv);




}
