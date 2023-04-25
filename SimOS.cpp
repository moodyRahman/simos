// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, int amountOfRAM)
{
    this->total_memory = amountOfRAM;
}

bool SimOS::NewProcess(int priority, int size)
{
    if (size > total_memory)
    {
        return false;
    }

    // opting to use iterators because it lets us use safer left/right accessors
    // without causing segfaults

    bool found_valid_memory = false;

    for (auto it = this->MemoryUsage.begin(); it != (this->MemoryUsage.end()); it++)
    {
        // case if there's only one entry in the memory usage array
        if (it + 1 == this->MemoryUsage.end())
        {
            if ((it->itemAddressEnd + size) < this->total_memory)
            {

                process_queue.push(
                    Process{
                        priority, size, PID_c});

                MemoryUsage.push_back(
                    MemoryItem{
                        (unsigned long long)it->itemAddressEnd,
                        (unsigned long long)it->itemAddressEnd + size,
                        (unsigned long long)size,
                        PID_c});

                found_valid_memory = true;
                break;
            }
        }
        else
        {
            if ((it + 1)->itemAddress - it->itemAddressEnd >= size)
            {
                process_queue.push(
                    Process{
                        priority, size, PID_c});
                MemoryUsage.push_back(
                    MemoryItem{
                        (unsigned long long)it->itemAddressEnd,
                        (unsigned long long)it->itemAddressEnd + size,
                        (unsigned long long)size,
                        PID_c});

                found_valid_memory = true;
                break;
            }
        }
    }

    // handle case if there are no processes using memory, ie MemoryUsage is empty
    // start the addresses from 1 so that when we subtract addresses, the difference
    // is the size between addresses
    if (MemoryUsage.size() == 0)
    {
        std::cout << "added one" << std::endl;
        found_valid_memory = true;
        process_queue.push(
            Process{
                priority, size, PID_c});
        MemoryUsage.push_back(
            MemoryItem{
                (unsigned long long)1,
                (unsigned long long)1 + size,
                (unsigned long long)size,
                PID_c});
    }

    if (found_valid_memory)
    {
        this->used_memory += size;
        this->PID_c++;
    }

    std::sort(MemoryUsage.begin(), MemoryUsage.end());

    return found_valid_memory;
}

bool SimOS::SimFork()
{
    return NewProcess(process_queue.top().priority, process_queue.top().size);
}

void SimOS::SimExit()
{
    process_queue.pop();
}
