#include "devicemanagement.h"
#include "ui_devicemanagement.h"
#include "ble/bledevice.h"
#include <QDebug>
#include <QProcess>

DeviceManagement::DeviceManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceManagement)
{
    ui->setupUi(this);
    ble_service_viewer = Q_NULLPTR;
    data_handler = Q_NULLPTR;
    motion_notify = false;

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
    connect(ui->motion_on_off, SIGNAL(clicked()), this, SLOT(ToggleMotionDescriptor()));
    connect(ui->scan_ble_characteristics, SIGNAL(clicked()), this, SLOT(ScanBleCharacteristics()));

    // ble device
    ble_device = new BleDevice();
    connect(ble_device, SIGNAL(devicesUpdated()), this, SLOT(UpdateBleDeviceList()));
    connect(ble_device, SIGNAL(updateChanged()), this, SLOT(UpdateBleString()));
    connect(ble_device, SIGNAL(stateChanged()), this, SLOT(ChangeBleState()));

    connect(ble_device, SIGNAL(disconnected(QString)), this, SLOT(DisconnectBleDevice(QString)));
    connect(ble_device, SIGNAL(servicesUpdated(QString)), this, SLOT(UpdateBleServiceList(QString)));
    connect(ble_device, SIGNAL(servicesUpdateFinished(QString)), this, SLOT(SortBleServiceList(QString)));

    connect(ble_device, SIGNAL(characteristicListUpdated(QString, QString)), this, SLOT(UpdateBleCharacteristicList(QString, QString)));
    connect(ble_device, SIGNAL(characteristicValueUpdated(QString, QString, QString)), this, SLOT(UpdateBleCharacteristicValue(QString, QString, QString)));
    connect(ble_device, SIGNAL(descriptorValueUpdated(QString, QString, QString)), this, SLOT(UpdateBleDescriptorValue(QString, QString, QString)));

    // find device
    find_device = new FindDevice(this);
    connect(find_device, SIGNAL(SignalDoScan()), this, SLOT(DoDeviceScan()));
    connect(find_device, SIGNAL(SignalSelectDevice(QModelIndexList)), this, SLOT(SelectDevice(QModelIndexList)));
}

DeviceManagement::~DeviceManagement()
{
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
    device_group_model->removeRow(current_row);
}

void DeviceManagement::ConnectDevice()
{
    int row_index = ui->device_list->currentIndex().row();
    QString name = device_list_model->data(device_list_model->index(row_index, 0)).toString();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();

    if(address.isEmpty())
        return;

    if(GetCurrentDeviceGroupConnectMode() == "BLE")
    {
        ble_device->connectDevice(address);
        ble_service_viewer->SetDevice(name, address);
    }
}

void DeviceManagement::DisconnectDevice()
{
    int row_index = ui->device_list->currentIndex().row();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();

    if(address.isEmpty())
        return;

    if(GetCurrentDeviceGroupConnectMode() == "BLE")
        ble_device->disconnectDevice(address);
}

void DeviceManagement::RemoveDevice()
{
    DisconnectDevice();

    int row_index = ui->device_list->currentIndex().row();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();

    if(GetCurrentDeviceGroupConnectMode() == "BLE")
        ble_service_viewer->RemoveService(true);

    device_list_model->removeRow(row_index);
}

void DeviceManagement::ToggleMotionDescriptor()
{
    // this is temp code. later we have to check device is connected and service is discovered

    int row_index = ui->device_list->currentIndex().row();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();
    QString service_uuid = "ef680400-9b35-4933-9b10-52ffa9740042";
    QString characteristic_uuid = "ef680406-9b35-4933-9b10-52ffa9740042";
    QString descriptor_uuid = "0x2902";

    QStringList uuid;
    uuid << service_uuid << characteristic_uuid << descriptor_uuid;

    QByteArray value;
    if(motion_notify == true)
    {
        value = QByteArray::fromHex("0000");
        motion_notify = false;
    }
    else
    {
        value = QByteArray::fromHex("0100");
        motion_notify = true;
    }

    ble_device->WriteDescriptor(address, uuid, value);
}

void DeviceManagement::ScanBleCharacteristics()
{
    int row_index = ui->device_list->currentIndex().row();
    QString address = device_list_model->data(device_list_model->index(row_index, 1)).toString();
    QProcess::execute("CLS");

    ble_device->scanCharacteristics(address);
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

void DeviceManagement::UpdateBleString()
{
    ui->scan_info->setText(ble_device->property("update").toString());
}

void DeviceManagement::ChangeBleState()
{

}

void DeviceManagement::DisconnectBleDevice(QString device_address)
{
    qDebug() << "device disconnected : " << device_address;
}

void DeviceManagement::UpdateBleServiceList(QString device_address)
{
    ble_service_viewer->RemoveService(true);

    DeviceInfo *device = ble_device->getDevice(device_address);
    QList<QObject*> services = device->getServices().value<QList<QObject*>>();

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

void DeviceManagement::SortBleServiceList(QString device_address)
{
    Q_UNUSED(device_address);
    ble_service_viewer->SortService();
}

void DeviceManagement::UpdateBleCharacteristicList(QString device_address, QString service_uuid)
{
    ble_service_viewer->RemoveCharacteristic(service_uuid);

    DeviceInfo *device = ble_device->getDevice(device_address);
    ServiceInfo *service = device->getService(service_uuid);
    QList<QObject*> characteristics = service->getCharacteristics().value<QList<QObject*>>();

    foreach(auto *cobj, characteristics)
    {
        CharacteristicInfo *characteristic = qobject_cast<CharacteristicInfo *>(cobj);
        QList<QObject*> descriptors = characteristic->getDescriptors().value<QList<QObject*>>();

        ble_service_viewer->AddCharacteristic(service->getUuid(),
                                              characteristic->getName(), characteristic->getUuid(), characteristic->getValue(), characteristic->getHandle(), characteristic->getPermission());

        qDebug() << "-------" << endl;
        qDebug() << "characteristic name : " << characteristic->getName();
        qDebug() << "characteristic uuid : " << characteristic->getUuid();
        qDebug() << "characteristic value : " << characteristic->getValue();
        qDebug() << "characteristic handle : " << characteristic->getHandle();
        qDebug() << "characteristic permission : " << characteristic->getPermission();


        for(auto dobj : descriptors)
        {
            DescriptorInfo *descriptor = qobject_cast<DescriptorInfo *>(dobj);
            ble_service_viewer->AddDescriptor(service->getUuid(), characteristic->getUuid(),
                                              descriptor->getName(), descriptor->getType(), descriptor->getUuid(), descriptor->getValue());

            qDebug() << "-------" << endl;
            qDebug() << "descriptor name : " << descriptor->getName();
            qDebug() << "descriptor type : " << descriptor->getType();
            qDebug() << "descriptor uuid : " << descriptor->getUuid();
            qDebug() << "descriptor value : " << descriptor->getValue();
            qDebug() << "descriptor handle : " << descriptor->getHandle();
        }
    }
}

void DeviceManagement::UpdateBleCharacteristicValue(QString device_address, QString service_uuid, QString character_uuid)
{
    DeviceInfo *device = ble_device->getDevice(device_address);
    ServiceInfo *service = device->getService(service_uuid);
    CharacteristicInfo *character = service->getCharacteristic(character_uuid);

    ble_service_viewer->UpdateCharacteristicValue(service_uuid, character_uuid, character->getValue());
}

void DeviceManagement::UpdateBleDescriptorValue(QString device_address, QString service_uuid, QString descriptor_uuid)
{
    DeviceInfo *device = ble_device->getDevice(device_address);
    ServiceInfo *service = device->getService(service_uuid);
    QList<QObject*> characteristics = service->getCharacteristics().value<QList<QObject*>>();

    foreach(auto *cobj, characteristics)
    {
        CharacteristicInfo *characteristic = qobject_cast<CharacteristicInfo *>(cobj);
        QList<QObject*> descriptors = characteristic->getDescriptors().value<QList<QObject*>>();

        for(auto dobj : descriptors)
        {
            DescriptorInfo *descriptor = qobject_cast<DescriptorInfo *>(dobj);
            if(descriptor->getUuid() == descriptor_uuid)
            {
                ble_service_viewer->UpdateDescriptorValue(service_uuid, characteristic->getUuid(), descriptor_uuid, descriptor->getValue());
                return;
            }
        }
    }
}

void DeviceManagement::DoDeviceScan()
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

    ui->device_list->setCurrentIndex(device_list_model->index(0,0));
}

QString DeviceManagement::GetCurrentDeviceGroupConnectMode()
{
    int current_row = ui->device_group->currentIndex().row();
    return device_group_model->data(device_group_model->index(current_row, 2)).toString();
}

void DeviceManagement::SetBleServiceViewer(BleServiceViewer *viewer)
{
    if(ble_service_viewer != Q_NULLPTR)
    {
        disconnect(ble_service_viewer, SIGNAL(SignalReadCharacter(QString, QStringList)), 0, 0);
        disconnect(ble_service_viewer, SIGNAL(SignalWriteCharacter(QString, QStringList, QByteArray)), 0, 0);
        disconnect(ble_service_viewer, SIGNAL(SignalReadDescriptor(QString, QStringList)), 0, 0);
        disconnect(ble_service_viewer, SIGNAL(SignalWriteDescriptor(QString, QStringList, QByteArray)), 0, 0);
    }

    ble_service_viewer = viewer;

    connect(ble_service_viewer, SIGNAL(SignalReadCharacter(QString, QStringList)), ble_device, SLOT(ReadCharacter(QString, QStringList)));
    connect(ble_service_viewer, SIGNAL(SignalWriteCharacter(QString, QStringList, QByteArray)), ble_device, SLOT(WriteCharacter(QString, QStringList, QByteArray)));
    connect(ble_service_viewer, SIGNAL(SignalReadDescriptor(QString, QStringList)), ble_device, SLOT(ReadDescriptor(QString, QStringList)));
    connect(ble_service_viewer, SIGNAL(SignalWriteDescriptor(QString, QStringList, QByteArray)), ble_device, SLOT(WriteDescriptor(QString, QStringList, QByteArray)));
}

void DeviceManagement::SetDataHandler(CollectData *handler)
{
    if(data_handler != Q_NULLPTR)
    {
        disconnect(ble_device, SIGNAL(characteristicValueBursted(QString, QString, QString, QString)), 0, 0);
    }

    data_handler = handler;

    connect(ble_device, SIGNAL(characteristicValueChanged(QString, QString, QString, QByteArray)), data_handler, SLOT(BleDataHandle(QString, QString, QString, QByteArray)));
}
