// general purpose utility methods

#include <utils.h>

#include <algorithm>
#include <limits>

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cfloat>
#include <sys/stat.h>

#include <glm/gtx/rotate_vector.hpp>

// https://stackoverflow.com/a/12774387/7381705
bool file_exists (const char* name){
  struct stat buffer;   
  return (stat (name, &buffer) == 0); 
}

// NOTE: Free buffer when done with it.
char* read_entire_file(const char* file)
{
	FILE* source_file = fopen(file, "rb");
	if (!source_file)
	{
		perror("poor");
		return NULL;
	}
	fseek(source_file, 0, SEEK_END);
	long src_size = ftell(source_file);
	fseek(source_file, 0, SEEK_SET);
	char* buffer = (char*) malloc(src_size + 1);
	fread(buffer, 1, src_size, source_file);
	fclose(source_file);
	buffer[src_size] = 0;
	return buffer;
}

// https://stackoverflow.com/a/32334103
bool nearly_equal(float a, float b){
	float epsilon = 128 * FLT_EPSILON;
	float abs_th = FLT_MIN;
	
  assert(std::numeric_limits<float>::epsilon() <= epsilon);
  assert(epsilon < 1.f);

  if (a == b) return true;

  auto diff = std::abs(a-b);
  auto norm = std::min((std::abs(a) + std::abs(b)), std::numeric_limits<float>::max());
  // or even faster: std::min(std::abs(a + b), std::numeric_limits<float>::max());
  // keeping this commented out until I update figures below
  return diff < std::max(abs_th, epsilon * norm);
}

// quick shorthand 
bool nearly_less_or_eq(float a, float b){
	return (a < b) || nearly_equal(a, b);
}

bool nearly_greater_or_eq(float a, float b){
	return (a > b) || nearly_equal(a, b);
}

// https://stackoverflow.com/a/16575025
bool isStringNumber(std::string& str){
	char* p;
	strtod(str.c_str(), &p);
	return *p == 0;
}

// check if two cubes are intersecting
bool cubesIntersecting(glm::vec3 p1, glm::vec3 s1, glm::vec3 p2, glm::vec3 s2){
	return (
		nearly_greater_or_eq(p1.x+s1.x/2.f, p2.x-s2.x/2.f) &&
		nearly_greater_or_eq(p2.x+s2.x/2.f, p1.x-s1.x/2.f) &&
		
		nearly_greater_or_eq(p1.y+s1.y/2.f, p2.y-s2.y/2.f) &&
		nearly_greater_or_eq(p2.y+s2.y/2.f, p1.y-s1.y/2.f) &&
		
		nearly_greater_or_eq(p1.z+s1.z/2.f, p2.z-s2.z/2.f) &&
		nearly_greater_or_eq(p2.z+s2.z/2.f, p1.z-s1.z/2.f)
	);
}

// check if a line intersects another line
// adapted from: https://stackoverflow.com/a/3746601
// I have no idea how this works, but it does!
bool linesIntersecting(glm::vec2 a1, glm::vec2 a2, glm::vec2 b1, glm::vec2 b2){
	glm::vec2 b = a2 - a1;
	glm::vec2 d = b2 - b1;
	float bDotDPerp = b.x * d.y - b.y * d.x;

	// if b dot d == 0, it means the lines are parallel so have infinite intersection points
	if (bDotDPerp == 0)
		return false;

	glm::vec2 c = b1 - a1;
	float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
	if (t < 0 || t > 1)
		return false;

	float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
	if (u < 0 || u > 1)
		return false;
	
	return true;
}

// returns true if v2 is clockwise of v1, returns false if v2 is counter-clockwise of v1
// normalized pls :)
bool vectorClockwise(glm::vec2 v1, glm::vec2 v2){
	// rotate v2 90 degrees
	glm::vec2 rv = glm::rotate(v2, glm::radians(90.f));
	
	float f = glm::dot(v1, rv);
	
	return nearly_greater_or_eq(f, 0.f);
}