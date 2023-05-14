
#pragma once

#include "Address.h"
#include <iostream>

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

        return os;
    }

    friend bool operator>(Hole const &a, Hole const &b)
    {
        if (a.size == b.size)
        {
            return a.start < b.start;
        }

        return a.size > b.size;
    }
};