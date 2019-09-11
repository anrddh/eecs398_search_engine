#pragma once

#include <vector>
#include <memory>
#include <iostream>

namespace fb 
{

const int ARRAYSIZE = 16;

template <class T>
class Queue
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;

    Queue()
        {
        start = end = std::make_shared<QueueNode>();
        }

    void push(T& value)
    {
        end = end->push(value);
    }

    void pop()
    {
        start = start->pop();
    }

    const_reference front() const
    {
        return start->front();
    }

    const_reference back() const
    {
        return end->back();
    }

    reference front()
    {
        return start->front();
    }

    reference back()
    {
        return end->back();
    }

    bool empty() const
    {
        return start == end && start->empty();
    }

private:
    class QueueNode : std::enable_shared_from_this<QueueNode>
    {
    public:
        QueueNode():
            next(nullptr), current(0)
            {
            std::cout << "making new array" << std::endl;
            data.reserve(ARRAYSIZE);
            }

        std::shared_ptr<QueueNode> push(T& value)
            {
            if(data.size() == ARRAYSIZE)
                {
                    std::cout << "Current node full" << std::endl;
                next = std::make_shared<QueueNode>();
                next->push(value);
                return next;
                }
            else
                {
                data.push_back(value);
                return this->shared_from_this();
                }
            }

        std::shared_ptr<QueueNode> pop()
            {
            ++current;
            if(current == ARRAYSIZE)
                {
                return next;
                }
            else
                {
                return this->shared_from_this();
                }
            }

        bool empty()
            {
            return current == data.size();
            }

        reference front()
            {
            return data[current];
            }

        const_reference front() const
            {
            return data[current];
            }

        reference back()
            {
            return data[data.size() - 1];
            }

        const_reference back() const
            {
            return data[data.size() - 1];
            }

    private:
        std::vector<T> data;
        std::shared_ptr<QueueNode> next;
        int current;
    };

    std::shared_ptr<QueueNode> start;
    std::shared_ptr<QueueNode> end;

};

};