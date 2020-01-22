#ifndef ADDDEVICEGROUP_H
#define ADDDEVICEGROUP_H

#include <QDialog>
#include <QAbstractButton>
#include <QList>
//#include <QJsonObject>

typedef struct _DeviceGroupItem
{
    QString group_name;
    QString target_device;
    QString connect_mode;
    QString capture_source;
    QString sampling_rate;
    QStringList sensor_type;
} DeviceGroupItem;

namespace Ui {
class AddDeviceGroup;
}

class AddDeviceGroup : public QDialog
{
    Q_OBJECT

public:
    explicit AddDeviceGroup(QWidget *parent = nullptr);
    ~AddDeviceGroup();
    void Clear();
    void ChangeSamplingRate(QString source);

signals:
    void SignalUpdateDeviceGroup( DeviceGroupItem *item);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_capture_source_currentIndexChanged(const QString &arg1);

private:
    Ui::AddDeviceGroup *ui;
    QList<int> motion_sampling;
    QList<int> audio_sampling;
//    QJsonObject json_obj;

public:
    DeviceGroupItem item;
};

#endif // ADDDEVICEGROUP_H
