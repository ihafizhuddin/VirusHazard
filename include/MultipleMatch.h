#ifndef MULTIPLE_MATCH_H
#define MULTIPLE_MATCH_H

#include <vector>

#include "Match.h"

class MultipleMatch : public std::vector<Match>
{
public:

    bool matched(Coord C)
    {
        vector<Match>::iterator it;
        for(it = begin(); it != end(); ++it)
        {
            if(it -> matched(C))
                return true;
        }
        return false;
    }

};

#endif