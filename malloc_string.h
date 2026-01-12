#ifndef MALLOC_STRING_H
#define MALLOC_STRING_H

#include <iostream>                     // for testing
#include <string>                       // for testing

#include "reallocpp.h"                  // deprecated, reallocpp
#include <cstdlib>                      // deprecated, malloc, realloc, free

#include <initializer_list>             // for vector functionality
#include "mimalloc_reallocpp.h"         // current-use, for mi_newMalloc, mi_reallocpp

template <typename T>
class vector {
public:
    vector();
    vector(std::initializer_list<T> init);              // initializer list constructor
    vector& operator=(std::initializer_list<T> init);   // initializer list assignment
    vector(const vector& other);                        // copy constructor 
    vector& operator=(const vector& other);             // copy assignment
    vector(vector&& other) noexcept;                    // move constructor
    vector& operator=(vector&& other) noexcept;         // move assignment operator

    const T& operator[](size_t index) const {            // bracket notation 
        // bounds checking
        if (index < 0 || index > m_numElements)
            throw std::out_of_range("Index out of range of vector");
        return m_data[index];
    }

    T& operator[](size_t index) {
        // bounds checking for now
        if (index < 0 || index > m_numElements)
            throw std::out_of_range("Index out of range of vector");
        return m_data[index];
    }
    ~vector();

    void resize(size_t newSize);    // resizes the memory capacity of the array
    void push_back(const T& value); // places element to the end of the vector
    void pop_back();                // remove last element
    void printelm();
    bool IsEmpty();                 // check if empty
    void shrink_to_fit();           // reduces capacity to the number of elements, if possible
    void reserve(size_t newMem);    // allocates raw memory, unitialized with objects

    T* begin();             // returns pointer to start of data
    T* end();               // returns pointer to the last element of data
    T& at(size_t index);
    void clear_resize();    // resets the number of elements and deallocates all memory   
    void clear();           // resets the number of elements, does not clear allocated memory
    void Zset();            // sets all possible elements to zero       
    size_t capacity();      // tells you the maximum number of elements that can be held
    size_t size();          // returns number of element 
private:
    size_t m_capacity;         // holds max number of elements
    size_t m_numElements;      // holds the current number of elements
    T* m_data;                 // pointer to data (nullptr if the vector is empty)
    bool isClass = std::is_class<T>::value;
};

#endif // MALLOC_STRING_H
