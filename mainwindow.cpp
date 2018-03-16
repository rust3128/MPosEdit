#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "loggingcategories.h"
#include <QMessageBox>

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
