// Moududur Rahman
#pragma once
#include <vector>
#include <iostream>
#include <queue>

struct Process
{
    int priority;
    int size;

    friend bool operator<(Process const &a, Process const &b)
    {
        return a.priority < b.priority;
    }

    friend std::ostream &operator<<(std::ostream &os, const Process &p)
    {
        os << "priority:" << p.priority << " | "
           << "memory usage: " << p.size;
    }
};

struct FileReadRequest
{
    int PID;
    std::string fileName;
};

class SimOS
{
public:
    std::priority_queue<Process, std::vector<Process>, std::less<Process>> process_queue;
    int total_memory;
    int used_memory;

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