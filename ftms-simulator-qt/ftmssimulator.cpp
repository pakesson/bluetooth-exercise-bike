#include "ftmssimulator.h"

FtmsSimulator::FtmsSimulator(QObject *parent) :
    QObject(parent)
{
    m_fitnessMachineValues = { 0x20, 0x80, 0x41, 0x42, 0x0234, 0x43, 0x0123, 0x2345, 0x10 };
}

void FtmsSimulator::run()
{
    qDebug() << "Running in simulation mode";

    m_fitnessMachineService = std::make_unique<FitnessMachineService>();
    m_fitnessMachineService->run();

    // Unused signals
    //QObject::connect(m_fitnessMachineService.get(), &FitnessMachineService::startRequested, this, &FtmsSimulator::start);
    //QObject::connect(m_fitnessMachineService.get(), &FitnessMachineService::stopRequested, this, &FtmsSimulator::stop);
    //QObject::connect(m_fitnessMachineService.get(), &FitnessMachineService::resetRequested, this, &FtmsSimulator::reset);
    //QObject::connect(m_fitnessMachineService.get(), &FitnessMachineService::resistanceChanged, this, &FtmsSimulator::setResistance);

    QObject::connect(m_fitnessMachineService.get(), &FitnessMachineService::simulationParametersChanged, this, &FtmsSimulator::simulationParametersChanged);

    m_simulationTimer.setSingleShot(false);
    m_simulationTimer.setInterval(1000);
    QObject::connect(&m_simulationTimer, &QTimer::timeout, this, &FtmsSimulator::simulate);
    m_simulationTimer.start();
}

void FtmsSimulator::simulationParametersChanged(int16_t windSpeed, int16_t grade, uint8_t crr, uint8_t cw)
{
    qDebug() << "FtmsSimulator::simulationParametersChanged(int16_t windSpeed, int16_t grade, uint8_t crr, uint8_t cw)";
    qDebug() << "\t" << windSpeed << " " << grade << " " << crr << " " << cw;
}

void FtmsSimulator::simulate()
{
    m_fitnessMachineValues.time += 1;
    m_fitnessMachineValues.distance += 1;
    m_fitnessMachineValues.rpm = (m_fitnessMachineValues.rpm + 1) % 200;
    m_fitnessMachineValues.power = (m_fitnessMachineValues.power + 1) % 400;
    m_fitnessMachineValues.speed = (m_fitnessMachineValues.speed + 1) % 50;

    m_fitnessMachineService->updateValues(m_fitnessMachineValues);
}
