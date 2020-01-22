#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    device_management = new DeviceManagement(this);
    ui->dock_layout->addWidget(device_management);
}

MainWindow::~MainWindow()
{
    delete ui;
}

