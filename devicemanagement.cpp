#include "devicemanagement.h"
#include "ui_devicemanagement.h"
#include "ble/device.h"
#include <QDebug>

DeviceManagement::DeviceManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceManagement),
    device_group_cnt(0)
{
    ui->setupUi(this);

    // device group
    device_group_model = new QStandardItemModel(0, 4, this);
    device_group_model->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    device_group_model->setHeaderData(1, Qt::Horizontal, QObject::tr("sensor"));
    device_group_model->setHeaderData(2, Qt::Horizontal, QObject::tr("mode"));
    device_group_model->setHeaderData(3, Qt::Horizontal, QObject::tr("count"));

    ui->device_group->setRootIsDecorated(false);
    ui->device_group->setAlternatingRowColors(true);
    ui->device_group->setModel(device_group_model);

    add_device_group = new AddDeviceGroup(this);
    connect(add_device_group, SIGNAL(SignalUpdateDeviceGroup(DeviceGroupItem*)), this, SLOT(UpdateDeviceGroup(DeviceGroupItem*)));

    connect(ui->gruop_add, SIGNAL(clicked()), this, SLOT(AddGroup()));
    connect(ui->find_device, SIGNAL(clicked()), this, SLOT(FindDevices()));
    connect(ui->group_remove, SIGNAL(clicked()), this, SLOT(RemoveGroup()));

    // device list
    device_list_model = new QStandardItemModel(0, 2, this);
    device_list_model->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    device_list_model->setHeaderData(1, Qt::Horizontal, QObject::tr("address"));

    ui->device_list->setRootIsDecorated(false);
    ui->device_list->setAlternatingRowColors(true);
    //ui->device_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->device_list->setModel(device_list_model);

    connect(ui->connect, SIGNAL(clicked()), this, SLOT(ConnectDevice()));
    connect(ui->disconnect, SIGNAL(clicked()), this, SLOT(DisconnectDevice()));
    connect(ui->remove, SIGNAL(clicked()), this, SLOT(RemoveDevice()));
    connect(ui->show_ble_services, SIGNAL(clicked()), this, SLOT(ShowBleServices()));

    // ble device
    ble_device = new Device();
    connect(ble_device, SIGNAL(devicesUpdated()), this, SLOT(UpdateBleDeviceList()));
    connect(ble_device, SIGNAL(servicesUpdated()), this, SLOT(UpdateBleServiceList()));
    connect(ble_device, SIGNAL(characteristicsUpdated()), this, SLOT(UpdateBleCharacteristicList()));
    connect(ble_device, SIGNAL(updateChanged()), this, SLOT(UpdateBleString()));
    connect(ble_device, SIGNAL(randomAddressChanged()), this, SLOT(ChangeRandomAddress()));
    connect(ble_device, SIGNAL(stateChanged()), this, SLOT(ChangeBleState()));
    connect(ble_device, SIGNAL(servicesUpdateFinished()), this, SLOT(ScanBleChracteristcs()));

    // find device
    find_device = new FindDevice(this);
    connect(find_device, SIGNAL(SignalDoScan()), this, SLOT(DoScan()));
    connect(find_device, SIGNAL(SignalSelectDevice(QModelIndexList)), this, SLOT(SelectDevice(QModelIndexList)));

    // ble service viewer
    ble_service_viewer = new BleServiceViewer(parent);
    ble_service_viewer->setGeometry(10, 10, 400, 500);
    ble_service_viewer->hide();
}

DeviceManagement::~DeviceManagement()
{
    delete ble_service_viewer;
    delete find_device;
    delete add_device_group;
    delete ble_device;
    delete device_group_model;
    delete device_list_model;
    delete ui;
}

void DeviceManagement::AddGroup()
{
    add_device_group->Clear();
    add_device_group->show();
}

void DeviceManagement::FindDevices()
{
    QString connect_mode = GetCurrentDeviceGroupConnectMode();
    qDebug() << "connect mode : " << connect_mode;

    if(connect_mode.isEmpty())
        return;

    find_device->setModal(true);
    find_device->show();
}

void DeviceManagement::RemoveGroup()
{
    int current_row = ui->device_group->currentIndex().row();
    if(device_group_model->removeRow(current_row))
        device_group_cnt = current_row-1;
}

void DeviceManagement::ConnectDevice()
{
    int row_index = ui->device_list->currentIndex().row();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();

    if(GetCurrentDeviceGroupConnectMode() == "BLE")
        ble_device->scanServices(address);
}

void DeviceManagement::DisconnectDevice()
{
    ble_device->disconnectFromDevice();
}

void DeviceManagement::RemoveDevice()
{
    int row_index = ui->device_list->currentIndex().row();
    ble_device->disconnectFromDevice();
    ble_service_viewer->RemoveService(true);
    device_list_model->removeRow(row_index);
}

void DeviceManagement::ShowBleServices()
{
    if(ble_service_viewer->isVisible())
        ble_service_viewer->hide();
    else
        ble_service_viewer->show();
}

void DeviceManagement::UpdateDeviceGroup(DeviceGroupItem* device_group_item)
{
    qDebug() << "1: " << device_group_item->group_name << endl
             << "2: " << device_group_item->target_device << endl
             << "3: " << device_group_item->connect_mode << endl
             << "4: " << device_group_item->capture_source << endl
             << "5: " << device_group_item->sampling_rate << endl
             << "6: " << device_group_item->sensor_type << endl;

    if(device_group_model->rowCount() > 0)
    {
        QModelIndexList match_list = device_group_model->match(device_group_model->index(0, 0), Qt::DisplayRole, device_group_item->group_name, 1, Qt::MatchExactly);


        foreach(QModelIndex match_idx, match_list)
        {
            qDebug() << "match index : " << match_idx.data().toString() << endl;
        }

        if(match_list.count() > 0)
            return;
    }

    QString sensors;
    foreach(QString str, device_group_item->sensor_type)
    {
        if(str == "Accelerometer")
        {
            sensors += "Acc ";
            continue;
        }
        else if(str == "Gyroscope")
        {
            sensors += "Gyro ";
            continue;
        }
        else
        {
            sensors += str;
            sensors += " ";
        }
    }
    sensors.remove(-1, 1);

    int row_count = device_group_model->rowCount();
    qDebug() << "row_count : " << row_count << endl;

    device_group_model->insertRow(row_count);
    device_group_model->setData(device_group_model->index(row_count, 0), device_group_item->group_name);
    device_group_model->setData(device_group_model->index(row_count, 1), sensors);
    device_group_model->setData(device_group_model->index(row_count, 2), device_group_item->connect_mode);
    device_group_model->setData(device_group_model->index(row_count, 3), "0");

    ui->device_group->setCurrentIndex(device_group_model->index(row_count,0));
    device_group_cnt = row_count+1;
}

void DeviceManagement::UpdateBleDeviceList()
{
    QList<QObject*> devices = ble_device->property("devicesList").value<QList<QObject*>>();
    find_device->scan_device_list_model->removeRows(0, find_device->scan_device_list_model->rowCount());

    int row = 0;
    foreach(QObject *obj, devices)
    {
        DeviceInfo *device = qobject_cast<DeviceInfo *>(obj);
        qDebug() << "-------" << endl;
        qDebug() << "device name : " << device->getName();
        qDebug() << "device address : " << device->getAddress();

        find_device->scan_device_list_model->insertRow(row);
        find_device->scan_device_list_model->setData(find_device->scan_device_list_model->index(row, 0), device->getName());
        find_device->scan_device_list_model->setData(find_device->scan_device_list_model->index(row, 1), device->getAddress());
        row++;
    }
}
void DeviceManagement::UpdateBleServiceList()
{
    ble_service_viewer->RemoveService(true);
    QList<QObject*> services = ble_device->getServices().value<QList<QObject*>>();

    foreach(QObject *obj, services)
    {
        ServiceInfo *service = qobject_cast<ServiceInfo *>(obj);
        ble_service_viewer->AddService(service->getName(), service->getUuid(), service->getType());

        qDebug() << "-------" << endl;
        qDebug() << "service name : " << service->getName();
        qDebug() << "service uuid : " << service->getUuid();
        qDebug() << "service type : " << service->getType();
    }
}

void DeviceManagement::UpdateBleCharacteristicList()
{
    QList<QObject*> characteristics = ble_device->getCharacteristics().value<QList<QObject*>>();

    foreach(QObject *obj, characteristics)
    {
        CharacteristicInfo *characteristic = qobject_cast<CharacteristicInfo *>(obj);

        QString uuid = temp_uuid;
        ble_service_viewer->AddCharacteristic(uuid, characteristic->getName(),
                                                    characteristic->getUuid(),
                                                    characteristic->getValue(),
                                                    characteristic->getHandle(),
                                                    characteristic->getPermission());

        qDebug() << "-------" << endl;
        qDebug() << "characteristic name : " << characteristic->getName();
        qDebug() << "characteristic uuid : " << characteristic->getUuid();
        qDebug() << "characteristic value : " << characteristic->getValue();
        qDebug() << "characteristic handle : " << characteristic->getHandle();
        qDebug() << "characteristic permission : " << characteristic->getPermission();
    }
}

void DeviceManagement::UpdateBleString()
{
    ui->scan_info->setText(ble_device->property("update").toString());
}

void DeviceManagement::ChangeRandomAddress()
{

}

void DeviceManagement::ChangeBleState()
{

}

void DeviceManagement::ScanBleChracteristcs()
{
    ble_service_viewer->SortService();
    QList<QObject*> services = ble_device->getServices().value<QList<QObject*>>();

    foreach(QObject* obj, services)
    {
        ServiceInfo *service = qobject_cast<ServiceInfo *>(obj);
        QString uuid = service->getUuid();
        temp_uuid = uuid;

        ble_device->connectToService(uuid);
        break;
    }
}

void DeviceManagement::DoScan()
{
    if(GetCurrentDeviceGroupConnectMode() == "BLE")
        ble_device->startDeviceDiscovery();
}

void DeviceManagement::SelectDevice(QModelIndexList row_indexes)
{
    device_list_model->removeRows(0, device_list_model->rowCount());

    int row = 0;
    foreach(const QModelIndex &row_index, row_indexes)
    {
        QModelIndex name_index = find_device->scan_device_list_model->index(row_index.row(), 0);
        QModelIndex address_index = find_device->scan_device_list_model->index(row_index.row(), 1);

        QString name = find_device->scan_device_list_model->data(name_index).toString();
        QString address = find_device->scan_device_list_model->data(address_index).toString();

        device_list_model->insertRow(row);
        device_list_model->setData(device_list_model->index(row, 0), name);
        device_list_model->setData(device_list_model->index(row, 1), address);
        row++;
    }

    ui->device_group->setCurrentIndex(device_group_model->index(0,0));
}

QString DeviceManagement::GetCurrentDeviceGroupConnectMode()
{
    int current_row = ui->device_group->currentIndex().row();
    return device_group_model->data(device_group_model->index(current_row, 2)).toString();
}
