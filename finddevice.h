#ifndef FINDDEVICE_H
#define FINDDEVICE_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class FindDevice;
}

class FindDevice : public QDialog
{
    Q_OBJECT

public:
    explicit FindDevice(QWidget *parent = nullptr);
    ~FindDevice();

signals:
    void SignalDoScan();
    void SignalSelectDevice(QModelIndexList row_indexes);

private slots:
    void SelectedDevice();

private:
    Ui::FindDevice *ui;

public:
    QStandardItemModel *scan_device_list_model;
};

#endif // FINDDEVICE_H
