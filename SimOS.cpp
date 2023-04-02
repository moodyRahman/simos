// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, int amountOfRAM)
{
    this->total_memory = amountOfRAM;
}

bool SimOS::NewProcess(int priority, int size)
{
    Process p{priority, size};
    this->process_queue.push(p);
}
