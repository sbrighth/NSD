#include "collectdata.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtEndian>

/*
 * 1. BLE description을 써서 notify on이 되면,
 *  - CollectionData 쪽에 box를 추가해준다(name, value 항목 추가
 *  - 이미 있다면 box 재활용
 *
 * 2. BLE description을 설정해서 notify off가 되면,
 *  - box 비활성화 또는 삭제
 *
 * 3. BLE characteristic uuid에 대한 정보들을 등록해서 box 등록시 자동으로 value 특징을 파악한다.
 *
 * 4. uuid 목록은 사용자가 추가, 수정할 수 있도록 한다.
 */

CollectData::CollectData(QWidget *parent) : QWidget(parent)
{
    name = new QLabel("motion data display");

    QLabel *accel_label[3];
    QLabel *gyro_label[3];
    QLabel *compass_label[3];
    QHBoxLayout *accel_layout[3];
    QHBoxLayout *gyro_layout[3];
    QHBoxLayout *compass_layout[3];

    accel_label[0] = new QLabel("accel_x");
    accel_label[1] = new QLabel("accel_y");
    accel_label[2] = new QLabel("accel_z");
    gyro_label[0] = new QLabel("gyro_x");
    gyro_label[1] = new QLabel("gyro_y");
    gyro_label[2] = new QLabel("gyro_z");
    compass_label[0] = new QLabel("compass_x");
    compass_label[1] = new QLabel("compass_y");
    compass_label[2] = new QLabel("compass_z");

    for(int i=0; i<3; i++)
    {
        accel_label[i]->setFixedWidth(80);
        gyro_label[i]->setFixedWidth(80);
        compass_label[i]->setFixedWidth(80);

        accel_value[i] = new QLineEdit();
        gyro_value[i] = new QLineEdit();
        compass_value[i] = new QLineEdit();

        accel_value[i]->setReadOnly(true);
        gyro_value[i]->setReadOnly(true);
        compass_value[i]->setReadOnly(true);

        accel_layout[i] = new QHBoxLayout();
        gyro_layout[i] = new QHBoxLayout();
        compass_layout[i] = new QHBoxLayout();

        accel_layout[i]->addWidget(accel_label[i]);
        accel_layout[i]->addWidget(accel_value[i]);
        gyro_layout[i]->addWidget(gyro_label[i]);
        gyro_layout[i]->addWidget(gyro_value[i]);
        compass_layout[i]->addWidget(compass_label[i]);
        compass_layout[i]->addWidget(compass_value[i]);
    }

    QVBoxLayout *layout_box = new QVBoxLayout();
    layout_box->addWidget(name);

    for(int i=0; i<3; i++)
         layout_box->addLayout(accel_layout[i]);
    for(int i=0; i<3; i++)
         layout_box->addLayout(gyro_layout[i]);
    for(int i=0; i<3; i++)
         layout_box->addLayout(compass_layout[i]);

    box = new QGroupBox("motion");
    box->setLayout(layout_box);

    QHBoxLayout *hlayout = new QHBoxLayout();
    QSpacerItem *hspacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout->addWidget(box);
    hlayout->addSpacerItem(hspacer);

    QVBoxLayout *layout = new QVBoxLayout();
    QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addLayout(hlayout);
    layout->addSpacerItem(spacer);

    setLayout(layout);

    sem_record.release();
    recording = false;
}

CollectData::~CollectData()
{
    filename_map.clear();
    sequence_map.clear();
}

void CollectData::BleDataHandle(QString device_address, QString service_uuid, QString characteristic_uuid, QByteArray value)
{
    name->setText(characteristic_uuid);

    if(characteristic_uuid == "ef680406-9b35-4933-9b10-52ffa9740042")
    {
        qint16 accel_val[3];
        qint16 gyro_val[3];
        qint16 compass_val[3];

        for(int i=0; i<3; i++)
        {
            accel_val[i] = qFromLittleEndian<qint16>(value.mid(i*2, 2).data());         //0 2 4
            gyro_val[i] = qFromLittleEndian<qint16>(value.mid(i*2+6, 2).data());        //6 8 10
            compass_val[i] = qFromLittleEndian<qint16>(value.mid(i*2+12, 2).data());    //12 14 16

            accel_value[i]->setText(QString::number(accel_val[i]));
            gyro_value[i]->setText(QString::number(gyro_val[i]));
            compass_value[i]->setText(QString::number(compass_val[i]));

            //qDebug() << QString("accel_%1 = ").arg(i) <<  value.mid(i*2, 2) << " (" << qFromLittleEndian<qint16>(value.mid(i*2, 2).data()) << ")";
        }

        //0xEF680406-9B35-4933-9B10-52FFA9740042
        //f8 ff f2   ff dc 03   ff ff fe   ff fd ff 000000000000

        sem_record.acquire();
        if(recording)
        {
            sem_record.release();

            QString filename = filename_map[device_address];
            quint64 sequence = sequence_map[device_address];
            sequence_map[device_address]++;

            QString text = QString("%1,%2,%3,%4,%5,%6,%7\n").arg(sequence)\
                                                            .arg(accel_val[0]).arg(accel_val[1]).arg(accel_val[2])\
                                                            .arg(gyro_val[0]).arg(gyro_val[1]).arg(gyro_val[2]);
            SaveFile(filename, text);
        }
        else
            sem_record.release();
    }
}

void CollectData::Record(QString address, bool start)
{
    if(start)
    {
        QString date = QDateTime::currentDateTime().toString("yyMMdd_hhmmss");
        QString filename = date + QString(".csv");
        QString header("sequence,AccelerometerX,AccelerometerY,AccelerometerZ,GyroscopeX,GyroscopeY,GyroscopeZ\n");

        SaveFile(filename, header, true);

        filename_map.insert(address, filename);
        sequence_map.insert(address, 0);

        sem_record.acquire();
        recording = true;
        sem_record.release();
    }
    else
    {
        sem_record.acquire();
        recording = false;
        sem_record.release();

        filename_map.remove(address);
        sequence_map.remove(address);
    }
}

int CollectData::SaveFile(QString filename, QString text, bool create)
{
    QFile file(filename);
    if (!file.open((create ? QIODevice::WriteOnly : QIODevice::Append) | QIODevice::Text))
        return -1;

    QTextStream out(&file);
    out << text;

    file.close();
    return 0;
}
