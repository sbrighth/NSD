#ifndef DEVICEMANAGEMENT_H
#define DEVICEMANAGEMENT_H

#include <QWidget>
#include <QStandardItemModel>
#include "adddevicegroup.h"
#include "finddevice.h"
#include "ble/device.h"
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
    void UpdateBleServiceList();
    void UpdateBleCharacteristicList();
    void UpdateBleString();
    void ChangeRandomAddress();
    void ChangeBleState();
    void ScanBleChracteristcs();

    //for find device widget
    void DoScan();

    //for device list
    void SelectDevice(QModelIndexList row_indexes);

    //for ble service view
    void ShowBleServices();

private:
    Ui::DeviceManagement *ui;

    QStandardItemModel *device_group_model;
    QStandardItemModel *device_list_model;
    AddDeviceGroup *add_device_group;
    FindDevice *find_device;
    Device *ble_device;
    BleServiceViewer *ble_service_viewer;

    DeviceGroupItem *item;

    int device_group_cnt;
    QString temp_uuid;
};

#endif // DEVICEMANAGEMENT_H
