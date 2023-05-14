#include "SimOS.h"

int main(int argc, char const *argv[])
{

    SimOS sim{2, 20000};

    sim.NewProcess(5, 1000);
    sim.display();

    sim.NewProcess(3, 1000);
    sim.display();

    sim.NewProcess(8, 1000);
    sim.display();

    sim.SimFork();
    sim.display();

    sim.DiskReadRequest(0, "abc");
    sim.display();

    sim.DiskReadRequest(1, "abc");
    sim.display();

    std::cout << "==================" << std::endl;

    sim.DiskReadRequest(1, "abc");
    // CHECK(sim.GetCPU() == 0);
    sim.display();
    // CHECK(sim.GetDisk(1).PID == 1);
    sim.display();
    auto q1{sim.GetDiskQueue(1)};
    sim.display();
}
