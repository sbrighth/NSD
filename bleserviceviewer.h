#ifndef BLESERVICEVIEWER_H
#define BLESERVICEVIEWER_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class BleServiceViewer : public QWidget
{
    Q_OBJECT
public:
    explicit BleServiceViewer(QWidget *parent = nullptr);
    ~BleServiceViewer();

    void SetDevice(QString device_name, QString device_address);
    void AddService(QString name, QString uuid, QString type);
    void RemoveService(bool all=false);
    void AddCharacteristic(QString service_uuid, QString name, QString uuid, QString value, QString handle, QString permission);
    void RemoveCharacteristic(QString service_uuid);
    void AddDescriptor(QString service_uuid, QString characteristic_uuid, QString name, QString type, QString uuid, QString value);

    void SortService();

private:  
    enum _ITEM_TYPE
    {
        ITEM_SERVICE=0,
        ITEM_CHARACTER,
        ITEM_DESCRIPTOR
    };

    enum _COLUMN_IDX
    {
        COLUMN_NUM=0,
        COLUMN_SERVICE,
        COLUMN_CHARACTER,
        COLUMN_DESCRIPTOR=COLUMN_CHARACTER,
        COLUMN_VALUE,
        COLUMN_IDX_CNT
    };
    enum _SERVICE_IDX
    {
        SERVICE_NAME=0,
        SERVICE_UUID,
        SERVICE_TYPE,
        SERVICE_CHARACTER,
        SERVICE_IDX_CNT
    };
    enum _CHARACTER_IDX
    {
        CHARACTER_NAME=0,
        CHARACTER_UUID,
        CHARACTER_VALUE,
        CHARACTER_HANDLE,
        CHARACTER_PERMISSION,
        CHARACTER_IDX_CNT
    };
    enum _DESCRIPTOR_IDX
    {
        DESCRIPTOR_NAME=0,
        DESCRIPTOR_TYPE,
        DESCRIPTOR_UUID,
        DESCRIPTOR_VALUE,
        DESCRIPTOR_IDX_CNT
    };

    QTreeWidget *ble_service_tree;
    QLabel *item_name;
    QLineEdit *item_value;
    QPushButton *value_read, *value_write;

    enum _ITEM_TYPE item_type;
    QString item_service_uuid;
    QString item_character_uuid;
    QString item_descriptor_uuid;
    QString device_name;
    QString device_address;

public slots:
    void DeleteDevice(QString device_address);
    void UpdateCharacteristicValue(QString service_uuid, QString characteristic_uuid, QString value);
    void UpdateDescriptorValue(QString service_uuid, QString characteristic_uuid, QString descriptor_uuid, QString value);

private slots:
    void ViewCurrentItem(QTreeWidgetItem* item, int column);
    void ReadBtnClicked();
    void WriteBtnClicked();

signals:
    void SignalReadCharacter(QString device_address, QStringList uuid);
    void SignalWriteCharacter(QString device_address, QStringList uuid, QByteArray val);
    void SignalReadDescriptor(QString device_address, QStringList uuid);
    void SignalWriteDescriptor(QString device_address, QStringList uuid, QByteArray val);
};

#endif // BLESERVICEVIWER_H
