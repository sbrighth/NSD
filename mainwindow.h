#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "devicemanagement.h"
#include "bleserviceviewer.h"
#include "collectdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public:
    DeviceManagement* device_management;
    BleServiceViewer* ble_service_viewer;
    CollectData *collect_data;
};
#endif // MAINWINDOW_H
