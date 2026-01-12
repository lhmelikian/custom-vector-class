#ifndef REALLOCPP_H
#define REALLOCPP_H

/*

accepts pointer to contiguous data, how much contiguous data was allocated, and how much new data total needed
returns pointer to new data
returns nullptr if new data has a size of 0

*/
template <typename T>
T* reallocpp(T*& block, size_t allocedMem, size_t size) {
    T* temp_ptr = nullptr;

    if (allocedMem == 1) {
        if (size == 1) {
            temp_ptr = new T;
            *temp_ptr = *block;
        } else if (size > 1) {
            temp_ptr = new T[size];
            *temp_ptr = *block;
        }
        delete block;
    } else {
        if (size == 1) {
            temp_ptr = new T;
            *temp_ptr = *block;
        } else  if (size > 1) {
            temp_ptr = new T[size];
            int numRepeats;
            if (size > allocedMem)
                numRepeats = allocedMem;
            else    
                numRepeats = size;
            for (int i = 0; i < numRepeats; i++) 
                temp_ptr[i] = block[i];
        }
        delete[] block;
    }
    block = nullptr;
    // will return nullptr if size = 0
    return temp_ptr;
}

#endif // REALLOCPP_H