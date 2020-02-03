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

#include "serviceinfo.h"
#include <QTimer>

ServiceInfo::ServiceInfo(QLowEnergyService *service):
    m_service(service)
{
    m_service->setParent(this);
}

ServiceInfo::~ServiceInfo()
{
    qDeleteAll(characteristics);
    characteristics.clear();
    characteristics_map.clear();
}

QLowEnergyService *ServiceInfo::service() const
{
    return m_service;
}

QString ServiceInfo::getName() const
{
    if (!m_service)
        return QString();

    return m_service->serviceName();
}

QString ServiceInfo::getType() const
{
    if (!m_service)
        return QString();

    QString result;
    if (m_service->type() & QLowEnergyService::PrimaryService)
        result += QStringLiteral("primary");
    else
        result += QStringLiteral("secondary");

    if (m_service->type() & QLowEnergyService::IncludedService)
        result += QStringLiteral(" included");

    result.prepend('<').append('>');

    return result;
}

QString ServiceInfo::getUuid() const
{
    if (!m_service)
        return QString();

    const QBluetoothUuid uuid = m_service->serviceUuid();
    bool success = false;
    quint16 result16 = uuid.toUInt16(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result16, 16);

    quint32 result32 = uuid.toUInt32(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result32, 16);

    return uuid.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
}

QVariant ServiceInfo::getCharacteristics()
{
    return QVariant::fromValue(characteristics);
}

CharacteristicInfo *ServiceInfo::getCharacteristic(QString uuid)
{
    return characteristics_map[uuid];
}

void ServiceInfo::scanCharacteristics()
{
    if (m_service == Q_NULLPTR)
        return;

    qDeleteAll(characteristics);
    characteristics.clear();
    characteristics_map.clear();
    emit characteristicsUpdated(getUuid());

    if (m_service->state() == QLowEnergyService::DiscoveryRequired) {
        //! [les-service-3]
        connect(m_service, &QLowEnergyService::stateChanged,
                this, &ServiceInfo::serviceDetailsDiscovered);
        m_service->discoverDetails();
        //setUpdate("Back\n(Discovering details...)");
        //! [les-service-3]
        return;
    }

    //discovery already done
    const QList<QLowEnergyCharacteristic> chars = m_service->characteristics();
    for (const QLowEnergyCharacteristic &ch : chars) {
        auto cInfo = new CharacteristicInfo(ch);
        characteristics.append(cInfo);
        characteristics_map.insert(cInfo->getUuid(), cInfo);
    }

    //QTimer::singleShot(0, this, &ServiceInfo::characteristicsUpdated);
    //QTimer::singleShot(0, this, SIGNAL(characteristicsUpdated(QString)), getUuid());
    emit characteristicsUpdated(getUuid());

}

void ServiceInfo::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState != QLowEnergyService::ServiceDiscovered) {
        // do not hang in "Scanning for characteristics" mode forever
        // in case the service discovery failed
        // We have to queue the signal up to give UI time to even enter
        // the above mode
        if (newState != QLowEnergyService::DiscoveringServices) {
            QMetaObject::invokeMethod(this, "characteristicsUpdated",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, getUuid()));
        }
        return;
    }

    auto service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    //! [les-chars]
    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    for (const QLowEnergyCharacteristic &ch : chars) {
        auto cInfo = new CharacteristicInfo(ch);
        characteristics.append(cInfo);
        characteristics_map.insert(cInfo->getUuid(), cInfo);
    }
    //! [les-chars]

    emit characteristicsUpdated(getUuid());
}
