/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "bledevice.h"

#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QDebug>
#include <QList>
#include <QMetaEnum>

BleDevice::BleDevice(QString adapter_address)
{
    //! [les-devicediscovery-1]
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(QBluetoothAddress(adapter_address));
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BleDevice::addDevice);
    connect(discoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
            this, &BleDevice::deviceScanError);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BleDevice::deviceScanFinished);
    //! [les-devicediscovery-1]

    setUpdate("Search");
 }

BleDevice::~BleDevice()
{
    delete discoveryAgent;
    qDeleteAll(devices);
    devices.clear();
    //qDeleteAll(devices_map);
    devices_map.clear();
}

void BleDevice::startDeviceDiscovery()
{
    qDeleteAll(devices);
    devices.clear();
    //qDeleteAll(devices_map);
    devices_map.clear();
    emit devicesUpdated();

    setUpdate("Scanning for devices ...");
    //! [les-devicediscovery-2]
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    //! [les-devicediscovery-2]

    if (discoveryAgent->isActive()) {
        m_deviceScanState = true;
        Q_EMIT stateChanged();
    }
}

void BleDevice::ReadCharacter(QString device_address, QStringList uuid)
{
    //uuid[0] = service_uuid
    //uuid[1] = characteristic_uuid
    QLowEnergyService *service = getDevice(device_address)->getService(uuid.at(0))->service();
    QLowEnergyCharacteristic *ch = getDevice(device_address)->getService(uuid.at(0))->getCharacteristic(uuid.at(1))->characteristic();

    service->readCharacteristic(*ch);
}

void BleDevice::WriteCharacter(QString device_address, QStringList uuid, QByteArray val)
{
    QLowEnergyService *service = getDevice(device_address)->getService(uuid.at(0))->service();
    QLowEnergyCharacteristic *ch = getDevice(device_address)->getService(uuid.at(0))->getCharacteristic(uuid.at(1))->characteristic();

    service->writeCharacteristic(*ch, val);
}

void BleDevice::ReadDescriptor(QString device_address, QStringList uuid)
{
    //uuid[0] = service_uuid
    //uuid[1] = characteristic_uuid
    //uuid[2] = descriptor_uuid
    QLowEnergyService *service = getDevice(device_address)->getService(uuid.at(0))->service();
    QLowEnergyDescriptor *desc = getDevice(device_address)->getService(uuid.at(0))->getCharacteristic(uuid.at(1))->getDescriptor(uuid.at(2))->descriptor();

    service->readDescriptor(*desc);
}

void BleDevice::WriteDescriptor(QString device_address, QStringList uuid, QByteArray val)
{
    QLowEnergyService *service = getDevice(device_address)->getService(uuid.at(0))->service();
    QLowEnergyDescriptor *desc = getDevice(device_address)->getService(uuid.at(0))->getCharacteristic(uuid.at(1))->getDescriptor(uuid.at(2))->descriptor();

    service->writeDescriptor(*desc, val);
}

//! [les-devicediscovery-3]
void BleDevice::addDevice(const QBluetoothDeviceInfo &info)
{
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
        setUpdate("Last device added: " + info.name());
}
//! [les-devicediscovery-3]

void BleDevice::deviceScanFinished()
{
    const QList<QBluetoothDeviceInfo> foundDevices = discoveryAgent->discoveredDevices();
    for (auto nextDevice : foundDevices)
        if (nextDevice.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
        {
            if(!nextDevice.name().startsWith("Thingy") && !nextDevice.name().startsWith("ELA"))
                    continue;

            auto *new_device = new DeviceInfo(nextDevice);
            devices.append(new_device);
            devices_map.insert(nextDevice.address().toString(), new_device);

            connect(new_device, SIGNAL(disconnected(QString)), this, SIGNAL(disconnected(QString)));
            connect(new_device, SIGNAL(servicesUpdated(QString)), this, SIGNAL(servicesUpdated(QString)));
            connect(new_device, SIGNAL(servicesUpdateFinished(QString)), this, SIGNAL(servicesUpdateFinished(QString)));
            connect(new_device, SIGNAL(servicesUpdateChanged(QString)), this, SIGNAL(servicesUpdateChanged(QString)));
            connect(new_device, SIGNAL(characteristicListUpdated(QString, QString)), this, SIGNAL(characteristicListUpdated(QString, QString)));
            connect(new_device, SIGNAL(characteristicValueUpdated(QString, QString, QString)), this, SIGNAL(characteristicValueUpdated(QString, QString, QString)));
            connect(new_device, SIGNAL(descriptorValueUpdated(QString, QString, QString)), this, SIGNAL(descriptorValueUpdated(QString, QString, QString)));
        }


    emit devicesUpdated();
    m_deviceScanState = false;
    emit stateChanged();
    if (devices.isEmpty())
        setUpdate("No Low Energy devices found...");
    else
        setUpdate("Done! Scan Again!");
}

QVariant BleDevice::getDevices()
{
    return QVariant::fromValue(devices);
}

DeviceInfo *BleDevice::getDevice(QString address)
{
    return devices_map[address];
}

QString BleDevice::getUpdate()
{
    return m_message;
}

void BleDevice::setUpdate(const QString &message)
{
    m_message = message;
    emit updateChanged();
}

void BleDevice::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        setUpdate("The Bluetooth adaptor is powered off, power it on before doing discovery.");
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        setUpdate("Writing or reading from the device resulted in an error.");
    else {
        static QMetaEnum qme = discoveryAgent->metaObject()->enumerator(
                    discoveryAgent->metaObject()->indexOfEnumerator("Error"));
        setUpdate("Error: " + QLatin1String(qme.valueToKey(error)));
    }

    m_deviceScanState = false;
    emit devicesUpdated();
    emit stateChanged();
}

bool BleDevice::state()
{
    return m_deviceScanState;
}

int BleDevice::getCount()
{
    return devices_map.size();
}

void BleDevice::connectDevice(QString device_address)
{
    if(device_address.isEmpty())
        return;

    getDevice(device_address)->scanServices();
}

void BleDevice::disconnectDevice(QString device_address)
{
    if(device_address.isEmpty())
        return;

    getDevice(device_address)->disconnectFromDevice();
}

void BleDevice::scanCharacteristics(QString device_address, QString service_uuid)
{
    if(device_address.isEmpty())
        return;

    DeviceInfo *device = getDevice(device_address);

    if(service_uuid.isEmpty())
    {
        QList<QObject*> services = device->getServices().value<QList<QObject*>>();

        foreach(QObject* obj, services)
        {
            ServiceInfo *service = qobject_cast<ServiceInfo *>(obj);
            service->scanCharacteristics();
        }
    }
    else
    {
        ServiceInfo *service = device->getService(service_uuid);
        service->scanCharacteristics();
    }
}
