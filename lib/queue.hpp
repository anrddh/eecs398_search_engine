#pragma once

#include <vector>
#include <memory>

namespace fb 
{

template <class T, int ArraySize = 32>
class Queue
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;

    Queue()
        : numElements(0)
        {
        start = nullptr;
        end = nullptr;
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

    void push( const value_type & value )
        {
        if( empty() )
            makeFirstNode();
        ++numElements;

        end = end->push( value );
        }

    void push( value_type && value )
        {
        if( empty() )
            makeFirstNode();
        ++numElements;

        end = end->push( std::move( value ) );
        }

    template<class ... Args>
    void emplace( Args&&... args) 
        {
        if( empty() )
            makeFirstNode();
        ++numElements;

        end = end->emplace( std::forward< Args >( args )... );
        }


    void pop()
        {
        if( empty() )
            throw;
        start->popAndMoveNext(start);
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
        return numElements == 0;
        }

    int size() const
        {
        return numElements;
        }

private:

    void makeFirstNode()
        {
            start = std::unique_ptr<QueueNode>(new QueueNode());
            end = start.get();
        }

    class QueueNode
    {
    public:
        QueueNode():
            next(nullptr), current(0)
            {
            data.reserve(ArraySize);
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
            if(data.size() == ArraySize)
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
            if(data.size() == ArraySize)
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

        template<class ... Args>
        QueueNode* emplace( Args&&... args) 
            {
            if(data.size() == ArraySize)
                {
                next = std::unique_ptr<QueueNode>(new QueueNode());
                next->emplace(std::forward< Args >( args )...);
                return next.get();
                }
            else
                {
                data.emplace_back(std::forward< Args >( args )...);
                return this;
                }
            }

        void popAndMoveNext(std::unique_ptr<QueueNode>& start)
            {
            ++current;
            if(current == ArraySize)
                {
                start = std::move(next);
                }
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