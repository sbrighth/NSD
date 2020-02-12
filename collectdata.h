#ifndef COLLECTDATA_H
#define COLLECTDATA_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QSemaphore>

class CollectData : public QWidget
{
    Q_OBJECT
public:
    explicit CollectData(QWidget *parent = nullptr);
    ~CollectData();

public slots:
    void BleDataHandle(QString device_address, QString service_uuid, QString characteristic_uuid, QByteArray value);
    void Record(QString address, bool start);
    int SaveFile(QString filename, QString text, bool create=false);

signals:

private:
    QGroupBox *box;
    QLabel *name;
    QLineEdit *accel_value[3];
    QLineEdit *gyro_value[3];
    QLineEdit *compass_value[3];

    QSemaphore sem_record;
    bool recording;
    QMap<QString, QString> filename_map;
    QMap<QString, quint64> sequence_map;
};

#endif // COLLECTDATA_H
