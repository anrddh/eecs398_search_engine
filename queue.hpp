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
        : numElements(0)
        {
        start = std::unique_ptr<QueueNode>(new QueueNode());
        end = start.get();
        }

    Queue(const Queue & other)
        : numElements(other.numElements) 
        {
            start = std::unique_ptr<QueueNode> (new QueueNode(*(other.start)));
            end = start.get();
            while(end->nextptr() != nullptr)
                end = end->nextptr();
        }

    Queue(Queue && other)
        : start(std::move(other.start)), end(other.end), numElements(other.numElements)
        {
        }

    void push( const value_type value )
        {
        end = end->push(value);
        ++numElements;
        }

    void push( value_type && value )
        {
        end = end->push( std::move( value ) );
        ++numElements;
        }

    void pop()
        {
        if( empty() )
            throw;
        if(start->popAndDone())
            {
            start = start->moveNext();
            }
        --numElements;
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
        // return start.get() == end && start->empty();
        return numElements == 0;
        }

    int size() const
        {
        return numElements;
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

        QueueNode(const QueueNode & other):
            current(other.current)
            {
            data = other.data;
            if( next != nullptr)
                next = std::unique_ptr<QueueNode> (new QueueNode(*(other.next)));
            else
                next = nullptr;
            }

        QueueNode* push(const value_type & value)
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

        QueueNode* push(value_type&& value)
            {
            if(data.size() == ARRAYSIZE)
                {
                next = std::unique_ptr<QueueNode>(new QueueNode());
                next->push(std::move(value));
                return next.get();
                }
            else
                {
                data.push_back(std::move(value));
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

        std::unique_ptr<QueueNode> moveNext()
            {
                return std::move(next);
            }

        QueueNode* nextptr()
            {
                return next.get();
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
    int numElements;

};

};