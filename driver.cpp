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

    s.MemoryUsage.push_back(
        MemoryItem{
            (unsigned long long)0,
            (unsigned long long)5,
            (unsigned long long)5,
            10});

    s.MemoryUsage.push_back(
        MemoryItem{
            (unsigned long long)15,
            (unsigned long long)20,
            (unsigned long long)5,
            11});

    s.MemoryUsage.push_back(
        MemoryItem{
            (unsigned long long)30,
            (unsigned long long)35,
            (unsigned long long)5,
            12});

    s.NewProcess(2, 15);
    s.NewProcess(2, 10);
    s.NewProcess(2, 5);
    s.NewProcess(2, 5);

    // while (!s.process_queue.empty())
    // {
    //     std::cout << s.process_queue.top() << "\n";
    //     s.process_queue.pop();
    // }

    std::cout << "=============" << std::endl;
    for (auto x : s.MemoryUsage)
    {
        std::cout << x << std::endl;
    }

    return 0;
}
