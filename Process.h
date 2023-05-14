
#pragma once

#include <iostream>
#include <vector>
#include <array>

#include "Address.h"
#include "Hole.h"

struct Process
{

    /**
     * by giving each enum in State a specific integer value, we can sort each
     * Process using State as what we compare with :)
     * guaranteeing that running processes are always far left, followed by waiting,
     * reading, and then zombie processes
     *
     * the running processes are then further sorted by priority and PID
     */
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

    // making this array into a const prevents us from allocating its memory every time :)
    const std::array<std::string, 4> mapping{"running", "waiting", "reading", "zombie"};

    friend bool operator<(Process const &a, Process const &b)
    {
        /**
         * running a sort on the vector of processes with this comparator
         * guarantees that the resultant vector is in the proper order
         * of execution
         */

        if (a.state != b.state) // processes with different states, sort by state
        {
            return a.state < b.state;
        }
        if (a.priority == b.priority) // equal priorities prefer older processes
        {
            return a.PID < b.PID;
        }
        return a.priority > b.priority; // otherwise, sort by priority
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
           << "state: " << p.mapping[p.state] << " | "
           << "parent: " << (p.parent) << " | "
           << "children: " << children_out;

        return os;
    }
};