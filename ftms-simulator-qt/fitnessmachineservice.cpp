#include "fitnessmachineservice.h"

#include <QBitArray>
#include <QDataStream>

namespace {
    const auto& ftmsServiceUUID = QBluetoothUuid((quint16)0x1826);

    const auto& ftmsFeatureCharUUID = QBluetoothUuid((quint16)0x2acc);
    const auto& ftmsIndoorBikeDataCharUUID = QBluetoothUuid((quint16)0x2ad2);
    const auto& ftmsTrainingStatusCharUUID = QBluetoothUuid((quint16)0x2ad3);
    const auto& ftmsSupportedSpeedRangeCharUUID = QBluetoothUuid((quint16)0x2ad4);
    const auto& ftmsSupportedResistanceRangeCharUUID = QBluetoothUuid((quint16)0x2ad6);
    const auto& ftmsSupportedHeartRateRangeCharUUID = QBluetoothUuid((quint16)0x2ad7);
    const auto& ftmsSupportedPowerRangeCharUUID = QBluetoothUuid((quint16)0x2ad8);
    const auto& ftmsControlPointCharUUID = QBluetoothUuid((quint16)0x2ad9);
    const auto& ftmsStatusCharUUID = QBluetoothUuid((quint16)0x2ada);
}

FitnessMachineService::FitnessMachineService(QObject *parent) : QObject(parent)
{

}

void FitnessMachineService::run()
{
    // Fitness Machine Features
    QLowEnergyCharacteristicData ftmsFeatureCharData;
    ftmsFeatureCharData.setUuid(ftmsFeatureCharUUID);
    // Fitness Machine Features Field:
    // Cadence Supported
    // Total Distance Supported
    // Resistance Level Supported
    // Heart Rate Measurement Supported
    // Elapsed Time Supported
    // Power Measurement Supported
    //
    // Target Setting Features Field:
    // Resistance Target Setting Supported
    ftmsFeatureCharData.setValue(QByteArray::fromHex("8654000004000000"));
    ftmsFeatureCharData.setProperties(QLowEnergyCharacteristic::Read);
    const QLowEnergyDescriptorData ftmsFeatureDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                             QByteArray(2, 0));
    ftmsFeatureCharData.addDescriptor(ftmsFeatureDescriptorData);

    // Indoor Bike Data
    QLowEnergyCharacteristicData ftmsIndoorBikeCharData;
    ftmsIndoorBikeCharData.setUuid(ftmsIndoorBikeDataCharUUID);
    ftmsIndoorBikeCharData.setProperties(QLowEnergyCharacteristic::Notify);
    const QLowEnergyDescriptorData ftmsIndoorBikeDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                                QByteArray(2, 0));
    ftmsIndoorBikeCharData.addDescriptor(ftmsIndoorBikeDescriptorData);

    // Supported Resistance Level Range
    QLowEnergyCharacteristicData ftmsSupportedResistanceRangeCharData;
    ftmsSupportedResistanceRangeCharData.setUuid(ftmsSupportedResistanceRangeCharUUID);
    ftmsSupportedResistanceRangeCharData.setValue(QByteArray::fromHex("0A0040010A00"));
    ftmsSupportedResistanceRangeCharData.setProperties(QLowEnergyCharacteristic::Read);

    // Fitness Machine Control Point
    QLowEnergyCharacteristicData ftmsControlPointCharData;
    ftmsControlPointCharData.setUuid(ftmsControlPointCharUUID);
    ftmsControlPointCharData.setProperties(QLowEnergyCharacteristic::Write | QLowEnergyCharacteristic::Indicate);
    const QLowEnergyDescriptorData ftmsControlPointDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                                  QByteArray(2, 0));
    ftmsControlPointCharData.addDescriptor(ftmsControlPointDescriptorData);

    // Fitness Machine Status
    QLowEnergyCharacteristicData ftmsStatusCharData;
    ftmsStatusCharData.setUuid(ftmsStatusCharUUID);
    ftmsStatusCharData.setProperties(QLowEnergyCharacteristic::Notify);
    const QLowEnergyDescriptorData ftmsStatusDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                            QByteArray(2, 0));
    ftmsStatusCharData.addDescriptor(ftmsStatusDescriptorData);

    // Configure service
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(ftmsServiceUUID);

    serviceData.addCharacteristic(ftmsFeatureCharData);
    serviceData.addCharacteristic(ftmsIndoorBikeCharData);
    serviceData.addCharacteristic(ftmsSupportedResistanceRangeCharData);
    serviceData.addCharacteristic(ftmsControlPointCharData);
    serviceData.addCharacteristic(ftmsStatusCharData);

    m_controller = std::unique_ptr<QLowEnergyController>(QLowEnergyController::createPeripheral());
    m_ftmsService = std::unique_ptr<QLowEnergyService>(m_controller->addService(serviceData));

    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName("BLE FTMS Simulator");
    advertisingData.setServices(QList<QBluetoothUuid>() << ftmsServiceUUID);
    // TODO: Add Service Data AD Type?

    qDebug() << "Advertising FTMS...";
    m_controller->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData,
                                             advertisingData);
    m_peripheralReady = true;

    QObject::connect(m_ftmsService.get(), &QLowEnergyService::characteristicChanged, this, &FitnessMachineService::peripheralCharacteristicChanged);

    QObject::connect(m_controller.get(), &QLowEnergyController::disconnected, this, []() {
        qDebug() << "FTMS controller disconnected";
    });
}

void FitnessMachineService::peripheralCharacteristicChanged(const QLowEnergyCharacteristic &c,
                                                            const QByteArray &value)
{
    if (c.uuid() != ftmsControlPointCharUUID)
    {
        qDebug() << "Unhandled characteristic changed (" << c.uuid().toString() << ")";
        return;
    }

    qDebug() << "Control point command";
    qDebug() << value.toHex();

    QDataStream inStream(value);
    inStream.setByteOrder(QDataStream::LittleEndian);

    switch(value[0])
    {
    case 0x0:
        qDebug() << "Control requested. Responding with Success...";
        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("800001")); // 0x80 = response, 0x01 = success
        break;
    case 0x1:
        qDebug() << "Reset requested. Responding with Success...";
        emit resetRequested();
        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("800101")); // 0x80 = response, 0x01 = success
        break;
    case 0x4:
        // Set resistance
        qDebug() << "Resistance change requested. Responding with Success...";
        emit resistanceChanged(value[1]);
        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("800401")); // 0x80 = response, 0x01 = success
        break;
    case 0x7:
        qDebug() << "Start/resume requested. Responding with Success...";
        emit startRequested();
        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("800701")); // 0x80 = response, 0x01 = success
        break;
    case 0x8:
        qDebug() << "Stop/pause requested. Responding with Success...";
        emit stopRequested();
        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("800801")); // 0x80 = response, 0x01 = success
        break;
    case 0x11:
        // Set simulation parameters. Wind speed, grade etc.
        qDebug() << "Simulation parameter change requested. Responding with Success...";

        m_ftmsService->writeCharacteristic(c, QByteArray::fromHex("801101")); // 0x80 = response, 0x01 = success

        uint8_t req;
        int16_t windSpeed;
        int16_t grade;
        uint8_t crr;
        uint8_t cw;

        inStream >> req;
        inStream >> windSpeed;
        inStream >> grade;
        inStream >> crr;
        inStream >> cw;

        emit simulationParametersChanged(windSpeed, grade, crr, cw);
        break;
    default:
        qDebug() << "Unhandled command: " << value[0];
        break;
    }
}

void FitnessMachineService::updateValues(FitnessMachineValues &values)
{
    if (m_peripheralReady)
    {
        QByteArray value;
        QDataStream valueDataStream(&value, QIODevice::ReadWrite);
        valueDataStream.setByteOrder(QDataStream::LittleEndian);

        valueDataStream << (uint16_t) 0b0100001110100; // Everything except for HR and calories

        valueDataStream << (uint16_t)(values.speed*10);
        valueDataStream << (uint16_t)(values.rpm*2);
        valueDataStream << (uint16_t)((values.distance) & 0xffff) << (uint8_t)((values.distance) >> 16);
        valueDataStream << values.resistance << values.power;
        valueDataStream << values.time;

        QLowEnergyCharacteristic characteristic = m_ftmsService->characteristic(ftmsIndoorBikeDataCharUUID);
        m_ftmsService->writeCharacteristic(characteristic, value);
    }
}
