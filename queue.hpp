#pragma once

#include <vector>
#include <memory>

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
        start = std::unique_ptr<QueueNode>(new QueueNode());
        end = start.get();
        }

    void push(T& value)
    {
        end = end->push(value);
    }

    void pop()
    {
        if(start->popAndDone())
        {
            start = start->nextNode();
        }
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
        return start.get() == end && start->empty();
    }

private:
    class QueueNode
    {
    public:
        QueueNode():
            next(nullptr), current(0)
            {
            data.reserve(ARRAYSIZE);
            }

        QueueNode* push(T& value)
            {
            if(data.size() == ARRAYSIZE)
                {
                next = std::unique_ptr<QueueNode>(new QueueNode());
                next->push(value);
                return next.get();
                }
            else
                {
                data.push_back(value);
                return this;
                }
            }

        bool popAndDone()
            {
            ++current;
            if(current == ARRAYSIZE)
                {
                return true;
                }
            else
                {
                return false;
                }
            }

        std::unique_ptr<QueueNode> nextNode()
            {
                return std::move(next);
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
        std::unique_ptr<QueueNode> next;
        int current;
    };

    std::unique_ptr<QueueNode> start;
    QueueNode* end;

};

};