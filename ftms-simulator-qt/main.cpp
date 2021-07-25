#include <QCoreApplication>

#include <ftmssimulator.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FtmsSimulator ftmsSimulator;
    ftmsSimulator.run();

    return a.exec();
}
