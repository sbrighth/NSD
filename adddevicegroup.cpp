#include "adddevicegroup.h"
#include "ui_adddevicegroup.h"
#include <QtDebug>
//#include <QFile>
//#include <QJsonDocument>
//#include <QJsonArray>
//#include <QJsonValue>
//#include <QJsonObject>
#include <QList>

AddDeviceGroup::AddDeviceGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDeviceGroup)
{
    ui->setupUi(this);  

    motion_sampling << 200 << 100 << 50 << 25 << 10 << 5;
    audio_sampling << 16000;

    /*
    ui->group_name->setText("Sensor1");

    QFile loadFile(QStringLiteral(":/res/json/target_device_info.json"));

    if(!loadFile.open(QIODevice::ReadOnly)){
        qWarning("Could not open json file to read");
        //return false;
    }

    QByteArray loadData = loadFile.readAll();
    loadFile.close();

    QJsonDocument json_doc(QJsonDocument::fromJson(loadData));
    json_obj = json_doc.object();

    // add all target_device names to combobox
    foreach(QString key_target_device, json_obj.keys())
    {
        ui->target_device->addItem(key_target_device);
    }

    ui->target_device->setCurrentIndex(0);

    QJsonObject obj_target_device = json_obj.value(ui->target_device->currentText()).toObject();

    foreach(QString val, obj_target_device.value("connect mode"))
    {
        ui->connect_mode->addItem(val);
    }

    foreach(QString val, obj_target_device.value("capture source"))
    {
        ui->capture_source->addItem(val);
    }

    foreach(QString val, obj_target_device.value("sampling rate"))
    {
        ui->sampling_rate->addItem(val);
    }
   */

    Clear();
}

AddDeviceGroup::~AddDeviceGroup()
{
    delete ui;
}

void AddDeviceGroup::Clear()
{
    ui->group_name->setText(QString("sensor1"));
    ui->target_device->setCurrentIndex(0);
    ui->connect_mode->setCurrentText("BLE");
    ui->capture_source->setCurrentText("Motion");
    ChangeSamplingRate("Motion");
    ui->sensor_type->setCurrentIndex(ui->capture_source->currentIndex());
    ui->accelerometer->setChecked(true);
    ui->gyroscope->setChecked(true);
    ui->audio->setChecked(true);
}

void AddDeviceGroup::on_buttonBox_accepted()
{
    item.group_name = ui->group_name->text();
    item.target_device = ui->target_device->currentText();
    item.connect_mode = ui->connect_mode->currentText();
    item.capture_source = ui->capture_source->currentText();
    item.sampling_rate = ui->sampling_rate->currentText();

    item.sensor_type.clear();
    if(item.capture_source == "Motion")
    {
        if(ui->accelerometer->isChecked() == true)
            item.sensor_type.append(ui->accelerometer->text());
        if(ui->gyroscope->isChecked() == true)
            item.sensor_type.append(ui->gyroscope->text());
    }
    else if(item.capture_source == "Audio")
    {
        if(ui->audio->isChecked() == true)
            item.sensor_type.append(ui->audio->text());
    }

    emit SignalUpdateDeviceGroup(&item);
}

void AddDeviceGroup::on_buttonBox_rejected()
{
    this->hide();
}

void AddDeviceGroup::on_capture_source_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->sensor_type->setCurrentIndex(ui->capture_source->currentIndex());

    QString current_source = ui->capture_source->currentText();
    ChangeSamplingRate(current_source);
}

void AddDeviceGroup::ChangeSamplingRate(QString source)
{
    QList<int> *ref_sampling;
    if(source == "Motion")
    {
        ref_sampling = &motion_sampling;
    }
    else
    {
        ref_sampling = &audio_sampling;
    }

    ui->sampling_rate->clear();
    foreach(int target_sampling, *ref_sampling)
    {
        ui->sampling_rate->addItem(QString::number(target_sampling));
    }
}
