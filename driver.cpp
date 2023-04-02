#include "SimOS.h"

int main(int argc, char const *argv[])
{
    SimOS s(5, 5);
    s.NewProcess(3, 5);
    s.NewProcess(4, 5);
    s.NewProcess(4, 5);
    s.NewProcess(4, 5);

    std::cout << "hello world" << std::endl;
    std::cout << s.process_queue.top() << std::endl;
    s.process_queue.pop();
    s.process_queue.pop();
    // s.process_queue.pop();
    std::cout << s.process_queue.top() << std::endl;

    return 0;
}
