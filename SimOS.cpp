// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, int amountOfRAM)
{
    this->total_memory = amountOfRAM;
    for (int x = 0; x < numberOfDisks; x++)
    {
        this->file_requests.push_back(std::deque<FileReadRequest>{});
    }
}

bool SimOS::NewProcess(int priority, ADDRESS size)
{
    return NewProcess(priority, size, 0);
}

bool SimOS::NewProcess(int priority, ADDRESS size, int parent_pid)
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

    std::vector<Hole> holes;

    if (MemoryUsage.begin()->itemAddress > 0)
    {
        holes.push_back(
            Hole{
                0,
                MemoryUsage.begin()->itemAddress,
                MemoryUsage.begin()->itemAddress});
    }

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
    auto head = process_queue.begin();
    head->state = Process::State::WAITING;
    for (auto x = head->children.begin(); x != head->children.end(); x++)
    {
        auto child = std::find_if(
            std::begin(process_queue),
            std::end(process_queue),
            [&pid = *x](const Process &p)
            {
                return p.PID == pid && p.state == Process::State::ZOMBIE;
            });

        // this also should never happen, but im leaving it here just in case
        if (child == process_queue.end())
        {
            continue;
        }
        else
        {
            process_queue.erase(child);
            x = head->children.erase(x);
            break;
        }
    }
    std::sort(process_queue.begin(), process_queue.end());
}

void SimOS::SimExit()
{
    if (process_queue.empty())
    {
        return;
    }

    auto victim = process_queue.begin();

    if (victim->parent != 0) // if victim has a parent process
    {
        auto parent = std::find_if(
            std::begin(process_queue),
            std::end(process_queue),
            [&parent_pid = victim->parent](const Process &p)
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
         *
         * im pretty sure this would never occur though, as the parent process
         * (with the lower PID than the child) would always go before any child
         * process with a higher PID
         *
         * implementing it here just in case
         */
        if (parent->state != Process::State::WAITING)
        {

            victim->state = Process::State::ZOMBIE;
            used_memory -= victim->size;

            MemoryUsage.erase(
                std::remove_if(MemoryUsage.begin(),
                               MemoryUsage.end(),
                               [&pid = victim->PID](const MemoryItem &m) -> bool
                               { return m.PID == pid; }),
                MemoryUsage.end());

            std::sort(MemoryUsage.begin(), MemoryUsage.end());
            std::sort(process_queue.begin(), process_queue.end());

            return;
        }
        else if (parent->state == Process::State::WAITING)
        {
            // if the parent was waiting for this process to finish, set the parent to running
            // and proceed to kill this process as usual
            parent->state = Process::State::RUNNING;
        }
    }

    int kill_pid = victim->PID;
    used_memory -= victim->size;

    std::vector<int> kill_list(victim->children);

    // the primary process to kill
    process_queue.erase(victim);
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

        if (c_victim_process->state == Process::State::READING)
        {
            bool canary = false;
            for (auto queue = file_requests.begin(); queue != file_requests.end(); queue++)
            {
                for (auto freq = queue->begin(); freq != queue->end(); freq++)
                {
                    if (freq->PID == c_victim)
                    {
                        queue->erase(freq);
                        canary = true;
                        break;
                    }
                }
                if (canary)
                {
                    break;
                }
            }
        }

        // auto c_victim_freq = std::find_if(file_requests.begin(),
        //                                   file_requests.end(),
        //                                   [&pid = c_victim](const std::deque<FileReadRequest> &m) -> bool
        //                                   {
        //                                       auto freq = std::find_if(m.begin(), m.end(), [&pid = pid](const FileReadRequest &f) -> bool
        //                                                                { return f.PID == pid; });
        //                                       return (freq != m.end());
        //                                   });
        // std::cout << c_victim_freq-> << std::endl;

        used_memory -= c_victim_process->size;

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
    for (auto x : process_queue)
    {
        if (x.state == Process::State::RUNNING)
        {
            return x.PID;
        }
    }
    return 0;
}

std::vector<int> SimOS::GetReadyQueue()
{
    std::vector<int> out{};
    for (auto x = process_queue.begin() + 1; x != process_queue.end(); x++)
    {
        if (x->state == Process::State::RUNNING)
        {
            out.push_back(x->PID);
        }
    }
    return out;
}

FileReadRequest SimOS::GetDisk(int diskNumber)
{

    return file_requests[diskNumber].empty() ? FileReadRequest{} : file_requests[diskNumber].front();
}

std::queue<FileReadRequest> SimOS::GetDiskQueue(int diskNumber)
{
    if (file_requests[diskNumber].size() <= 1)
    {
        return std::queue<FileReadRequest>{};
    }
    std::deque<FileReadRequest> out(file_requests[diskNumber].begin() + 1, file_requests[diskNumber].end());
    return std::queue<FileReadRequest>(out);
}

std::vector<MemoryItem> SimOS::GetMemory()
{
    return this->MemoryUsage;
}

void SimOS::display()
{
    std::cout << "RUNNING PROCESSES" << std::endl;
    for (auto x : process_queue)
    {
        std::cout << x << std::endl;
    }

    std::cout << std::endl;

    std::cout << "MEMORY MAP" << std::endl;
    for (auto x : MemoryUsage)
    {
        std::cout << x << std::endl;
    }

    std::cout << "memory usage: " << used_memory << std::endl;
    std::cin.get();

    std::cout << "============================" << std::endl;
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName)
{
    auto reader = process_queue.begin();

    if (reader->state != Process::State::RUNNING)
    {
        return;
    }

    reader->state = Process::State::READING;
    file_requests[diskNumber].push_back(FileReadRequest{reader->PID, fileName});
    std::sort(process_queue.begin(), process_queue.end());
}

void SimOS::DiskJobCompleted(int diskNumber)
{
    int reader_pid = file_requests[diskNumber].front().PID;
    file_requests[diskNumber].pop_front();
    auto reader = std::find_if(process_queue.begin(),
                               process_queue.end(),
                               [&pid = reader_pid](const Process &m) -> bool
                               {
                                   return m.PID == pid;
                               });

    reader->state = Process::State::RUNNING;
    std::sort(process_queue.begin(), process_queue.end());
}
