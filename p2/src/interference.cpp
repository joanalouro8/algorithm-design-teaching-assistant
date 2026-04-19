#include "interference.h"

bool sameInstruction(const RangePoint &a, const RangePoint &b)
{
    if (a.instruction != b.instruction)
    {
        return false;
    }
    if ((a.def && b.last) || (a.last && b.def))
    {
        return false;
    }
    return true;
}

bool has_interference(const Web &web1, const Web &web2)
{
    auto it_web1 = web1.points.begin();
    auto it_web2 = web2.points.begin();

    while (it_web1 != web1.points.end() && it_web2 != web2.points.end())
    {
        if (sameInstruction(*it_web1, *it_web2))
        {
            return true;
        }
        else if (*it_web1 < *it_web2)
        {
            ++it_web1;
        }
        else
        {
            ++it_web2;
        }
    }
    return false;
}