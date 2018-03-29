#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "loggingcategories.h"
#include "connectioneditdialog.h"
#include <QMessageBox>
#include <QDebug>

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

void MainWindow::showLoginDialog()
{
    LoginDialog *loginDlg = new LoginDialog();

    loginDlg->setFixedSize(322,211);
    loginDlg->move(this->geometry().center().x() - loginDlg->geometry().center().x(), this->geometry().center().y() - loginDlg->geometry().center().y());
    loginDlg->exec();
    if(loginDlg->result()==QDialog::Accepted){
        currentUser = loginDlg->getUserData();
        sendUser2StatusBar();
        connCentralDB();
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

void MainWindow::connCentralDB()
{
    QSqlDatabase dblite = QSqlDatabase::database("QSQLITE","options");
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
        break;
    default:
        break;
    }

}

void MainWindow::addNewConnection()
{
    ConnectionEditDialog *connNewDlg = new ConnectionEditDialog(-1);
    connNewDlg->move(this->geometry().center().x() - connNewDlg->geometry().center().x(), this->geometry().center().y() - connNewDlg->geometry().center().y());
    connNewDlg->exec();
}
