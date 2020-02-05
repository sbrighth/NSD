#include "bleserviceviewer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>


BleServiceViewer::BleServiceViewer(QWidget *parent) : QWidget(parent)
{
    ble_service_tree = new QTreeWidget(this);
    QStringList tree_header;

    tree_header << "num" << "service" << "character"  << "value";
    ble_service_tree->setHeaderLabels(tree_header);

    ble_service_tree->setColumnWidth(COLUMN_NUM, 15);
    ble_service_tree->setColumnWidth(COLUMN_SERVICE, 80);
    ble_service_tree->setColumnWidth(COLUMN_SERVICE, 80);

    QSpacerItem *vspacer1 = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

    item_name = new QLabel(this);
    item_name->setFixedWidth(80);
    item_name->setText("value");
    item_value = new QLineEdit(this);
    value_read = new QPushButton("read");
    value_read->setFixedWidth(50);
    value_read->setDisabled(true);
    value_write = new QPushButton("write");
    value_write->setFixedWidth(50);
    value_write->setDisabled(true);

    QHBoxLayout *item_value_layout = new QHBoxLayout;
    item_value_layout->addWidget(item_name);
    item_value_layout->addWidget(item_value);
    item_value_layout->addWidget(value_read);
    item_value_layout->addWidget(value_write);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ble_service_tree);
    layout->addSpacerItem(vspacer1);
    layout->addLayout(item_value_layout);

    this->setLayout(layout);

    connect(ble_service_tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(ViewCurrentItem(QTreeWidgetItem*, int)));
    connect(value_read, SIGNAL(clicked()), this, SLOT(ReadBtnClicked()));
    connect(value_write, SIGNAL(clicked()), this, SLOT(WriteBtnClicked()));
}

BleServiceViewer::~BleServiceViewer()
{
    delete ble_service_tree;
}


void BleServiceViewer::SetDevice(QString device_name, QString device_address)
{
    this->device_name = device_name;
    this->device_address = device_address;
}

void BleServiceViewer::AddService(QString name, QString uuid, QString type)
{
    int service_cnt = ble_service_tree->topLevelItemCount();

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
    }
    else
    {
        QTreeWidgetItem *current_service = ble_service_tree->currentItem();
        ble_service_tree->removeItemWidget(current_service, COLUMN_NUM);
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

void BleServiceViewer::DeleteDevice(QString device_address)
{
    Q_UNUSED(device_address)
}

void BleServiceViewer::UpdateCharacteristicValue(QString service_uuid, QString characteristic_uuid, QString value)
{
    int service_cnt = ble_service_tree->topLevelItemCount();

    for(int service_idx=0; service_idx<service_cnt; service_idx++)
    {
        QTreeWidgetItem *service_root = ble_service_tree->topLevelItem(service_idx);
        if(service_root->child(SERVICE_UUID)->text(COLUMN_VALUE) != service_uuid)      //uuid
            continue;

        for(int character_idx=SERVICE_CHARACTER; character_idx<service_root->childCount(); character_idx++)
        {
            if(service_root->child(character_idx)->child(CHARACTER_UUID)->text(COLUMN_VALUE) != item_character_uuid)
                continue;

            service_root->child(character_idx)->child(CHARACTER_VALUE)->setText(COLUMN_VALUE, value);
            item_value->setText(value);
        }
    }
}

void BleServiceViewer::UpdateDescriptorValue(QString service_uuid, QString characteristic_uuid, QString descriptor_uuid, QString value)
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

            int descriptor_cnt = (character_root->childCount() - CHARACTER_IDX_CNT) / DESCRIPTOR_IDX_CNT;
            if(descriptor_cnt <= 0)
                continue;

            for(int descriptor_idx=0; descriptor_idx<descriptor_cnt; descriptor_idx++)
            {
                if(character_root->child(CHARACTER_IDX_CNT+descriptor_idx*DESCRIPTOR_IDX_CNT+DESCRIPTOR_UUID)->text(COLUMN_VALUE) == item_descriptor_uuid)
                {
                    character_root->child(CHARACTER_IDX_CNT+descriptor_idx*DESCRIPTOR_IDX_CNT+DESCRIPTOR_VALUE)->setText(COLUMN_VALUE, value);
                    item_value->setText(value);
                }
            }
        }
    }
}

void BleServiceViewer::ViewCurrentItem(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if(item->parent() == Q_NULLPTR || item->text(COLUMN_VALUE).isEmpty())
        return;

    /* set item type */
    if(!item->text(COLUMN_SERVICE).isEmpty())
    {
        item_type = ITEM_SERVICE;

        item_name->setText("value");
        item_value->clear();
        item_value->setReadOnly(true);
        value_read->setEnabled(false);
        value_write->setEnabled(false);
        return;
    }
    else if(!item->text(COLUMN_CHARACTER).isEmpty())
    {
        item_type = ITEM_CHARACTER;

        QTreeWidgetItem *character = item->parent();
        QTreeWidgetItem *service = character->parent();
        item_service_uuid = service->child(SERVICE_UUID)->text(COLUMN_VALUE);
        item_character_uuid = character->child(CHARACTER_UUID)->text(COLUMN_VALUE);

        int character_item_idx = character->indexOfChild(item);
        if(character_item_idx >= CHARACTER_IDX_CNT)
            item_type = ITEM_DESCRIPTOR;

        if(item_type == ITEM_CHARACTER)
        {
            QString character_value = character->child(CHARACTER_VALUE)->text(COLUMN_VALUE);
            QString character_permission = character->child(CHARACTER_PERMISSION)->text(COLUMN_VALUE);
            QStringList permission_list = character_permission.split(" ");
            permission_list.removeOne("(");
            permission_list.removeOne(")");

            item_name->setText("value");
            item_value->setText(character_value);

            if(permission_list.contains("Read"))
                value_read->setEnabled(true);
            else
                value_read->setEnabled(false);

            if(permission_list.contains("Write"))
            {
                value_write->setEnabled(true);
                item_value->setReadOnly(false);
            }
            else
            {
                value_write->setEnabled(false);
                item_value->setReadOnly(true);
            }
        }
        else
        {
            int descriptor_idx = (character_item_idx - CHARACTER_IDX_CNT) / DESCRIPTOR_IDX_CNT;        //0 base
            item_descriptor_uuid = character->child(CHARACTER_IDX_CNT+descriptor_idx+DESCRIPTOR_UUID)->text(COLUMN_VALUE);
            QString descriptor_value = character->child(CHARACTER_IDX_CNT+descriptor_idx+DESCRIPTOR_VALUE)->text(COLUMN_VALUE);

            item_name->setText("desc_value");
            item_value->setText(descriptor_value);
            item_value->setReadOnly(false);
            value_read->setEnabled(true);
            value_write->setEnabled(true);
        }

    }
    else if(!item->text(COLUMN_DESCRIPTOR).isEmpty())
    {
        item_type = ITEM_DESCRIPTOR;
    }
    else
        return;
}

void BleServiceViewer::ReadBtnClicked()
{
    item_value->clear();
    QStringList uuid_list;
    uuid_list << item_service_uuid << item_character_uuid;

    if(item_type == ITEM_CHARACTER)
        emit SignalReadCharacter(device_address, uuid_list);
    else if(item_type == ITEM_DESCRIPTOR)
    {
        uuid_list.append(item_descriptor_uuid);
        emit SignalReadDescriptor(device_address, uuid_list);
    }
}

void BleServiceViewer::WriteBtnClicked()
{
    QStringList uuid_list;
    uuid_list << item_service_uuid << item_character_uuid;

    QByteArray value;
    if(item_value->text() != "<none>")
        value = QByteArray::fromHex(item_value->text().toUtf8());

    if(item_type == ITEM_CHARACTER)
        emit SignalWriteCharacter(device_address, uuid_list, value);
    else if(item_type == ITEM_DESCRIPTOR)
    {
        uuid_list.append(item_descriptor_uuid);
        emit SignalWriteDescriptor(device_address, uuid_list, value);
    }
}
