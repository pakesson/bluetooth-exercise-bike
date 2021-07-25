#ifndef FITNESSMACHINESERVICE_H
#define FITNESSMACHINESERVICE_H

#include <memory>

#include <QObject>

#include <QLowEnergyController>
#include <QLowEnergyService>

#include <QLowEnergyAdvertisingData>
#include <QLowEnergyAdvertisingParameters>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyServiceData>

typedef struct
{
    uint8_t maxResistance = 0x0;
    uint16_t speed;
    uint16_t rpm;
    uint8_t heartRate;
    uint32_t distance;
    int16_t power;
    uint16_t calories;
    uint16_t time;
    int16_t resistance;
} FitnessMachineValues;

class FitnessMachineService : public QObject
{
    Q_OBJECT
public:
    explicit FitnessMachineService(QObject *parent = nullptr);

    void run();

public slots:
    void updateValues(FitnessMachineValues &values);

signals:
    void resistanceChanged(int16_t newResistance);
    void startRequested();
    void stopRequested();
    void resetRequested();
    void simulationParametersChanged(int16_t windSpeed, int16_t grade, uint8_t crr, uint8_t cw);

private slots:
    void peripheralCharacteristicChanged(const QLowEnergyCharacteristic &c,
                                         const QByteArray &value);

private:
    std::unique_ptr<QLowEnergyController> m_controller = nullptr;
    std::unique_ptr<QLowEnergyService> m_ftmsService = nullptr;

    bool m_peripheralReady = false;
};

#endif // FITNESSMACHINESERVICE_H
