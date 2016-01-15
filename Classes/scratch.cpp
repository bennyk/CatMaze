//
//  scratch.cpp
//  CatMaze
//
//  Created by Benny Khoo on 13/01/2016.
//
//

#include "scratch.hpp"

#include <functional>
#include <queue>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

struct Record {
    int _cost;
    int _id;
    
    Record(int cost, int id) : _cost(cost), _id(id) {}
    
    int compare(Record &b) const
    {
        return _cost - b._cost;
    }
    
    std::string getDesc() const {
        std::stringstream ss;
        ss << "{cost " << _cost << " id " << _id << "}";
        return ss.str();
    }
};

struct Compare {
    bool operator() (Record &a, Record &b)
    {
        return a.compare(b) > 0;
    }
};

class PriorityQueue : public std::priority_queue<Record, std::deque<Record>, Compare>
{
    using Iterator = std::deque<Record>::iterator;
public:
    Iterator find(int id)
    {
        auto first = c.begin();
        while (first!= c.end()) {
            if (first->_id == id) {
                return first;
            }
            ++first;
        }
        return c.end();
    }
    
    bool contains(int id)
    {
        auto first = c.begin();
        auto last = c.end();
        while (first!=last) {
            if (first->_id == id)
                return true;
            ++first;
        }
        
        return false;
    }
    
    void remove(int id)
    {
        auto iter = find(id);
        assert(iter != c.end());
        
        iter->_id = -1;
    }
    
    const Record &lowestTrace()
    {
        while (!empty() && top()._id == -1) {
            std::cout << "pop to discard" << std::endl;
            pop();
        }
        return top();
    }
    
};

int main()
{
    std::vector<Record> data = { {8, 1}, {3, 2}, {10, 3}, {9, 4}, {7, 5}, {1, 6} };
    
    PriorityQueue q;
    
    for (auto d : data) {
        q.push(d);
    }
    
    assert(q.contains(20) == false);
    
    assert(q.contains(1) == true);
    
    auto a = q.find(1);
    a->_id = 99;
    
    q.remove(6);
    
    // {cost 3 id 2} {cost 7 id 5} {cost 8 id 99} {cost 9 id 4} {cost 10 id 3}
    while(!q.empty()) {
        std::cout << q.lowestTrace().getDesc() << " ";
        q.pop();
    }
    std::cout << '\n';

    
    return 0;
}
