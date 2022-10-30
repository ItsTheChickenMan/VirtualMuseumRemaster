// general purpose utility methods

#ifndef VMR_UTILS_H
#define VMR_UTILS_H

#include <cstdlib>
#include <cstdint>
#include <cfloat>

#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// allocate memory for a type and return the address for the allocated memory
template <typename T>
T* allocateMemoryForType(){
	return (T*)malloc( sizeof(T) );
}

// square a type
template <typename T>
T square(T x){
	return x*x;
}

// check if a file exists
bool file_exists (const char* name);

// read the contents of a file and return a char buffer of the contents (free the buffer when done!)
char* read_entire_file(const char* file);

// float stuff
bool nearly_equal(float a, float b);
bool nearly_less_or_eq(float a, float b);
bool nearly_greater_or_eq(float a, float b);

// check if string is a number
bool isStringNumber(std::string& str);

// various intersection methods
bool cubesIntersecting(glm::vec3 p1, glm::vec3 s1, glm::vec3 p2, glm::vec3 s2);
bool linesIntersecting(glm::vec2 a1, glm::vec2 a2, glm::vec2 b1, glm::vec2 b2);
bool vectorClockwise(glm::vec2 v1, glm::vec2 v2);

#endif