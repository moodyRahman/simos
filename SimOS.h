// Moududur Rahman
#pragma once
#include <vector>
#include <iostream>
#include <queue>

struct Process
{
    int priority;
    int size;
};

struct FileReadRequest
{
    int PID;
    std::string fileName;
};

class SimOS
{
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