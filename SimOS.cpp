// Moududur Rahman
#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, ADDRESS amountOfRAM)
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
    // keeping a running tally of the current memory use was bug-prone,
    // settling for calculating the total memory use on a per instantiation basis
    int n_used_memory = 0;
    for (auto x : process_queue)
    {
        n_used_memory += x.size;
    }
    if (size > total_memory - n_used_memory)
    {
        return false;
    }

    bool found_valid_memory = false;

    // handle special case
    // if there are no memory elements, make one from 0 to size
    if (MemoryUsage.empty())
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

    // handle edge case
    // create a hole from address 0 until the first memeory element
    // if there is a gap starting from address 0
    if (MemoryUsage.begin()->itemAddress > 0)
    {
        holes.push_back(
            Hole{
                0,
                MemoryUsage.begin()->itemAddress,
                MemoryUsage.begin()->itemAddress});
    }

    // because we sort MemoryUsage after modifying it, we can guarantee that
    // iterating through it will give us adjacent items in memory
    for (auto it = this->MemoryUsage.begin(); it != this->MemoryUsage.end(); it++)
    {
        // once we reach the last memeory element, add a hole from the end of that
        // element until the highest address
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
            // if where this element ends if where the next element begins, there is no hole
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

    /**
     *
     * sort holes according to their sizes, smallest holes first
     * prefer earlier holes for same sized holes
     * std::sort isnt stable, so the Hole comparator also has an address fallback
     * if sizes are equal
     */
    std::sort(holes.begin(), holes.end(), std::greater<Hole>());

    // Hole *ideal = nullptr;
    // using this instead of a tripwire/ canary boolean
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

    // we've successfuly allocated the best fit memory for this process!
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

    /**
     * sort the arrays so MemoryUsage stays contigious
     * and so that the top of process_queue is the running process
     *
     * while this might seem inefficient, it's almost identical to using a priority queue
     * std::sort is an inplace sort, and with a single element that's not in order
     * that single element will bubble to it's proper position, call it effectively
     * O(n)
     */
    std::sort(MemoryUsage.begin(), MemoryUsage.end());
    std::sort(process_queue.begin(), process_queue.end());

    return true;
}

bool SimOS::SimFork()
{
    auto top = process_queue.begin();

    if (top->state != Process::State::RUNNING || top == process_queue.end())
    {
        std::cout << "no valid process, ignoring" << std::endl;
        return false;
    }

    top->children.push_back(PID_c);
    // lets let NewProcess figure out whether this is a legal fork and allocating its memory
    return NewProcess(process_queue[0].priority, process_queue[0].size, process_queue[0].PID);
}

void SimOS::SimWait()
{
    auto head = process_queue.begin();

    if (head->state != Process::State::RUNNING || head == process_queue.end())
    {
        std::cout << "no valid process, ignoring" << std::endl;
        return;
    }

    head->state = Process::State::WAITING;
    for (auto x = head->children.begin(); x != head->children.end(); x++)
    {
        // search for a zombie child
        auto child = std::find_if(
            std::begin(process_queue),
            std::end(process_queue),
            [&pid = *x](const Process &p)
            {
                return p.PID == pid && p.state == Process::State::ZOMBIE;
            });

        // we did not find the zombified child, do nothing and try the next child
        if (child == process_queue.end())
        {
            continue;
        }
        else
        {
            // once we do find the zombie child, prevent the revival of any further children
            // and dont set this process to wait
            process_queue.erase(child);
            x = head->children.erase(x);
            head->state = Process::State::RUNNING;
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

    if (victim->state != Process::State::RUNNING || victim == process_queue.end())
    {
        std::cout << "no valid process, ignoring" << std::endl;
        return;
    }

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
         * the cascading termination algo should prevent the existence of any orphans
         */
        if (parent == std::end(process_queue))
        {
            // std::cout << "so... you've found an orphan.... that shouldnt happen" << std::endl;
            return;
        }

        /**
         * if the parent hasn't called wait yet, this process becomes a zombie
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

    // std::sort(MemoryUsage.begin(), MemoryUsage.end());
    // std::sort(process_queue.begin(), process_queue.end());

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

        // if the victim is reading, handle removing it from the file read queue
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
                    // once we've deleted the victims file read request, do nothing else
                    break;
                }
            }
        }
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
    // we make a deque that has all but the currently reading file
    // which we then cast to queue
    std::deque<FileReadRequest> out(file_requests[diskNumber].begin() + 1, file_requests[diskNumber].end());
    return std::queue<FileReadRequest>(out);
}

std::vector<MemoryItem> SimOS::GetMemory()
{
    // we can safely do this without worrying about zombie processes because
    // we deleted their memory during the zombification process
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
    if (diskNumber >= file_requests.size() || diskNumber < 0)
    {
        std::cout << "bad disk number, ignoring instruction" << std::endl;
        return;
    }
    auto reader = process_queue.begin();

    // if the "top" process is anything but running, do nothing
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
    if (diskNumber >= file_requests.size() || diskNumber < 0)
    {
        std::cout << "bad disk number, ignoring instruction" << std::endl;
        return;
    }
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
