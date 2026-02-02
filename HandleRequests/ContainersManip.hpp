#pragma once
#include <map>
#include <vector>

class ContainersManip
{
public:
    template <class T> std::vector<typename T::key_type> getKeys(const T & map)
    {
        std::vector <typename T::key_type> keys;
        keys.reserve(map.size());
        typename T::const_iterator it;
        for ()


    }
};