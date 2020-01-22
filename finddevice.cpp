#include "finddevice.h"
#include "ui_finddevice.h"

FindDevice::FindDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDevice)
{
    ui->setupUi(this);
    connect(ui->scan, SIGNAL(clicked()), this, SIGNAL(SignalDoScan()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(SelectedDevice()));

    scan_device_list_model = new QStandardItemModel(0, 2, this);
    scan_device_list_model->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    scan_device_list_model->setHeaderData(1, Qt::Horizontal, QObject::tr("address"));

    ui->scan_device_list->setModel(scan_device_list_model);
    ui->scan_device_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

FindDevice::~FindDevice()
{
    delete scan_device_list_model;
    delete ui;
}

void FindDevice::SelectedDevice()
{
    emit SignalSelectDevice(ui->scan_device_list->selectionModel()->selectedRows());
}
