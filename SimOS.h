// Moududur Rahman
#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <memory>
#include <deque>

#define ADDRESS unsigned long long

struct Process
{

    enum State
    {
        RUNNING = 0,
        WAITING = 1,
        ZOMBIE = 2
    };

    int priority;
    ADDRESS size;
    int PID;
    State state; // 0 -> running, 1 -> waiting, 2-> zombie
    int parent;
    std::vector<int> children;

    friend bool operator<(Process const &a, Process const &b)
    {
        /**
         * while std::priority queue is weakly stable, it's not guaranteed to maintain
         * order of insertion when two processes have the same priority.
         * fall back to ordering by PID for equal priority processes.
         * additionally, guarantee that waiting processes are always handled last
         */

        if (a.state != b.state)
        {
            return a.state < b.state;
        }

        // if (a.state < b.state)
        // {
        //     return a.state < b.state;
        // }

        // if (b.state > a.state)
        // {
        //     return b.state > a.state;
        // }

        if (a.priority == b.priority)
        {
            return b.PID > a.PID;
        }
        return a.priority > b.priority;
    }

    friend std::ostream &operator<<(std::ostream &os, const Process &p)
    {

        std::string children_out = "";
        if (!p.children.empty())
        {
            for (auto x : p.children)
            {
                children_out += std::to_string(x) + ", ";
            }
        }

        os << "pid: " << p.PID << " | "
           << "priority:" << p.priority << " | "
           << "memory usage: " << p.size << " | "
           << "state: " << (p.state == Process::State::RUNNING ? "running" : p.state == Process::State::WAITING ? "waiting"
                                                                                                                : "zombie")
           << " | "
           << "parent: " << (p.parent) << " | children: " << children_out;

        return os;
    }
};

struct FileReadRequest
{
    int PID;
    std::string fileName;
};

struct MemoryItem
{
    ADDRESS itemAddress;
    ADDRESS itemAddressEnd;
    ADDRESS itemSize;
    int PID; // PID of the process using this chunk of memory

    friend std::ostream &operator<<(std::ostream &os, const MemoryItem &p)
    {
        os << "address: " << p.itemAddress << " to " << p.itemAddressEnd << " | "
           << "size:" << p.itemSize << " | "
           << "PID: " << p.PID;
    }

    friend bool operator<(MemoryItem const &a, MemoryItem const &b)
    {
        return a.itemAddress < b.itemAddress;
    }
};

struct Hole
{
    ADDRESS start;
    ADDRESS end;
    ADDRESS size;

    friend std::ostream &operator<<(std::ostream &os, const Hole &p)
    {
        os << "hole from: " << p.start << " | "
           << "until " << p.end << " | "
           << "size: " << p.size;
    }

    // it's a little hacky but it's
    friend bool operator>(Hole const &a, Hole const &b)
    {
        return a.size > b.size;
    }
};

class SimOS
{
public:
    std::deque<Process> process_queue;
    // TODO: reimplement this with a heap so that we can guarantee that it's in order when we traverse it
    std::vector<MemoryItem> MemoryUsage;
    int total_memory;
    int used_memory = 0;
    int PID_c = 1;

    SimOS(int numberOfDisks, int amountOfRAM);
    bool NewProcess(int priority, ADDRESS size);
    bool NewProcess(int priority, ADDRESS size, int parent_pid);
    bool SimFork();
    void SimExit();
    void SimWait();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);
    int GetCPU();
    std::vector<int> GetReadyQueue();
    FileReadRequest GetDisk(int diskNumber);
    std::queue<FileReadRequest> GetDiskQueue(int diskNumber);
    void display();
};