// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, int amountOfRAM)
{
    this->total_memory = amountOfRAM;
}

bool SimOS::NewProcess(int priority, int size)
{
    return NewProcess(priority, size, 0);
}

bool SimOS::NewProcess(int priority, int size, int parent_pid)
{
    if (size > total_memory - used_memory)
    {
        return false;
    }

    bool found_valid_memory = false;

    if (MemoryUsage.size() == 0)
    {
        process_queue.push_front(
            Process{
                priority, size, PID_c, Process::State::RUNNING, parent_pid});
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

    // if (MemoryUsage.size() == 1)
    // {
    //     std::cout << "there" << std::endl;
    //     auto left = MemoryUsage[0];
    //     process_queue.push_front(
    //         Process{
    //             priority, size, PID_c, 0});
    //     MemoryUsage.push_back(
    //         MemoryItem{
    //             (ADDRESS)left.itemAddressEnd,
    //             (ADDRESS)left.itemAddressEnd + size,
    //             (ADDRESS)size,
    //             PID_c});

    //     this->used_memory += size;
    //     this->PID_c++;
    //     return true;
    // }

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

    process_queue.push_front(
        Process{
            priority, size, PID_c, Process::State::RUNNING, parent_pid});
    MemoryUsage.push_back(
        MemoryItem{
            (ADDRESS)ideal->start,
            (ADDRESS)ideal->start + size,
            (ADDRESS)size,
            PID_c});

    this->used_memory += size;
    this->PID_c++;

    std::sort(MemoryUsage.begin(), MemoryUsage.end());
    std::sort(process_queue.begin(), process_queue.end());

    return true;
}

bool SimOS::SimFork()
{
    auto top = process_queue.begin();
    top->children.push_back(PID_c);
    return NewProcess(process_queue[0].priority, process_queue[0].size, process_queue[0].PID);
}

void SimOS::SimWait()
{
    process_queue[0].state = Process::State::WAITING;
    std::sort(process_queue.begin(), process_queue.end());
}

void SimOS::SimExit()
{
    if (process_queue.empty())
    {
        return;
    }

    Process victim = process_queue[0];

    if (victim.parent != 0) // if victim has a parent process
    {
        auto parent = std::find_if(
            std::begin(process_queue),
            std::end(process_queue),
            [&parent_pid = victim.parent](const Process &p)
            {
                return p.PID == parent_pid;
            });

        /**
         * this boolean should never be true!!!
         * this will only execute if the process we're exiting has a parent that
         * doesn't exist/ has already exited, ie an orphan process.
         *
         * the cascading termination should prevent the existence of any orphans
         */
        if (parent == std::end(process_queue))
        {
            std::cout << "so... you've found an orphan.... that shouldnt happen" << std::endl;
            return;
        }

        /**
         * if the parent hasn't called wait yet, this process becomes a zombie
         */
        if (parent->state == 0)
        {
            victim.state = Process::State::ZOMBIE;
            return;
        }
        else // the parent is in a waiting state, set the parent to be runnable now
        {
            parent->state = Process::State::RUNNING;
        }
    }

    int kill_pid = victim.PID;
    used_memory -= victim.size;

    std::vector<int> kill_list(victim.children);

    // the primary process to kill
    process_queue.pop_front();
    MemoryUsage.erase(
        std::remove_if(MemoryUsage.begin(),
                       MemoryUsage.end(),
                       [&pid = kill_pid](const MemoryItem &m) -> bool
                       { return m.PID == pid; }),
        MemoryUsage.end());

    std::sort(MemoryUsage.begin(), MemoryUsage.end());
    std::sort(process_queue.begin(), process_queue.end());

    // now that we've killed the parent, it's time to cascade kill children

    while (!kill_list.empty())
    {
        int c_victim = kill_list[0];
        kill_list.erase(kill_list.begin());
        auto c_victim_process = std::find_if(process_queue.begin(),
                                             process_queue.end(),
                                             [&pid = c_victim](const Process &m) -> bool
                                             {
                                                 return m.PID == pid;
                                             });

        // append all the children of the process we're killing to kill_list
        kill_list.insert(kill_list.end(), c_victim_process->children.begin(), c_victim_process->children.end());

        MemoryUsage.erase(
            std::remove_if(MemoryUsage.begin(),
                           MemoryUsage.end(),
                           [&pid = c_victim](const MemoryItem &m) -> bool
                           { return m.PID == pid; }),
            MemoryUsage.end());

        process_queue.erase(
            std::remove_if(process_queue.begin(),
                           process_queue.end(),
                           [&pid = c_victim](const Process &m) -> bool
                           { return m.PID == pid; }),
            process_queue.end());
    }

    std::sort(MemoryUsage.begin(), MemoryUsage.end());
    std::sort(process_queue.begin(), process_queue.end());
}

int SimOS::GetCPU()
{
    return process_queue[0].PID;
}
