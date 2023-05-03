#include "SimOS.h"

int main(int argc, char const *argv[])
{

    SimOS s(5, 500);
    // s.NewProcess(3, 5);
    // s.NewProcess(4, 5);
    // s.NewProcess(4, 5);
    // s.NewProcess(4, 5);
    // s.NewProcess(3, 5);
    // s.NewProcess(3, 5);
    // s.NewProcess(2, 5);
    // s.NewProcess(2, 5);
    // s.NewProcess(5, 5);
    // s.NewProcess(5, 5);
    // s.NewProcess(2, 5);
    s.NewProcess(2, 15);
    s.NewProcess(2, 10);
    s.NewProcess(4, 10);
    s.NewProcess(3, 5);
    s.NewProcess(2, 10);
    s.NewProcess(2, 10);
    s.NewProcess(3, 5);
    s.NewProcess(3, 5);
    s.NewProcess(4, 5);
    s.NewProcess(3, 5);

    s.SimExit();
    std::cout << "exited" << std::endl;
    s.SimExit();
    std::cout << "exited" << std::endl;

    s.NewProcess(2, 600);
    s.NewProcess(2, 5);
    s.NewProcess(4, 5);
    s.NewProcess(4, 5);
    s.NewProcess(2, 5);
    s.NewProcess(2, 5);

    // while (!s.process_queue.empty())
    // {
    //     std::cout << s.process_queue.top() << "\n";
    //     s.process_queue.pop();
    // }

    std::cout
        << "=============" << std::endl;
    for (auto x : s.MemoryUsage)
    {
        std::cout << x << std::endl;
    }

    return 0;
}
