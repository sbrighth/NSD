#include "bleserviceviewer.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QHash>

BleServiceViewer::BleServiceViewer(QWidget *parent) : QWidget(parent)
{
    ble_service_tree = new QTreeWidget(this);
    QStringList tree_header;


    tree_header << "num" << "service" << "character"  << "value";
    ble_service_tree->setHeaderLabels(tree_header);

    ble_service_tree->setColumnWidth(COLUMN_NUM, 15);
    ble_service_tree->setColumnWidth(COLUMN_SERVICE, 80);
    ble_service_tree->setColumnWidth(COLUMN_SERVICE, 80);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(ble_service_tree);

    this->setLayout(layout);
    service_cnt = 0;
}

BleServiceViewer::~BleServiceViewer()
{
    delete ble_service_tree;
}


void BleServiceViewer::AddService(QString name, QString uuid, QString type)
{
    service_cnt++;

    QTreeWidgetItem *service_name = new QTreeWidgetItem();
    service_name->setText(COLUMN_SERVICE, "name");
    service_name->setText(COLUMN_VALUE, name);

    QTreeWidgetItem *service_uuid = new QTreeWidgetItem();
    service_uuid->setText(COLUMN_SERVICE, "uuid");
    service_uuid->setText(COLUMN_VALUE, uuid);

    QTreeWidgetItem *service_type = new QTreeWidgetItem();
    service_type->setText(COLUMN_SERVICE, "type");
    service_type->setText(COLUMN_VALUE, type);

    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(COLUMN_NUM, QString("service %1").arg(service_cnt));
    root->insertChild(SERVICE_NAME, service_name);
    root->insertChild(SERVICE_UUID, service_uuid);
    root->insertChild(SERVICE_TYPE, service_type);

    ble_service_tree->addTopLevelItem(root);
    ble_service_tree->expandItem(root);
}

void BleServiceViewer::RemoveService(bool all)
{
    if(all)
    {
        int cnt = ble_service_tree->topLevelItemCount();
        for(int service_idx = cnt-1; service_idx >= 0; service_idx--)
            ble_service_tree->takeTopLevelItem(service_idx);

        service_cnt = 0;
    }
    else
    {
        QTreeWidgetItem *current_service = ble_service_tree->currentItem();
        ble_service_tree->removeItemWidget(current_service, COLUMN_NUM);

        service_cnt--;
        if(service_cnt < 0)
            service_cnt = 0;
    }
}

void BleServiceViewer::AddCharacteristic(QString service_uuid, QString name, QString uuid, QString value, QString handle, QString permission)
{
    int service_cnt = ble_service_tree->topLevelItemCount();

    for(int service_idx=0; service_idx<service_cnt; service_idx++)
    {
        QTreeWidgetItem *service_root = ble_service_tree->topLevelItem(service_idx);
        if(service_root->child(SERVICE_UUID)->text(COLUMN_VALUE) != service_uuid)      //uuid
            continue;

        QTreeWidgetItem *character_name = new QTreeWidgetItem();
        character_name->setText(COLUMN_CHARACTER, "name");
        character_name->setText(COLUMN_VALUE, name);

        QTreeWidgetItem *character_uuid = new QTreeWidgetItem();
        character_uuid->setText(COLUMN_CHARACTER, "uuid");
        character_uuid->setText(COLUMN_VALUE, uuid);

        QTreeWidgetItem *character_value = new QTreeWidgetItem();
        character_value->setText(COLUMN_CHARACTER, "value");
        character_value->setText(COLUMN_VALUE, value);

        QTreeWidgetItem *character_handle = new QTreeWidgetItem();
        character_handle->setText(COLUMN_CHARACTER, "handle");
        character_handle->setText(COLUMN_VALUE, handle);

        QTreeWidgetItem *character_permission = new QTreeWidgetItem();
        character_permission->setText(COLUMN_CHARACTER, "permission");
        character_permission->setText(COLUMN_VALUE, permission);

        QTreeWidgetItem *character_root = new QTreeWidgetItem();
        character_root->setText(COLUMN_SERVICE, "chracteristic");
        character_root->insertChild(CHARACTER_NAME, character_name);
        character_root->insertChild(CHARACTER_UUID, character_uuid);
        character_root->insertChild(CHARACTER_VALUE, character_value);
        character_root->insertChild(CHARACTER_HANDLE, character_handle);
        character_root->insertChild(CHARACTER_PERMISSION, character_permission);

        service_root->addChild(character_root);
        character_root->setExpanded(false);
    }
}

void BleServiceViewer::RemoveCharacteristic(QString service_uuid)
{
    int service_cnt = ble_service_tree->topLevelItemCount();

    for(int service_idx=0; service_idx<service_cnt; service_idx++)
    {
        QTreeWidgetItem *service_root = ble_service_tree->topLevelItem(service_idx);
        if(service_root->child(SERVICE_UUID)->text(COLUMN_VALUE) != service_uuid)      //uuid
            continue;

        for(int character_idx=service_root->childCount()-1; character_idx>=SERVICE_CHARACTER; character_idx--)
            service_root->takeChild(character_idx);
    }
}

void BleServiceViewer::AddDescriptor(QString service_uuid, QString characteristic_uuid, QString name, QString type, QString uuid, QString value)
{
    int service_cnt = ble_service_tree->topLevelItemCount();

    for(int service_idx=0; service_idx<service_cnt; service_idx++)
    {
        QTreeWidgetItem *service_root = ble_service_tree->topLevelItem(service_idx);
        if(service_root->child(SERVICE_UUID)->text(COLUMN_VALUE) != service_uuid)      //uuid
            continue;

        int character_cnt = service_root->childCount() - SERVICE_CHARACTER;
        if(character_cnt <= 0)
            continue;

        for(int character_idx=0; character_idx<character_cnt; character_idx++)
        {
            QTreeWidgetItem *character_root = service_root->child(character_idx+SERVICE_CHARACTER);
            if(character_root->child(CHARACTER_UUID)->text(COLUMN_VALUE) != characteristic_uuid)
                continue;

            QTreeWidgetItem *descriptor_name = new QTreeWidgetItem();
            descriptor_name->setText(COLUMN_DESCRIPTOR, "desc_name");
            descriptor_name->setText(COLUMN_VALUE, name);

            QTreeWidgetItem *descriptor_type = new QTreeWidgetItem();
            descriptor_type->setText(COLUMN_DESCRIPTOR, "desc_type");
            descriptor_type->setText(COLUMN_VALUE, type);

            QTreeWidgetItem *descriptor_uuid = new QTreeWidgetItem();
            descriptor_uuid->setText(COLUMN_DESCRIPTOR, "desc_uuid");
            descriptor_uuid->setText(COLUMN_VALUE, uuid);

            QTreeWidgetItem *descriptor_value = new QTreeWidgetItem();
            descriptor_value->setText(COLUMN_DESCRIPTOR, "desc_value");
            descriptor_value->setText(COLUMN_VALUE, value);

            character_root->addChild(descriptor_name);
            character_root->addChild(descriptor_type);
            character_root->addChild(descriptor_uuid);
            character_root->addChild(descriptor_value);
        }
    }

}

void BleServiceViewer::SortService()
{
    ble_service_tree->sortItems(COLUMN_NUM, Qt::AscendingOrder);
}
