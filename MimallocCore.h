/*  
	-----------------------------------------------------------------------------------------------------------
	this header file has two functions in it:
	mi_newMalloc and mi_reallocpp
	mi_newMalloc will create bitwise memory, then construct objects within the memory if typename T is a class

	mi_reallocpp is an object-friendly implementation of realloc. it will safely decrease or expand the size of 
	memory and safely handle destruction and construction. 
	-----------------------------------------------------------------------------------------------------------

*/	

#ifndef MIMALLOC_CORE_HPP
#define MIMALLOC_CORE_HPP

#include <type_traits>	// for std::is_class
#include <mimalloc.h>	// for mi_malloc, mi_expand
#include <cstddef>		// need std::alignof for proper alignment

/*
	mi_newMalloc will create memory and construct in place if needed
*/

template <typename T>
T* mi_newMalloc(T* temp_data, size_t size, bool isClass) {
	// aligned allocation
	temp_data = static_cast<T*> (mi_malloc_aligned(sizeof(T) * size, alignof(T)));
	if (temp_data == nullptr)
		throw std::runtime_error("Memory allocation failed");
	if (isClass) {
		for (size_t i = 0; i < size; i++) {
			T* temp_ptr = temp_data + i;
			temp_ptr = new(temp_ptr) T;
		}
	}

	return temp_data;
}
/*
	mi_reallocpp safely expands memory in place if it can, and if not
	it will try to create new memory and copy old memory over
	constructs objects in all memory, leaves nothing uninitialized
	returns nullptr if both fail
*/

template <typename T>
T* mi_reallocpp(T*& block, size_t allocedMem, size_t newSize) {
	T* temp_data = nullptr;
	// before we expand, we have to check the new vs old size in order to 
	// properly call destructors
	if (std::is_class<T>::value) {
		if (newSize < allocedMem) {
			size_t endDestroy = allocedMem - newSize;
			for (int i = allocedMem - 1; i > (allocedMem - 1 - endDestroy); i--)
				block[i].~T();
		}
	}

	T* temp_ptr = nullptr;
	temp_ptr = static_cast<T*> (mi_expand(block, sizeof(T) * newSize));
	if (temp_ptr == nullptr) {
		// couldn't resize in place, must find new contiguous memory
		// this means that we are expanding
		temp_ptr = static_cast<T*> (mi_malloc_aligned(sizeof(T) * newSize, alignof(T)));
		if (std::is_class<T>::value) {
			for (size_t i = 0; i < newSize; i++) {
				// new mjust return lvalue
				temp_data = temp_ptr + i;
				temp_data = new(temp_data) T;
			}
		}
		// TODO: Simplify
		int numRepeats;
		if (newSize < allocedMem)
			numRepeats = newSize;
		else
			numRepeats = allocedMem;

		for (size_t i = 0; i < numRepeats; i++)
			temp_ptr[i] = block[i];
		// safely destroy everything
		if (std::is_class<T>::value) {
			for (size_t i = 0; i < allocedMem; i++) {
				block[i].~T();
			}
		}
		mi_free(block);
		return temp_ptr;
	}

	// placement new to construct objects we created 
	if (newSize > allocedMem) {
		size_t numCreate = newSize - allocedMem;
		// from the last element of the current allocation until the end of the new size
		for (int i = allocedMem - 1; i < newSize; i++) {
			// new must return lvalue
			temp_data = temp_ptr + i;
			temp_data = new(temp_data) T;
		}
	}
	// now block == temp_ptr
	return temp_ptr;
}

/*
	mi_rawReallocpp functions the same as mi_reallocpp
	however, if it needs to expand memory, it will leave the expanded memory 
	unconstructed
	i.e. if current size is 3 and we expand to 5, 'elements' 4 and 5 will be unconstructed
*/

template <typename T>
T* mi_rawReallocpp(T*& block, size_t allocedMem, size_t newSize) {
	T* temp_data = nullptr;
	// destroy blocks if downsizing
	if (std::is_class<T>::value) {
		if (newSize < allocedMem) {
			size_t endDestroy = allocedMem - newSize;
			for (int i = allocedMem - 1; i > (allocedMem - 1 - endDestroy); i--)
				block[i].~T();
		}
	}

	T* temp_ptr = nullptr;
	temp_ptr = static_cast<T*> (mi_expand(block, sizeof(T) * newSize));
	if (temp_ptr == nullptr) {
		// could not expand in place, must find new raw memory
		// will only reach this position if upsizing memory 
		temp_ptr = static_cast<T*> (mi_malloc_aligned(sizeof(T) * newSize, alignof(T)));
		// only construct as many as needed, copy into, leave rest unconstructed
		for (size_t i = 0; i < allocedMem; i++) {
			if (std::is_class<T>::value) {
				temp_data = temp_ptr + i;
				temp_data = new(temp_data) T;
			}
			// this is fucked 
			temp_ptr[i] = block[i];
			if (std::is_class<T>::value)
				block[i].~T();
		}
		mi_free(block);
		block = nullptr;
	}

	// NOTE**: at this point, memory between allocedMem and newSize (exclusive) is UNINITIALIZED. BEWARE
	// return temp_ptr bc it works for both temp_ptr == nullptr and not, since block == temp_ptr
	return temp_ptr;
}

#endif // MIMALLOC_CORE_HPP