#ifndef BLESERVICEVIEWER_H
#define BLESERVICEVIEWER_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class BleServiceViewer : public QWidget
{
    Q_OBJECT
public:
    explicit BleServiceViewer(QWidget *parent = nullptr);
    ~BleServiceViewer();

    void AddService(QString name, QString uuid, QString type);
    void RemoveService(bool all=false);
    void AddCharacteristic(QString service_uuid, QString name, QString uuid, QString value, QString handle, QString permission);
    void RemoveCharacteristic(QString service_uuid);

public:
    QTreeWidget *ble_service_tree;

private:
    int service_cnt;

    enum _COLUMN_IDX
    {
        COLUMN_NUM=0,
        COLUMN_SERVICE,
        COLUMN_CHARACTER,
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

public slots:
    void SortService();
signals:

};

#endif // BLESERVICEVIWER_H
