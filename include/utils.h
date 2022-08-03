// general purpose utility methods

#ifndef VHR_UTILS_H
#define VHR_UTILS_H

#include <cstdlib>
#include <cstdint>

// allocate memory for a type and return the address for the allocated memory
template <typename T>
T* allocateMemoryForType(){
	return (T*)malloc( sizeof(T) );
}

// read the contents of a file and return a char buffer of the contents (free the buffer when done!)
char* read_entire_file(const char* file);

#endif