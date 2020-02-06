#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    device_management = new DeviceManagement(this);
    ble_service_viewer = new BleServiceViewer(this);
    device_management->SetBleServiceViewer(ble_service_viewer);

    ui->dock_layout->addWidget(device_management);
    ui->tabWidget->addTab(ble_service_viewer, "ble_service");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete device_management;
    delete ble_service_viewer;
}

