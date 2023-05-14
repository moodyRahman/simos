
#pragma once

#include <iostream>
#include <vector>
#include <array>

#include "Address.h"
#include "Hole.h"

struct Process
{

    enum State
    {
        RUNNING = 0,
        WAITING = 1,
        READING = 2,
        ZOMBIE = 3,
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

        std::array<std::string, 4> enum2s{"running", "waiting", "reading", "zombie"};

        os << "pid: " << p.PID << " | "
           << "priority:" << p.priority << " | "
           << "memory usage: " << p.size << " | "
           << "state: " << enum2s[p.state] << " | "
           << "parent: " << (p.parent) << " | "
           << "children: " << children_out;

        return os;
    }
};