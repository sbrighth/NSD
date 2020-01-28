#ifndef DEVICEMANAGEMENT_H
#define DEVICEMANAGEMENT_H

#include <QWidget>
#include <QStandardItemModel>
#include "adddevicegroup.h"
#include "finddevice.h"
#include "ble/bledevice.h"
#include "bleserviceviewer.h"

namespace Ui {
class DeviceManagement;
}

class DeviceManagement : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceManagement(QWidget *parent = nullptr);
    ~DeviceManagement();
    QString GetCurrentDeviceGroupConnectMode();

private slots:

    //for group push buttons
    void AddGroup();
    void FindDevices();
    void RemoveGroup();

    //for device push buttons
    void ConnectDevice();
    void DisconnectDevice();
    void RemoveDevice();

    // for device group list
    void UpdateDeviceGroup(DeviceGroupItem *item);

    //for ble device
    void UpdateBleDeviceList();
    void UpdateBleString();
    void ChangeBleState();

    void DisconnectBleDevice(QString device_address);
    void UpdateBleServiceList(QString device_address);
    void ScanBleChracteristcs(QString device_address);

    void UpdateBleCharacteristicList(QString device_address, QString service_uuid);

    //for find device widget
    void DoScan();

    //for device list
    void SelectDevice(QModelIndexList row_indexes);

    //for ble service view
    void ShowBleServices();

    void TestCharacteristics();

private:
    Ui::DeviceManagement *ui;

    QStandardItemModel *device_group_model;
    QStandardItemModel *device_list_model;
    AddDeviceGroup *add_device_group;
    FindDevice *find_device;
    BleDevice *ble_device;
    BleServiceViewer *ble_service_viewer;

    DeviceGroupItem *item;
};

#endif // DEVICEMANAGEMENT_H
