// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, int amountOfRAM)
{
    this->total_memory = amountOfRAM;
}

bool SimOS::NewProcess(int priority, int size)
{
    if (size > total_memory - used_memory)
    {
        return false;
    }

    bool found_valid_memory = false;

    if (MemoryUsage.size() == 0)
    {
        process_queue.push(
            Process{
                priority, size, PID_c, 1});
        MemoryUsage.push_back(
            MemoryItem{
                (ADDRESS)0,
                (ADDRESS)size,
                (ADDRESS)size,
                PID_c});
        this->used_memory += size;
        this->PID_c++;
        return true;
    }

    if (MemoryUsage.size() == 1)
    {
        auto left = MemoryUsage[0];
        process_queue.push(
            Process{
                priority, size, PID_c, 1});
        MemoryUsage.push_back(
            MemoryItem{
                (ADDRESS)left.itemAddressEnd,
                (ADDRESS)left.itemAddressEnd + size,
                (ADDRESS)size,
                PID_c});

        this->used_memory += size;
        this->PID_c++;
        return true;
    }

    std::vector<Hole> holes;

    for (auto it = this->MemoryUsage.begin(); it != this->MemoryUsage.end(); it++)
    {
        if (it + 1 == MemoryUsage.end())
        {
            holes.push_back(
                Hole{
                    it->itemAddressEnd,
                    (ADDRESS)total_memory,
                    (ADDRESS)total_memory - it->itemAddressEnd});
            break;
        }
        else
        {
            if (it->itemAddressEnd == (*(it + 1)).itemAddress)
            {
                continue;
            }
            else
            {
                holes.push_back(
                    Hole{
                        it->itemAddressEnd,
                        (*(it + 1)).itemAddress,
                        (*(it + 1)).itemAddress - it->itemAddressEnd});
            }
        }
    }

    std::sort(holes.begin(), holes.end(), std::greater<Hole>());

    // Hole *ideal = nullptr;
    std::shared_ptr<Hole> ideal = nullptr;

    for (auto h : holes)
    {
        if (h.size >= size)
        {
            ideal = std::make_shared<Hole>(h);
        }
    }

    if (ideal == nullptr)
    {
        return false;
    }

    process_queue.push(
        Process{
            priority, size, PID_c, 0});
    MemoryUsage.push_back(
        MemoryItem{
            (ADDRESS)ideal->start,
            (ADDRESS)ideal->start + size,
            (ADDRESS)size,
            PID_c});

    this->used_memory += size;
    this->PID_c++;

    std::sort(MemoryUsage.begin(), MemoryUsage.end());
    return true;
}

bool SimOS::SimFork()
{
    return NewProcess(process_queue.top().priority, process_queue.top().size);
}

void SimOS::SimExit()
{
    if (process_queue.empty())
    {
        return;
    }

    int kill_pid = process_queue.top().PID;
    used_memory -= process_queue.top().size;
    process_queue.pop();
    MemoryUsage.erase(
        std::remove_if(MemoryUsage.begin(),
                       MemoryUsage.end(),
                       [&pid = kill_pid](const MemoryItem &m) -> bool
                       { return m.PID == pid; }),
        MemoryUsage.end());
}
