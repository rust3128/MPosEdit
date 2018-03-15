#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1600,900);

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

    loginDlg->setFixedSize(496,184);
    loginDlg->move(this->geometry().center().x() - loginDlg->geometry().center().x(), this->geometry().center().y() - loginDlg->geometry().center().y());
    loginDlg->exec();
//    this->setCentralWidget(loginDlg);
}
