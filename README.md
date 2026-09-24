# High-Performance Custom C++ Vector

A custom `std::vector` implementation made for high-performance applications, real-time applications where memory fragmentation must be minimized

Standard dynamic arrays in C++ resize by creating entirely new memory blocks, copying the elements, and destroying the old block. This implementation uses Microsoft's mimalloc to attempt **in-place memory expansion** (like with `realloc` in C)

## Features

* **In-Place Reallocation:** Uses `mi_expand` to resize memory allocations, byte-wise, in place
* **Separation of Allocation and Construction:** Uses template metaprogramming (`std::is_class`) and placement new to construct objects only when necessary
* **Explicit Lifecycle Management:** Safely manages nested data structures and complex objects with explicit destructor calls (`~T()`)
* **Rule of 5:** Includes C++ move semantics (constructors and assignments)
* **Aligned Memory Allocation:** Uses `mi_malloc_aligned` and `alignof(T)` to guarantee memory alignment

## Why it matters
In real-time embedded systems and memory-constrained applications, this library prevents fragmentation and unnecessary latency spikes. 

## Technologies Used
* **C++11/C++14** 
* **Microsoft mimalloc** (High-performance drop-in allocator)
