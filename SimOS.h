// Moududur Rahman
#pragma once
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <memory>
#include <deque>

#include "Hole.h"
#include "Process.h"
#include "MemoryItem.h"

struct FileReadRequest
{
    int PID;
    std::string fileName;
};

class SimOS
{
public:
    std::deque<Process> process_queue;
    // TODO: reimplement this with a heap so that we can guarantee that it's in order when we traverse it
    std::vector<MemoryItem> MemoryUsage;
    ADDRESS total_memory;
    ADDRESS used_memory = 0;
    int PID_c = 1;
    std::vector<std::deque<FileReadRequest>> file_requests;

    SimOS(int numberOfDisks, ADDRESS amountOfRAM);
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
    std::vector<MemoryItem> GetMemory();
    void display();
};