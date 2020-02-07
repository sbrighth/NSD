#ifndef COLLECTDATA_H
#define COLLECTDATA_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

class CollectData : public QWidget
{
    Q_OBJECT
public:
    explicit CollectData(QWidget *parent = nullptr);
    ~CollectData();

public slots:
    void BleDataHandle(QString device_address, QString service_uuid, QString characteristic_uuid, QByteArray value);

signals:

private:
    QGroupBox *box;
    QLabel *name;
    QLineEdit *accel_value[3];
    QLineEdit *gyro_value[3];
    QLineEdit *compass_value[3];
};

#endif // COLLECTDATA_H
