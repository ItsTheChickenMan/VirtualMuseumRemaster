// mouse management

#ifndef VMR_MOUSE_H
#define VMR_MOUSE_H

// includes //
#include <graphics.h>
#include <glm/glm.hpp>

// methods //
void initMouseManager(Window* window);
glm::vec2 getMousePosition();
glm::vec2 getMouseDelta();
void resetMouseDelta();

#endif