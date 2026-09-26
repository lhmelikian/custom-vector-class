#ifndef CUSTOM_VECTOR_HPP
#define CUSTOM_VECTOR_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <initializer_list>
#include <type_traits>
#include <new>
#include <stdexcept>

#include "reallocpp.h"       // legacy fallback
#include "MimallocCore.h"    // mimalloc integration

#define RESIZE_CAPACITY_FACTOR 20

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
        if (index >= m_numElements) // Changed from (index < 0 || index > m_numElements)
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

template <typename T>
vector<T>::vector()
    : m_capacity(0), m_data(nullptr), m_numElements(0)
{
    // no initial memory
}

template <typename T>
vector<T>::vector(std::initializer_list<T> init) {
    // create room for the incoming data
    size_t initSize = init.size();
    m_capacity = initSize;
    // m_data = static_cast<T*> (malloc((sizeof(T)) * m_capacity));
    // m_data = new T[m_capacity];

    // this constructs objects in the new memory
    // necessary because constructing from init List
    // we know that we will be populating all objects
    m_data = mi_newMalloc(m_data, m_capacity, isClass);
    if (m_data == nullptr) 
        throw std::runtime_error("Memory allocation failed with initList");
    m_numElements = initSize;

    const T* pInit = init.begin();
    for (size_t i = 0; i < initSize; i++) {
        m_data[i] = pInit[i];
    }
}

template <typename T>
vector<T>& vector<T>::operator=(std::initializer_list<T> init) {
    int initSize = init.size();
    const T* pInit = init.begin();
    // need to deal with whatever memory we have currently
    if (m_capacity == 0 && m_data == nullptr) {
        if (initSize == 0)
            return *this;

        // important because we using initList 
        m_data = mi_newMalloc(m_data, initSize, isClass);

        for (size_t i = 0; i < initSize; i++)
            m_data[i] = pInit[i];

    } else {
        // what if initlist is empty
        if (initSize == 0) {
            this->clear_resize();
            return *this;
        }
        // data exists, we must reallocate
        // T* temp_ptr = reallocpp(m_data, m_capacity, initSize);
      
        // fine because we know that all objects in capacity must be populated
        T* temp_ptr = mi_reallocpp(m_data, m_capacity, initSize);
        if (temp_ptr == nullptr)
            throw std::runtime_error("Memory reallocation failed during initlist assignment");
        // unnecessary to do this. TODO: simplify 
        m_data = temp_ptr;
        temp_ptr = nullptr;
        for (size_t i = 0; i < initSize; i++) 
            m_data[i] = pInit[i];
    }
    m_capacity = initSize;
    m_numElements = initSize;
    return *this;
}
template <typename T>
vector<T>::vector(const vector& other) {
    // deep copy
    // don't copy the capacity, why?
    m_capacity = other.m_numElements;
    m_numElements = other.m_numElements;
    // m_data = static_cast<T*> (malloc(sizeof(T) * m_capacity));
    // m_data = new T[m_capacity];

    // good because we know all data must be populated? -- not sure
    // TODO: unncessary to copy the capacity, just copy the elements
    m_data = mi_newMalloc(m_data, m_capacity, isClass);

    // copy all the elements
    for (size_t i = 0; i < m_numElements; i++) 
        m_data[i] = other.m_data[i];
}

template <typename T>
vector<T>::vector(vector&& other) noexcept {
    // steal our variables
    // don't steal the capacity, though
    m_capacity = other.m_numElements;    m_numElements = other.m_numElements;
    if (other.m_data != nullptr) {
        m_data = other.m_data;
        other.m_data = nullptr;
    } else {
        m_data = nullptr;
    }
    other.m_capacity = 0;
    other.m_numElements = 0;
}

template <typename T>
vector<T>& vector<T>::operator=(vector&& other) noexcept {
    if (m_data != nullptr) {
        // ideally we only destroy as much as there are elements
        // TODO: change ideally from m_capacity to m_numElements
        // i believe this works however it is untested
        for (size_t i = 0; i < m_numElements; i++) {
            if (isClass)
                m_data[i].~T();
        }
        mi_free(m_data);
    }

    // move assignment, don't copy capacity
    m_capacity = other.m_numElements;
    m_numElements = other.m_numElements;
    // need to clean up current data
    // bc it is move assignment

    if (other.m_data == nullptr) {
        m_data = nullptr;
    } else {   
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    other.m_capacity = 0;
    other.m_numElements = 0;
    return *this;
}
template <typename T> 
vector<T>& vector<T>::operator=(const vector& other) {
    if (this != &other) {
        // TODO: determine if we should free and malloc or just
        // do someth`ing with data and realloc
        // i will save this in a temp var for now

        // TODO: copy assignment shouldn't necessarily copy capacity
        size_t tempCap = m_capacity;
        m_capacity = other.m_capacity;
        m_numElements = other.m_numElements;
        if (other.m_data != nullptr) {
            // perform a deep copy
            if (m_data != nullptr) {
                // T* temp_ptr = static_cast<T*> (realloc(m_data, sizeof(T) * m_capacity));
                // T* temp_ptr = reallocpp(m_data, tempCap, m_capacity);
                T* temp_ptr = mi_reallocpp(m_data, tempCap, m_capacity);
                if (temp_ptr == nullptr)
                    throw std::runtime_error("Memory reallocation failed during copy assignment realloc");
                // unncessary. TODO: Fix
                m_data = temp_ptr;
                temp_ptr = nullptr;
            } else {
                // m_data = static_cast<T*> (malloc(sizeof(T) * m_capacity));
                // m_data = new T[m_capacity];
                m_data = mi_newMalloc(m_data, m_capacity, isClass);
                if (m_data == nullptr)
                    throw std::runtime_error("Memory reallocation failed during copy assignment malloc");    
            } 
            for (size_t i = 0; i < m_numElements; i++) 
                m_data[i] = other.m_data[i];
        } else {
            m_data = nullptr;
        }
    }
    return *this;
}

template <typename T>
vector<T>::~vector() {
    // destroy our memory
    if (m_data != nullptr) {
        // TODO: ideally, we only destroy for as many elements as we have
        for (size_t i = 0; i < m_numElements; i++) {
            if (isClass)
                m_data[i].~T();
        }
        mi_free(m_data);
        m_data = nullptr;
    }
}

template <typename T>
void vector<T>::resize(size_t newSize) {
    if (newSize == 0) {
        this->clear_resize();
        return;
    }
    if (newSize == m_capacity) 
        return;
    // T* temp_ptr = reallocpp(m_data, m_capacity, newSize);
    
    // fine because resize requires instantiation
    T* temp_ptr = mi_reallocpp(m_data, m_capacity, newSize);
    if (temp_ptr == nullptr)
        throw std::runtime_error("Memory reallocation failed during vector resize");
    // numElements is newSize when you resive, because those are constructed objects
    /*if (newSize < m_numElements) 
        m_numElements = newSize;*/
    m_numElements = newSize;
    if (newSize > m_capacity)
        m_capacity = newSize;

    // Unnecessary. TODO: fix
    m_data = temp_ptr;
    temp_ptr = nullptr;
}

template <typename T>
void vector<T>::push_back(const T& value) {
    m_numElements++;
    // this is hacky but it will work
    T* temp_ptr = nullptr;
    // resize array if adding our element exceeds the capacity
    if (m_capacity == 0) {
        // m_data = static_cast<T*> (malloc(sizeof(T)));
        // m_data = new T;
        m_data = mi_newMalloc(m_data, 1, isClass);
        if (m_data == nullptr) 
            throw std::runtime_error("Initial memory allocation failed");
        m_capacity = 1;
    } else if (m_numElements > m_capacity) {
        // T* temp_ptr = static_cast<T*> (realloc(m_data, sizeof(T) * (2 * m_capacity)));
        // T* temp_ptr = reallocpp(m_data, m_capacity, 2 * m_capacity);

        // functionality: want to increase capacity and only instantiate in where im pushing
        
        // here is bad because it instantiates in all of m_capacity
        // T* temp_ptr = mi_reallocpp(m_data, m_capacity, 20 + m_capacity);

        // allocates new raw memory
        temp_ptr = mi_rawReallocpp(m_data, m_capacity, RESIZE_CAPACITY_FACTOR + m_capacity);
        if (temp_ptr == nullptr) 
            throw std::runtime_error("Memory allocation failed during push back");

        // unncessary. TODO: fix
        m_data = temp_ptr;
        temp_ptr = nullptr;

        m_capacity += RESIZE_CAPACITY_FACTOR;
    }

    // now instantiate object in where we're about to push
    if (isClass) {
        temp_ptr = m_data + m_numElements - 1;
        temp_ptr = new(temp_ptr) T;
    }

    m_data[m_numElements - 1] = value;
}

template <typename T>
T& vector<T>::at(size_t index) {
    // because indexes are zero based, minus 1
    if (index > m_numElements - 1) {
        throw std::out_of_range("Requested index out of range of vector");
    }
    return m_data[index];
}

template <typename T>
size_t vector<T>::size() {
    return m_numElements;
}

template <typename T>
void vector<T>::clear() {
    m_numElements = 0;
}

template <typename T>
void vector<T>::clear_resize() {
    // i want vector to resize itself 
    // return memory to system 
    if (m_capacity > 0) {
        if (m_data != nullptr) {
            // only constructed objects will be elements
            for (size_t i = 0; i < m_numElements; i++) {
                if (isClass)
                    m_data[i].~T();
            }
            mi_free(m_data);
            m_data = nullptr;
        }
    }
    m_capacity = 0;
    m_numElements = 0;
}

// not fixed yet
template <typename T>
void vector<T>::shrink_to_fit() {
    if (m_capacity == 0 || m_capacity == m_numElements)
        return;
    // T* temp_ptr = reallocpp(m_data, m_capacity, m_numElements);
    T* temp_ptr = mi_reallocpp(m_data, m_capacity, m_numElements);
    if (temp_ptr == nullptr)
        throw std::runtime_error("Memory reallocation failed in shrink to fit");

    // unncessary. TODO: fix
    m_data = temp_ptr;
    temp_ptr = nullptr;
    m_capacity = m_numElements;
}

template <typename T>
void vector<T>::pop_back() {
    // TODO: destroy
    if (isClass)
        m_data[m_numElements - 1].~T();
    m_numElements--;
}

template <typename T>
bool vector<T>::IsEmpty() {
    if (m_numElements == 0) 
        return true;
    return false;
}

template <typename T>
void vector<T>::reserve(size_t newMem) {
    // i need some way to 
    // if nothing, we must mimalloc
    T* temp_ptr = nullptr;
    if (m_data == nullptr) {
        m_data = static_cast<T*> (mi_malloc_aligned(sizeof(T) * newMem, alignof(T)));
    }
    else {
        if (newMem <= m_capacity)
            return;
        // these are pure bytes
        temp_ptr = mi_rawReallocpp(m_data, m_capacity, newMem);
        if (temp_ptr == nullptr)
            throw std::runtime_error("Memory reallocation failed during reserve");
        m_data = temp_ptr;
        temp_ptr = nullptr;
        // if able to expand in place, then do nothing! 
        // well they are already equal at this point
        // temp_ptr = nullptr;
        // make new, uninitialized space
        // need to think about this slightly more
    }
    // now we have to update capacity, number of elements remains
    m_capacity = newMem;
}

template <typename T>
void vector<T>::printelm() {
    for (size_t i = 0; i < m_numElements; i++) {
        std::cout << m_data[i] << " ";
    }
}

template <typename T>
size_t vector<T>::capacity() {
    return m_capacity;
}

template <typename T>
void vector<T>::Zset() {
    if (m_data == nullptr)
        return;
    for (size_t i = 0; i < m_capacity; i++) {
        m_data[i] = T();
    }
}

template <typename T>
T* vector<T>::begin() {
    return m_data;
}

template <typename T>
T* vector<T>::end() {
    return (m_data + m_numElements) - 1;
}

#endif // CUSTOM_VECTOR_HPP
