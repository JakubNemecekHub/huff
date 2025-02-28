#pragma once

#include <functional>  // std::less
#include <vector>

template<typename T, typename Compare = std::less<T>>
class Heap
{
private:

    std::vector<T> heap_;
    Compare comp_;

    void heapify_up(size_t index)
    {
        while ( index > 0 )
        {
            size_t parent { (index -1)/2 };
            if ( !comp_(heap_[parent], heap_[index]) ) break;
            std::swap(heap_[parent], heap_[index]);
            index = parent;
        }
    }

    void heapify_down(size_t index)
    {
        size_t left    { 2 * index + 1 };
        size_t right   { 2 * index + 2 };
        size_t largest { index         };
        if ( left < heap_.size() && comp_(heap_[largest], heap_[left]))
        {
            largest = left;
        }
        if ( right < heap_.size() && comp_(heap_[largest], heap_[right]))
        {
            largest = right;
        }
        if ( largest != index )
        {
            std::swap(heap_[index], heap_[largest]);
            heapify_down(largest);
        }
    }

public:

    explicit Heap(Compare comp = Compare())
        : comp_{std::move(comp)} {}

    template<typename... Args>
    void emplace(Args&&... args)
    {
        heap_.emplace_back(std::forward<Args>(args)...);
        heapify_up(heap_.size() - 1);
    }

    void push(T&& value)
    {
        heap_.push_back(std::move(value));
        heapify_up(heap_.size() - 1);
    }

    const T& top() const
    {
        if ( heap_.empty() ) throw std::out_of_range("Heap is empty");
        return heap_.front();
    }

    T extract_top()
    {
        if ( heap_.empty() ) throw std::out_of_range("Heap is empty");
        T top { std::move(heap_.front()) };
        if ( heap_.size() > 1 ) heap_.front() = std::move(heap_.back());
        heap_.pop_back();
        if ( !heap_.empty() ) heapify_down(0);
        return top;
    }

    bool empty() const noexcept
    {
        return heap_.empty();
    }

    size_t size() const noexcept
    {
        return heap_.size();
    }

};
