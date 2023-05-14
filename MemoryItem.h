// Moududur Rahman
#pragma once
#include <iostream>

#include "Address.h"

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

        return os;
    }

    friend bool operator<(MemoryItem const &a, MemoryItem const &b)
    {
        return a.itemAddress < b.itemAddress;
    }
};