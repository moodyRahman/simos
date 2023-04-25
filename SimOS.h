// Moududur Rahman
#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>

struct Process
{
    int priority;
    int size;
    int PID;

    friend bool operator<(Process const &a, Process const &b)
    {
        /**
         * while std::priority queue is weakly stable, it's not guaranteed to maintain
         * order of insertion when two processes have the same priority.
         * fall back to ordering by PID for equal priority processes
         */
        if (a.priority == b.priority)
        {
            return b.PID > a.PID;
        }
        return a.priority < b.priority;
    }

    friend std::ostream &operator<<(std::ostream &os, const Process &p)
    {
        os << "pid: " << p.PID << " | "
           << "priority:" << p.priority << " | "
           << "memory usage: " << p.size;
    }
};

struct FileReadRequest
{
    int PID;
    std::string fileName;
};

struct MemoryItem
{
    unsigned long long itemAddress;
    unsigned long long itemAddressEnd;
    unsigned long long itemSize;
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

class SimOS
{
public:
    std::priority_queue<Process, std::vector<Process>, std::less<Process>> process_queue;
    // TODO: reimplement this with a heap so that we can guarantee that it's in order when we traverse it
    std::vector<MemoryItem> MemoryUsage;
    int total_memory;
    int used_memory = 0;
    int PID_c = 1;

    SimOS(int numberOfDisks, int amountOfRAM);
    bool NewProcess(int priority, int size);
    bool SimFork();
    void SimExit();
    void SimWait();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);
    int GetCPU();
    std::vector<int> GetReadyQueue();
    FileReadRequest GetDisk(int diskNumber);
    std::queue<FileReadRequest> GetDiskQueue(int diskNumber);
};