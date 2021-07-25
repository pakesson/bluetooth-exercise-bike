#ifndef FTMSSIMULATOR_H
#define FTMSSIMULATOR_H

#include <QObject>
#include <QTimer>

#include <memory>

#include "fitnessmachineservice.h"

class FtmsSimulator  : public QObject
{
    Q_OBJECT
public:
    explicit FtmsSimulator(QObject *parent = nullptr);

    void run();

private slots:
    void simulationParametersChanged(int16_t windSpeed, int16_t grade, uint8_t crr, uint8_t cw);
    void simulate();

private:
    QTimer m_simulationTimer;
    std::unique_ptr<FitnessMachineService> m_fitnessMachineService = nullptr;

    FitnessMachineValues m_fitnessMachineValues;
};

#endif // FTMSSIMULATOR_H
