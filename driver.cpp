#include "SimOS.h"

int main(int argc, char const *argv[])
{

    SimOS s(5, 500);
    s.NewProcess(2, 15);
    s.NewProcess(2, 10);

    s.SimFork();
    s.SimFork();

    s.NewProcess(3, 5);
    s.SimExit();
    s.SimWait();
    s.SimExit();
    s.display();
    s.SimFork();
    s.display();
    s.SimWait();
    s.SimWait();
    s.SimWait();
    s.display();
    s.SimExit();
    s.display();

    // s.SimFork();
    // s.SimFork();

    // s.SimWait();

    // s.NewProcess(4, 10);

    // s.NewProcess(2, 10);
    // s.NewProcess(2, 10);
    // s.SimFork();
    // s.SimWait();
    // s.SimExit();

    // std::cout << s.process_queue.top() << std::endl;
    // while (!s.process_queue.empty())
    // {
    //     std::cout << s.process_queue[0] << "\n";
    //     s.process_queue.;
    // }

    // std::cout
    //     << "=============" << std::endl;
    // for (auto x : s.MemoryUsage)
    // {
    //     std::cout << x << std::endl;
    // }

    return 0;
}
